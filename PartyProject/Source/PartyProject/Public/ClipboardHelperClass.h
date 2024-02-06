// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "ClipboardHelperClass.generated.h"

/**
 * 
 */
UCLASS()
class UClipboardHelperClass : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Copy Text to Clipboard"), Category = "Clipboard Addon")
	static void CopyTextToClipboard(const FString& TextToCopy);
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Paste Text from Clipboard"), Category = "Clipboard Addon")
	static void PasteTextFromClipboard();
};
