#pragma once
#include "Process.h"

class Signature {
 public:
  Signature(std::vector<char> dump, DWORD dump_rva);
  DWORD GetD3DDevice(const Process* process) const;
  DWORD GetObjectsManager(const Process* process) const;
  DWORD GetLocalPlayer(const Process* process) const;
  DWORD GetDrawCircle(const Process* process) const;
  DWORD GetPopRet(const Process* process) const;
  DWORD GetGameRenderer(const Process* process) const;
  DWORD GetWorldToScreen(const Process* process) const;
  DWORD GetGameTime(const Process* process) const;
  DWORD GetIssueOrder(const Process* process) const;
  DWORD GetCastSpell(const Process* process) const;
  DWORD GetHudInstance(const Process* process) const;

 private:
  const BYTE* SearchPattern(BYTE* pattern, const char* mask, size_t mask_len) const;

  std::vector<char> dump_;
  DWORD dump_rva_;
};