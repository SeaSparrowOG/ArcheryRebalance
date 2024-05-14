#include "installHook.h"

namespace Hooks {

	struct UpdateCombatThreat
	{
		static void thunk(RE::CombatThreatMap* a_threatMap, RE::Projectile* a_projectile)
		{
			if (a_projectile) {
				const auto* base = a_projectile->GetBaseObject();
				const auto* projectileBase = base ? base->As<RE::BGSProjectile>() : nullptr;
				const auto* weaponSource = projectileBase ? a_projectile->weaponSource : nullptr;

				if (weaponSource && weaponSource->IsBow()) {
					float boundMultiplier = 1.0f;
					float weightMultiplier = 1.0f;

					if (weaponSource->IsBound() && weaponSource->GetWeight() < 10.0f) {
						boundMultiplier = 2.0f;
					}

					//Bows are split into 4 categories. Light, medium, fluffy, and DAMN.
					//Light: Weight of 5 or under. Multiplier: 0.3.
					//Medium: Weight of 10 or udner. Multiplier: 0.6.
					//Fluffy: Weight of 15 or udner. Multiplier: 1.0.
					//DAMN: Weight of 20 or udner. Multiplier: 1.3.
					float weaponWeight = weaponSource->GetWeight();
					if (weaponWeight <= 5.0f) {
						weightMultiplier = 0.3f;
					}
					else if (weaponWeight <= 5.0f) {
						weightMultiplier = 0.6f;
					}
					else if (weaponWeight <= 5.0f) {
						weightMultiplier = 1.0f;
					}
					else {
						weightMultiplier = 1.3f;
					}

					weightMultiplier *= boundMultiplier;
					a_projectile->linearVelocity *= weightMultiplier;
				}
			}
			func(a_threatMap, a_projectile);
		}
		static inline REL::Relocation<decltype(thunk)> func;
	};

	void Install() {
		REL::Relocation<std::uintptr_t> target{ RELOCATION_ID(43030, 44222), OFFSET_3(0x3CB, 0x79D, 0x3A8) };
		stl::write_thunk_call<UpdateCombatThreat>(target.address());
		_loggerInfo("Installed hook.");
	}
}