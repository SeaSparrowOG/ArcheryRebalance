#include "eventHandler.h"
#include "settings.h"
#include "equipmentAdjuster.h"
#include "installHook.h"

#include <stddef.h>

namespace {
    
    void InitializeLogging() {
        auto path = SKSE::log::log_directory();
        if (!path) {
            SKSE::stl::report_and_fail("Unable to lookup SKSE logs directory.");
        }
        *path /= SKSE::PluginDeclaration::GetSingleton()->GetName();
        *path += L".log";

        std::shared_ptr<spdlog::logger> log;
        if (IsDebuggerPresent()) {
            log = std::make_shared<spdlog::logger>(
                "Global", std::make_shared<spdlog::sinks::msvc_sink_mt>());
        } else {
            log = std::make_shared<spdlog::logger>(
                "Global", std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true));
        }
        log->set_level(spdlog::level::info);
        log->flush_on(spdlog::level::info);

        spdlog::set_default_logger(std::move(log));
        spdlog::set_pattern("[%H:%M:%S.%e]: %v");
    }

    
    void InitializeMessaging() {
        if (!SKSE::GetMessagingInterface()->RegisterListener([](SKSE::MessagingInterface::Message* message) {
            switch (message->type) {
                case SKSE::MessagingInterface::kDataLoaded:

                    //In here, do the Arrow/Bolt/Bow/Crossbow changes.
                    if (ARSettings::Settings::CheckBoolSetting("bGetShouldBuffArrows", "Settings")) {

                        if (!Adjuster::AdjustArrows()) {

                            SKSE::stl::report_and_error("Archery Rebalance: Failed to adjust arrows while the setting was turned on in the ini.");
                        }
                    }

                    if (ARSettings::Settings::CheckBoolSetting("bGetShouldBuffBolts", "Settings")) {

                        if (!Adjuster::AdjustBolts(ARSettings::Settings::CheckBoolSetting("bGetShouldBoltsPierceArmor", "Settings"))) {

                            SKSE::stl::report_and_error("Archery Rebalance: Failed to adjust bolts while the setting was turned on in the ini.");
                        }
                    }

                    if (ARSettings::Settings::CheckBoolSetting("bGetShouldNerfBows", "Settings")) {

                        if (!Adjuster::AdjustBows(ARSettings::Settings::CheckBoolSetting("bGetShouldEqualizeBows", "Settings"))) {

                            SKSE::stl::report_and_error("Archery Rebalance: Failed to adjust bows while the setting was turned on in the ini.");
                        }
                    }

                    if (ARSettings::Settings::CheckBoolSetting("bGetShouldNerfBows", "Settings")) {

                        if (!Adjuster::AdjustCrossbows()) {

                            SKSE::stl::report_and_error("Archery Rebalance: Failed to adjust crossbows while the setting was turned on in the ini.");
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


extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Load(const SKSE::LoadInterface * a_skse)
{
    //Logging and INI setup
    InitializeLogging();
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