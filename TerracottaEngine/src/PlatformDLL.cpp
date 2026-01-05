#include "PlatformDLL.hpp"

#if defined(_WIN32)
    #define WIN32_LEAN_AND_MEAN
    #define NOMINMAX
    #define NOGDI
    #include <Windows.h>
    #undef near
    #undef far

    DLL_HANDLE LoadDLL(const char* path) {
        return (DLL_HANDLE)LoadLibraryA(path);
    }

    void UnloadDLL(DLL_HANDLE dll) {
        FreeLibrary((HMODULE)dll);
    }

    void* GetDLLSymbol(DLL_HANDLE dll, const char* symbol) {
        return (void*)GetProcAddress((HMODULE)dll, symbol);
    }

#elif defined(__APPLE__) || defined(__linux__)
    #include <dlfcn.h>

    DLL_HANDLE LoadDLL(const char* path) {
        return dlopen(path, RTLD_NOW);
    }

    void UnloadDLL(DLL_HANDLE dll) {
        dlclose(dll);
    }

    void* GetDLLSymbol(DLL_HANDLE dll, const char* symbol) {
        return dlsym(dll, symbol);
    }
#endif