#pragma once
#include <windows.h>

class Log {
 public:
  static void Initialize();
  static void LogStr(const char* msg);
  static void LogPrintf(const char* format, ...);

 private:
  static HANDLE logfile_;
};