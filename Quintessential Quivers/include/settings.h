#pragma once

#include <SimpleIni.h>

/*
This file reads the associated INI, or creates it if not found. 
The ini contains a single group called "settings" and controls
wether or not ammo and ranged weaponry will be adjusted this run.
*/

namespace QQSettings {

	class Settings
	{
	public:
		static bool GetShouldAdjustArrows();
		static bool GetShouldAdjustBolts();
		static bool GetShouldAdjustBows();
		static bool GetShouldAdjustCrossbows();

		static bool INIExists();
		static void CreateINI();
	};
}