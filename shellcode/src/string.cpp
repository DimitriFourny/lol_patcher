#include "string.h"

typedef unsigned char BYTE;

void* memcpy(void* dst, const void* src, size_t size) {
  BYTE* dst_b = static_cast<BYTE*>(dst);
  const BYTE* src_b = static_cast<const BYTE*>(src);

  while (size > 0) {
    *dst_b++ = *src_b++;
    size--;
  }
  return dst;
}

void* memset(void* ptr, int value, size_t num) {
  unsigned char* data = static_cast<unsigned char*>(ptr);
  for (size_t i = 0; i < num; i++) {
    data[i] = value;
  }
  return ptr;
}

int strcmp(const char* str1, const char* str2) {
  while (*str1 && *str2 && *str1 == *str2) {
    str1++;
    str2++;
  }
  return *str1 - *str2;
}

int tolower2(int character) {
  if (character >= 'A' && character <= 'Z') {
    return character + ('a' - 'A');
  }
  return character;
}

int stricmp(const char* str1, const char* str2) {
  while (*str1 && *str2 && tolower2(*str1) == tolower2(*str2)) {
    str1++;
    str2++;
  }
  return *str1 - *str2;
}

int memcmp(const void* ptr1, const void* ptr2, size_t num) {
  const char* mem1 = static_cast<const char*>(ptr1);
  const char* mem2 = static_cast<const char*>(ptr2);
  while (num && *mem1 == *mem2) {
    mem1++;
    mem2++;
    num--;
  }
  return *mem1 - *mem2;
}

char* strncpy(char* destination, const char* source, size_t num) {
  while (num && *source) {
    *destination++ = *source++;
    num--;
  }
  return destination;
}

size_t strlen(const char* str) {
  size_t len = 0;
  while (*str) {
    str++;
    len++;
  }
  return len;
}

void* memchr(const void* ptr, int c, size_t num) {
  unsigned char* data = const_cast<unsigned char*>(static_cast<const unsigned char*>(ptr));
  while (num) {
    if (*data == static_cast<unsigned char>(c)) {
      return data;
    }
    data++;
    num--;
  }
  return nullptr;
}

void* memmove(void* dest, const void* src, size_t len) {
  char* d = static_cast<char*>(dest);
  const char* s = static_cast<const char*>(src);
  if (d < s) {
    while (len--) {
      *d++ = *s++;
    }
  } else {
    const char* lasts = s + (len - 1);
    char* lastd = d + (len - 1);
    while (len--) {
      *lastd-- = *lasts--;
    }
  }
  return dest;
}

char* itoa(int value, char* out, int base) {
  if (value < 0) {
    out[0] = '-';
  }

  int len = 0;
  int value_copy = value;
  do {
    value_copy /= base;
    len++;
  } while (value_copy);

  out[len] = '\0';

  size_t i = len - 1;
  if (value < 0) {
    value = -value;
    i++;
  }

  do {
    out[i--] = static_cast<char>(value % base) + '0';
    value /= base;
  } while (value);

  return out;
}