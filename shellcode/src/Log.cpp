#include "Log.h"
#include "WinApi.h"
#include "printf.h"

HANDLE Log::logfile_ = nullptr;

// static
void Log::Initialize() {
  logfile_ = WinApi::CreateFileW(L"C:\\Users\\Dimitri\\Desktop\\debug.log", GENERIC_READ | GENERIC_WRITE,
                                 FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
}

// static
void Log::LogStr(const char* msg) {
  DWORD nb_writen = 0;
  if (logfile_) {
    WinApi::WriteFile(logfile_, msg, strlen(msg), &nb_writen, NULL);
    WinApi::FlushFileBuffers(logfile_);
  }
}

// static
void Log::LogPrintf(const char* format, ...) {
  char buffer[2048];
  va_list va;
  va_start(va, format);
  vsnprintf(buffer, sizeof(buffer), format, va);
  va_end(va);

  LogStr(buffer);
}