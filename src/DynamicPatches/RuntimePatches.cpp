#include "RuntimePatches.h"

#include "Settings/INI/INISettings.h"

namespace RuntimePatches
{

    struct Adjustments
    {
        std::optional<float> boltDamageModifier{};
        std::optional<float> boltSpeedModifier{};
        std::optional<float> arrowDamageModifier{};
        std::optional<float> arrowSpeedModifier{};
    };

    bool PatchItems(const Adjustments& adjustments) {
        REX::INFO("  - Patching runtime data..."sv);
        auto* dh = RE::TESDataHandler::GetSingleton();
        if (!dh) {
            REX::CRITICAL("    >Failed to get the game's data handler. You will likely crash later."sv);
            return false;
        }
        auto& weapons = dh->GetFormArray<RE::TESObjectWEAP>();
        auto& allAmmo = dh->GetFormArray<RE::TESAmmo>();
        if (weapons.empty()) {
            REX::WARN("      >Game's weapon array returned empty - you might crash later."sv);
        }
        if (allAmmo.empty()) {
            REX::WARN("      >Game's ammo array returned empty - you might crash later."sv);
        }

        const bool skipBolts = !adjustments.boltDamageModifier.has_value() &&
                                !adjustments.boltSpeedModifier.has_value();
        const bool skipArrows = !adjustments.arrowDamageModifier.has_value() && 
                                !adjustments.arrowSpeedModifier.has_value();

        for (auto* ammo : allAmmo) {
            if (!ammo || !ammo->GetPlayable()) {
                continue;
            }
            auto* proj = ammo->data.projectile;
            if (!proj) {
                continue;
            }
            const bool isBolt = ammo->IsBolt();
            if (isBolt && !skipBolts) {
                if (adjustments.boltDamageModifier.has_value()) {
                    ammo->data.damage += adjustments.boltDamageModifier.value();
                }
                if (adjustments.boltSpeedModifier.has_value()) {
                    proj->data.speed += adjustments.boltSpeedModifier.value();
                }
            }
            else if (!isBolt && !skipArrows) {
                if (adjustments.arrowDamageModifier.has_value()) {
                    ammo->data.damage += adjustments.arrowDamageModifier.value();
                }
                if (adjustments.arrowSpeedModifier.has_value()) {
                    proj->data.speed += adjustments.arrowSpeedModifier.value();
                }
            }
        }
        return true;
    }

    bool PatchSettings() {
        using Tweak = std::pair<std::string_view, float>;
        constexpr std::array<Tweak, 5> tweaks = {
            Tweak("Combat|f1PArrowTiltUpAngle"sv, 0.2f),
            Tweak("Combat|f1PboltTiltUpAngle"sv, 0.2f),
            Tweak("Combat|f3PArrowTiltUpAngle"sv, 0.7f),
            Tweak("Combat|fMagnetismStrafeHeadingMult"sv, 0.0f),
            Tweak("Combat|fMagnetismLookingMult"sv, 0.0f)
        };

        REX::INFO("  - Patching game settings..."sv);
        auto* gameSettings = RE::GameSettingCollection::GetSingleton();
        if (!gameSettings) {
            REX::CRITICAL("    >Failed to get the game's internal game settings."sv);
            return false;
        }
        for (const auto& tweak : tweaks) {
            REX::INFO("    >Setting {} to {}..."sv, tweak.first, tweak.second);
            auto* setting = gameSettings->GetSetting(tweak.first.data());
            if (!setting) {
                REX::WARN("      - Failed to find the setting."sv);
                continue;
            }
            if (setting->GetType() != RE::Setting::Type::kFloat) {
                REX::WARN("      - Setting is not a float."sv);
                continue;
            }
            setting->data.f = tweak.second;
        }
        return true;
    }

    bool RunPatchers() {
        Adjustments adjustments{};
        float adjustArrowDamageBy = 
            Settings::INI::GetSetting<float>(Settings::INI::ARROWS_ADJUST_DAMAGE.data()).value_or(0.0f);
        float adjustArrowSpeedBy = 
            Settings::INI::GetSetting<float>(Settings::INI::ARROWS_ADJUST_SPEED.data()).value_or(0.0f);
        float adjustBoltDamageBy = 
            Settings::INI::GetSetting<float>(Settings::INI::BOLTS_ADJUST_DAMAGE.data()).value_or(0.0f);
        float adjustBoltSpeedBy = 
            Settings::INI::GetSetting<float>(Settings::INI::BOLTS_ADJUST_SPEED.data()).value_or(0.0f);

        adjustArrowDamageBy = std::clamp(adjustArrowDamageBy, -100.0f, 100.0f);
        adjustBoltDamageBy = std::clamp(adjustBoltDamageBy, -100.0f, 100.0f);
        
        adjustArrowSpeedBy = std::clamp(adjustArrowSpeedBy, -1000.0f, 5000.0f);
        adjustBoltSpeedBy = std::clamp(adjustBoltSpeedBy, -1000.0f, 5000.0f);

        if (adjustBoltDamageBy != 0.0f) {
            adjustments.boltSpeedModifier = adjustBoltDamageBy;
        }
        if (adjustBoltSpeedBy != 0.0f) {
            adjustments.boltSpeedModifier = adjustBoltSpeedBy;
        }
        if (adjustArrowDamageBy != 0.0f) {
            adjustments.boltSpeedModifier = adjustArrowDamageBy;
        }
        if (adjustArrowSpeedBy != 0.0f) {
            adjustments.boltSpeedModifier = adjustArrowSpeedBy;
        }

        bool success = PatchItems(adjustments);
        if (Settings::INI::GetSetting<bool>(Settings::INI::PATCHES_ENFORCE_SETTINGS.data()).value_or(false)) {
            success &= PatchSettings();
        }
        else {
            REX::INFO("  - Skipping ini settings..."sv);
        }
        return success;
    }
}