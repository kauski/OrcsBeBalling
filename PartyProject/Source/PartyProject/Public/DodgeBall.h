// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Math/Vector.h"
#include "GameFramework/Actor.h"
#include "Engine/EngineTypes.h"
#include "UObject/ConstructorHelpers.h"
#include "GameFramework/Character.h"
#include "GameFramework/Actor.h"
#include "Components/PrimitiveComponent.h"
#include "DodgeBall.generated.h"

UCLASS()
class PARTYPROJECT_API ADodgeBall : public AActor
{
	GENERATED_BODY()
	
public:	
	ADodgeBall* ballRef;

	// Sets default values for this actor's properties
	ADodgeBall();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dodgeball Collider")
	class USphereComponent* dodgeBallCollider;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dodgeball ThrowVelocityBasevalue")
	int throwVelocityBaseValue;


	UPROPERTY(Replicated)
	float throwVelocityMultiplier;


	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Test");
	UStaticMeshComponent* body;

	UPROPERTY(Replicated)
	bool bIsPickedUp;

	//UPROPERTY()
	bool bHitWall;
	bool bHitGround;
	bool bHitPlayer;

	UPROPERTY(Replicated)
	FVector bBallLocation;

	UPROPERTY(Replicated)
	FVector bBallVelocity;

	UPROPERTY(Replicated)
	FRotator bBallRotation;

	UPROPERTY()
	float pickUpRadius;

	UPROPERTY()
	float throwHitBoxRadius;

	UPROPERTY()
	AActor* ground;

	UPROPERTY()
	UFunction* function;

	UPROPERTY()
	AGameStateBase* gameState;
	UPROPERTY(Replicated)
	bool bIsAlive;
	UPROPERTY(Replicated)
	bool bScoringDone;

	UPROPERTY(Replicated)
	float maxChargeTresholdTime;



	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	void OnPlayerPickUp(const FString& pName_);
	void GetThrown(FVector playerForwardVector, float chargeTime_, FVector playerPos);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_OnPlayerPickUp();
	bool Server_OnPlayerPickUp_Validate();
	void Server_OnPlayerPickUp_Implementation();

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void Multi_OnPlayerPickUp();
	bool Multi_OnPlayerPickUp_Validate();
	void Multi_OnPlayerPickUp_Implementation();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_GetThrown(FVector playerForwardVector);
	bool Server_GetThrown_Validate(FVector playerForwardVector);
	void Server_GetThrown_Implementation(FVector playerForwardVector);

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void Multi_GetThrown(FVector playerForwardVector);
	bool Multi_GetThrown_Validate(FVector playerForwardVector);
	void Multi_GetThrown_Implementation(FVector playerForwardVector);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetPickedUp(bool bPickedUp);
	bool Server_SetPickedUp_Validate(bool bPickedUp);
	void Server_SetPickedUp_Implementation(bool bPickedUp);

	ACharacter* playerCharacter;
	FString playerName;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	void BallHitWallSound();
	void BallHitGroundSound();
	void BallHitPlayerSound();
	void PlayerHitScreamSound(AActor* player);
	void PlayerThrowSound();

	FVector currentLocation;
	

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Audio")
	AActor* actorTarget;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	USoundBase* hitWallSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	USoundBase* hitGroundSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	USoundBase* hitPlayerSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	USoundBase* playerScreamSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	USoundBase* throwSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	float fMaxAttuneationDistance;
};
