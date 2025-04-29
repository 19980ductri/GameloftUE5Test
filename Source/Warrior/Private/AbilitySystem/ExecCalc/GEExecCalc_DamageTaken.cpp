// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/ExecCalc/GEExecCalc_DamageTaken.h"

#include "DebugHelper.h"
#include "WarriorGameplayTags.h"
#include "AbilitySystem/WarriorAttributeSet.h"


struct FWarriorDamageCapture
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(AttackPower)
	DECLARE_ATTRIBUTE_CAPTUREDEF(DefensePower)
	DECLARE_ATTRIBUTE_CAPTUREDEF(DamageTaken)
	FWarriorDamageCapture()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UWarriorAttributeSet, AttackPower, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UWarriorAttributeSet, DefensePower, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UWarriorAttributeSet, DamageTaken, Target, false);
	}
};

static const FWarriorDamageCapture& GetWarriorDamageCapture()
{
	static FWarriorDamageCapture WarriorDamageCapture;
	return WarriorDamageCapture;
}

UGEExecCalc_DamageTaken::UGEExecCalc_DamageTaken()
{
	/*FProperty* AttackPowerProp = FindFieldChecked<FProperty>(
		UWarriorAttributeSet::StaticClass(),
		GET_MEMBER_NAME_CHECKED(UWarriorAttributeSet, AttackPower)
		);

	FGameplayEffectAttributeCaptureDefinition AttackPowerCaptureDef(
		AttackPowerProp,
		EGameplayEffectAttributeCaptureSource::Source,
		false
		);

	RelevantAttributesToCapture.Add(AttackPowerCaptureDef);*/

	RelevantAttributesToCapture.Add(GetWarriorDamageCapture().AttackPowerDef);
	RelevantAttributesToCapture.Add(GetWarriorDamageCapture().DefensePowerDef);
	RelevantAttributesToCapture.Add(GetWarriorDamageCapture().DamageTakenDef);
	
}

void UGEExecCalc_DamageTaken::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	
	const FGameplayEffectSpec& EffectSpec = ExecutionParams.GetOwningSpec();

	//these all below will be set in MakeHeroDamageEffectSpecHandle();

	/*EffectSpec.GetContext().GetSourceObject();
	EffectSpec.GetContext().GetAbility();
	EffectSpec.GetContext().GetInstigator();
	EffectSpec.GetContext().GetEffectCauser();
	*/
	
	
	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = EffectSpec.CapturedSourceTags.GetAggregatedTags();
	EvaluateParameters.TargetTags = EffectSpec.CapturedTargetTags.GetAggregatedTags();
	
	float SourceAttackPower = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetWarriorDamageCapture().AttackPowerDef, EvaluateParameters, SourceAttackPower);

	/*Debug::Print(TEXT("-------------------------------------------------"));
	Debug::Print(TEXT("Source attack power: "), SourceAttackPower);*/
	
	float BaseDamage = 0.f;
	int32 UsedLightComboCount = 0;
	int32 UsedHeavyComboCount = 0;

	for (const TPair<FGameplayTag, float>& TagMagnitude : EffectSpec.SetByCallerTagMagnitudes)
	{
		if (TagMagnitude.Key.MatchesTagExact(WarriorGameplayTags::Shared_SetByCaller_BaseDamage))
		{
			BaseDamage = TagMagnitude.Value;
		}

		if (TagMagnitude.Key.MatchesTagExact(WarriorGameplayTags::Player_SetByCaller_AttackType_Light))
		{
			UsedLightComboCount = TagMagnitude.Value;
		}

		if (TagMagnitude.Key.MatchesTagExact(WarriorGameplayTags::Player_SetByCaller_AttackType_Heavy))
		{
			UsedHeavyComboCount = TagMagnitude.Value;
		}
	}
	float TargetDefPower = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetWarriorDamageCapture().DefensePowerDef, EvaluateParameters, TargetDefPower);

	/*float DamageTaken = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetWarriorDamageCapture().DamageTakenDef, EvaluateParameters, DamageTaken);*/

	if (UsedLightComboCount != 0)
	{
		const float DamageIncreasePercentLight = (UsedLightComboCount - 1) * .05f + 1.f;
		BaseDamage *= DamageIncreasePercentLight;
	}

	if (UsedHeavyComboCount != 0)
	{
		const float DamageIncreasePercentHeavy = (UsedHeavyComboCount * .15f) + 1.f;
		BaseDamage *= DamageIncreasePercentHeavy;
	}

	const float FinalDamageDone = BaseDamage * SourceAttackPower / TargetDefPower;
	if (FinalDamageDone > 0.f)
	{
		//DamageTaken += FinalDamageDone;
		const FGameplayModifierEvaluatedData EvaluatedData(GetWarriorDamageCapture().DamageTakenProperty, EGameplayModOp::Override, FinalDamageDone);
		OutExecutionOutput.AddOutputModifier(EvaluatedData);
	}

	
}
