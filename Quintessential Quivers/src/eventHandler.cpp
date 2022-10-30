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

		if (!theActorPtr->GetEquippedObject(false)->As<RE::TESObjectWEAP>()) {

			return RE::BSEventNotifyControl::kContinue;
		}

		RE::TESObjectWEAP* theActorsWeaponPtr = theActorPtr->GetEquippedObject(false)->As<RE::TESObjectWEAP>();

		 nullptr;

		if (!theActorPtr->GetCurrentAmmo()) {

			return RE::BSEventNotifyControl::kContinue;
		}

		RE::TESAmmo* theActorsAmmoPtr = theActorPtr->GetCurrentAmmo();

		//Actor has both a weapon and ammo equipped. Evaluate whether or not they match.
		bool weaponMatchingAmmo = false;

		if (theActorsWeaponPtr->IsBow() && !theActorsAmmoPtr->IsBolt()) {

			weaponMatchingAmmo = true;
		}
		else if (theActorsWeaponPtr->IsCrossbow() && theActorsAmmoPtr->IsBolt()) {

			weaponMatchingAmmo = true;
		}

		if (weaponMatchingAmmo == false) {

			return RE::BSEventNotifyControl::kContinue;
		}

		theActorPtr->AsActorValueOwner()->SetBaseActorValue(RE::ActorValue::kMarksmanModifier, theActorsWeaponPtr->GetAttackDamage() + theActorsAmmoPtr->GetRuntimeData().data.damage);
		theActorPtr->AsActorValueOwner()->SetBaseActorValue(RE::ActorValue::kMarksmanPowerModifier, 0);

		//If the actor is the player, we need to update the damage indicator.
		if (theActorPtr->IsPlayerRef()) {

			RE::PlayerCharacter* thePlayer = RE::PlayerCharacter::GetSingleton();
			float previousBowDamage = theActorsWeaponPtr->GetAttackDamage();
			theActorsWeaponPtr->attackDamage += theActorsAmmoPtr->GetRuntimeData().data.damage;
			float reportedDamage = thePlayer->GetDamage(thePlayer->GetEquippedEntryData(false));
			RE::ConsoleLog::GetSingleton()->Print("Current Damage: %f", reportedDamage);
			theActorsWeaponPtr->attackDamage = previousBowDamage;
		}

		return RE::BSEventNotifyControl::kContinue;
	}
}