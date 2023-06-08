// Fill out your copyright notice in the Description page of Project Settings.


#include "PromptTriggerBox.h"

#include "OdesaTest/GameSystems/OdessaLevelScriptActor.h"
#include "OdesaTest/GameSystems/OdessaGameState.h"
#include "Kismet/GameplayStatics.h"
#include "OdesaTest/GameSystems/OdessaDialogueSubsystem.h"

void APromptTriggerBox::TrySetCharacterType(AOdessaCharacter* OdessaCharacter)
{
	const TSubclassOf<AOdessaCharacter> ClassType = OdessaCharacter->GetClass()->GetSuperClass();
	const EVesselType* VesselType = CharacterEnumMap.Find(ClassType);
	if (!VesselType)
	{
		return;
	}
	
	EVesselType Type = *VesselType;
	
	if (Type == EVesselType::Robot)
	{
		const AOdessaGameState* GameState = Cast<AOdessaGameState>(GetWorld()->GetGameState());
		if (GameState->bAreVesselsCombined)
			Type = EVesselType::Combined;
	}

	CurrentCharacterType = Type;
}

const AOdessaCharacter* APromptTriggerBox::GetPossessedVesselInCollider(EExecutionResult& Branches) const
{
	TArray<AActor*> Actors;
	GetOverlappingActors(Actors, AOdessaCharacter::StaticClass());

	const AActor* PossessedVessel = Cast<AActor>(UGameplayStatics::GetPlayerPawn(this, 0));

	if (!PossessedVessel)
	{
		Branches = EExecutionResult::False;
		return nullptr;
	}

	for (const auto* Actor : Actors)
	{
		if (!Actor)
			continue;
		
		if (Actor == PossessedVessel)
		{
			Branches = EExecutionResult::True;
			return Cast<AOdessaCharacter>(Actor);
		}
	}
	
	Branches = EExecutionResult::False;
	return nullptr;
}

bool APromptTriggerBox::ContainsVessel(const EVesselType& Collection, const EVesselType& Vessel)
{
	return (static_cast<uint8>(Collection) & static_cast<uint8>(Vessel)) == static_cast<uint8>(Vessel);
}

void APromptTriggerBox::CheckTriggerEligibility(EExecutionResult& Branches)
{
	const auto DialogueSubSystem = GetGameInstance()->GetSubsystem<UOdessaDialogueSubsystem>();
	if (!DialogueSubSystem)
	{
		UE_LOG(LogOdessaCharacter, Error, TEXT("Cannot find Dialogue Subsystem."))
		Branches = EExecutionResult::False;
		return;
	}
		
	if (!DialogueSubSystem->IsCurrentActiveWidgetNull())
	{
		UE_LOG(LogOdessaCharacter, Warning, TEXT("A dialogue is already being displayed."))
		Branches = EExecutionResult::False;
		return;
	}
	
	if (!bIsOneTimeTrigger)
	{
		Branches = EExecutionResult::True;
		return;
	}

	if (!bHasTriggeredBefore)
	{
		Branches = EExecutionResult::True;
		return;
	}

	if (bCanTriggerAgainUntilActionPerformed && !bHasPerformedAction)
	{
		Branches = EExecutionResult::True;
		return;
	}

	Branches = EExecutionResult::False;
}

void APromptTriggerBox::BeginPlay()
{
	Super::BeginPlay();
	AOdessaLevelScriptActor* OdessaLevel = Cast<AOdessaLevelScriptActor>(GetWorld()->GetLevelScriptActor());
	if (!OdessaLevel) {
		UE_LOG(LogOdessaCharacter, Error, TEXT("Cannot get the Odessa level script actor"))
		return;
	}
	OdessaLevel->OnFinishedSwitching.AddUObject(this, &APromptTriggerBox::OnFinishedSwitching);
	
	UOdessaDialogueSubsystem* DialogueSubSystem = GetGameInstance()->GetSubsystem<UOdessaDialogueSubsystem>();
	if (!DialogueSubSystem)
	{
		UE_LOG(LogOdessaCharacter, Error, TEXT("Cannot get the Odessa Dialogue Subsystem."))
		return;
	}
	DialogueSubSystem->OnCurrentWidgetCleared.AddUObject(this, &APromptTriggerBox::OnSubsystemClearedWidget);
}

void APromptTriggerBox::RegisterToDialogueSubsystem()
{
	const auto DialogueSubSystem = GetGameInstance()->GetSubsystem<UOdessaDialogueSubsystem>();
	if (!DialogueSubSystem)
	{
		UE_LOG(LogOdessaCharacter, Error, TEXT("Cannot find Dialogue Subsystem."))
		return;
	}
		
	if (!DialogueSubSystem->IsCurrentActiveWidgetNull())
	{
		UE_LOG(LogOdessaCharacter, Warning, TEXT("A dialogue is already being displayed."))
		return;
	}

	DialogueSubSystem->SetCurrentActiveWidget(CurrentWidget);
}

void APromptTriggerBox::UnregisterFromDialogueSubsystem()
{
	const auto DialogueSubSystem = GetGameInstance()->GetSubsystem<UOdessaDialogueSubsystem>();
	if (!DialogueSubSystem)
	{
		UE_LOG(LogOdessaCharacter, Error, TEXT("Cannot find Dialogue Subsystem."))
		return;
	}

	DialogueSubSystem->SetCurrentActiveWidget(nullptr);
}
