#pragma once

namespace Settings
{
	namespace INI
	{
		bool Read();

		class Holder :
			public REX::TSingleton<Holder>
		{
		public:
			bool StoreSettings();
			void DumpSettings();

			template <typename T>
			std::optional<T> GetStoredSetting(const std::string& a_settingName) {
				if constexpr (std::is_same_v<T, float>) {
					auto it = floatSettings.find(a_settingName);
					if (it != floatSettings.end()) return it->second;
				}
				else if constexpr (std::is_same_v<T, std::string>) {
					auto it = stringSettings.find(a_settingName);
					if (it != stringSettings.end()) return it->second;
				}
				else if constexpr (std::is_same_v<T, long>) {
					auto it = longSettings.find(a_settingName);
					if (it != longSettings.end()) return it->second;
				}
				else if constexpr (std::is_same_v<T, bool>) {
					auto it = boolSettings.find(a_settingName);
					if (it != boolSettings.end()) return it->second;
				}
				else {
					static_assert(always_false<T>, "Called GetStoredSetting with unsupported type.");
				}
				return std::nullopt;
			}

		private:
			std::map<std::string, long>        longSettings;
			std::map<std::string, bool>        boolSettings;
			std::map<std::string, float>       floatSettings;
			std::map<std::string, std::string> stringSettings;

			bool OverrideSettings();
		};

		inline static constexpr const std::uint8_t EXPECTED_COUNT = 11;

		inline static constexpr const std::string_view GENERAL_PLAYER_ONLY = "General|bPlayerOnly"sv;
		inline static constexpr const std::string_view GENERAL_DYNAMIC_BOW_DRAW_SPEED = "General|bAdjustBowDrawSpeed"sv;
		inline static constexpr const std::string_view GENERAL_DYNAMIC_CROSSBOW_DRAW_SPEED = "General|bAdjustCrossbowDrawSpeed"sv;
		inline static constexpr const std::string_view GENERAL_ACCOUNT_CONJURATION = "General|bAccountConjuration"sv;
		inline static constexpr const std::string_view GENERAL_CONJURATION_WEIGHT = "General|fConjurationWeight"sv;

		inline static constexpr const std::string_view BOLTS_PENETRATE_ARMOR = "Bolts|bPenetrateArmor"sv;
		inline static constexpr const std::string_view BOLTS_ADJUST_DAMAGE = "Bolts|fAdjustDamage"sv;
		inline static constexpr const std::string_view BOLTS_ADJUST_SPEED = "Bolts|fAdjustSpeed"sv;

		inline static constexpr const std::string_view ARROWS_ADJUST_DAMAGE = "Arrows|fAdjustDamage"sv;
		inline static constexpr const std::string_view ARROWS_ADJUST_SPEED = "Arrows|fAdjustSpeed"sv;

		inline static constexpr const std::string_view PATCHES_ENFORCE_SETTINGS = "Patches|bEnforceArcherySettings"sv;


		inline static constexpr const std::array<std::string_view, EXPECTED_COUNT> EXPECTED_SETTINGS = {
			GENERAL_PLAYER_ONLY,
			GENERAL_DYNAMIC_BOW_DRAW_SPEED,
			GENERAL_DYNAMIC_CROSSBOW_DRAW_SPEED,
			GENERAL_ACCOUNT_CONJURATION,
			GENERAL_CONJURATION_WEIGHT,

			BOLTS_PENETRATE_ARMOR,
			BOLTS_ADJUST_DAMAGE,
			BOLTS_ADJUST_SPEED,

			ARROWS_ADJUST_DAMAGE,
			ARROWS_ADJUST_SPEED,

			PATCHES_ENFORCE_SETTINGS
		};

		template <typename T>
		std::optional<T> GetSetting(const std::string& a_settingName) {
			static auto* holder = Holder::GetSingleton();
			return holder->GetStoredSetting<T>(a_settingName);
		}
	}
}