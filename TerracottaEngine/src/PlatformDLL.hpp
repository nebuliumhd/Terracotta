#pragma once
#include <string>

namespace TerracottaEngine
{
using DLL_HANDLE = void*;

struct DLLLoadResult {
    DLL_HANDLE Handle;
    std::string TempPath;
};

void* GetDLLSymbol(DLL_HANDLE dll, const char* symbol);
DLLLoadResult LoadDLLForHotReload(const char* originalPath);
void UnloadDLLAndCleanup(DLL_HANDLE dll, const std::string& tempPath);
} // namespace TerracottaEngine