#pragma once

/*
This file reads the associated INI, or creates it if not found. 
The ini contains a single group called "settings" and controls
wether or not ammo and ranged weaponry will be adjusted this run.
*/

namespace ARSettings {

	class Settings
	{
	public:
		
		static bool INIExists();
		static bool CheckINI();
		static void CreateINI();
		static bool CheckBoolSetting(const char* a_key, const char* a_section);
		static float CheckFloatSetting(const char* a_key, const char* a_section);
	};
}