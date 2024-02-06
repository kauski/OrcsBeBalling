// Copyright Epic Games, Inc. All Rights Reserved.

#include "PartyProjectCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Math/Vector2D.h"
#include "Math/Vector.h"
#include "Components/SphereComponent.h"
#include "UObject/Class.h"
#include "UObject/UnrealType.h"
#include "Components/SceneComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/EngineTypes.h"
#include "Kismet/KismetMathLibrary.h"
#include "Math/UnrealMathUtility.h"
#include "Components/ArrowComponent.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include <Net/UnrealNetwork.h>
#include "Misc/OutputDeviceNull.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/GameStateBase.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Widget.h"
#include "Components/WidgetComponent.h"
#include "Camera/PlayerCameraManager.h"


#define GroundTraceChannel	ECC_GameTraceChannel1



APartyProjectCharacter::APartyProjectCharacter()
{
	Percent = 1.0f;
	rotSpeedMultiplier = 20;
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 100.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;



	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 50.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = false; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
	//FollowCamera->SetupAttachment(RootComponent);

	LastDashTime = 0.0f;
	TimeSinceLastDash = 0.0f;
	MovementCooldownDuration = 0.0f;
	dashDirection.X = 0.0f;
	dashDirection.Y = 0.0f;
	LTplayerCurrentRotation = this->GetActorRotation();
	SetReplicates(true);
	SetReplicateMovement(true);

	/////////*dodgeForce = 2400.0f;
	////////dodgeCooldownDuration = 2.0f;
	////////LastDodgeTime = 0.0f;
	////////TimeSinceLastDodge = 0.0f;
	////////MovementCooldownDurationDodge = 1.5f;*/

	bHasBall = false;

}

void APartyProjectCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Here you specify which properties should be replicated.
	// For example, if you have a replicated FVector property called BallLocation:
	DOREPLIFETIME(APartyProjectCharacter, bHasBall);
	DOREPLIFETIME(APartyProjectCharacter, ballRef);
	DOREPLIFETIME(APartyProjectCharacter, name);
	DOREPLIFETIME(APartyProjectCharacter, LTplayerCurrentRotation);
	DOREPLIFETIME(APartyProjectCharacter, LTmouseLoc);
	DOREPLIFETIME(APartyProjectCharacter, LTmouseDir);
	DOREPLIFETIME(APartyProjectCharacter, LTimpactLoc)
		DOREPLIFETIME(APartyProjectCharacter, LTrotationMultiplier);
	DOREPLIFETIME(APartyProjectCharacter, LTwantedRot)
		DOREPLIFETIME(APartyProjectCharacter, ballsNearPlayer);
}
struct Flocalparameters
{
	FString x;

};

struct Chargetimelocalparameters
{
	double currentTime;
};

struct PCparams
{
	APlayerController* pc;
};


void APartyProjectCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();



	//	ADodgeBall* ball;
		//Add Input Mapping Context
	/////////*LastDodgeTime -= MovementCooldownDurationDodge;
	////////TimeSinceLastDodge -= MovementCooldownDurationDodge;*/
//	ADodgeBall* ball;
	//Add Input Mapping Context

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
			//	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("subsystem: " + this->GetFName().ToString()));

		}
	}


	TArray<UStaticMeshComponent*> points;
	GetComponents<UStaticMeshComponent>(points);
	if (!points.IsEmpty())
	{
		for (size_t i = 0; i < points.Num(); i++)
		{
			if (points[i]->ComponentHasTag("Point"))
			{
				point = points[i];

				//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Purple, TEXT("joo joo " + point->GetName()));

				break;
			}

		}

	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Purple, TEXT("points on empty "));

	}

	if (IsValid(point))
	{
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Purple, TEXT("joo joo " + point->GetName()));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Purple, TEXT("ei muute oo valid"));

	}



	gs = UGameplayStatics::GetGameState(GetWorld());

	AActor* actorBall;
	actorBall = UGameplayStatics::GetActorOfClass(GetWorld(), ADodgeBall::StaticClass());
	if (IsValid(actorBall))
	{
		ballRef = Cast<ADodgeBall>(actorBall);
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Purple, TEXT("Found ball: " + ballRef->GetName()));
	}

	//Bunch of setters here since can't do a lot of them in the constructor
	playerHitBox = GetCapsuleComponent();
	playerHitBox->OnComponentBeginOverlap.AddDynamic(this, &APartyProjectCharacter::OnOverlap);
	playerHitBox->OnComponentEndOverlap.AddDynamic(this, &APartyProjectCharacter::OnOverlapEnded);
	playerBaseRotationRate = 1000;
	dashForce = 2400.0f;
	DashCooldownDuration = 5.1f;
	chargeTime = 0;
	bchargeStarted = false;


	updateBlueprintFunction = this->FindFunction("UpdateThrowIndicator");
	updateKnockCooldown = this->FindFunction("KnockBackTimer");
	//if (IsValid(updateBlueprintFunction))
	//{
	//	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, TEXT("Jee"));
	//}
	//else
	//{
	//	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("Boo"));
	//}

	resetIndicatorBlueprintFunction = this->FindFunction("ResetThrowIndicator");
	playerThrewFunction = gs->FindFunction("SR_PlayerThrew");
	pickedUpBallFunction = gs->FindFunction("SR_PickedUpBall");
	if (!IsValid(resetIndicatorBlueprintFunction))
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("resetindicatorblueprint function null in character beginplaycpp"));
	}




}



void APartyProjectCharacter::OnOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ADodgeBall* ball = Cast<ADodgeBall>(OtherActor))
	{

		if (!ballsNearPlayer.Contains(ball))
		{
			ballsNearPlayer.Add(ball);
			//int x = ballsNearPlayer.Num();
			//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Overlap Begin with ball" + ball->GetName() + "Balls near player: " + FString::FromInt(x)));
		}

		isNearlBall = true;
	}

}

void APartyProjectCharacter::OnOverlapEnded(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

	if (ADodgeBall* ball = Cast<ADodgeBall>(OtherActor))
	{
		if (ballsNearPlayer.Contains(ball))
		{
			if (ballsNearPlayer.Contains(ball))
			{
				ballsNearPlayer.Remove(ball);
			}

			if (ballsNearPlayer.IsEmpty())
			{
				isNearlBall = false;
			}
			//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Overlap with ball ended" + ball->GetName()));
		}

		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Overlap End with ball"));
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void APartyProjectCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {

		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//new actions
		EnhancedInputComponent->BindAction(playerMovementAction, ETriggerEvent::Triggered, this, &APartyProjectCharacter::Move);
		EnhancedInputComponent->BindAction(inputPickUpAction, ETriggerEvent::Triggered, this, &APartyProjectCharacter::PickUp);

		//EnhancedInputComponent->BindAction(inputDashDirection, ETriggerEvent::Triggered, this, &APartyProjectCharacter::Direction);

		//Dashinputs
		//TODO put dashinputs under same input
		EnhancedInputComponent->BindAction(inputDashMouse, ETriggerEvent::Triggered, this, &APartyProjectCharacter::Dash);
		EnhancedInputComponent->BindAction(inputDashGamepad, ETriggerEvent::Triggered, this, &APartyProjectCharacter::Dash);
		/////*dodge Left
		////EnhancedInputComponent->BindAction(DodgeActionLeft, ETriggerEvent::Triggered, this, &APartyProjectCharacter::Dodge, -90.0f);
		////Dodge right
		////EnhancedInputComponent->BindAction(DodgeActionRight, ETriggerEvent::Triggered, this, &APartyProjectCharacter::Dodge, 90.0f);*/


		// Moving
		//EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APartyProjectCharacter::Move);


		//Aiming
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APartyProjectCharacter::Look);
		EnhancedInputComponent->BindAction(aimAction_controller, ETriggerEvent::Triggered, this, &APartyProjectCharacter::AimBall_Controller);
		//EnhancedInputComponent->BindAction(aimAction_Mouse, ETriggerEvent::Triggered, this, &APartyProjectCharacter::AimBall_Mouse);
		//EnhancedInputComponent->BindAction(aimAction_Mouse, ETriggerEvent::Completed, this, &APartyProjectCharacter::PlayerNotAiming);
		EnhancedInputComponent->BindAction(aimAction_controller, ETriggerEvent::Completed, this, &APartyProjectCharacter::PlayerNotAiming);
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Controls binded: " + this->GetFName().ToString()));
		EnhancedInputComponent->BindAction(playerThrow, ETriggerEvent::Started, this, &APartyProjectCharacter::ThrowChargeStarted);
		EnhancedInputComponent->BindAction(playerThrow, ETriggerEvent::Completed, this, &APartyProjectCharacter::ThrowChargeStopped);



	}
}



void APartyProjectCharacter::ThrowChargeStarted()
{
	if (bHasBall && !bchargeStarted)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Ayy lmao"));
		chargeTime = 0;
		bchargeStarted = true;
	}
}


void APartyProjectCharacter::ThrowChargeStopped()
{

	if (bHasBall && bchargeStarted)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("Chargetime: " + FString::SanitizeFloat(chargeTime)));
		ThrowBall(ballRef);
		isAiming = false;
	}
}

void APartyProjectCharacter::SendCurrentTimeToWidget(float currentTime_)
{
	Chargetimelocalparameters currentTimeParam;
	currentTimeParam.currentTime = currentTime_;

	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, TEXT("param: " + FString::SanitizeFloat(currentTime_)));

	if (currentTime_ <= 2.5f)
	{
		this->ProcessEvent(updateBlueprintFunction, &currentTimeParam);
	}
	else
	{
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("NOTSENDING: "));

	}

}




void APartyProjectCharacter::ResetAndHideIndicator()
{
	Flocalparameters nullParams;

	if (IsValid(resetIndicatorBlueprintFunction))
	{
		this->ProcessEvent(resetIndicatorBlueprintFunction, &nullParams);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("SOMEHTING WENT WRONG IN RESETANDHIDE INDICATOR IN CHARACTER CPP: "));

	}
}



void APartyProjectCharacter::PlayerNotAiming()
{
	isAiming = false;

}

void APartyProjectCharacter::AimBall_Mouse(/*const FInputActionValue& Value*/)
{
	if (bHasBall && IsLocallyControlled())
	{

		//toisen janin paremmat koodit
		float xValue;
		float yValue;
		APlayerController* playerController = GetWorld()->GetFirstPlayerController();
		playerController->GetMousePosition(xValue, yValue);
		FVector playerLocation = GetActorLocation();
		FVector2D CharInScreen;
		playerController->ProjectWorldLocationToScreen(playerLocation, CharInScreen);

		FVector2D Result;
		Result.X = -(yValue - CharInScreen.Y);
		Result.Y = xValue - CharInScreen.X;

		FVector DirectionVector(Result.X, Result.Y, 0);
		DirectionVector.Normalize();
		FRotator NewRotation = DirectionVector.Rotation();
		SetActorRotation(NewRotation);
		isAiming = true;
		gamepadActive = false;

		//pruunaa nää möyhemmin
		if (!HasAuthority())
		{
			Server_AimBall_Mouse(bHasBall, NewRotation);
		}
		else
		{
			Multi_AimBall_Mouse(bHasBall, NewRotation);
		}
	}

}



void APartyProjectCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (Percent < 1.0) {
		CallBlueprintFunction();
	}

	AimBall_Mouse();

	if (bchargeStarted)
	{
		chargeTime += DeltaSeconds;
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("ChargeTime: " + FString::SanitizeFloat(chargeTime)));
		SendCurrentTimeToWidget(chargeTime);
	}

}

void APartyProjectCharacter::Server_AimBall_Mouse_Implementation(bool bBall, FRotator NewRotation_)
{

	Multi_AimBall_Mouse_Implementation(bBall, NewRotation_);

}

void APartyProjectCharacter::Multi_AimBall_Mouse_Implementation(bool bBall, FRotator NewRotation_)
{
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Client aimball mouse triggered"));

	if (!IsLocallyControlled()) {
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("syvempi aimball mouse triggered"));
		if (bBall)
		{

			isAiming = true;


			SetActorRotation(NewRotation_);

		}
	}



}

//no security checks
bool APartyProjectCharacter::Server_AimBall_Mouse_Validate(bool bBall, FRotator NewRotation_)
{
	return true;
}

//no security checks
bool APartyProjectCharacter::Multi_AimBall_Mouse_Validate(bool bBall, FRotator NewRotation_)
{
	return true;
}

void APartyProjectCharacter::PrintPlayerIndex()
{
	// Get the player controller for the current character
	APlayerController* PlayerController = Cast<APlayerController>(Controller);
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT());
}

void APartyProjectCharacter::AimBall_Controller(const FInputActionValue& Value)
{

	if (bHasBall)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("PASDGFGHGHHHHH "));
		isAiming = true;
		FVector2D MovementVector = Value.Get<FVector2D>();
		FRotator playerCurrentRotation = GetActorRotation();
		FRotator targetRotation;
		FVector movement3DVector;
		movement3DVector.X = MovementVector.X;
		movement3DVector.Y = MovementVector.Y;
		movement3DVector.Normalize();
		LTplayerCurrentRotation = playerCurrentRotation;

		double PV = movement3DVector.GetClampedToSize(0.0, 1).Length();

		targetRotation = UKismetMathLibrary::MakeRotFromX(movement3DVector);


		SetActorRotation(FMath::RInterpTo(playerCurrentRotation, targetRotation, FApp::GetDeltaTime(), rotSpeedMultiplier));
		SetActorRotation(targetRotation);

		UE_LOG(LogTemp, Warning, TEXT("Target Rotation: Pitch=%.2f, Yaw=%.2f, Roll=%.2f"),
			targetRotation.Pitch, targetRotation.Yaw, targetRotation.Roll);

		// Alternatively, you can use GEngine->AddOnScreenDebugMessage for on-screen debugging
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Target Rotation: Pitch=%.2f, Yaw=%.2f, Roll=%.2f"),
			targetRotation.Pitch, targetRotation.Yaw, targetRotation.Roll));

		APlayerController* playerController = GetWorld()->GetFirstPlayerController();
		//check whether gamepad is being used and remove deadzones
		float ThumbstickThreshold = 0.1f;
		FVector playerLocation = GetActorLocation();
		FVector2D direction = Value.Get<FVector2D>();
		if (direction.SizeSquared() > ThumbstickThreshold * ThumbstickThreshold) {

			gamepadActive = true;
		}
		else {
			gamepadActive = false;
		}

		if (!HasAuthority())
		{

			Server_AimBall_Controller(bHasBall, playerCurrentRotation, targetRotation, rotSpeedMultiplier);
		}
		else
		{

			Multi_AimBall_Controller(bHasBall, playerCurrentRotation, targetRotation, rotSpeedMultiplier);
		}

		if (playerCurrentRotation.Pitch != 0.0 || playerCurrentRotation.Roll != 0.0)
		{
			FRotator fRot(0, 0, playerCurrentRotation.Yaw);
			playerCurrentRotation = fRot;
		}



		//FRotator printRot = FMath::RInterpTo(playerCurrentRotation, targetRotation, FApp::GetDeltaTime(), rotSpeedMultiplier);
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, TEXT(" Ddd : :" + playerCurrentRotation.ToString()));
		/*GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT(" this :" + printRot.ToString()));
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT(" target rot :" + movement3DVector.ToString()));
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT(" length: :"));*/
	}

}


void APartyProjectCharacter::Server_AimBall_Controller_Implementation(bool bBall, FRotator currentRotation_, FRotator targetRotation_, float rotSpeedMultiplier_)
{
	Multi_AimBall_Controller_Implementation(bBall, currentRotation_, targetRotation_, rotSpeedMultiplier_);
}

void APartyProjectCharacter::Multi_AimBall_Controller_Implementation(bool bBall, FRotator currentRotation_, FRotator targetRotation_, float rotSpeedMultiplier_)
{


	SetActorRotation(FMath::RInterpTo(currentRotation_, targetRotation_, FApp::GetDeltaTime(), rotSpeedMultiplier_));

	if (currentRotation_.Pitch != 0.0 || currentRotation_.Roll != 0.0)
	{
		FRotator fRot(0, 0, currentRotation_.Yaw);
		currentRotation_ = fRot;
	}

}

bool APartyProjectCharacter::Server_AimBall_Controller_Validate(bool bBall, FRotator currentRotation_, FRotator targetRotation_, float rotSpeedMultiplier_)
{
	return true;
}

bool APartyProjectCharacter::Multi_AimBall_Controller_Validate(bool bBall, FRotator currentRotation_, FRotator targetRotation_, float rotSpeedMultiplier_)
{
	return true;
}

void APartyProjectCharacter::PickUp()
{
	//logic for picking up the ball will go here
	if (isNearlBall && !CanMove() && !bHasBall)
	{
		bHasBall = true;

		const FTransform socketTransform = GetMesh()->GetSocketTransform("s_weapon", ERelativeTransformSpace::RTS_World);
		AController* c = this->GetController();
		APlayerController* playerC = Cast<APlayerController>(c);
		playerName = GetPlayerState()->GetPlayerName();
		FVector s(0, 0, 0);
		FRotator r;
		FAttachmentTransformRules rules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepRelative, true);

		AController* a = this->GetController();

		FActorSpawnParameters params;
		params.Owner = playerC;
		params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		if (!ballsNearPlayer.IsEmpty())
		{
			float nearestDistanceToPlayer = 99999.0;
			for (size_t i = 0; i < ballsNearPlayer.Num(); i++)
			{
				float x = ballsNearPlayer[i]->GetDistanceTo(this);
				if (x < nearestDistanceToPlayer)
				{
					ballRef = ballsNearPlayer[i];
				}
			}
			//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Purple, TEXT("nearest to player:  " + ballRef->GetName()));
		}

		ballRef->OnPlayerPickUp(playerName);
		ballRef->AttachToComponent(point, rules, "s_weapon");
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("BALLNAME:  " + ballRef->GetName()));

		//FString x = springArmComp->GetChildComponent(0)->GetName();

		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, TEXT("child NAME: " + x));




		if (!HasAuthority()) {
			//Clientill� tapahtuva asia
			Server_PickUp(s, r, ballRef, playerC, playerName);


		}
		else {
			//Serverill� tapahtuva asia
			CallScorerUpdate(playerName);
			PCparams p;
			p.pc = Cast<APlayerController>(Controller);
			gs->ProcessEvent(pickedUpBallFunction, &p);
			Multi_PickUp(s, r, ballRef, playerC, playerName);

		}
	}


}



void APartyProjectCharacter::CallScorerUpdate(const FString& Pname_)
{

	//AGameStateBase* gs = UGameplayStatics::GetGameState(GetWorld());

	UFunction* function = gs->FindFunction("SR_SetScorer");
	Flocalparameters params;

	params.x = Pname_;

	if (IsValid(function) && IsValid(gs))
	{
		gs->ProcessEvent(function, &params);
	}
	else
	{
		if (!IsValid(function))
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("function is not valid"));
		}
		else if (!IsValid(gs))
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("gamestate is not valid"));

		}

	}


}

bool APartyProjectCharacter::Server_PickUp_Validate(FVector location, FRotator rotation, ADodgeBall* passedBallRef, APlayerController* pc_, const FString& Pname_)
{
	return true;
}

void APartyProjectCharacter::Server_PickUp_Implementation(FVector location, FRotator rotation, ADodgeBall* passedBallRef, APlayerController* pc_, const FString& Pname_)
{
	UE_LOG(LogTemp, Warning, TEXT("Server_PickUp_Implementation HAS BEEN CALLED"));

	UE_LOG(LogTemp, Warning, TEXT("Server_ThrowBall_Implementation HAS BEEN CALLED"));
	if (!IsValid(passedBallRef))
	{
		passedBallRef = ballRef;
	}


	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("SERVER PICKUP CALLED"));

	if (IsValid(passedBallRef)) {
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("PNAME SERVER IMPLEMENTAATIOSSA: " + Pname_));

		CallScorerUpdate(Pname_);
		PCparams p;
		p.pc = Cast<APlayerController>(pc_);
		gs->ProcessEvent(pickedUpBallFunction, &p);
		Multi_PickUp(location, rotation, passedBallRef, pc_, Pname_);
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("passedBallRef is null in Server_PickUp_Implementation!"));

		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("BALLREF INVALID IN PICKUP SERVER IMPLEMENTATION"));

	}
}

bool APartyProjectCharacter::Multi_PickUp_Validate(FVector location, FRotator rotation, ADodgeBall* passedBallRef, APlayerController* pc_, const FString& Pname_)
{
	return true;
}

void APartyProjectCharacter::Multi_PickUp_Implementation(FVector location, FRotator rotation, ADodgeBall* passedBallRef, APlayerController* pc_, const FString& Pname_)
{

	UE_LOG(LogTemp, Warning, TEXT("Multi_PickUp_Implementation HAS BEEN CALLED"));
	if (!IsLocallyControlled()) {
		if (IsValid(/*ballRef*/passedBallRef)) {
			if (isNearlBall && !CanMove() && !bHasBall)
			{
				const FTransform socketTransform = GetMesh()->GetSocketTransform("s_weapon", ERelativeTransformSpace::RTS_World);
				bHasBall = true;
				FVector s(0, 0, 0);

				FAttachmentTransformRules rules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepRelative, true);
				FActorSpawnParameters params;
				params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				params.Owner = pc_;

				passedBallRef->AttachToComponent(point, rules, "s_weapon");
				passedBallRef->OnPlayerPickUp(Pname_);

				//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("BALLNAMEMULTI:  " + passedBallRef->GetName()));



			}
			/*	else if (bHasBall)
				{
					ThrowBall(passedBallRef);
				}*/
		}
		else {
			UE_LOG(LogTemp, Error, TEXT("passedBallRef is null in Multi_PickUp_Implementation!"));
		}
	}
}

void APartyProjectCharacter::ThrowBall(ADodgeBall* passedBallRef)
{

	if (Controller != nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("hascontroller"));
		Controller->GetCharacter()->GetCharacterMovement()->bOrientRotationToMovement = true;
	}



	isAiming = false;
	FDetachmentTransformRules::KeepRelativeTransform;
	FVector fVec = GetActorForwardVector();


	if (gamepadActive) {
		FVector characterLoc = GetActorLocation();
		FVector charForwardVector = GetActorForwardVector();
		float distanceAhead = 500.0f;
		FVector newBallPos = characterLoc + charForwardVector * distanceAhead;
		passedBallRef->SetActorLocation(newBallPos);
		passedBallRef->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
		passedBallRef->GetThrown(fVec, chargeTime, GetActorLocation());
		
	}
	else {

		APlayerController* playerController = GetWorld()->GetFirstPlayerController();
		FTransform ballLoc = passedBallRef->GetActorTransform();
		FVector BallLocation = ballLoc.GetLocation();
		FVector2D ballInScreen;
		playerController->ProjectWorldLocationToScreen(BallLocation, ballInScreen);

		float distance = 10000.0f;

		FHitResult HitResult;
		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(playerController->GetPawn());
		bool bhit = playerController->GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Visibility), true, HitResult);
		

		
		
	
		if (bhit) {
			
			FVector HitLocation = HitResult.Location;
			HitLocation.Z = 0.0f;
			fVec = HitLocation - BallLocation;
			fVec.Normalize();
			
		}
		
		passedBallRef->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
		passedBallRef->GetThrown(fVec, chargeTime, GetActorLocation());



	}
	
		bHasBall = false;
	bchargeStarted = false;
	ResetAndHideIndicator();
	APlayerController* pc = Cast<APlayerController>(Controller);

	if (ballsNearPlayer.Contains(passedBallRef))
	{
		ballsNearPlayer.Remove(passedBallRef);
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, TEXT("REMOVEDBALLREF: " + passedBallRef->GetName()));
		for (size_t i = 0; i < ballsNearPlayer.Num(); i++)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Purple, TEXT("Balls near player: " + ballsNearPlayer[i]->GetName()));
		}
	}

	if (!HasAuthority()) {
		//Clientill� tapahtuva asia
		Server_ThrowBall(fVec, passedBallRef, chargeTime, pc);
	}
	else {
		//Serverill� tapahtuva asia
		PCparams params;
		params.pc = Cast<APlayerController>(Controller);
		gs->ProcessEvent(playerThrewFunction, &params);
		Multi_ThrowBall(fVec, passedBallRef, chargeTime);
	}
}

bool APartyProjectCharacter::Server_ThrowBall_Validate(FVector location, ADodgeBall* passedBallRef, float chargeTime_, APlayerController* pc_)
{
	return true;
}

void APartyProjectCharacter::Server_ThrowBall_Implementation(FVector location, ADodgeBall* passedBallRef, float chargeTime_, APlayerController* pc_)
{
	UE_LOG(LogTemp, Warning, TEXT("Server_ThrowBall_Implementation HAS BEEN CALLED"));
	if (!IsValid(passedBallRef))
	{
		passedBallRef = ballRef;
	}

	if (IsValid(passedBallRef)) {
		PCparams params;
		params.pc = Cast<APlayerController>(pc_);
		gs->ProcessEvent(playerThrewFunction, &params);

		Multi_ThrowBall(location, passedBallRef, chargeTime_);
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("passedBallRef is null in Server_ThrowBall_Implementation!"));
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("BALLREF INVALID"));

	}

}

bool APartyProjectCharacter::Multi_ThrowBall_Validate(FVector location, ADodgeBall* passedBallRef, float chargeTime_)
{
	return true;
}

void APartyProjectCharacter::Multi_ThrowBall_Implementation(FVector location, ADodgeBall* passedBallRef, float chargeTime_)
{
	UE_LOG(LogTemp, Warning, TEXT("Multi_ThrowBall_Implementation HAS BEEN CALLED"));

	if (!IsLocallyControlled()) {
		if (IsValid(/*ballRef*/passedBallRef)) {
			FDetachmentTransformRules detachmentRules(EDetachmentRule::KeepRelative, false); // Define the detachment rules.

			passedBallRef->DetachFromActor(detachmentRules);
			passedBallRef->GetThrown(location, chargeTime_, GetActorLocation());

			if (ballsNearPlayer.Contains(passedBallRef))
			{
				ballsNearPlayer.Remove(passedBallRef);
				//	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Purple, TEXT("REMOVEDBALLREF MULTICAST: " + passedBallRef->GetName()));

			}
			//ballRef->DetachFromActor(detachmentRules);
			//ballRef->GetThrown(location);

			bHasBall = false;
		}
		else {
			//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("BALLREF INVALID"));
		}
	}
}



void APartyProjectCharacter::Move(const FInputActionValue& Value)
{
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, TEXT(" MOVE "));

	// input is a Vector2D
	if (!CanMove()) {

		FVector2D MovementVector = Value.Get<FVector2D>();

		if (Controller != nullptr)
		{
			// find out which way is forward
			const FRotator Rotation = Controller->GetControlRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);
			const FRotator nullRotator(0, 0, 0);
			FRotator currentRot = GetActorRotation();
			if (isAiming)
			{
				//Stopping player from turning as they move and setting rotaion rate to 0, because else the Charactermovement component will try forcefully to 
				//turn the character everyframe
				Controller->GetCharacter()->GetCharacterMovement()->bOrientRotationToMovement = false;
				Controller->GetCharacter()->GetCharacterMovement()->RotationRate.Yaw = 0;
				float x = Controller->GetCharacter()->GetCharacterMovement()->RotationRate.Yaw;



			}
			else
			{
				//reseting the values back when not aiming
				Controller->GetCharacter()->GetCharacterMovement()->bOrientRotationToMovement = true;
				Controller->GetCharacter()->GetCharacterMovement()->RotationRate.Yaw = playerBaseRotationRate;

			}


			const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
			// get right vector 
			const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

			AddMovementInput(ForwardDirection, MovementVector.Y);
			AddMovementInput(RightDirection, MovementVector.X);


		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, TEXT(" OH LAWD NULLPOINTR "));

		}
	}
}


void APartyProjectCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}


void APartyProjectCharacter::Direction(const FInputActionValue& Value) {
	//right thumbstick direction
	dashDirection = Value.Get<FVector2D>();
	UE_LOG(LogTemp, Warning, TEXT("Dash Direction: %s"), *dashDirection.ToString());

}


void APartyProjectCharacter::DashMouse()
{
	Dash();
	
}

void APartyProjectCharacter::DashGamepad() {
	Dash();

	
}

void APartyProjectCharacter::Dash() {
	//dashes player towards mouse/gamepad right thumb
	if (!CanDash()) {

		APlayerController* playerController = GetWorld()->GetFirstPlayerController();
		//dash mechanic and replication
		FRotator Rotation = GetActorRotation();
		FVector DashDirection = Rotation.Vector();
		DashDirection.Normalize();
		FVector LaunchVelocity = DashDirection * dashForce;
		GetCharacterMovement()->Launch(LaunchVelocity);
		LastDashTime = GetWorld()->GetTimeSeconds();

		ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
		Flocalparameters params;

		this->ProcessEvent(updateKnockCooldown, 0);

		// Check if the player character is of the correct class (ThirdPersonCharacter).

		CallBlueprintFunction();

		if (!HasAuthority())
		{
			Server_Dash(LaunchVelocity);
		}
		else
		{
			Multi_Dash(LaunchVelocity);
		}
	}
}
void APartyProjectCharacter::CallBlueprintFunction() {
	//call blueprint function for dashcooldown visuals
	Percent = (GetWorld()->GetTimeSeconds() - LastDashTime) / DashCooldownDuration;
	FOutputDeviceNull ar;

	FString PercentString = FString::Printf(TEXT("%f"), Percent);
	FString FunctionCall = FString::Printf(TEXT("Updateper %s,"), *PercentString);
	const TCHAR* params = *FunctionCall;
	this->CallFunctionByNameWithArguments(params, ar, NULL, true);
	//UE_LOG(LogTemp, Warning, TEXT("The value of percent is: %s"), *PercentString);
}



void APartyProjectCharacter::Server_Dash_Implementation(FVector Launch) {

	GetCharacterMovement()->Launch(Launch);

}
void APartyProjectCharacter::Multi_Dash_Implementation(FVector Launch) {

}
bool APartyProjectCharacter::Server_Dash_Validate(FVector Launch)
{
	return true;
}
bool APartyProjectCharacter::Multi_Dash_Validate(FVector Launch) {
	return true;
}


bool APartyProjectCharacter::CanDash() {
	//dash ö to dash again
	if (Ragdolled)
	{
		return true;
	}
	float CurrentTime = GetWorld()->GetTimeSeconds();
	TimeSinceLastDash = CurrentTime - LastDashTime;

	return TimeSinceLastDash < DashCooldownDuration;

}
bool APartyProjectCharacter::CanMove() {
	//Cooldown for moving after using dash
	float CurrentTime = GetWorld()->GetTimeSeconds();
	TimeSinceLastDash = CurrentTime - LastDashTime;
	return TimeSinceLastDash < MovementCooldownDuration;
}

