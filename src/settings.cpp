#include "settings.h"
#include "simpleIni.h"

constexpr auto path = L"Data/SKSE/Plugins/ArcheryRebalance.ini";

namespace ARSettings {

	bool Settings::INIExists()
	{
		std::fstream theINI;
		theINI.open("./Data/SKSE/Plugins/ArcheryRebalance.ini");

		if (theINI.is_open()) {

			theINI.close();
			return true;
		}

		return false;
	}

	bool Settings::CheckINI() {

		CSimpleIniA ini;
		ini.SetUnicode();
		ini.LoadFile(path);

		if (!ini.KeyExists("Settings", "bGetShouldBuffArrows")) {

			return false;
		}

		if (!ini.KeyExists("Settings", "bGetShouldBuffBolts")) {

			return false;
		}

		if (!ini.KeyExists("Settings", "bGetShouldNerfBows")) {

			return false;
		}

		if (!ini.KeyExists("Settings", "bGetShouldEqualizeBows")) {

			return false;
		}

		if (!ini.KeyExists("Settings", "bGetShouldBoltsPierceArmor")) {

			return false;
		}

		if (!ini.KeyExists("Settings", "bShouldAdjustProjectiles")) {

			return false;
		}

		if (!ini.KeyExists("Settings", "bShouldAdjustBowDrawSpeed")) {

			return false;
		}

		if (!ini.KeyExists("Settings", "bQuintessentialQuivers")) {

			return false;
		}

		if (!ini.KeyExists("Settings", "bAdjustArrowSpeed")) {

			return false;
		}

		if (!ini.KeyExists("Settings", "bAdjustBoltSpeed")) {

			return false;
		}

		if (!ini.KeyExists("Settings", "fNewBoltSpeed")) {

			return false;
		}

		if (!ini.KeyExists("Settings", "fNewArrowSpeed")) {

			return false;
		}
		
		return true;
	}

	void Settings::CreateINI() {

		CSimpleIniA ini;
		ini.SetUnicode();
		ini.LoadFile(path);

		ini.Delete("Settings", NULL);
		
		ini.SetBoolValue("Settings", "bGetShouldBuffArrows",       true,  ";Applies a formula to change the overall damage of all arrows in the game.");
		ini.SetBoolValue("Settings", "bAdjustArrowSpeed",          true,  ";If bGetShouldBuffArrows is set to true, buffs the overall arrow speed.");
		ini.SetLongValue("Settings", "fNewArrowSpeed",             3000.0,";If bAdjustArrowSpeed is set to true, this is the new projectile base speed.");
		ini.SetBoolValue("Settings", "bGetShouldBuffBolts",        true,  ";Applies a formula to change the overall damage of all bolts in the game.");
		ini.SetBoolValue("Settings", "bAdjustBoltSpeed",           true,  ";If bGetShouldBuffBolts is set to true, buffs the overall arrow speed.");
		ini.SetLongValue("Settings", "fNewBoltSpeed",              4500.0,";If bAdjustBoltSpeed is set to true, this is the new projectile base speed.");
		ini.SetBoolValue("Settings", "bGetShouldNerfBows",         false, ";Applies a formula to change the overall damage of all bows in the game.");
		ini.SetBoolValue("Settings", "bGetShouldEqualizeBows",     true,  ";Makes the draw speed of all bows in the game 1.0");
		ini.SetBoolValue("Settings", "bGetShouldNerfCrossbows",    false, ";Applies a formula to change the overall damage of all crossbows in the game.");
		ini.SetBoolValue("Settings", "bGetShouldBoltsPierceArmor", true,  ";Bolts will natively ignore armor.");
		ini.SetBoolValue("Settings", "bShouldAdjustProjectiles",   true,  ";The heavier the bow, the faster the projectile will fire.");
		ini.SetBoolValue("Settings", "bShouldAdjustBowDrawSpeed",  true,  ";The heavier the bow, the harder it will be to draw.");
		ini.SetBoolValue("Settings", "bQuintessentialQuivers",     false, ";The original functionality of the mod. Best to leave this off.");
		ini.SaveFile(path);
	}

	bool Settings::CheckBoolSetting(const char* a_key, const char* a_section) {

		CSimpleIniA ini;
		ini.SetUnicode();
		ini.LoadFile(path);

		return ini.GetBoolValue(a_section, a_key, true);
	}

	float Settings::CheckFloatSetting(const char* a_key, const char* a_section) {

		CSimpleIniA ini;
		ini.SetUnicode();
		ini.LoadFile(path);

		return ini.GetLongValue(a_section, a_key, 3000.0);
	}
}