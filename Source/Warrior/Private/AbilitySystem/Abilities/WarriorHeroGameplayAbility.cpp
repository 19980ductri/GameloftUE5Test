// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/WarriorHeroGameplayAbility.h"

#include "WarriorGameplayTags.h"
#include "Character/WarriorHeroCharacter.h"
#include "Controllers/WarriorHeroController.h"

AWarriorHeroCharacter* UWarriorHeroGameplayAbility::GetHeroCharacterFromActorInfo()
{
	if (!CachedHeroCharacter.IsValid())
	{
		CachedHeroCharacter	= Cast<AWarriorHeroCharacter>(CurrentActorInfo->AvatarActor);
	}

	return 	CachedHeroCharacter.IsValid() ? CachedHeroCharacter.Get() : nullptr;
}

AWarriorHeroController* UWarriorHeroGameplayAbility::GetHeroControllerFromActorInfo()
{
	if (!CachedHeroController.IsValid())
	{
		CachedHeroController = Cast<AWarriorHeroController>(CurrentActorInfo->PlayerController);
	}

	return 	CachedHeroController.IsValid() ? CachedHeroController.Get() : nullptr;
}

UHeroCombatComponent* UWarriorHeroGameplayAbility::GetHeroCombatComponentFromActorInfo()
{
	return GetHeroCharacterFromActorInfo()->GetHeroCombatComponent();
}

FGameplayEffectSpecHandle UWarriorHeroGameplayAbility::MakeHeroDamageEffectSpecHandle(
	TSubclassOf<UGameplayEffect> EffectClass,
	const float InWeaponBaseDamage,
	const FGameplayTag InCurrentAttackTag,
	const int32 InUsedComboCount)
{
	check(EffectClass);

	FGameplayEffectContextHandle Context = GetWarriorAbilitySystemComponentFromActorInfo()->MakeEffectContext();
	Context.SetAbility(this);
	Context.AddSourceObject(GetAvatarActorFromActorInfo());
	Context.AddInstigator(GetAvatarActorFromActorInfo(), GetAvatarActorFromActorInfo());
	
	FGameplayEffectSpecHandle EffectSpecHandle = GetWarriorAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(EffectClass, GetAbilityLevel(),Context);
	
	EffectSpecHandle.Data->SetSetByCallerMagnitude(WarriorGameplayTags::Shared_SetByCaller_BaseDamage, InWeaponBaseDamage);

	if (InCurrentAttackTag.IsValid())
	{
		EffectSpecHandle.Data->SetSetByCallerMagnitude(InCurrentAttackTag, InUsedComboCount);
	}
	
	return EffectSpecHandle;
}

bool UWarriorHeroGameplayAbility::GetAbilityRemainCooldownByTag(FGameplayTag InCooldownTag, float& TotalCooldownTime,
	float& RemainCooldownTime)
{
	check(InCooldownTag.IsValid());
	FGameplayEffectQuery CooldownQuery = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(InCooldownTag.GetSingleTagContainer());

	TArray<TPair<float, float>> TimeRemainDuration = GetAbilitySystemComponentFromActorInfo()->GetActiveEffectsTimeRemainingAndDuration(CooldownQuery);
	if (!TimeRemainDuration.IsEmpty())
	{
		RemainCooldownTime = TimeRemainDuration[0].Key;
		TotalCooldownTime = TimeRemainDuration[0].Value;
	}
	return RemainCooldownTime > 0.f;
}
