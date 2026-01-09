#pragma once

using LogFn = void(*)(const char*);
using IsKeyStartPressFn = bool(*)(int);

namespace TerracottaEngine
{
struct EngineAPI
{
	LogFn Log;
	IsKeyStartPressFn IsKeyStartPress;
};
}