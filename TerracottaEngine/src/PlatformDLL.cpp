#include <filesystem>
#include <chrono>
#include "PlatformDLL.hpp"
#include "spdlog/spdlog.h"

namespace TerracottaEngine
{
#if defined(_WIN32)
    #define WIN32_LEAN_AND_MEAN
    #define NOMINMAX
    #define NOGDI
    #include <Windows.h>
    #undef near
    #undef far

    static DLL_HANDLE LoadDLL(const char* path)
    {
        return (DLL_HANDLE)LoadLibraryA(path);
    }

    static void UnloadDLL(DLL_HANDLE dll)
    {
        if (dll)
            FreeLibrary((HMODULE)dll);
    }

    void* GetDLLSymbol(DLL_HANDLE dll, const char* symbol)
    {
        return (void*)GetProcAddress((HMODULE)dll, symbol);
    }

#elif defined(__APPLE__) || defined(__linux__)
    #include <dlfcn.h>

    static DLL_HANDLE LoadDLL(const char* path)
    {
        return dlopen(path, RTLD_NOW);
    }

    static void UnloadDLL(DLL_HANDLE dll)
    {
        if (dll)
            dlclose(dll);
    }

    void* GetDLLSymbol(DLL_HANDLE dll, const char* symbol)
    {
        return dlsym(dll, symbol);
    }
#endif

// Cross-platform hot-reload
DLLLoadResult LoadDLLForHotReload(const char* originalPath)
{
    DLLLoadResult result = {nullptr, ""};

    try {
        std::filesystem::path original(originalPath);
        if (!std::filesystem::exists(original)) {
            SPDLOG_ERROR("DLL not found: {}", originalPath);
            return result;
        }

        auto now = std::chrono::system_clock::now();
        auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();

        std::string tempFilename = original.stem().string() + "_" + std::to_string(timestamp) + original.extension().string();
        std::filesystem::path tempPath = original.parent_path() / tempFilename;

        // Copy the DLL
        std::filesystem::copy(original, tempPath, std::filesystem::copy_options::overwrite_existing);

        // Load the copy
        result.Handle = LoadDLL(tempPath.string().c_str());
        if (result.Handle) {
            result.TempPath = tempPath.string();
            SPDLOG_INFO("Loaded DLL copy: {}", result.TempPath);
        } else {
            std::filesystem::remove(tempPath);
            SPDLOG_ERROR("Failed to load DLL copy: {}", tempPath.string());
        }
    } catch (const std::exception& e) {
        SPDLOG_ERROR("Exception during DLL hot-reload: {}", e.what());
    }

    return result;
}

void UnloadDLLAndCleanup(DLL_HANDLE dll, const std::string& tempPath)
{
    // Have to unload DLL first
    UnloadDLL(dll);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    if (!tempPath.empty()) {
        try {
            if (std::filesystem::exists(tempPath)) {
                std::filesystem::remove(tempPath);
                SPDLOG_INFO("Cleaned up temp DLL: {}", tempPath);
            }
        } catch (const std::exception& e) {
            SPDLOG_WARN("Failed to delete temp DLL: {}", e.what());
        }
    }
}
}