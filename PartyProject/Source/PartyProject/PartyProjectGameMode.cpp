// Copyright Epic Games, Inc. All Rights Reserved.

#include "PartyProjectGameMode.h"
#include "PartyProjectCharacter.h"
#include "Kismet/GameplayStatics.h"

#include "UObject/ConstructorHelpers.h"

APartyProjectGameMode::APartyProjectGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	//if (PlayerPawnBPClass.Class != NULL)
	//{
	//	//DefaultPawnClass = PlayerPawnBPClass.Class;

	//}

}


//}
//
//void APartyProjectGameMode::BeginPlay()
//{
//	
//
//}
//
//void APartyProjectGameMode::PlayerConnect(APartyProjectCharacter* newPlayer)
//{
//	APlayerController* newPlayercontroller = newPlayer->GetController<APlayerController>();
//
//	allPlayerControllers.Add(newPlayercontroller);
//	
//
//
//
//}