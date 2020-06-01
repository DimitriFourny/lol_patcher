#include "PEHeaders.h"
#include "Hash.h"
#include "string.h"

// static
void* PEHeaders::GetFuncAddr(void* dll, const char* function_name) {
  const DWORD base = reinterpret_cast<DWORD>(dll);
  auto* dos_header = reinterpret_cast<IMAGE_DOS_HEADER*>(base);
  if (dos_header->e_magic != IMAGE_DOS_SIGNATURE) {
    return nullptr;
  }

  auto* nt_headers = reinterpret_cast<IMAGE_NT_HEADERS32*>(base + dos_header->e_lfanew);
  if (nt_headers->Signature != IMAGE_NT_SIGNATURE) {
    return nullptr;
  }

  auto* data_directory = &nt_headers->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
  if (!data_directory->VirtualAddress) {
    return nullptr;
  }

  auto* export_dir = reinterpret_cast<IMAGE_EXPORT_DIRECTORY*>(base + data_directory->VirtualAddress);
  auto* names = reinterpret_cast<DWORD*>(base + export_dir->AddressOfNames);
  auto* names_ordinal = reinterpret_cast<WORD*>(base + export_dir->AddressOfNameOrdinals);
  auto* functions = reinterpret_cast<DWORD*>(base + export_dir->AddressOfFunctions);

  char* name = nullptr;
  for (size_t i = 0; export_dir->NumberOfFunctions; i++) {
    name = reinterpret_cast<char*>(base + names[i]);
    if (stricmp(name, function_name) == 0) {
      return reinterpret_cast<void*>(base + functions[names_ordinal[i]]);
    }
  }

  return nullptr;
}

// static
void* PEHeaders::GetFuncAddr(void* dll, DWORD function_hash) {
  const DWORD base = reinterpret_cast<DWORD>(dll);
  auto* dos_header = reinterpret_cast<IMAGE_DOS_HEADER*>(base);
  if (dos_header->e_magic != IMAGE_DOS_SIGNATURE) {
    return nullptr;
  }

  auto* nt_headers = reinterpret_cast<IMAGE_NT_HEADERS32*>(base + dos_header->e_lfanew);
  if (nt_headers->Signature != IMAGE_NT_SIGNATURE) {
    return nullptr;
  }

  auto* data_directory = &nt_headers->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
  if (!data_directory->VirtualAddress) {
    return nullptr;
  }

  auto* export_dir = reinterpret_cast<IMAGE_EXPORT_DIRECTORY*>(base + data_directory->VirtualAddress);
  auto* names = reinterpret_cast<DWORD*>(base + export_dir->AddressOfNames);
  auto* names_ordinal = reinterpret_cast<WORD*>(base + export_dir->AddressOfNameOrdinals);
  auto* functions = reinterpret_cast<DWORD*>(base + export_dir->AddressOfFunctions);

  char* name = nullptr;
  for (size_t i = 0; export_dir->NumberOfFunctions; i++) {
    name = reinterpret_cast<char*>(base + names[i]);
    if (Hash::Djb(name) == function_hash) {
      return reinterpret_cast<void*>(base + functions[names_ordinal[i]]);
    }
  }

  return nullptr;
}
