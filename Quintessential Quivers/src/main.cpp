#include "eventHandler.h"
#include "settings.h"
#include "equipmentAdjuster.h"
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
                    if (QQSettings::Settings::GetShouldAdjustArrows()) {

                        if (!Adjuster::AdjustArrows()) {

                            SKSE::stl::report_and_fail("Quintessential Quivers: Could not adjust arrows while setting was turned on.");
                        }
                    }

                    if (QQSettings::Settings::GetShouldAdjustBolts()) {

                        if (!Adjuster::AdjustBolts()) {

                            SKSE::stl::report_and_fail("Quintessential Quivers: Could not adjust bolts while setting was turned on.");
                        }
                    }

                    if (QQSettings::Settings::GetShouldAdjustBows()) {

                        if (!Adjuster::AdjustBows()) {

                            SKSE::stl::report_and_fail("Quintessential Quivers: Could not adjust bows while setting was turned on.");
                        }
                    }

                    if (QQSettings::Settings::GetShouldAdjustCrossbows()) {

                        if (!Adjuster::AdjustCrossbows()) {

                            SKSE::stl::report_and_fail("Quintessential Quivers: Could not adjust crossbows while setting was turned on.");
                        }
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
    if (!QQSettings::Settings::INIExists()) {

        QQSettings::Settings::CreateINI();
    }

    SKSE::Init(a_skse);
    InitializeMessaging();

    Events::Register(); 

    return true;
}