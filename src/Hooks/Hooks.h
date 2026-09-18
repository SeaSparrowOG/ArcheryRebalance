#pragma once

namespace Hooks {
	bool Install();

	struct HitDataHook
	{
		inline static void CombatHit(RE::Actor* a_target, RE::HitData* a_hitData);
		inline static REL::Relocation<decltype(CombatHit)> _combatHit;
		inline static bool Install();
	};

	struct GetWeaponSpeed
	{
		inline static float GetProjectileFireSpeed(RE::ActorValueOwner* a_owner, 
											RE::TESObjectWEAP* a_weap,
											bool a_left);
		inline static float GetAnimationSpeed(RE::ActorValueOwner* a_owner, 
											RE::TESObjectWEAP* a_weap,
											bool a_left);
		inline static REL::Relocation<decltype(GetProjectileFireSpeed)> _getProjectileFireSpeed;
		inline static REL::Relocation<decltype(GetAnimationSpeed)> _getAnimationSpeed;
		inline static bool Install();
	};
}