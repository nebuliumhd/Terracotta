#pragma once

using EngineHandle = void*;
using LogFn = void(*)(EngineHandle, const char*);

namespace TerracottaEngine
{
struct EngineAPI
{
	EngineHandle Instance;

	LogFn Log;
};
}