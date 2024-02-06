// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DodgeBall.h"
#include "UObject/ConstructorHelpers.h"
#include "GameFramework/Actor.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "BallSpawner.generated.h"


UCLASS()
class ABallSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	
	ABallSpawner();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dodgeball")
	ADodgeBall* ball;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dodgeball")
	UBlueprint* bpBall;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner Mesh")
	UStaticMeshComponent* spawnerMesh;


	FTransform spawnLocation;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
