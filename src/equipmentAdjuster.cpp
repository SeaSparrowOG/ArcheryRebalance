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
				this->fAdditionalArrowDamage = 100.f;
			}
			else {
				this->fAdditionalArrowDamage = a_additionalDamage;
			}
		}
		else {
			this->fAdditionalArrowDamage = 0.0f;
		}
	}

	void BoltAdjuster::UpdateBoltDamageSettings(bool a_adjustBoltDamage, double a_additionalDamage) {
		this->bBuffBoltDamage = a_adjustBoltDamage;

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
		return true;
	}

	bool BoltAdjuster::Adjust() {
		return true;
	}

	bool BowAdjuster::Adjust() {
		return true;
	}
}