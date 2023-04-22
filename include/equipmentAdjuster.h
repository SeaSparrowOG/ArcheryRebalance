#pragma once

namespace Adjuster {

	bool AdjustArrows(bool a_bShouldAdjustSpeed, float a_fNewSpeed);
	bool AdjustBolts(bool a_PierceArmor, bool a_bShouldAdjustSpeed, float a_fNewSpeed);
	bool AdjustBows(bool a_Equalize);
	bool AdjustCrossbows();
}