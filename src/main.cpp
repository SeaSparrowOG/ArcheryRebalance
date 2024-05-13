#include "eventHandler.h"
#include "settings.h"
#include "equipmentAdjuster.h"
#include "installHook.h"

#include <stddef.h>

namespace {
    
    void SetupLog() {
        auto logsFolder = SKSE::log::log_directory();
        if (!logsFolder) SKSE::stl::report_and_fail("SKSE log_directory not provided, logs disabled.");

        auto pluginName = Version::PROJECT;
        auto logFilePath = *logsFolder / std::format("{}.log", pluginName);
        auto fileLoggerPtr = std::make_shared<spdlog::sinks::basic_file_sink_mt>(logFilePath.string(), true);
        auto loggerPtr = std::make_shared<spdlog::logger>("log", std::move(fileLoggerPtr));

        spdlog::set_default_logger(std::move(loggerPtr));
        spdlog::set_level(spdlog::level::info);
        spdlog::flush_on(spdlog::level::info);

        //Pattern
        spdlog::set_pattern("%v");
    }

    
    void InitializeMessaging() {
        if (!SKSE::GetMessagingInterface()->RegisterListener([](SKSE::MessagingInterface::Message* message) {
            switch (message->type) {
                case SKSE::MessagingInterface::kDataLoaded:

                    //In here, do the Arrow/Bolt/Bow/Crossbow changes.
                    if (ARSettings::Settings::CheckBoolSetting("bGetShouldBuffArrows", "Settings")) {

                        bool bShouldAdjustSpeed = ARSettings::Settings::CheckBoolSetting("bAdjustArrowSpeed", "Settings");
                        float fNewArrowSpeed = ARSettings::Settings::CheckFloatSetting("fNewArrowSpeed", "Settings");

                        if (!Adjuster::AdjustArrows(bShouldAdjustSpeed, fNewArrowSpeed)) {
                            SKSE::stl::report_and_fail("Archery Rebalance: Failed to adjust arrows while the setting was turned on in the ini.");
                        }
                    }

                    if (ARSettings::Settings::CheckBoolSetting("bGetShouldBuffBolts", "Settings")) {

                        bool bShouldAdjustBoltSpeed = ARSettings::Settings::CheckBoolSetting("bAdjustBoltSpeed", "Settings");
                        float fNewBoltSpeed = ARSettings::Settings::CheckFloatSetting("fNewBoltSpeed", "Settings");

                        if (!Adjuster::AdjustBolts(ARSettings::Settings::CheckBoolSetting("bGetShouldBoltsPierceArmor", "Settings"), bShouldAdjustBoltSpeed, fNewBoltSpeed)) {

                            SKSE::stl::report_and_fail("Archery Rebalance: Failed to adjust bolts while the setting was turned on in the ini.");
                        }
                    }

                    if (ARSettings::Settings::CheckBoolSetting("bGetShouldNerfBows", "Settings")) {

                        if (!Adjuster::AdjustBows(ARSettings::Settings::CheckBoolSetting("bGetShouldEqualizeBows", "Settings"))) {

                            SKSE::stl::report_and_fail("Archery Rebalance: Failed to adjust bows while the setting was turned on in the ini.");
                        }
                    }

                    break;

                case SKSE::MessagingInterface::kPostLoad:

                    if (ARSettings::Settings::CheckBoolSetting("bShouldAdjustProjectiles", "Settings")) {

                        Hook::Install();
                        SKSE::log::info("Hooked functions.");
                    }
                    break;

            }
        })) {

            SKSE::stl::report_and_fail("Unable to register message listener.");
        }
    }
}

#ifdef SKYRIM_AE
extern "C" DLLEXPORT constinit auto SKSEPlugin_Version = []() {
    SKSE::PluginVersionData v;
    v.PluginVersion({ Version::MAJOR, Version::MINOR, Version::PATCH });
    v.PluginName(Version::PROJECT);
    v.AuthorName(Version::PROJECT_AUTHOR);
    v.UsesAddressLibrary();
    v.UsesUpdatedStructs();
    v.CompatibleVersions({
        SKSE::RUNTIME_1_6_1130,
        _1_6_1170 });
    return v;
    }();
#else 
extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Query(const SKSE::QueryInterface * a_skse, SKSE::PluginInfo * a_info)
{
    a_info->infoVersion = SKSE::PluginInfo::kVersion;
    a_info->name = "EnchantmentArtExtender";
    a_info->version = 1;

    const auto ver = a_skse->RuntimeVersion();
    if (ver
#	ifndef SKYRIMVR
        < SKSE::RUNTIME_1_5_39
#	else
        > SKSE::RUNTIME_VR_1_4_15_1
#	endif
        ) {
        SKSE::log::critical(FMT_STRING("Unsupported runtime version {}"), ver.string());
        return false;
    }

    return true;
}
#endif

extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Load(const SKSE::LoadInterface * a_skse) {
    SetupLog();
    _loggerInfo("Starting up {}.", Version::PROJECT);
    _loggerInfo("Plugin Version: {}.{}.{}", Version::MAJOR, Version::MINOR, Version::PATCH);
    _loggerInfo("Version build:");

#ifdef SKYRIM_AE
    _loggerInfo("    >Latest Version.");
#else
    _loggerInfo("    >1.5 Version. Do not report ANY issues with this version.");
#endif

    if (!ARSettings::Settings::INIExists()) {

        SKSE::log::info("INI not found. Generating...");
        ARSettings::Settings::CreateINI();
    }
    else {

        if (!ARSettings::Settings::CheckINI()) {

            SKSE::log::info("INI found but is incorrect. Rebuilding...");
            ARSettings::Settings::CreateINI();
    }
}

    SKSE::Init(a_skse);
    InitializeMessaging();

    if (ARSettings::Settings::CheckBoolSetting("bShouldAdjustBowDrawSpeed", "Settings")) {

        Events::Register();
    }

    if (ARSettings::Settings::CheckBoolSetting("bQuintessentialQuivers", "Settings")) {

        Events::RegisterQQ();
    }

    return true;
}