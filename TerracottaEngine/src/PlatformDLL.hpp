#pragma once

// Just declare the functions - implement them in a .cpp file
using DLL_HANDLE = void*;
DLL_HANDLE LoadDLL(const char* path);
void UnloadDLL(DLL_HANDLE dll);
void* GetDLLSymbol(DLL_HANDLE dll, const char* symbol);