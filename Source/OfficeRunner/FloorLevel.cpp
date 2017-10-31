// Fill out your copyright notice in the Description page of Project Settings.

#include "FloorLevel.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"

#include <map>
#include <set>

// Generator helper class
class State
{
public:
	State() :
		m_currentSetId(0)
	{
	}

	void next(std::vector<FRoomState>& row)
	{
		m_sets.clear();
		for (auto& room : row) {
			if (room.setId != -1) {
				auto it = m_sets.find(room.setId);
				if (it != m_sets.end()) {
					m_sets[room.setId] = { &room };
				}
				else {
					m_sets[room.setId].insert(&room);
				}
			}
		}
	}

	bool inSameSet(const FRoomState& room1, const FRoomState& room2) const
	{
		return room1.setId == room2.setId;
	}

	void mergeSets(FRoomState& room1, FRoomState& room2)
	{
		int currentSetId = room1.setId;
		int otherSetId = room2.setId;

		auto& otherSet = m_sets[otherSetId];
		m_sets[currentSetId].insert(otherSet.begin(), otherSet.end());

		for (auto& cell : otherSet) {
			cell->setId = currentSetId;
		}

		m_sets.erase(otherSetId);
	}

	void eraseFromSet(FRoomState& room)
	{
		if (room.setId == -1) {
			return;
		}

		auto& currentSet = m_sets[room.setId];
		currentSet.erase(&room);
		if (currentSet.empty()) {
			m_sets.erase(room.setId);
		}

		room.setId = -1;
	}

	void createOwnSet(FRoomState& room)
	{
		if (room.setId != -1) {
			return;
		}

		int setId = m_currentSetId++;
		m_sets[setId] = { &room };
		room.setId = setId;
	}

	bool canAddBackWall(FRoomState& room)
	{
		if (room.setId == -1) {
			return false;
		}

		bool hasOtherPotentialCells = false;
		for (auto& c : m_sets[room.setId]) {
			if (c != &room && !c->HasBackWall) {
				hasOtherPotentialCells = true;
				break;
			}
		}
		return hasOtherPotentialCells;
	}

private:
	std::map<int, std::set<FRoomState*>> m_sets;
	int m_currentSetId;
};


// Sets default values
AFloorLevel::AFloorLevel()
{
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AFloorLevel::BeginPlay()
{
	std::vector<std::vector<FRoomState>> rooms = GenerateLabyrinth(Width, Height);

	RoomStates.SetNum(Height);
	for (int i = 0; i < Height; ++i) {
		RoomStates[i].Rooms.SetNum(Width);
		for (int j = 0; j < Width; ++j) {
			auto& room = RoomStates[i].Rooms[j];

			room.HasFrontWall = (i == 0);
			room.HasBackWall = (i == Height - 1) || rooms[i][j].HasBackWall;
			room.HasLeftWall = (j == 0);
			room.HasRightWall = (j == Width - 1) || rooms[i][j].HasRightWall;
		}
	}

	Super::BeginPlay();
}

// Called every frame
void AFloorLevel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

std::vector<std::vector<FRoomState>> AFloorLevel::GenerateLabyrinth(size_t width, size_t height)
{
	std::vector<std::vector<FRoomState>> rooms;

	if (width == 0 || height == 0) {
		return rooms;
	}

	State state;

	for (size_t i = 0; i < height; ++i) {
		if (i == 0) {
			rooms.push_back(std::vector<FRoomState>(width));
		}
		else {
			rooms.push_back(rooms[i - 1]);
			state.next(rooms[i]);

			for (size_t j = 0; j < width; ++j) {
				auto& room = rooms[i][j];
				room.HasRightWall = false;
				if (room.HasBackWall) {
					state.eraseFromSet(room);
					room.HasBackWall = false;
				}
			}
		}

		auto& row = rooms[i];

		for (auto& cell : row) {
			if (cell.setId == -1) {
				state.createOwnSet(cell);
			}
		}

		for (size_t j = 0; j < width - 1; ++j) {
			if (state.inSameSet(row[j], row[j + 1]) || rand() % 2 > 0) {
				row[j].HasRightWall = true;
			}
			else {
				state.mergeSets(row[j], row[j + 1]);
			}
		}

		for (size_t j = 0; j < width; ++j) {
			if (rand() % 4 > 1 && state.canAddBackWall(row[j])) {
				row[j].HasBackWall = true;
			}
		}

		if (i == height - 1) {
			for (size_t j = 0; j < width - 1; ++j) {
				if (!state.inSameSet(row[j], row[j + 1])) {
					row[j].HasRightWall = false;
					state.mergeSets(row[j], row[j + 1]);
				}
			}
		}
	}

	return rooms;
}
