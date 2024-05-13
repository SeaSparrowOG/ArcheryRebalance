#include "eventHandler.h"

namespace EventHandler {
	bool OnEquip::RegisterListener() {
		auto* eventHolder = RE::ScriptEventSourceHolder::GetSingleton();
		if (!eventHolder) return false;

		eventHolder->AddEventSink(this);
		return true;
	}

	RE::BSEventNotifyControl OnEquip::ProcessEvent(const RE::TESEquipEvent* a_event, RE::BSTEventSource<RE::TESEquipEvent>* a_eventSource) {
		if (!a_event || !a_eventSource) return continueEvent;
		if (!a_event->equipped) return continueEvent;

		return continueEvent;
	}
}