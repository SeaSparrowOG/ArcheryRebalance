#include "Hooks.h"

#include "RE/Offset.h"
#include "Settings/INI/INISettings.h"

namespace Hooks {
	bool Install() {
		REX::INFO("Installing hooks..."sv);
		bool success = HitDataHook::Install();
		success &= GetWeaponSpeed::Install();
		return success;
	}

    inline void HitDataHook::CombatHit(RE::Actor *a_target, RE::HitData *a_hitData) {
		if (!a_target || !a_hitData) {
 			_combatHit(a_target, a_hitData);
			return;
		}

		static const bool penetrateArmor = Settings::INI::GetSetting<bool>(Settings::INI::BOLTS_PENETRATE_ARMOR.data()).
									value_or(true);
		static const bool onlyPlayer = Settings::INI::GetSetting<bool>(Settings::INI::GENERAL_PLAYER_ONLY.data()).
									value_or(false);
		if (!penetrateArmor) {
			_combatHit(a_target, a_hitData);
			return;
		}

		const auto* weap = a_hitData->weapon;
		if (!weap || !weap->IsCrossbow()) {
			_combatHit(a_target, a_hitData);
			return;
		}

		const auto& aggressorHandle = a_hitData->aggressor;
		if (onlyPlayer && (!aggressorHandle.get().get() || !aggressorHandle.get()->IsPlayerRef())) {
			_combatHit(a_target, a_hitData);
			return;
		}

		const float blockedDamage = a_hitData->resistedPhysicalDamage;
		a_hitData->totalDamage += blockedDamage;
		a_hitData->physicalDamage += blockedDamage;
		a_hitData->resistedPhysicalDamage = 0.0f;
		a_hitData->percentBlocked = 0.0f;
		_combatHit(a_target, a_hitData);
    }

    inline bool HitDataHook::Install()
    {
        REX::INFO("  - Installing Combat Hit hook..."sv);
		if (!REL::Pattern<"E8">().match(RE::Offset::Actor::combatHit.address())) {
			REX::CRITICAL("    >Failed to validate pattern for the Combat Hit hook."sv);
			return false;
		}

		auto& trampoline = REL::GetTrampoline();
		_combatHit = trampoline.write_call<5>(RE::Offset::Actor::combatHit.address(), &CombatHit);
		return true;
    }

	static float GetFactor(RE::ActorValueOwner* a_owner, RE::TESObjectWEAP* a_weap)
	{
		static const bool playerOnly = Settings::INI::GetSetting<bool>(Settings::INI::GENERAL_PLAYER_ONLY.data()).
										value_or(false);
		static const bool accountForConjuration = Settings::INI::GetSetting<bool>(Settings::INI::GENERAL_ACCOUNT_CONJURATION.data()).
										value_or(true);
		static const float conjurationWeight = std::clamp(Settings::INI::GetSetting<float>(Settings::INI::GENERAL_CONJURATION_WEIGHT.data()).
										value_or(0.6f), 0.0f, 1.0f);

		if (!a_owner || !a_weap) {
			return 1.0f;
		}
		if (!a_weap->IsBow()) {
			return 1.0f;
		}

		const auto* aggressor = skyrim_cast<const RE::Actor*>(a_owner);
		if (!aggressor) {
			return 1.0f;
		}

		const bool isPlayer = aggressor->IsPlayerRef();
		if (playerOnly && !isPlayer) {
			return 1.0f;
		}

		const bool isBound = a_weap->IsBound();
		const float weapWeight = std::clamp(a_weap->GetWeight(), 5.0f, 20.0f);

		float archerySkill = std::clamp(a_owner->GetActorValue(RE::ActorValue::kArchery), 15.0f, 300.0f);
		if (isBound && accountForConjuration) {
			const float conjurationSkill = std::clamp(a_owner->GetActorValue(RE::ActorValue::kConjuration), 15.0f, 300.0f);
			archerySkill = (1.0f - conjurationWeight) * archerySkill + conjurationWeight * conjurationSkill;
		}

		//Formula: Skill / (5 * Weapon Weight)
		//Weapon weight is between 5 and 20.
		return std::clamp(archerySkill / (3.0f * weapWeight), 0.2f, 1.5f);
	}

    inline float GetWeaponSpeed::GetProjectileFireSpeed(RE::ActorValueOwner *a_owner, 
		RE::TESObjectWEAP *a_weap, 
		bool a_left)
    {
		const float speed = _getProjectileFireSpeed(a_owner, a_weap, a_left);
		const float factor = GetFactor(a_owner, a_weap);
		if (factor == 1.0f) {
			return speed;
		}
		
        return speed * factor;
    }

    inline float GetWeaponSpeed::GetAnimationSpeed(RE::ActorValueOwner *a_owner, 
		RE::TESObjectWEAP *a_weap, 
		bool a_left)
    {
        const float speed = _getAnimationSpeed(a_owner, a_weap, a_left);
		const float factor = GetFactor(a_owner, a_weap);
		if (factor == 1.0f) {
			return speed;
		}
		
        return speed * factor;
    }

    inline bool GetWeaponSpeed::Install()
    {
        REX::INFO("  - Installing Weapon Speed hook..."sv);
		if (!REL::Pattern<"E8">().match(RE::Offset::ActorValueOwner::getWeaponSpeed.address())) {
			REX::CRITICAL("    >Failed to validate pattern for the Weapon Speed hook."sv);
			return false;
		}

		auto& trampoline = REL::GetTrampoline();
		_getProjectileFireSpeed = trampoline.write_call<5>(RE::Offset::ActorValueOwner::getWeaponSpeed.address(), &GetProjectileFireSpeed);

		REX::INFO("  - Installing Animation Speed hook..."sv);
		if (!REL::Pattern<"E8">().match(RE::Offset::ActorValueOwner::getAnimationSpeed.address())) {
			REX::CRITICAL("    >Failed to validate pattern for the Animation Speed hook."sv);
			return false;
		}

		_getAnimationSpeed = trampoline.write_call<5>(RE::Offset::ActorValueOwner::getAnimationSpeed.address(), &GetAnimationSpeed);
		return true;
    }
}