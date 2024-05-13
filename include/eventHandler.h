#pragma once

namespace EventHandler {
#define continueEvent RE::BSEventNotifyControl::kContinue

	/*
	Listens for actors equipping (bows and arrows) or (crossbows and bolts) and reacts appropriately.
	*/
	class OnEquip :
		public RE::BSTEventSink<RE::TESEquipEvent>,
		public clib_util::singleton::ISingleton<OnEquip> {
	public:
		//Registers the listener, so it can actually respond to game events.
		bool RegisterListener();
		void UpdateDrawSpeedSetting(bool a_enableDynamicDraw, bool a_enableConjurationFactor, double a_conjurationWeight);

	private:
		RE::BSEventNotifyControl ProcessEvent(const RE::TESEquipEvent* a_event, RE::BSTEventSource<RE::TESEquipEvent>* a_eventSource) override;

		bool   bAdjustBowDrawSpeed;
		bool   bAccountConjuration;
		double fConjurationWeight;
	};
}