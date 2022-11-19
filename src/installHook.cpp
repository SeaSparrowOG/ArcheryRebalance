#include "installHook.h"

namespace Hook {

	struct UpdateCombatThreat
	{
		static void thunk(RE::CombatThreatMap* a_threatMap, RE::Projectile* a_projectile)
		{
			if (a_projectile) {
				const auto base = a_projectile->GetBaseObject();
				const auto projectileBase = base ? base->As<RE::BGSProjectile>() : nullptr;
				const auto weaponSource = a_projectile->GetProjectileRuntimeData().weaponSource;

				if (projectileBase && weaponSource) {

					a_projectile->GetProjectileRuntimeData().linearVelocity *= weaponSource->GetWeight() / 13 + 0.5;
				}
			}

			func(a_threatMap, a_projectile);
		}
		static inline REL::Relocation<decltype(thunk)> func;
	};

	void Install() {

		REL::Module::Runtime runtimeVersion = REL::Module::GetRuntime();
		int offset;

		switch (runtimeVersion) {

		case REL::Module::Runtime::AE:

			offset = 0x79D;
			break;

		case REL::Module::Runtime::SE:

			offset = 0x3CB;
			break;

		case REL::Module::Runtime::VR:

			offset = 0x3A8;
			break;

		default:

			SKSE::stl::report_and_fail("Archery Rebalance: Cannot find Skyrim version. Aborting load...");
			break;

		}

		REL::Relocation<std::uintptr_t> target{ RELOCATION_ID(43030, 44222), REL::VariantOffset(0x3CB, 0x79D, 0x3A8) };

		stl::write_thunk_call<UpdateCombatThreat>(target.address());

		SKSE::log::info("Managed to hook.");
	}
}