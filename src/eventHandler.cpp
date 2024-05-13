#include "eventHandler.h"

namespace EventHandler {
	bool OnEquip::RegisterListener() {
		if (!this->bAdjustBowDrawSpeed) return true;

		auto* eventHolder = RE::ScriptEventSourceHolder::GetSingleton();
		if (!eventHolder) return false;

		eventHolder->AddEventSink(this);
		return true;
	}

	void OnEquip::UpdateDrawSpeedSetting(bool a_enableDynamicDraw, bool a_enableConjurationFactor, double a_conjurationWeight) {
		this->bAdjustBowDrawSpeed = a_enableDynamicDraw;
		this->bAccountConjuration = a_enableConjurationFactor;

		if (a_conjurationWeight >= 0.0f && a_conjurationWeight <= 1.0f) {
			this->fConjurationWeight = a_conjurationWeight;
		}
		else {
			this->fConjurationWeight = 0.6f;
		}
	}

	RE::BSEventNotifyControl OnEquip::ProcessEvent(const RE::TESEquipEvent* a_event, RE::BSTEventSource<RE::TESEquipEvent>* a_eventSource) {
		if (!a_event || !a_eventSource) return continueEvent;
		if (!a_event->equipped) return continueEvent;

		//Make sure an actor is the event's actor.
		auto* eventReference = a_event->actor.get();
		auto* eventActor = eventReference ? eventReference->As<RE::Actor>() : nullptr;
		if (!eventActor) return continueEvent;

		//Make sure that a weapon or ammo piece was equipped while a weapon is in the right hand.
		auto eventID = a_event->baseObject;
		auto* eventForm = RE::TESForm::LookupByID(eventID);
		auto* eventWeapon = eventForm ? eventForm->As<RE::TESObjectWEAP>() : nullptr;
		auto* eventAmmo = (eventForm && !eventWeapon) ? eventForm->As<RE::TESAmmo>() : nullptr;
		auto* rightEquipData = (eventAmmo || eventWeapon) ? eventActor->GetEquippedEntryData(false) : nullptr;
		if (!rightEquipData) return continueEvent;

		//Make sure that the actor has equipped ammo and either a bow or crossbow equipped.
		auto rightEquipObject = rightEquipData->object;
		auto* rightEquipWeapon = rightEquipObject->As<RE::TESObjectWEAP>();
		bool isBow = rightEquipWeapon ? rightEquipWeapon->IsBow() : false;
		bool isCrossbow = rightEquipWeapon ? rightEquipWeapon->IsCrossbow() : false;
		auto* currentAmmo = (isBow && !isCrossbow) ? eventActor->GetCurrentAmmo() : nullptr;
		if (!currentAmmo) return continueEvent;

		//Make sure that the ammo matches the ranged weapon equipped.
		if ((isBow && currentAmmo->IsBolt())) {
			return continueEvent;
		}

		//Beyond this point, the actor has a bow or crossbow with matching ammo.
		float actorArchery = eventActor->GetActorValue(RE::ActorValue::kArchery);
		float weaponWeight = rightEquipWeapon->GetWeight();

		if (weaponWeight < 5.0f) {
			weaponWeight = 5.0f;
		}
		else if (weaponWeight > 20.0f) {
			weaponWeight = 20.0f;
		}

		if (this->bAccountConjuration &&rightEquipWeapon->IsBound()) {
			float actorConjuration = eventActor->GetActorValue(RE::ActorValue::kConjuration);
			actorArchery = (1.0f - this->fConjurationWeight) * actorArchery + this->fConjurationWeight * actorConjuration;
		}

		//Formula: Skill / (5 * Weapon Weight)
		//Weapon weight is between 5 and 20.
		float weaponSpeedFactor = 0.0f;

		if (weaponSpeedFactor > 1.5f) {
			weaponSpeedFactor = 1.5f;
		}
		else if (weaponSpeedFactor < 0.5f) {
			weaponSpeedFactor = 0.5f;
		}
		
		rightEquipWeapon->weaponData.speed *= weaponSpeedFactor;

		return continueEvent;
	}
}