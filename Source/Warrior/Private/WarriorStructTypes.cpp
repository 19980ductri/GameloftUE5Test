#include "WarriorStructTypes.h"
#include "AbilitySystem/Abilities/WarriorGameplayAbility.h"


bool FWarriorAbilitySet::IsValid() const
{
	return InputTag.IsValid() && AbilityToGrant;
}

