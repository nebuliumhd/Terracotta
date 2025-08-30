#pragma once

using EngineHandle = void*;

namespace TerracottaEngine
{
struct EngineAPI
{
	EngineHandle Instance;

	void (*Log)(EngineHandle eng, const char* msg);
};
}