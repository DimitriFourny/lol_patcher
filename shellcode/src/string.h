#pragma once
#include <stddef.h>

extern "C" void* memcpy(void* dst, const void* src, size_t size);
extern "C" void* memset(void* ptr, int value, size_t num);
extern "C" int strcmp(const char* str1, const char* str2);
extern "C" int tolower2(int character);
extern "C" int stricmp(const char* str1, const char* str2);
extern "C" int memcmp(const void* ptr1, const void* ptr2, size_t num);
extern "C" char* strncpy(char* destination, const char* source, size_t num);
extern "C" size_t strlen(const char* str);
extern "C" void* memchr(const void* ptr, int c, size_t num);
extern "C" void* memmove(void* dest, const void* src, size_t len);
extern "C" char* itoa(int value, char* out, int base);
