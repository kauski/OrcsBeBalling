// Fill out your copyright notice in the Description page of Project Settings.


#include "ClipboardHelperClass.h"
#include "Engine/Engine.h"
#include "GenericPlatform/GenericPlatformMisc.h"


void UClipboardHelperClass::CopyTextToClipboard(const FString& TextToCopy)
{
	UE_LOG(LogTemp, Warning, TEXT("CopyTextToClipBoard HAS BEEN CALLED"));

	const TCHAR* TextPtr = *TextToCopy;

	FGenericPlatformMisc::ClipboardCopy(TextPtr);
}

void UClipboardHelperClass::PasteTextFromClipboard()
{
	UE_LOG(LogTemp, Warning, TEXT("PasteTextFromClipBoard HAS BEEN CALLED"));
	FString TextFromClipboard;

	FGenericPlatformMisc::ClipboardPaste(TextFromClipboard);
}
/*
void ClipboardHelperClass::CopyTextToClipboard(const FString& TextToCopy) {
	const TCHAR* TextPtr = *TextToCopy;
	
	if (OpenClipboard(nullptr)) {
		EmptyClipboard();

		HGLOBAL hClipboardData = GlobalAlloc(GMEM_DDESHARE, (TextToCopy.Len() + 1) * sizeof(TCHAR));
		if (hClipboardData) {
			TCHAR* pchData = static_cast<TCHAR*>(GlobalLock(hClipboardData));
			if (pchData) {
				FCString::Strcpy(pchData, TextToCopy.Len() + 1, TextPtr);
				GlobalUnlock(hClipboardData);

				SetClipboardData(CF_UNICODETEXT, hClipboardData);
			}
		}
		CloseClipboard();
	}
}*/