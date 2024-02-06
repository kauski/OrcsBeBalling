// Fill out your copyright notice in the Description page of Project Settings.
#include "DodgeBall.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Controller.h"
#include "GameFramework/PlayerController.h"
#include <Net/UnrealNetwork.h>
#include "PartyProject/PartyProjectCharacter.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"

// Sets default values
ADodgeBall::ADodgeBall()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	body = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DodgeBallBody"));
	dodgeBallCollider = CreateDefaultSubobject<USphereComponent>(TEXT("Dodge Col"));

	static
		ConstructorHelpers::FObjectFinder<UStaticMesh>SphereMeshAsset(TEXT("StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));
	this->body->SetStaticMesh(SphereMeshAsset.Object);

	dodgeBallCollider->SetupAttachment(body);
	body->SetRelativeScale3D(FVector(0.5, 0.5, 0.5));


	this->Tags.Add("Ball");
	


	
	//SetReplicates(true);
	//SetReplicateMovement(true);
}

void ADodgeBall::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Here you specify which properties should be replicated.
	// For example, if you have a replicated FVector property called BallLocation:
	//DOREPLIFETIME(ADodgeBall, bBallLocation);
	//DOREPLIFETIME(ADodgeBall, bBallVelocity);
	//DOREPLIFETIME(ADodgeBall, bBallRotation);
	DOREPLIFETIME(ADodgeBall, bIsPickedUp);

}

struct Flocalparameters
{

};
void ADodgeBall::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->ActorHasTag("GroundActor") && !bHitGround)
	{
		//&& bIsAlive && !bScoringDone
		bHitGround = true;

		BallHitGroundSound();
		bIsAlive = false;
		bScoringDone = true;
		dodgeBallCollider->SetSphereRadius(pickUpRadius, true);
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Ball touched ground"));
	}

	if (OtherActor->ActorHasTag("WallActor") && !bHitWall)
	{
		//&& !bScoringDone
		bHitWall = true;

		body->ComponentVelocity;
		BallHitWallSound();
		bIsAlive = false;
		bScoringDone = true;
		dodgeBallCollider->SetSphereRadius(pickUpRadius, true);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Ball hit wall"));
	}
	/*
	if (OtherActor->ActorHasTag("Player") && !bHitPlayer) {
		bHitPlayer = true;

		PlayerHitScreamSound(OtherActor);
		BallHitPlayerSound();
	}
	*/
	if (APartyProjectCharacter* p = Cast<APartyProjectCharacter>(OtherActor))
	{
		FString touchingPlayer = p->GetPlayerState()->GetPlayerName();


		if (bIsAlive && !bScoringDone && touchingPlayer != playerName)
		{
			PlayerHitScreamSound(OtherActor);
			BallHitPlayerSound();
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("" + p->GetName()));
			Flocalparameters params;
			if (IsValid(function) && IsValid(gameState))
			{
				gameState->ProcessEvent(function, &params);
				bScoringDone = true;
				bIsAlive = false;
				dodgeBallCollider->SetSphereRadius(pickUpRadius, true);
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Hit registered on player"));
			}
			else if (!IsValid(function))
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Dodgeball function is invalid"));
			}
			else if (!IsValid(gameState))
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Dodgeball gamestate is invalid"));
			}
		}
	}


	

}


void ADodgeBall::OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Overlap End"));
	if (OtherActor->ActorHasTag("WallActor")) {
		bHitWall = false;
		bScoringDone = false;
	}
	if (OtherActor->ActorHasTag("GroundActor")) {
		bHitGround = false;
		bScoringDone = false;
	}
	if (OtherActor->ActorHasTag("Player")) {
		bHitPlayer = false;
		bScoringDone = false;
	}
}

void ADodgeBall::BallHitWallSound()
{
	if (hitWallSound && actorTarget) {
		UGameplayStatics::PlaySoundAtLocation(this, hitWallSound, actorTarget->GetActorLocation());
	}
}

void ADodgeBall::BallHitGroundSound()
{
	if (hitGroundSound && actorTarget) {
		UGameplayStatics::PlaySoundAtLocation(this, hitGroundSound, actorTarget->GetActorLocation());
	}
}

void ADodgeBall::BallHitPlayerSound()
{
	if (hitPlayerSound && actorTarget) {
		UGameplayStatics::PlaySoundAtLocation(this, hitPlayerSound, actorTarget->GetActorLocation());
	}
}

void ADodgeBall::PlayerHitScreamSound(AActor* player)
{
	if (playerScreamSound && player) {
		UGameplayStatics::PlaySoundAtLocation(this, playerScreamSound, actorTarget->GetActorLocation());
	}
}

void ADodgeBall::PlayerThrowSound()
{
	if (throwSound) {
		UGameplayStatics::PlaySoundAtLocation(this, throwSound, this->GetActorLocation());
	}
}

bool ADodgeBall::Server_SetPickedUp_Validate(bool bPickedUp)
{
	return true;
}

void ADodgeBall::Server_SetPickedUp_Implementation(bool bPickedUp)
{
	bIsPickedUp = bPickedUp;
}

void ADodgeBall::OnPlayerPickUp(const FString& pName_)
{
	//Removes physics and stops generating overlaps
	body->SetSimulatePhysics(false);
	dodgeBallCollider->SetGenerateOverlapEvents(false);
	body->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	playerName = pName_;
	GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, TEXT("I have been picked up :" + this->GetName()) +  pName_);

	if (!HasAuthority()) {
		//Server_OnPlayerPickUp();
		Server_SetPickedUp(true);
	}
	else {
		if (GetNetMode() == NM_Client) {
			//Multi_OnPlayerPickUp();
			Server_SetPickedUp(true);
		}
	}
}

void ADodgeBall::GetThrown(FVector playerForwardVector, float chargeTime_, FVector playerPos)
{
	UE_LOG(LogTemp, Warning, TEXT("Player Forward Vector: X=%.2f, Y=%.2f, Z=%.2f"),
		playerForwardVector.X, playerForwardVector.Y, playerForwardVector.Z);
	dodgeBallCollider->SetSphereRadius(throwHitBoxRadius, true);
	body->SetSimulatePhysics(true);
	dodgeBallCollider->SetGenerateOverlapEvents(true);
	PlayerThrowSound();
	
	if (chargeTime_ > maxChargeTresholdTime)
	{
		
		body->AddImpulse(playerForwardVector * throwVelocityBaseValue * throwVelocityMultiplier * body->GetMass(), "", true);

	//	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("ISO HEITTO: " + FString::SanitizeFloat(chargeTime_)));
	}
	else
	{
		body->AddImpulse(playerForwardVector * throwVelocityBaseValue * body->GetMass(), "", true);
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("NORMI HEITTO: " + FString::SanitizeFloat(chargeTime_)));
	}
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Throw End"));
	body->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
	bIsAlive = true;
	bScoringDone = false;
	float distance = 175.0f;

	FVector newPos = playerPos + playerForwardVector * distance;
	SetActorLocation(newPos);

	if (!HasAuthority()) {
		//Server_GetThrown(playerForwardVector);
	}
	else {
		//Multi_GetThrown(playerForwardVector);
	}

}

bool ADodgeBall::Server_OnPlayerPickUp_Validate()
{
	return true;
}

void ADodgeBall::Server_OnPlayerPickUp_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("Server_OnPlayerPickUp_Implementation HAS BEEN CALLED"));

	Multi_OnPlayerPickUp();

}

bool ADodgeBall::Multi_OnPlayerPickUp_Validate()
{
	return true;
}

void ADodgeBall::Multi_OnPlayerPickUp_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("Multi_OnPlayerPickUp_Implementation HAS BEEN CALLED"));

	//Removes physics and stops generating overlaps
	body->SetSimulatePhysics(false);
	dodgeBallCollider->SetGenerateOverlapEvents(false);
	body->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
}

bool ADodgeBall::Server_GetThrown_Validate(FVector playerForwardVector)
{
	return true;
}

void ADodgeBall::Server_GetThrown_Implementation(FVector playerForwardVector)
{
	Multi_GetThrown(playerForwardVector);
}

bool ADodgeBall::Multi_GetThrown_Validate(FVector playerForwardVector)
{
	return true;
}

void ADodgeBall::Multi_GetThrown_Implementation(FVector playerForwardVector)
{

	body->SetSimulatePhysics(true);
	dodgeBallCollider->SetGenerateOverlapEvents(true);
	body->AddImpulse(playerForwardVector * throwVelocityBaseValue * body->GetMass(), "", true);
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Throw End"));
	body->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);

}



// Called when the game starts or when spawned
void ADodgeBall::BeginPlay()
{
	Super::BeginPlay();
	dodgeBallCollider->OnComponentBeginOverlap.AddDynamic(this, &ADodgeBall::OnOverlapBegin);
	dodgeBallCollider->OnComponentEndOverlap.AddDynamic(this, &ADodgeBall::OnOverlapEnd);
	bScoringDone = false;
	bIsAlive = false;
	pickUpRadius = 120;
	throwHitBoxRadius = 52;
	playerName = "";
	gameState = UGameplayStatics::GetGameState(GetWorld());
	function = gameState->FindFunction("SR_UpdateScore");

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), "GroundActor", FoundActors);
	if (!FoundActors.IsEmpty())
	{
		ground = FoundActors[0];
		//GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, TEXT("Ground: " +  ground->GetName()));

	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, TEXT("Couldn't find ground in dodgeball Beginplay"));
	}


	dodgeBallCollider->SetCollisionProfileName("Trigger");
	body->SetSimulatePhysics(true);
	body->SetMassOverrideInKg("", 50, true);
	body->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	dodgeBallCollider->SetGenerateOverlapEvents(true);
	dodgeBallCollider->SetUseCCD(true);
	dodgeBallCollider->SetSphereRadius(pickUpRadius, true);
	//bBallLocation = body->GetComponentLocation();
	//bBallVelocity = body->GetComponentVelocity();
	//bBallRotation = body->GetComponentRotation();
	if (!HasAnyFlags(RF_ClassDefaultObject)) {
		bReplicates = true;
		
		//bReplicateMovement = true;
	}

	/*
	AController* Controller = GetController();

	if (Controller && Controller->IsLocalController()) {
		bIsLocallyControlled = true;
	}
	*/

	//currentLocation = GetActorLocation();
	throwVelocityBaseValue = 30;
	maxChargeTresholdTime = 2.5f;
	throwVelocityMultiplier = 4;

}

// Called every frame
void ADodgeBall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

