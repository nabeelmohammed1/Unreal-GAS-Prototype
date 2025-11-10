// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GASAbilitySystem.generated.h"

/**
 * 
 */
UCLASS()
class PORTFOLIOV3_API UGASAbilitySystem : public UAbilitySystemComponent
{
	GENERATED_BODY()

protected:
	TArray<FGameplayAbilitySpec> LastActivationAbilities;
	
public:
	UGASAbilitySystem();

protected:
	virtual void BeginPlay() override;

	void OnRep_ActivateAbilities() override;

public:
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
