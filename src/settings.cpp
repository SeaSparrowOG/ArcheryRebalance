#include "settings.h"
#include "equipmentAdjuster.h"
#include "eventHandler.h"
#include "installHook.h"

namespace {
	bool ShouldRebuildINI(CSimpleIniA* a_ini) {
		const char* generalKeys[] = {
			"bBuffArrowDamage",
			"bBuffBoltDamage",
			"bIncreaseArrowSpeed",
			"bIncreaseBoltSpeed",
			"bAdjustBowDrawSpeed",
			"bAccountConjuration",

			"fAdditionalArrowDamage",
			"fAdditionalBoltDamage",
			"fNewArrowSpeed",
			"fNewBoltSpeed",
			"fConjurationWeight"
		};
		int keyNumber = sizeof(generalKeys) / sizeof(generalKeys[0]);

		std::list<CSimpleIniA::Entry> keyHolder;
		a_ini->GetAllKeys("General", keyHolder);
		if (keyNumber != keyHolder.size()) return true;

		for (auto* key : generalKeys) {
			if (!a_ini->KeyExists("General", key)) return true;
		}
		return false;
	}

	bool ValidateINI() {
		try {
			std::filesystem::path f{ "Data/SKSE/Plugins/ArcheryRebalance.ini" };
			bool createEntries = false;
			if (!std::filesystem::exists(f)) {
				std::fstream createdINI;
				createdINI.open(f, std::ios::out);
				createdINI.close();
				createEntries = true;
			}

			CSimpleIniA ini;
			ini.SetUnicode();
			ini.LoadFile(f.c_str());
			if (!createEntries) { createEntries = ShouldRebuildINI(&ini); }

			if (createEntries) {
				ini.Delete("General", NULL);
				ini.SaveFile(f.c_str());

				ini.SetBoolValue("General", "bBuffArrowDamage", false,
					";Increases arrow damage by a specific amount.");

				ini.SetBoolValue("General", "bBuffBoltDamage", false,
					";Increases bolt damage by a specific amount.");

				ini.SetBoolValue("General", "bIncreaseArrowSpeed", true,
					";Increases arrow speed TO a specific amount.");

				ini.SetBoolValue("General", "bIncreaseBoltSpeed", true,
					";Increases bolt speed TO a specific amount.");

				ini.SetBoolValue("General", "bAdjustBowDrawSpeed", true,
					";Main functionality of the mod, allows for dynamic draw speed based on bow weight and archery skill.");

				ini.SetBoolValue("General", "bAccountConjuration", false,
					";If using a bound weapon, accounts for conjuration skill in addition to archery for dynamic draw speed. No effect if bAdjustBowDrawSpeed is set to false");


				ini.SetDoubleValue("General", "fAdditionalArrowDamage", 0.0f,
					";The amount by which to increase arrow damage. Must be bigger or equal to 0.0. No effect if bBuffArrowDamage is set to false.");

				ini.SetDoubleValue("General", "fAdditionalBoltDamage", 0.0f,
					";The amount by which to increase bolt damage. Must be bigger or equal to 0.0. No effect if bBuffBoltDamage is set to false.");

				ini.SetDoubleValue("General", "fNewArrowSpeed", 4000.0f,
					";The new arrow speed. No effect if bIncreaseArrowSpeed is set to false.");

				ini.SetDoubleValue("General", "fNewBoltSpeed", 7000.0f,
					";The new bolt speed. No effect if bIncreaseBoltSpeed is set to false.");

				ini.SetDoubleValue("General", "fConjurationWeight", 0.6f,
					";The weight given to conjuration when calculating dynamic draw speed. Must be between 0.0 and 1.0, no effect if bAccountConjuration is set to false.");
				ini.SaveFile(f.c_str());
			}
		}
		catch (std::exception e) {
			_loggerError("Critical error while validating INI. Error code: {}", e.what());
			return false;
		}
		return true;
	}
}

namespace Settings {
	bool InitializeSettings() {
		if (!ValidateINI()) return false;
		auto* arrowAdjuster = AdjustWeapons::ArrowAdjuster::GetSingleton();
		auto* boltAdjuster = AdjustWeapons::BoltAdjuster::GetSingleton();
		auto* onEquipListener = EventHandler::OnEquip::GetSingleton();
		auto* OnLoadListener = EventHandler::OnLoad::GetSingleton();

		if (!(arrowAdjuster && boltAdjuster && onEquipListener && OnLoadListener)) {
			_loggerError("Failed to fetch adjuster singletons. I have no idea if this is possible.");
			return false;
		}

		std::filesystem::path f{ "Data/SKSE/Plugins/ArcheryRebalance.ini" };
		CSimpleIniA ini;
		ini.SetUnicode();
		ini.LoadFile(f.c_str());

		bool bBuffArrows = ini.GetBoolValue("General", "bBuffArrowDamage");
		double fAdditionalArrowDamage = ini.GetDoubleValue("General", "fAdditionalArrowDamage");
		bool bAdjustArrowSpeed = ini.GetBoolValue("General", "bIncreaseArrowSpeed");
		double fNewArrowSpeed = ini.GetDoubleValue("General", "fNewArrowSpeed");

		bool bBuffBolts = ini.GetBoolValue("General", "bBuffBoltDamage");
		double fAdditionalBoltDamage = ini.GetDoubleValue("General", "fAdditionalBoltDamage");
		bool bAdjustBoltSpeed = ini.GetBoolValue("General", "bIncreaseBoltSpeed");
		double fNewBoltSpeed = ini.GetDoubleValue("General", "fNewBoltSpeed");

		bool bEnableMainFunctionality = ini.GetBoolValue("General", "bAdjustBowDrawSpeed");
		bool bAccountForConjurationSkill = ini.GetBoolValue("General", "bAccountConjuration");
		double fConjurationSkillWeight = ini.GetDoubleValue("General", "fConjurationWeight");

		onEquipListener->UpdateDrawSpeedSetting(bEnableMainFunctionality, bAccountForConjurationSkill, fConjurationSkillWeight);
		OnLoadListener->UpdateDrawSpeedSetting(bEnableMainFunctionality, bAccountForConjurationSkill, fConjurationSkillWeight);
		boltAdjuster->UpdateBoltSpeedSettings(bAdjustBoltSpeed, fNewBoltSpeed);
		boltAdjuster->UpdateBoltDamageSettings(bBuffBolts, fAdditionalBoltDamage);
		arrowAdjuster->UpdateArrowDamageSettings(bBuffArrows, fAdditionalArrowDamage);
		arrowAdjuster->UpdateArrowSpeedSettings(bAdjustArrowSpeed, fNewArrowSpeed);

		if (!AdjustWeapons::AdjustWeapons()) {
			_loggerError("Failed to adjust weapons.");
			return false;
		}

		onEquipListener->RegisterListener();
		OnLoadListener->RegisterListener();
		Hooks::Install();
		return true;
	}
}