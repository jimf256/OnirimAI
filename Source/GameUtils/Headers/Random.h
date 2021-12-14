#pragma once
#include <random>

// -------------------------------------------------------------------------------------------------

namespace Random
{
	int GetSeed();
	void SetSeed(int seed);
	void SetRandomSeed();

	std::mt19937& GetRandomNumberEngine();
}

// -------------------------------------------------------------------------------------------------
