// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Public/DodgeBall.h"
#include "Components/WidgetComponent.h"
#include "PartyProjectCharacter.generated.h"





UCLASS(config=Game)
class APartyProjectCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	
	/*
	UPROPERTY(Replicated)
	ADodgeBall* ballRef;
	*/
	/*
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UEnhancedInputComponent* inputsystem;*/




	UFUNCTION()
	void OnOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnOverlapEnded(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	


public:
	APartyProjectCharacter();
	void CallBlueprintFunction();

	UPROPERTY(Replicated)
	UStaticMeshComponent* point;

	UPROPERTY()
	float chargeTime;

	UFUNCTION()
	void ThrowChargeStarted();
	UFUNCTION()
	void SendCurrentTimeToWidget(float currentTime_);
	UFUNCTION()
	void ThrowChargeStopped();

	UFUNCTION()
	void ResetAndHideIndicator();

	UFUNCTION(BlueprintCallable)
	void ThrowBall(ADodgeBall* passedBallRef);

	UPROPERTY(EditAnywhere, category = "Blueprint class");
	AActor* blueprintActor;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, category = "Rotation")
	float playerBaseRotationRate;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, category = "Rotation")
	float rotSpeedMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, category = "Dash")
	float dashForce;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, category = "Dash")
	float DashCooldownDuration;
	float LastDashTime;
	float TimeSinceLastDash;
	FVector2D dashDirection;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, category = "Dash")
	float MovementCooldownDuration;
	float Percent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Dash")
	bool Ragdolled = false;
	


	UPROPERTY(Replicated)
	FString name;
	///////*UPROPERTY(EditAnywhere, BlueprintReadOnly, category = "Dodge")
	//////float dodgeForce;
	//////UPROPERTY(EditAnywhere, BlueprintReadOnly, category = "Dodge")
	//////float MovementCooldownDurationDodge;
	//////float dodgeCooldownDuration;
	//////float LastDodgeTime;
	//////float TimeSinceLastDodge;*/
	UPROPERTY(Replicated, BlueprintReadWrite)
	ADodgeBall* ballRef;


	//UPROPERTY(Replicated)
	UPROPERTY(Replicated,EditAnywhere, BlueprintReadWrite);
	bool bHasBall;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool gamepadActive = false;

	UPROPERTY(Replicated)
	FVector LTmouseLoc;

	UPROPERTY(Replicated)
	FVector LTmouseDir;

	UPROPERTY(Replicated)
	FVector LTimpactLoc;

	UPROPERTY(Replicated)
	FRotator LTwantedRot;
	UPROPERTY(Replicated)
	int LTrotationMultiplier;
	
	UPROPERTY(Replicated)
	FRotator LTplayerCurrentRotation;

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_PickUp(FVector location, FRotator rotation, ADodgeBall* passedBallRef, APlayerController* pc_, const FString& Pname_);
	bool Server_PickUp_Validate(FVector location, FRotator rotation, ADodgeBall* passedBallRef, APlayerController* pc_, const FString& Pname_/*, FAttachmentTransformRules transformRules*/);
	void Server_PickUp_Implementation(FVector location, FRotator rotation, ADodgeBall* passedBallRef, APlayerController* pc_,  const FString& Pname_  /*, FAttachmentTransformRules transformRules*/);

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void Multi_PickUp(FVector location, FRotator rotation, ADodgeBall* passedBallRef, APlayerController* pc_, const FString& Pname_ /*, FAttachmentTransformRules transformRules*/);
	bool Multi_PickUp_Validate(FVector location, FRotator rotation, ADodgeBall* passedBallRef, APlayerController* pc_, const FString& Pname_  /*, FAttachmentTransformRules transformRules*/);
	void Multi_PickUp_Implementation(FVector location, FRotator rotation, ADodgeBall* passedBallRef, APlayerController* pc_, const FString& Pname_  /*, FAttachmentTransformRules transformRules*/);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_ThrowBall(FVector location, ADodgeBall* passedBallRef, float chargeTime_, APlayerController* pc_);
	bool Server_ThrowBall_Validate(FVector location, ADodgeBall* passedBallRef, float chargeTime_, APlayerController* pc_);
	void Server_ThrowBall_Implementation(FVector location, ADodgeBall* passedBallRef, float chargeTime_, APlayerController* pc_);

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void Multi_ThrowBall(FVector location, ADodgeBall* passedBallRef, float chargeTime_);
	bool Multi_ThrowBall_Validate(FVector location, ADodgeBall* passedBallRef, float chargeTime_);
	void Multi_ThrowBall_Implementation(FVector location, ADodgeBall* passedBallRef, float chargeTime_);
	

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_AimBall_Mouse(bool bBall, FRotator wantedRot_);
	bool Server_AimBall_Mouse_Validate(bool bBall, FRotator wantedRot_);
	void Server_AimBall_Mouse_Implementation(bool bBall, FRotator wantedRot_);


	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void Multi_AimBall_Mouse(bool bBall, FRotator wantedRot_);
	bool Multi_AimBall_Mouse_Validate(bool bBall, FRotator wantedRot_);
	void Multi_AimBall_Mouse_Implementation(bool bBall, FRotator wantedRot_);


	UFUNCTION(Server, Reliable, WithValidation)
	void Server_AimBall_Controller(bool bBall, FRotator currentRotation_, FRotator targetRotation_, float rotSpeedMultiplier_ );
	bool Server_AimBall_Controller_Validate(bool bBall, FRotator currentRotation_, FRotator targetRotation_, float rotSpeedMultiplier_);
	void Server_AimBall_Controller_Implementation(bool bBall, FRotator currentRotation_, FRotator targetRotation_, float rotSpeedMultiplier_);

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void Multi_AimBall_Controller(bool bBall, FRotator currentRotation_, FRotator targetRotation_, float rotSpeedMultiplier_);
	bool Multi_AimBall_Controller_Validate(bool bBall, FRotator currentRotation_, FRotator targetRotation_, float rotSpeedMultiplier_);
	void Multi_AimBall_Controller_Implementation(bool bBall, FRotator currentRotation_, FRotator targetRotation_, float rotSpeedMultiplier_);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Dash(FVector Launch);
	bool Server_Dash_Validate(FVector Launch);
	void Server_Dash_Implementation(FVector Launch);
	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void Multi_Dash(FVector Launch);
	bool Multi_Dash_Validate(FVector Launch);
	void Multi_Dash_Implementation(FVector Launch);

	/*UFUNCTION(Server, Reliable, WithValidation)
	void Server_UpdateThrowIndicator(float time_);
	bool Server_UpdateThrowIndicator_Validate(float time_);
	void Server_UpdateThrowIndicator_Implementation(float time_);

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void Multi_UpdateThrowIndicator(float time_);
	bool Multi_UpdateThrowIndicator_Validate(float time_);
	void Multi_UpdateThrowIndicator_Implementation(float time_);*/

	UPROPERTY()
	UFunction* playerThrewFunction;


	UPROPERTY()
	UFunction* pickedUpBallFunction;

	UPROPERTY()
	AGameStateBase* gs;

	//UPROPERTY(EditAnywhere, BlueprintReadOnly, category = "Dash")
	//float dashHeightForce;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enhanced input")
	UInputAction* inputDashMouse;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enhanced input")
	UInputAction* inputDashGamepad;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enhanced input")
	UInputAction* playerThrow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enhanced input")
	UInputAction* inputPickUpAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enhanced input")
	UInputAction* playerMovementAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enhanced input")
	UInputAction* aimAction_controller;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enhanced input")
	UInputAction* aimAction_Mouse;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enhanced input")
	UInputAction* inputDashDirection;
	

	UCapsuleComponent* playerHitBox;

	


	UPROPERTY()
	FString playerName;

	bool isNearlBall;

	//*******variables for throw charging *******

	UPROPERTY()
	bool bchargeStarted;


	UPROPERTY()
	UFunction* updateBlueprintFunction;

	UPROPERTY()
	UFunction* updateKnockCooldown;

	UPROPERTY()
	UFunction* resetIndicatorBlueprintFunction;

	

protected:	

	/** Called for movement input */
	void Move(const FInputActionValue& Value);
	void Direction(const FInputActionValue& Value);
	/** Called for looking input */
	void Look(const FInputActionValue& Value);
	bool isAiming;
	void PickUp();
	void Dash();
	void DashGamepad();
	void DashMouse();
	
	bool CanMove();
	void CallScorerUpdate(const FString& Pname_);
	void AimBall_Controller(const FInputActionValue& Value);
	void AimBall_Mouse(/*const FInputActionValue& Value*/);
	virtual void Tick(float DeltaSeconds) override;
	void PrintPlayerIndex();
	/////////*bool CanDodge();	
	////////void Dodge(float DodgeDirection);*/
	
	UPROPERTY(Replicated)
	TArray<ADodgeBall*> ballsNearPlayer;

	bool hasBall;
	void PlayerNotAiming();
	UFUNCTION(BlueprintCallable)
	bool CanDash();

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// To add mapping context
	virtual void BeginPlay();

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

