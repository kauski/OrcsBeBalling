// Fill out your copyright notice in the Description page of Project Settings.


#include "BallSpawner.h"


// Sets default values
ABallSpawner::ABallSpawner()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	spawnerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cube Mesh"));
	static ConstructorHelpers::FObjectFinder<UBlueprint>myBpAsset(TEXT("/Script/Engine.Blueprint'/Game/MyDodgeBall.MyDodgeBall'"));
	

	if (myBpAsset.Object != NULL)
	{
		bpBall = (UBlueprint*)myBpAsset.Object->GetClass();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("myBpAsset was null line 21 in ballspawnercpp"));
	}

	static
		ConstructorHelpers::FObjectFinder<UStaticMesh> spawnCube(TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	this->spawnerMesh->SetStaticMesh(spawnCube.Object);

	this->SetRootComponent(spawnerMesh);
	
}

// Called when the game starts or when spawned
void ABallSpawner::BeginPlay()
{

	Super::BeginPlay();
	
	FActorSpawnParameters params;
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	spawnLocation = this->GetActorTransform();
	spawnLocation.SetLocation(spawnLocation.GetLocation() - FVector(0, 0, 50));
	ball = GetWorld()->SpawnActor<ADodgeBall>(ball->GetClass(), spawnLocation, params);
	GEngine->AddOnScreenDebugMessage(-1, 50.f, FColor::Red, TEXT("Spawning ball: " + spawnLocation.ToString()));
}

// Called every frame
void ABallSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

