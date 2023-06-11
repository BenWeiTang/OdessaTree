// Fill out your copyright notice in the Description page of Project Settings.


#include "OdessaDialogueSubsystem.h"

void UOdessaDialogueSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UOdessaDialogueSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UOdessaDialogueSubsystem::SetCurrentActiveWidget(UUserWidget* Widget)
{
	CurrentActiveWidget = Widget;
	if (Widget == nullptr) OnCurrentWidgetCleared.Broadcast();
}
