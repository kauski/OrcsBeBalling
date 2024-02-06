// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PartyProjectCharacter.h"
#include "PartyProjectGameMode.generated.h"

UCLASS(minimalapi)
class APartyProjectGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	APartyProjectGameMode();
	//virtual void BeginPlay();
	//void PlayerConnect(APartyProjectCharacter* newPlayer);

	//TArray<APlayerController*> allPlayerControllers;

};



