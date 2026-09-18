#pragma once

namespace RE::Offset
{
	namespace Actor
	{
		inline static const auto combatHit = REL::Relocation<std::uintptr_t>(REL::ID(38627), 0x4A8);
	}
	namespace ActorValueOwner
	{
		//1407e46c0
		inline static const auto getWeaponSpeed = REL::Relocation<std::uintptr_t>(REL::ID(44108), 0x5CB);
		inline static const auto getAnimationSpeed = REL::Relocation<std::uintptr_t>(REL::ID(42779), 0x29);
	}
}