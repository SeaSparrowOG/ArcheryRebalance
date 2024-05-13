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

		if (CrossbowAdjuster::GetSingleton()->Adjust()) {
			_loggerInfo("Finished adjusting crossbows.");
		}
		else {
			_loggerError("Encountered error while adjusting crossbows.");
			return false;
		}

		return true;
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

	bool CrossbowAdjuster::Adjust() {
		return true;
	}
}