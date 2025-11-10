// GASCharacterBase.cpp

#include "GASCharacterBase.h"
#include "BasicAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Components/CapsuleComponent.h"
#include "PortfolioV3/GASAbilitySystem.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AGASCharacterBase::AGASCharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true; // ensure the actor itself replicates

	// Ability System Component
	AbilitySystemComponent = CreateDefaultSubobject<UGASAbilitySystem>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(ASCReplicationMode);

	// Capsule size
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Use movement-based rotation, not controller yaw
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Character movement tuning (your values kept)
	UCharacterMovementComponent* Move = GetCharacterMovement();
	Move->bOrientRotationToMovement = true;
	Move->RotationRate = FRotator(0.f, 540.f, 0.f);
	Move->JumpZVelocity = 600.f;
	Move->AirControl = 0.2f;
	Move->MaxWalkSpeed = 600.f;
	Move->MinAnalogWalkSpeed = 150.f;
	Move->BrakingDecelerationWalking = 2048.f;
	Move->BrakingDecelerationFalling = 1500.f;

	//Add BAsic Attribute set
	BasicAttributeSet = CreateDefaultSubobject<UBasicAttributeSet>(TEXT("BasicAttributeSet"));
}

void AGASCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	// Safe to call on both server and owning client
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
	}
}

void AGASCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGASCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AGASCharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// Server path: refresh ASC actor info after possession
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
		GrantAbilities(StartingAbilities);
	}
}

void AGASCharacterBase::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// Client path for replicated pawns
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
	}
}

UAbilitySystemComponent* AGASCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

TArray<FGameplayAbilitySpecHandle> AGASCharacterBase::GrantAbilities(TArray<TSubclassOf<UGameplayAbility>> AbilitiesToGrant)
{
	if (!AbilitySystemComponent || !HasAuthority())
	{
		return TArray<FGameplayAbilitySpecHandle>();
	}

	TArray<FGameplayAbilitySpecHandle> AbilityHandles;

	for (TSubclassOf<UGameplayAbility> Ability : AbilitiesToGrant)
	{
		if (Ability)
		{
			FGameplayAbilitySpecHandle SpecHandle = AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(Ability, 1,-1, this));

			AbilityHandles.Add(SpecHandle);
		}
	}
	SendAbilitiesChangedEvent();
	return AbilityHandles;
}

void AGASCharacterBase::RemoveAbilities(TArray<FGameplayAbilitySpecHandle> AbilityHandlesToRemove)
{
	if (!AbilitySystemComponent || !HasAuthority())
	{
		return;
	}

	for (FGameplayAbilitySpecHandle& AbilityHandles : AbilityHandlesToRemove)
	{
		AbilitySystemComponent->ClearAbility(AbilityHandles);
	}
	SendAbilitiesChangedEvent();
}

void AGASCharacterBase::SendAbilitiesChangedEvent()
{
	FGameplayEventData EventData;
	EventData.EventTag = FGameplayTag::RequestGameplayTag(FName("Event.Abilities.Changed"));
	EventData.Instigator = this;
	EventData.Target = this;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, EventData.EventTag, EventData);
}

void AGASCharacterBase::ServerSendGameplayEventToSelf_Implementation(FGameplayEventData EventData)
{
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, EventData.EventTag, EventData);
}
