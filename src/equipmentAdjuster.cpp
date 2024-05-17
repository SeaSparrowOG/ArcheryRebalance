#include "equipmentAdjuster.h"

namespace AdjustWeapons {
	bool AdjustWeapons() {
		if (ArrowAdjuster::GetSingleton()->Adjust()) {
			_loggerInfo("Finished adjusting arrows.");
		}
		else {
			_loggerError("Encountered error while adjusting arrows.");
			return false;
		}

		if (BoltAdjuster::GetSingleton()->Adjust()) {
			_loggerInfo("Finished adjusting bolts.");
		}
		else {
			_loggerError("Encountered error while adjusting bolts.");
			return false;
		}

		if (BowAdjuster::GetSingleton()->Adjust()) {
			_loggerInfo("Finished adjusting bows.");
		}
		else {
			_loggerError("Encountered error while adjusting bows.");
			return false;
		}

		return true;
	}

	void ArrowAdjuster::UpdateArrowDamageSettings(bool a_adjustArrowDamage, double a_additionalDamage) {
		this->bBuffArrowDamage = a_adjustArrowDamage;

		if (a_additionalDamage >= 0.0f) {
			if (a_additionalDamage > 100.0f) {
				this->fAdditionalArrowDamage = 100.0f;
			}
			else {
				this->fAdditionalArrowDamage = a_additionalDamage;
			}
		}
		else {
			this->fAdditionalArrowDamage = 0.0f;
		}
	}

	void BoltAdjuster::UpdateBoltDamageSettings(bool a_boltsPenetrateArmor, bool a_adjustBoltDamage, double a_additionalDamage) {
		this->bBuffBoltDamage = a_adjustBoltDamage;
		this->bBoltsPenetrateArmor = a_boltsPenetrateArmor;

		if (a_additionalDamage >= 0.0f) {
			if (a_additionalDamage > 100.0f) {
				this->fAdditionalBoltDamage = 100.f;
			}
			else {
				this->fAdditionalBoltDamage = a_additionalDamage;
			}
		}
		else {
			this->fAdditionalBoltDamage = 0.0f;
		}
	}

	void ArrowAdjuster::UpdateArrowSpeedSettings(bool a_adjustArrowSpeed, double a_newArrowSpeed) {
		this->bIncreaseArrowSpeed = a_adjustArrowSpeed;

		if (a_newArrowSpeed > 1000.0f) {
			if (a_newArrowSpeed > 15000.0f) {
				this->fNewArrowSpeed = 15000.0f;
			}
			else {
				this->fNewArrowSpeed = a_newArrowSpeed;
			}
		}
		else {
			this->bIncreaseArrowSpeed = false;
			this->fNewArrowSpeed = 0.0f;
		}
	}

	void BoltAdjuster::UpdateBoltSpeedSettings(bool a_adjustBoltSpeed, double a_newBoltSpeed) {
		this->bIncreaseBoltSpeed = a_adjustBoltSpeed;

		if (a_newBoltSpeed > 1000.0f) {
			if (a_newBoltSpeed > 15000.0f) {
				this->fNewBoltSpeed = 15000.0f;
			}
			else {
				this->fNewBoltSpeed = a_newBoltSpeed;
			}
		}
		else {
			this->bIncreaseBoltSpeed = false;
			this->fNewBoltSpeed = 0.0f;
		}
	}

	bool ArrowAdjuster::Adjust() {
		if (!bBuffArrowDamage && !bIncreaseArrowSpeed) return true;

		const auto& dataHandler = RE::TESDataHandler::GetSingleton();
		std::vector<std::pair<std::string, std::pair<bool, bool>>> adjustedArrows;

		if (!dataHandler) {
			_loggerError("Failed to get the data handler.");
			return false;
		}

		const auto& ammoArray = dataHandler->GetFormArray<RE::TESAmmo>();
		for (auto* ammo : ammoArray) {
			auto& ammoData = ammo->data;
			if (ammo->IsBolt()) continue;
			if (!ammo->GetPlayable()) continue;
			if (ammoData.damage < 1.0f) continue;
			std::string ammoName = ammo->GetName();
			bool bAdjustedSpeed = false;
			bool bAdjustedDamage = false;

			if (this->bIncreaseArrowSpeed) {
				auto* ammoProjectile = ammoData.projectile;
				if (ammoProjectile) {
					ammoProjectile->data.speed = this->fNewArrowSpeed;
					bAdjustedSpeed = true;
				}
			}

			if (this->bBuffArrowDamage) {
				ammoData.damage += this->fAdditionalArrowDamage;
				bAdjustedDamage = true;
			}

			if (!ammoName.empty() && (bAdjustedDamage || bAdjustedSpeed)) {
				std::pair<std::string, std::pair<bool, bool>> newPair;
				newPair.first = ammoName;
				newPair.second.first = bAdjustedSpeed;
				newPair.second.second = bAdjustedDamage;
				adjustedArrows.push_back(newPair);
			}
		}

		if (!adjustedArrows.empty()) {
			_loggerInfo("");
			_loggerInfo("================================================================");
			_loggerInfo("Arrow Adjustment Report:");
			_loggerInfo("New speed: {}.", this->fNewArrowSpeed);
			_loggerInfo("Additional damage: {}.", this->fAdditionalArrowDamage);
			for (auto& arrowInfo : adjustedArrows) {
				_loggerInfo("    >{}:", arrowInfo.first);
				_loggerInfo("        Adjusted Speed: {}, Adjusted Damage: {}.", arrowInfo.second.first, arrowInfo.second.second);
			}
			_loggerInfo("================================================================");
		}
		return true;
	}

	bool BoltAdjuster::Adjust() {
		if (!bBuffBoltDamage && !bIncreaseBoltSpeed && !bBoltsPenetrateArmor) return true;

		const auto& dataHandler = RE::TESDataHandler::GetSingleton();
		std::vector<std::pair<std::string, std::pair<bool, bool>>> adjustedBolts;

		if (!dataHandler) {
			_loggerError("Failed to get the data handler.");
			return false;
		}

		const auto& ammoArray = dataHandler->GetFormArray<RE::TESAmmo>();
		for (auto* ammo : ammoArray) {
			auto& ammoData = ammo->data;
			if (!ammo->IsBolt()) continue;
			if (!ammo->GetPlayable()) continue;
			if (ammoData.damage < 1.0f) continue;
			std::string ammoName = ammo->GetName();
			bool bAdjustedSpeed = false;
			bool bAdjustedDamage = false;

			if (this->bIncreaseBoltSpeed) {
				auto* ammoProjectile = ammoData.projectile;
				if (ammoProjectile) {
					ammoProjectile->data.speed = this->fNewBoltSpeed;
					bAdjustedSpeed = true;
				}
			}

			if (this->bBuffBoltDamage) {
				ammoData.damage += this->fAdditionalBoltDamage;
				bAdjustedDamage = true;
			}

			if (!ammoName.empty() && (bAdjustedDamage || bAdjustedSpeed)) {
				std::pair<std::string, std::pair<bool, bool>> newPair;
				newPair.first = ammoName;
				newPair.second.first = bAdjustedSpeed;
				newPair.second.second = bAdjustedDamage;
				adjustedBolts.push_back(newPair);
			}
		}

		_loggerInfo("");
		_loggerInfo("================================================================");
		_loggerInfo("Bolt Adjustment Report:");
		_loggerInfo("New speed: {}.", this->fNewBoltSpeed);
		_loggerInfo("Additional damage: {}.", this->fAdditionalBoltDamage);

		for (auto& boltInfo : adjustedBolts) {
			_loggerInfo("    >{}:", boltInfo.first);
			_loggerInfo("        Adjusted Speed: {}, Adjusted Damage: {}.", boltInfo.second.first, boltInfo.second.second);
		}
		_loggerInfo("================================================================");
		return true;
	}

	bool BowAdjuster::Adjust() {
		const auto& dataHandler = RE::TESDataHandler::GetSingleton();
		std::vector<std::string> adjustedBows;

		if (!dataHandler) {
			_loggerError("Failed to get the data handler.");
			return false;
		}

		const auto& weaponArray = dataHandler->GetFormArray<RE::TESObjectWEAP>();
		for (auto* weapon : weaponArray) {
			if (!weapon->GetPlayable()) continue;
			if (!weapon->IsBow()) continue;
			std::string bowName = weapon->GetName();
			
			weapon->weaponData.speed = 1.0f;
			if (!bowName.empty()) {
				adjustedBows.push_back(bowName);
			}
		}

		std::sort(adjustedBows.begin(), adjustedBows.end());
		_loggerInfo("");
		_loggerInfo("================================================================");
		_loggerInfo("Bow Adjustment Report:");
		for (auto& bow : adjustedBows) {
			_loggerInfo("    >{}", bow);
		}
		_loggerInfo("================================================================");
		return true;
	}
}