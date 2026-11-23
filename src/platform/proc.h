#pragma once

#define PROC_ADDRESS(libraryName, procName) \
    GetProcAddress(LoadLibrary(TEXT(libraryName)), procName)

#define LIB_FUNCTION(returnType, libraryName, procName, ...) \
    typedef returnType _##procName(__VA_ARGS__); \
    _##procName* procName = (_##procName*)PROC_ADDRESS(libraryName, #procName);

#define STR(x) #x
