// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <vector>

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FloorLevel.generated.h"

USTRUCT(BlueprintType)
struct FRoomState
{
	GENERATED_BODY()

	FRoomState() :
		HasFrontWall(false),
		HasBackWall(false),
		HasLeftWall(false),
		HasRightWall(false),
		setId(-1)
	{}

	UPROPERTY(BlueprintReadWrite)
	bool HasFrontWall;

	UPROPERTY(BlueprintReadWrite)
	bool HasBackWall;

	UPROPERTY(BlueprintReadWrite)
	bool HasLeftWall;

	UPROPERTY(BlueprintReadWrite)
	bool HasRightWall;

	int setId;
};

USTRUCT(BlueprintType)
struct FRoomsRow
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	TArray<FRoomState> Rooms;
};

UCLASS()
class OFFICERUNNER_API AFloorLevel : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	AFloorLevel();

	// Called every frame
	virtual void Tick(float DeltaTime) override;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Width;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Height;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FRoomsRow> RoomStates;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	static std::vector<std::vector<FRoomState>> GenerateLabyrinth(size_t width, size_t height);
};
