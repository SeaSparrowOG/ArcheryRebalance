#include "eventHandler.h"

namespace Events {

	OnEquipEventHandler* OnEquipEventHandler::GetSingleton() {

		static OnEquipEventHandler singleton;
		return &singleton;
	}

	void OnEquipEventHandler::RegisterListener() {

		RE::ScriptEventSourceHolder* eventHolder = RE::ScriptEventSourceHolder::GetSingleton();
		eventHolder->AddEventSink(OnEquipEventHandler::GetSingleton());

	}

	RE::BSEventNotifyControl OnEquipEventHandler::ProcessEvent(const RE::TESEquipEvent* a_event, RE::BSTEventSource<RE::TESEquipEvent>* a_eventSource) {

		if (!a_event || !a_eventSource) {

			return RE::BSEventNotifyControl::kContinue;
		}

		//We're only interested if something was EQUIPPED.
		if (!a_event->equipped) {

			return RE::BSEventNotifyControl::kContinue;
		}

		//And it needs to be a weapon or ammo. Store these variables
		RE::TESForm* theObjectPtr = RE::TESObject::LookupByID(a_event->baseObject);


		if (!theObjectPtr->As<RE::TESObjectWEAP>() && !theObjectPtr->As<RE::TESAmmo>()) {

			return RE::BSEventNotifyControl::kContinue;
		}

		//Ammo or weapon equiped. Evaluate wether we should update the Actor's damage.

		if (!a_event->actor->As<RE::Actor>()) {

			return RE::BSEventNotifyControl::kContinue;
		}

		RE::Actor* theActorPtr = a_event->actor->As<RE::Actor>();

		if (!theActorPtr->GetEquippedObject(false)) {

			return RE::BSEventNotifyControl::kContinue;
		}

		if (!theActorPtr->GetEquippedObject(false)->As<RE::TESObjectWEAP>()) {

			return RE::BSEventNotifyControl::kContinue;
		}

		RE::TESObjectWEAP* theActorsWeaponPtr = theActorPtr->GetEquippedObject(false)->As<RE::TESObjectWEAP>();

		if (!theActorPtr->GetCurrentAmmo()) {

			return RE::BSEventNotifyControl::kContinue;
		}

		RE::TESAmmo* theActorsAmmoPtr = theActorPtr->GetCurrentAmmo();

		//Actor has both a weapon and ammo equipped. Evaluate whether or not they match.
		bool weaponMatchingAmmo = false;

		if (theActorsWeaponPtr->IsBow() && !theActorsAmmoPtr->IsBolt()) {

			weaponMatchingAmmo = true;
		}
		else if (theActorsWeaponPtr->IsCrossbow() && !(theActorsAmmoPtr->GetRuntimeData().data.flags & RE::AMMO_DATA::Flag::kNonBolt)) {

			weaponMatchingAmmo = true;
		}

		if (weaponMatchingAmmo == false) {

			return RE::BSEventNotifyControl::kContinue;
		}

		//Actor equipped bow/crossbow with matching ammo. Set the new draw speed:


		float fActorsMarksmanSkill = theActorPtr->AsActorValueOwner()->GetActorValue(RE::ActorValue::kArchery);

		if (fActorsMarksmanSkill < 15.0) {

			fActorsMarksmanSkill = 15.0;
		}

		float fActorsBowWeight = theActorsWeaponPtr->GetWeight();

		if (fActorsBowWeight < 5.0) {

			fActorsBowWeight = 5.0;
		}


		theActorsWeaponPtr->weaponData.speed = 1.2 - pow(theActorsWeaponPtr->GetWeight() - 4.9, 1.5) / (1.5 * theActorPtr->AsActorValueOwner()->GetActorValue(RE::ActorValue::kArchery));

		if (theActorsWeaponPtr->weaponData.speed < 0.4) {

			theActorsWeaponPtr->weaponData.speed = 0.4;
		}

		return RE::BSEventNotifyControl::kContinue;
	}
}