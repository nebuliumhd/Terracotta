#pragma once

using EngineHandle = void*;
using LogFn = void(*)(EngineHandle, const char*);
using IsKeyStartPressFn = bool(*)(EngineHandle, int);

namespace TerracottaEngine
{
struct EngineAPI
{
	EngineHandle Instance;

	LogFn Log;
	IsKeyStartPressFn IsKeyStartPress;
};
}