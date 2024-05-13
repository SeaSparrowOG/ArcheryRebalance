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
	private:
	};

	class CrossbowAdjuster : public clib_util::singleton::ISingleton<BowAdjuster> {
	public:
		bool Adjust();
	private:
	};

	class ArrowAdjuster : public clib_util::singleton::ISingleton<BowAdjuster> {
	public:
		bool Adjust();
	private:
	};

	class BoltAdjuster : public clib_util::singleton::ISingleton<BowAdjuster> {
	public:
		bool Adjust();
	private:
	};
}