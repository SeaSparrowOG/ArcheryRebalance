#include "eventHandler.h"

namespace Events {

	OnEquipEventHandler* OnEquipEventHandler::GetSingleton() {

		static OnEquipEventHandler singleton;
		return &singleton;
	}
	
	OnEquipQuintessentialHandler* OnEquipQuintessentialHandler::GetSingleton() {

		static OnEquipQuintessentialHandler singleton;
		return &singleton;
	}

	void OnEquipEventHandler::RegisterListener() {

		RE::ScriptEventSourceHolder* eventHolder = RE::ScriptEventSourceHolder::GetSingleton();
		eventHolder->AddEventSink(OnEquipEventHandler::GetSingleton());

	}

	void OnEquipQuintessentialHandler::RegisterListener() {

		RE::ScriptEventSourceHolder* eventHolder = RE::ScriptEventSourceHolder::GetSingleton();
		eventHolder->AddEventSink(OnEquipQuintessentialHandler::GetSingleton());

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

		if (theActorsWeaponPtr->IsBow() && (theActorsAmmoPtr->GetRuntimeData().data.flags & RE::AMMO_DATA::Flag::kNonBolt)) {

			weaponMatchingAmmo = true;
		}

		if (weaponMatchingAmmo == false) {

			return RE::BSEventNotifyControl::kContinue;
		}

		//Actor equipped bow with matching ammo. Set the new draw speed:


		float fActorsMarksmanSkill = theActorPtr->AsActorValueOwner()->GetActorValue(RE::ActorValue::kArchery);

		if (fActorsMarksmanSkill < 15.0) {

			fActorsMarksmanSkill = 15.0;
		}

		float fActorsBowWeight = theActorsWeaponPtr->GetWeight();

		if (fActorsBowWeight < 5.0) {

			fActorsBowWeight = 5.0;
		}

		if (theActorsWeaponPtr->IsBound()) {

			fActorsBowWeight = 15.0;
		}

		float fArchery = theActorPtr->AsActorValueOwner()->GetActorValue(RE::ActorValue::kArchery);

		theActorsWeaponPtr->weaponData.speed = fArchery/200 + pow(fArchery/100, 2) - (pow(fActorsBowWeight - 15,3)) / 750;

		if (theActorsWeaponPtr->weaponData.speed < 0.4) {

			theActorsWeaponPtr->weaponData.speed = 0.4;
		}

		if (theActorsWeaponPtr->weaponData.speed > 1.5) {

			theActorsWeaponPtr->weaponData.speed = 1.5;
		}

		return RE::BSEventNotifyControl::kContinue;
	}

	RE::BSEventNotifyControl OnEquipQuintessentialHandler::ProcessEvent(const RE::TESEquipEvent* a_event, RE::BSTEventSource<RE::TESEquipEvent>* a_eventSource) {

		if (!a_event || !a_eventSource) {

			if (!a_event->baseObject) {

				return RE::BSEventNotifyControl::kContinue;
			}

			RE::TESForm* theUnequippedObjectPtr = RE::TESObject::LookupByID(a_event->baseObject);

			if (theUnequippedObjectPtr->IsAmmo()) {

				//Ammo unequipped. Check to see if it is a bolt or an arrow.
				RE::TESAmmo* theUnequippedAmmo = theUnequippedObjectPtr->As<RE::TESAmmo>();

				if (theUnequippedAmmo->GetRuntimeData().data.flags & RE::AMMO_DATA::Flag::kNonBolt) {

					//Arrow

					if (!a_event->actor->As<RE::Actor>()) {

						return RE::BSEventNotifyControl::kContinue;
					}

					RE::Actor* theUnequippedActor = a_event->actor->As<RE::Actor>();

					if (!theUnequippedActor->GetEquippedObject(false)) {

						return RE::BSEventNotifyControl::kContinue;
					}

					if (!theUnequippedActor->GetEquippedObject(false)->As<RE::TESObjectWEAP>()) {

						return RE::BSEventNotifyControl::kContinue;
					}

					if (!theUnequippedActor->GetEquippedObject(false)->As<RE::TESObjectWEAP>()->IsBow()) {

						return RE::BSEventNotifyControl::kContinue;
					}

					theUnequippedActor->AsActorValueOwner()->SetBaseActorValue(RE::ActorValue::kMarksmanModifier, theUnequippedActor->AsActorValueOwner()->GetActorValue(RE::ActorValue::kMarksmanModifier) - theUnequippedAmmo->GetRuntimeData().data.damage);
				}
				else {

					//Bolt
					if (!a_event->actor->As<RE::Actor>()) {

						return RE::BSEventNotifyControl::kContinue;
					}

					RE::Actor* theUnequippedActor = a_event->actor->As<RE::Actor>();

					if (!theUnequippedActor->GetEquippedObject(false)) {

						return RE::BSEventNotifyControl::kContinue;
					}

					if (!theUnequippedActor->GetEquippedObject(false)->As<RE::TESObjectWEAP>()) {

						return RE::BSEventNotifyControl::kContinue;
					}

					if (!!theUnequippedActor->GetEquippedObject(false)->As<RE::TESObjectWEAP>()->IsCrossbow()) {

						return RE::BSEventNotifyControl::kContinue;
					}

					theUnequippedActor->AsActorValueOwner()->SetBaseActorValue(RE::ActorValue::kMarksmanModifier, theUnequippedActor->AsActorValueOwner()->GetActorValue(RE::ActorValue::kMarksmanModifier) - theUnequippedAmmo->GetRuntimeData().data.damage);
				}
			}
			else if (theUnequippedObjectPtr->IsWeapon()) {

				//Weapon Unequipped. Check if it is a bow or a crossbow.
				RE::TESObjectWEAP* theUnequippedWeapon = theUnequippedObjectPtr->As<RE::TESObjectWEAP>();

				if (theUnequippedWeapon->IsBow()) {

					//Unequipped bow.
					if (!a_event->actor->As<RE::Actor>()) {

						return RE::BSEventNotifyControl::kContinue;
					}

					RE::Actor* theUnequippedActor = a_event->actor->As<RE::Actor>();

					if (!theUnequippedActor->GetCurrentAmmo()) {

						return RE::BSEventNotifyControl::kContinue;
					}

					RE::TESAmmo* theUnequippedAmmo = theUnequippedActor->GetCurrentAmmo();

					if (!(theUnequippedAmmo->GetRuntimeData().data.flags & RE::AMMO_DATA::Flag::kNonBolt)) {

						return RE::BSEventNotifyControl::kContinue;
					}

					theUnequippedActor->AsActorValueOwner()->SetBaseActorValue(RE::ActorValue::kMarksmanModifier, theUnequippedActor->AsActorValueOwner()->GetActorValue(RE::ActorValue::kMarksmanModifier) - theUnequippedAmmo->GetRuntimeData().data.damage);
				}
				else if (theUnequippedWeapon->IsCrossbow()) {

					//Unequipped crossbow.
					if (!a_event->actor->As<RE::Actor>()) {

						return RE::BSEventNotifyControl::kContinue;
					}

					RE::Actor* theUnequippedActor = a_event->actor->As<RE::Actor>();

					if (!theUnequippedActor->GetCurrentAmmo()) {

						return RE::BSEventNotifyControl::kContinue;
					}

					RE::TESAmmo* theUnequippedAmmo = theUnequippedActor->GetCurrentAmmo();

					if (theUnequippedAmmo->GetRuntimeData().data.flags & RE::AMMO_DATA::Flag::kNonBolt) {

						return RE::BSEventNotifyControl::kContinue;
					}

					theUnequippedActor->AsActorValueOwner()->SetBaseActorValue(RE::ActorValue::kMarksmanModifier, theUnequippedActor->AsActorValueOwner()->GetActorValue(RE::ActorValue::kMarksmanModifier) - theUnequippedAmmo->GetRuntimeData().data.damage);
				}
			}

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

		nullptr;

		if (!theActorPtr->GetCurrentAmmo()) {

			return RE::BSEventNotifyControl::kContinue;
		}

		RE::TESAmmo* theActorsAmmoPtr = theActorPtr->GetCurrentAmmo();

		//Actor has both a weapon and ammo equipped. Evaluate whether or not they match.
		bool weaponMatchingAmmo = false;

		if (theActorsWeaponPtr->IsBow() && (theActorsAmmoPtr->GetRuntimeData().data.flags & RE::AMMO_DATA::Flag::kNonBolt)) {

			weaponMatchingAmmo = true;
		}
		else if (theActorsWeaponPtr->IsCrossbow() && !(theActorsAmmoPtr->GetRuntimeData().data.flags & RE::AMMO_DATA::Flag::kNonBolt)) {

			weaponMatchingAmmo = true;
		}

		if (weaponMatchingAmmo == false) {

			return RE::BSEventNotifyControl::kContinue;
		}

		theActorPtr->AsActorValueOwner()->SetBaseActorValue(RE::ActorValue::kMarksmanModifier, theActorsWeaponPtr->GetAttackDamage() + theActorsAmmoPtr->GetRuntimeData().data.damage);
		theActorPtr->AsActorValueOwner()->SetBaseActorValue(RE::ActorValue::kMarksmanPowerModifier, theActorPtr->AsActorValueOwner()->GetActorValue(RE::ActorValue::kMarksmanModifier) - theActorsAmmoPtr->GetRuntimeData().data.damage);

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