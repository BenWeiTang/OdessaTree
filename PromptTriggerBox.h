// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OdesaTest/Characters/OdessaCharacter.h"
#include "Engine/DataTable.h"
#include "Engine/TriggerBox.h"
#include "PromptTriggerBox.generated.h"

/**
 * 
 */
UENUM(BlueprintType, meta=(Bitflags, UseEnumValuesAsMaskValuesInEditor="true"))
enum class EVesselType : uint8
{
	None     = 0 UMETA(Hidden),
	Robot    = 1 << 0,
	Mushroom = 1 << 1,
	Plant    = 1 << 2,
	Combined = 1 << 3,
};
ENUM_CLASS_FLAGS(EVesselType);

USTRUCT(BlueprintType)
struct FPromptText
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(Bitmask, BitmaskEnum="EVesselType"))
	uint8 TriggerFor;

	// This text comes before the input icon (if it exists).
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText PrefixText;
	
	// This text comes after the input icon (if it exists).
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText SuffixText;

	// Additional image to display on the widget. This will appear on top of the prompt text.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* Image;
};

UENUM(BlueprintType)
enum class EExecutionResult : uint8
{
	True,
	False
};

UCLASS(Blueprintable)
class ODESATEST_API APromptTriggerBox : public ATriggerBox
{
	GENERATED_BODY()

protected:
	// The container of all the dialogues this Prompt Trigger Box has.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FPromptText> PromptTexts;

	// The text that comes before the input icon (if it exits), which is specified in the Input Row 
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FText PrefixText;
	
	// The text that comes after the input icon (if it exits), which is specified in the Input Row 
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FText SuffixText;

	/** The gamepad input that is required to cancel this widget on the screen.
	 * This input should correspond to the Input Action specified in the Input Row
	 * If nothing is specified here, this widget will only be canceled through other mechanisms, such as leaving the trigger box, if applicable.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FKey GamepadCancelInput;

	/** The keyboard/mouse input that is required to cancel this widget on the screen.
	 * This input should correspond to the Input Action specified in the Input Row
	 * If nothing is specified here, this widget will only be canceled through other mechanisms, such as leaving the trigger box, if applicable.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FKey KeyboardMouseCancelInput;

	// Is this Prompt Trigger Box a one-time trigger? If true, this Prompt Trigger Box will only trigger once unless Can Trigger Again Until Action Performed is also true.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsOneTimeTrigger;

	// Can this Prompt Trigger Box continue to trigger until the action (specified in Gamepad Cancel Input or Keyboard Mouse Cancel Input) has been performed?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="bIsOneTimeTrigger"))
	bool bCanTriggerAgainUntilActionPerformed;

	// Will this Prompt Trigger Box be hidden after a certain amount of time?
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bHideLater;

	// Hide this Prompt Trigger Box after the specified amount of time, in seconds.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="bHideLater"))
	float HideAfterSeconds;

	/** Should this Prompt Trigger Box be canceled when the Player exits it?
	 * Note that setting to false can cause widgets to overlap each other in some cases.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bHideWhenExiting = true;

	/** The Row in the Input Action Data Table that determines the icon to be display in this widget.
	 * Note that Input Row does not determine the canceling actions of this Prompt Trigger Box. The Gamepad Cancel Input or the Keyboard Mouse Cancel Input does.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDataTableRowHandle InputRow;

	// The current vessel that is causing this Prompt Trigger Box to trigger
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite)
	EVesselType CurrentCharacterType;

	// Has this Prompt Trigger Box found a compatible Prompt Text that contains Current Character Type?
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite)
	bool bFoundCharacter;

	// The optional Image to be displayed. If specified, the Image will show up above the Prompt Text.
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<UTexture2D> Image;

private:
	// Has the Player performed the Input Action specified in GamepadCancelInput or KeyboardMouseCancelInput?
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(AllowPrivateAccess=true))
	bool bHasPerformedAction;

	// Has this Prompt Trigger Box triggered before?
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(AllowPrivateAccess=true))
	bool bHasTriggeredBefore;

	// The cache of EVesselTypes with their corresponding AOdessaCharacter classes.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(AllowPrivateAccess=true))
	TMap<TSubclassOf<AOdessaCharacter>, EVesselType> CharacterEnumMap;

	// The current widget instance.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(AllowPrivateAccess=true))
	TObjectPtr<UUserWidget> CurrentWidget;
	
protected:
	UFUNCTION()
	virtual void BeginPlay() override;

	// Callback of an event invoked after the characters have finished switching.
	UFUNCTION(BlueprintImplementableEvent)
	void OnFinishedSwitching();

	// Register Current Widget of this Prompt Trigger Box to Odessa Dialogue Subsystem.
	UFUNCTION(BlueprintCallable)
	void RegisterToDialogueSubsystem();
	
	// Unregister Current Widget of this Prompt Trigger Box from Odessa Dialogue Subsystem.
	UFUNCTION(BlueprintCallable)
	void UnregisterFromDialogueSubsystem();

	UFUNCTION(BlueprintImplementableEvent)
	void OnSubsystemClearedWidget();

private:

	// Try to find this Odessa Character in the Character Enum Map. If successful, set Current Character Type to the corresponding type.
	UFUNCTION(BlueprintCallable)
	void TrySetCharacterType(AOdessaCharacter* OdessaCharacter);

	// Try to get the currently possessed Odessa Character inside this Prompt Trigger Box
	UFUNCTION(BlueprintCallable, BlueprintPure = false, meta=(ExpandEnumAsExecs = "Branches", ToolTip="Try getting the Vessel that is in the Collider and is currently being possessed."))
	const AOdessaCharacter* GetPossessedVesselInCollider(EExecutionResult& Branches) const;

	// Does this Prompt Trigger Box contains this type of Vessel?
	UFUNCTION(BlueprintPure)
	static bool ContainsVessel(const EVesselType& Collection, const EVesselType& Vessel);

	/** Check the Player's eligibility of triggering this Prompt Trigger Box
	 * This includes checking whether it is a one-time trigger against the relevant information, and whether Current Active Widget in Odessa Dialogue Subsystem is currently occupied.
	 */
	UFUNCTION(BlueprintCallable, meta=(ExpandEnumAsExecs="Branches"))
	void CheckTriggerEligibility(EExecutionResult& Branches);
};