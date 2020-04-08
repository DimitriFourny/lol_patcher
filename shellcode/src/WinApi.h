#pragma once
#include <windows.h>
#include <d3dx9.h>

typedef HMODULE(WINAPI* type_get_module_handle)(LPCSTR);
typedef HANDLE(WINAPI* type_create_file)(
    LPCWSTR lpFileName,
    DWORD dwDesiredAccess,
    DWORD dwShareMode,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    DWORD dwCreationDisposition,
    DWORD dwFlagsAndAttributes,
    HANDLE hTemplateFile);
typedef BOOL(WINAPI* type_write_file)(HANDLE hFile,
                                      LPCVOID lpBuffer,
                                      DWORD nNumberOfBytesToWrite,
                                      LPDWORD lpNumberOfBytesWritten,
                                      LPOVERLAPPED lpOverlapped);
typedef BOOL(WINAPI* type_flush_file_buffers)(HANDLE hFile);
typedef HRESULT(WINAPI* type_sleep)(DWORD dwMilliseconds);
typedef HRESULT(WINAPI* type_create_line)(LPDIRECT3DDEVICE9 pDevice,
                                          LPD3DXLINE* ppLine);
typedef HRESULT(WINAPI* type_create_font)(_In_ LPDIRECT3DDEVICE9 pDevice,
                                          _In_ INT Height,
                                          _In_ UINT Width,
                                          _In_ UINT Weight,
                                          _In_ UINT MipLevels,
                                          _In_ BOOL Italic,
                                          _In_ DWORD CharSet,
                                          _In_ DWORD OutputPrecision,
                                          _In_ DWORD Quality,
                                          _In_ DWORD PitchAndFamily,
                                          _In_ LPCWSTR pFacename,
                                          _Out_ LPD3DXFONT* ppFont);

class WinApi {
 public:
  static void Initialize(void* kernel32);
  static type_get_module_handle GetModuleHandleA;
  static type_create_file CreateFileW;
  static type_write_file WriteFile;
  static type_flush_file_buffers FlushFileBuffers;
  static type_sleep Sleep;
  static type_create_line D3DXCreateLine;
  static type_create_font D3DXCreateFont;
};