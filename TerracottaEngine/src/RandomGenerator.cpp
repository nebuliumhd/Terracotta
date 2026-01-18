#include <ctime>
#include "spdlog/spdlog.h"
#include "RandomGenerator.hpp"

namespace TerracottaEngine
{
RandomGenerator::RandomGenerator(SubsystemManager& subsystemManager, int seed, float frequency) :
	Subsystem(subsystemManager), m_seed(seed), m_frequency(frequency)
{}
RandomGenerator::~RandomGenerator()
{}

bool RandomGenerator::Init()
{
	// No seed set
	if (m_seed == -1) {
		m_randomGen = std::mt19937(static_cast<uint32_t>(std::time(nullptr)));
		m_seed = m_randomGen();
	} else {
		m_randomGen = std::mt19937(m_seed);
	}

	m_noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
	m_noise.SetSeed(m_seed);
	m_noise.SetFrequency(m_frequency);
	
	return true;
}
void RandomGenerator::OnUpdate(const float deltaTime)
{

}
void RandomGenerator::Shutdown()
{
	SPDLOG_INFO("RandomGenerator shutdown complete.");
}

int RandomGenerator::GenerateRandomInt()
{
	return m_randomGen();
}
int RandomGenerator::GenerateRandomInt(int min, int max)
{
	std::uniform_int_distribution<int> dist(min, max);
	return dist(m_randomGen);
}

void RandomGenerator::GetNoise2D(int width, int height, float* outData)
{
	std::vector<float> noise(width * height);
	int i = 0;
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			noise[i++] = m_noise.GetNoise((float)x, (float)y);
		}
	}
	std::memcpy(outData, noise.data(), noise.size() * sizeof(float));
}
} // namespace TerracottaEngine