// Fill out your copyright notice in the Description page of Project Settings.


#include "GASAbilitySystem.h"
#include "GASCharacterBase.h"


UGASAbilitySystem::UGASAbilitySystem()
{
	// Optional: enable ticking & replication if you need it
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicated(true);
}


void UGASAbilitySystem::BeginPlay()
{
	Super::BeginPlay();
	// Custom initialization code can go here
}


void UGASAbilitySystem::OnRep_ActivateAbilities()
{
	Super::OnRep_ActivateAbilities();
	AGASCharacterBase *Character = Cast<AGASCharacterBase>(GetOwner());
	if (!Character) return;

	bool bAbilitiesChanged = false;
	if (LastActivationAbilities.Num() != ActivatableAbilities.Items.Num())
	{
		bAbilitiesChanged = true;
	}
	else
	{
		for (int32 i = 0; i < LastActivationAbilities.Num(); i++)
		{
			if (LastActivationAbilities[i].Ability != ActivatableAbilities.Items[i].Ability)
			{
				bAbilitiesChanged = false;
				break;
			}
		}
	}
	
	if (bAbilitiesChanged)
	{
		Character->SendAbilitiesChangedEvent();
		LastActivationAbilities = ActivatableAbilities.Items;
	}
}



void UGASAbilitySystem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	// Custom per-frame update code can go here
}