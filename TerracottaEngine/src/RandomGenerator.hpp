#pragma once
#include <random>
#include <vector>
#include "fastnoiselite/FastNoiseLite.h"
#include "Subsystem.hpp"

namespace TerracottaEngine
{
// Please for the love of god allocate this on the heap
class RandomGenerator : public Subsystem
{
public:
	RandomGenerator(SubsystemManager& subsystemManager, int seed = -1, float frequency = 0.1f);
	~RandomGenerator();

	bool Init();
	void OnUpdate(const float deltaTime);
	void Shutdown();

	int GenerateRandomInt();
	int GenerateRandomInt(int min, int max);

	void GetNoise2D(int width, int length, float* outData);
private:
	std::mt19937 m_randomGen;
	FastNoiseLite m_noise;
	int m_seed;
	float m_frequency;
	std::random_device m_rd;
};
} // namespace TerracottaEngine