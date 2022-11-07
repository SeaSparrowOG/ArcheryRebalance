#include "settings.h"
#include "simpleIni.h"

constexpr auto path = L"Data/SKSE/Plugins/QuintessentialQuivers.ini";
namespace QQSettings {
	bool Settings::GetShouldAdjustArrows()
	{

		CSimpleIniA ini;
		ini.SetUnicode();
		ini.LoadFile(path);

		if (ini.GetBoolValue("Settings", "bGetShouldAdjustArrows", true)) {

			return true;
		}

		return false;
	}

	bool Settings::GetShouldAdjustBolts()
	{

		CSimpleIniA ini;
		ini.SetUnicode();
		ini.LoadFile(path);

		if (ini.GetBoolValue("Settings", "bGetShouldAdjustBolts", true)) {

			return true;
		}

		return false;
	}

	bool Settings::GetShouldAdjustBows()
	{

		CSimpleIniA ini;
		ini.SetUnicode();
		ini.LoadFile(path);

		if (ini.GetBoolValue("Settings", "bGetShouldAdjustBows", true)) {

			return true;
		}
		return false;
	}

	bool Settings::GetShouldAdjustCrossbows()
	{

		CSimpleIniA ini;
		ini.SetUnicode();
		ini.LoadFile(path);

		if (ini.GetBoolValue("Settings", "bGetShouldAdjustCrossbows", true)) {

			return true;
		}

		return false;
	}

	bool Settings::INIExists()
	{
		std::fstream theINI;
		theINI.open("./Data/SKSE/Plugins/QuintessentialQuivers.ini");

		if (theINI.is_open()) {

			theINI.close();
			return true;
		}

		return false;
	}

	void Settings::CreateINI() {

		CSimpleIniA ini;
		ini.SetUnicode();
		ini.LoadFile(path);

		ini.Delete("Settings", NULL);
		ini.SetBoolValue("Settings", "bGetShouldAdjustArrows", true, ";If true, Quintessential Quivers will scan through all available arrows and adjust their damage.");
		ini.SetBoolValue("Settings", "bGetShouldAdjustBolts", true, ";If true, Quintessential Quivers will scan through all available bolts and adjust their damage.");
		ini.SetBoolValue("Settings", "bGetShouldAdjustBows", true, ";If true, Quintessential Quivers will scan through all available bows and adjust their damage.");
		ini.SetBoolValue("Settings", "bGetShouldAdjustCrossbows", true, ";If true, Quintessential Quivers will scan through all available crossbows and adjust their damage.");

		ini.SaveFile(path);
	}
}