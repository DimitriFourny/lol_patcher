#include "Shellcode.h"
#define NO_DEBUG
#include <windows.h>
#include "Debug.h"

#define SHELLCODE_DLL L"shellcode.dll"
#define ADDR_IS_BTW(addr, down, up) ((addr) >= (down) && (addr) <= (up))

Shellcode::Shellcode() : buffer_(nullptr), buffer_size_(0) {}

/**
  Read and map 'shellcode.dll' in memory.
*/
bool Shellcode::Map() {
  wchar_t filepath[1024];
  size_t path_pos = GetModuleFileNameW(NULL, filepath, sizeof(filepath));
  while (filepath[path_pos] != '\\') {
    path_pos--;
  }
  wcscpy(&filepath[path_pos + 1], SHELLCODE_DLL);

  HANDLE file = CreateFileW(filepath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if (file == INVALID_HANDLE_VALUE) {
    debug_printf("Cannot open '%ls': 0x%x\n", filepath, GetLastError());
    return false;
  }

  size_t filesize = 0;
  DWORD filesize_high;
  DWORD filesize_low = GetFileSize(file, &filesize_high);
  if (filesize_low == INVALID_FILE_SIZE) {
    debug_printf("GetFileSize failed: 0x%x\n", GetLastError());
    return false;
  }
  filesize = filesize_low + filesize_high * 0x10000;
  debug_printf("Shellcode size: 0x%x\n", filesize);

  void* buffer = VirtualAlloc(NULL, filesize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
  if (!buffer) {
    debug_printf("VirtualAlloc failed\n");
    CloseHandle(file);
    return false;
  }

  DWORD nb_reads = 0;
  bool success = ReadFile(file, buffer, filesize, &nb_reads, NULL);
  if (!success || !nb_reads) {
    debug_printf("ReadFile failed: 0x%x\n", GetLastError());
    VirtualFree(buffer, 0, MEM_RELEASE);
    CloseHandle(file);
    return false;
  }

  CloseHandle(file);
  buffer_ = reinterpret_cast<unsigned char*>(buffer);
  buffer_size_ = filesize;
  debug_printf("Shellcode mapped at 0x%p (size = 0x%x)\n", buffer_, buffer_size_);
  return true;
}

/**
  Inject a PE executable like a shellcode. It loads sections with an arbitrary
  image base, fix the relocations but doesn't import the IAT or EAT and doesn't
  write the PE headers to not be easily detected.
 */
bool Shellcode::InjectInProcess(Process* process, const ExternConfig* config, DWORD* new_end_scene, DWORD* new_reset) {
  if (!buffer_) {
    debug_printf("Map() the shellcode before injecting it\n");
    return false;
  }
  if (!process) {
    debug_printf("Invalid process\n");
    return false;
  }

  // Get the tables. No need to check the headers: we have created this file
  DWORD base = reinterpret_cast<DWORD>(buffer_);
  auto* dos_header = reinterpret_cast<IMAGE_DOS_HEADER*>(base);
  auto* nt_headers = reinterpret_cast<IMAGE_NT_HEADERS*>(base + dos_header->e_lfanew);
  auto* section_table = reinterpret_cast<IMAGE_SECTION_HEADER*>(base + dos_header->e_lfanew + sizeof(IMAGE_NT_HEADERS));

  DWORD reloc_va = nt_headers->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress;
  IMAGE_SECTION_HEADER* reloc_section = nullptr;

  // Allocating the space in memory
  debug_printf("Reserving 0x%x bytes\n", nt_headers->OptionalHeader.SizeOfImage);
  DWORD new_imagebase = process->VirtualAlloc(0, nt_headers->OptionalHeader.SizeOfImage, MEM_RESERVE, PAGE_NOACCESS);
  if (!new_imagebase) {
    debug_printf("Cannot allocate space for all the image\n");
    return false;
  }
  debug_printf("New image base: 0x%x\n", new_imagebase);

  // Map all the sections and search the relocation table
  debug_printf("Mapping the sections\n");
  for (size_t i = 0; i < nt_headers->FileHeader.NumberOfSections; i++) {
    if (section_table[i].VirtualAddress == reloc_va) {
      debug_printf("  Relocation section found\n");
      reloc_section = &section_table[i];
      continue;
    }

    // Allocating the section
    DWORD target_addr = new_imagebase + section_table[i].VirtualAddress;
    size_t allocation_size = section_table[i].SizeOfRawData;
    if (section_table[i].Misc.VirtualSize > section_table[i].SizeOfRawData) {
      allocation_size = section_table[i].Misc.VirtualSize;
    }

    debug_printf("  Allocating the section '%s' at 0x%08X of size 0x%08x\n", section_table[i].Name, target_addr,
                 allocation_size);
    DWORD success = process->VirtualAlloc(target_addr, allocation_size, MEM_COMMIT, PAGE_READWRITE);
    if (!success) {
      debug_printf("    Cannot allocate the section\n");
      return false;
    }
    process->MemSet(target_addr, 0, allocation_size);

    // Copy the data
    void* src = reinterpret_cast<void*>(base + section_table[i].PointerToRawData);
    size_t copy_size = section_table[i].SizeOfRawData;
    if (section_table[i].Misc.VirtualSize < section_table[i].SizeOfRawData) {
      copy_size = section_table[i].Misc.VirtualSize;
    }
    debug_printf("    Copy 0x%x bytes from 0x%p to 0x%08X\n", copy_size, src, target_addr);
    process->WriteProcMem(target_addr, src, copy_size);
  }

  // Now we do the relocations
  if (!reloc_section) {
    debug_printf("Relocation section not found\n");
    return false;
  }

  char* relocs = reinterpret_cast<char*>(base + reloc_section->PointerToRawData);
  char* relocs_end = relocs + reloc_section->Misc.VirtualSize;
  debug_printf("Parsing relocs from 0x%p to 0x%p\n", relocs, relocs_end);

  while (relocs < relocs_end) {
    const DWORD* base_block_header = reinterpret_cast<DWORD*>(relocs);
    DWORD reloc_page_rva = base_block_header[0];
    DWORD reloc_block_size = base_block_header[1];

    auto* relocs_array = reinterpret_cast<const WORD*>(relocs + 2 * sizeof(DWORD));
    DWORD nb_relocs = (reloc_block_size - 2 * sizeof(DWORD)) / sizeof(WORD);

    debug_printf("  %d relocs (block_size = 0x%x) to proceed starting at 0x08%x\n", nb_relocs, reloc_block_size,
                 reloc_page_rva);
    for (size_t i = 0; i < nb_relocs; i++) {
      WORD type = (relocs_array[i] & 0xf000) >> 12;
      if (type == IMAGE_FILE_MACHINE_UNKNOWN) {  // Used for padding
        continue;
      } else if (type != IMAGE_REL_BASED_HIGHLOW) {
        debug_printf("  Relocation %d but only 32bits relocations supported\n", type);
        return false;
      }

      WORD offset = relocs_array[i] & 0xfff;
      DWORD process_addr = new_imagebase + reloc_page_rva + offset;
      DWORD* local_addr = reinterpret_cast<DWORD*>(base + Rva2FileOffset(reloc_page_rva) + offset);
      DWORD new_value = *local_addr - nt_headers->OptionalHeader.ImageBase + new_imagebase;

      debug_printf("    *0x%08X = 0x%08X => 0x%08X\n", process_addr, *local_addr, new_value);
      process->WriteProcMem(process_addr, &new_value, sizeof(new_value));
    }

    relocs += reloc_block_size;
  }

  // Writing the configuration
  debug_printf("Writing the configuration\n");
  DWORD hk_end_scene_addr = new_imagebase + GetFunctionRVA("_Z10HkEndSceneP16IDirect3DDevice9@4");
  DWORD hk_reset_addr = new_imagebase + GetFunctionRVA("_Z7HkResetP16IDirect3DDevice9P23_D3DPRESENT_PARAMETERS_@8");
  DWORD extern_config_addr = new_imagebase + GetFunctionRVA("extern_config");
  debug_printf("  hk_end_scene:  0x%08X\n", hk_end_scene_addr);
  debug_printf("  hk_reset:      0x%08X\n", hk_reset_addr);
  debug_printf("  extern_config: 0x%08X\n", extern_config_addr);
  *new_end_scene = hk_end_scene_addr;
  *new_reset = hk_reset_addr;

  std::vector<char> config_blob = config->GetConfigBlob();
  process->WriteProcMem(extern_config_addr, config_blob.data(), config_blob.size());

  // Modify the sections protection
  debug_printf("Modifying the sections protections\n");
  for (size_t i = 0; i < nt_headers->FileHeader.NumberOfSections; i++) {
    if (section_table[i].VirtualAddress == reloc_va) {
      reloc_section = &section_table[i];
      continue;
    }

    DWORD target_addr = new_imagebase + section_table[i].VirtualAddress;
    size_t allocation_size = section_table[i].SizeOfRawData;
    if (section_table[i].Misc.VirtualSize > section_table[i].SizeOfRawData) {
      allocation_size = section_table[i].Misc.VirtualSize;
    }

    const DWORD charac = section_table[i].Characteristics;
    DWORD memprot = 0;
    if (charac & IMAGE_SCN_MEM_EXECUTE) {
      memprot = PAGE_EXECUTE_READ;
    } else if (charac & IMAGE_SCN_MEM_READ && IMAGE_SCN_MEM_WRITE) {
      memprot = PAGE_READWRITE;
    } else {
      memprot = PAGE_READONLY;
    }

    debug_printf("  Modifying the access of 0x%08X to 0x%x\n", target_addr, memprot);
    if (!process->VirtualProtect(target_addr, allocation_size, memprot)) {
      debug_printf("  Cannot modify the section access\n");
      return false;
    }
  }

  // Execute the entrypoint
  // It will be done later via an hooking only, so not in this function
  // debug_printf("Executing the entry point 0x%08x\n", hk_end_scene_addr);
  // process->CreateThreadAndWait(hk_end_scene_addr);

  return true;
}

/**
  Convert a relative address inside a loaded PE executable to a file offset
  inside the executable.
*/
DWORD Shellcode::Rva2FileOffset(DWORD rva) {
  DWORD base = reinterpret_cast<DWORD>(buffer_);
  auto* dos_header = reinterpret_cast<IMAGE_DOS_HEADER*>(base);
  auto* nt_headers = reinterpret_cast<IMAGE_NT_HEADERS32*>(base + dos_header->e_lfanew);
  auto* section_table = reinterpret_cast<IMAGE_SECTION_HEADER*>(base + dos_header->e_lfanew + sizeof(IMAGE_NT_HEADERS));

  IMAGE_SECTION_HEADER* section = nullptr;
  for (size_t i = 0; i < nt_headers->FileHeader.NumberOfSections; i++) {
    if (ADDR_IS_BTW(rva, section_table[i].VirtualAddress,
                    section_table[i].VirtualAddress + section_table[i].SizeOfRawData)) {
      section = &section_table[i];
      break;
    }
  }
  if (!section) {
    return 0;
  }

  // The export directory is not always at the beggining of a section
  DWORD offset = rva - section->VirtualAddress;
  return section->PointerToRawData + offset;
}

/**
  Get the relative offset of a function in the PE executable.
*/
DWORD Shellcode::GetFunctionRVA(const char* function_name) {
  if (!buffer_) {
    debug_printf("GetFunctionRVA: no shellcode loaded\n");
    return 0;
  }

  DWORD base = reinterpret_cast<DWORD>(buffer_);
  auto* dos_header = reinterpret_cast<IMAGE_DOS_HEADER*>(base);
  auto* nt_headers = reinterpret_cast<IMAGE_NT_HEADERS32*>(base + dos_header->e_lfanew);
  auto* section_table = reinterpret_cast<IMAGE_SECTION_HEADER*>(base + dos_header->e_lfanew + sizeof(IMAGE_NT_HEADERS));

  // Search the export table
  auto* data_directory = &nt_headers->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
  if (!data_directory->VirtualAddress) {
    debug_printf("GetFunctionRVA: No export table\n");
    return 0;
  }
  auto* export_dir = reinterpret_cast<IMAGE_EXPORT_DIRECTORY*>(base + Rva2FileOffset(data_directory->VirtualAddress));

  // Search the function name
  auto* names = reinterpret_cast<DWORD*>(base + Rva2FileOffset(export_dir->AddressOfNames));
  auto* names_ordinal = reinterpret_cast<WORD*>(base + Rva2FileOffset(export_dir->AddressOfNameOrdinals));
  auto* functions = reinterpret_cast<DWORD*>(base + Rva2FileOffset(export_dir->AddressOfFunctions));

  char* name = nullptr;
  for (size_t i = 0; export_dir->NumberOfFunctions; i++) {
    name = reinterpret_cast<char*>(base + Rva2FileOffset(names[i]));
    if (!_stricmp(name, function_name)) {
      return functions[names_ordinal[i]];
    }
  }

  return 0;
}