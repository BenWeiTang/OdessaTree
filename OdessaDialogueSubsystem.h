// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OdessaDialogueSubsystem.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnCurrentWidgetCleared);

/**
 * 
 */
UCLASS()
class ODESATEST_API UOdessaDialogueSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	// Event called when the Current Active Widget is set to nullptr
	FOnCurrentWidgetCleared OnCurrentWidgetCleared;
	
private:
	// The instance of a widget this Subsystem currently holds.
	UPROPERTY()
	TObjectPtr<UUserWidget> CurrentActiveWidget;
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// Get the widget instance this Subsystem currently holds.
	FORCEINLINE const UUserWidget* GetCurrentActiveWidget() const {return CurrentActiveWidget;}

	// Does this Subsystem currently holds a widget instance?
	UFUNCTION(BlueprintPure)
	FORCEINLINE bool IsCurrentActiveWidgetNull() const {return CurrentActiveWidget == nullptr;}

	// Set the widget instance this Subsystem will hold.
	UFUNCTION()
	void SetCurrentActiveWidget(UUserWidget* Widget);

};
