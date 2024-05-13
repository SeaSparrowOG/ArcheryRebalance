#pragma once

namespace AdjustWeapons {
	/*
	Here for convinience. Adjusts weapons based on settings.  Should be called after settings are initialized
	since that function fills in the classes' attributes.
	*/
	bool AdjustWeapons();

	class BowAdjuster : public clib_util::singleton::ISingleton<BowAdjuster> {
	public:
		bool Adjust();
	};

	class ArrowAdjuster : public clib_util::singleton::ISingleton<ArrowAdjuster> {
	public:
		bool Adjust();
		void UpdateArrowDamageSettings(bool a_adjustArrowDamage, double a_additionalDamage);
		void UpdateArrowSpeedSettings(bool a_adjustArrowSpeed, double a_newArrowSpeed);

	private:
		bool   bBuffArrowDamage;
		bool   bIncreaseArrowSpeed;
		double fAdditionalArrowDamage;
		double fNewArrowSpeed;
	};

	class BoltAdjuster : public clib_util::singleton::ISingleton<BoltAdjuster> {
	public:
		bool Adjust();
		void UpdateBoltDamageSettings(bool a_adjustBoltDamage, double a_additionalDamage);
		void UpdateBoltSpeedSettings(bool a_adjustBoltSpeed, double a_newBoltSpeed);

	private:
		bool   bBuffBoltDamage;
		bool   bIncreaseBoltSpeed;
		double fAdditionalBoltDamage;
		double fNewBoltSpeed;
	};
}