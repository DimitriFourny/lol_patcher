#include "D3dHook.h"
#include "Debug.h"

D3dHook::D3dHook(std::shared_ptr<Process> process) {
  process_ = std::move(process);
}

bool D3dHook::LoadDeviceVtable(const Signature* signature) {
  d3d_vtable_.clear();

  DWORD d3d_device = signature->GetD3DDevice(process_.get());
  debug_printf("d3d_device = 0x%08X\n", d3d_device);
  d3d_device_addr_ = d3d_device;

  DWORD d3d_device_vtable = 0;
  if (!process_->ReadProcMem(d3d_device, &d3d_device_vtable, sizeof(d3d_device_vtable))) {
    debug_printf("Cannot read D3D device vtable\n");
    return false;
  }
  debug_printf("d3d_device_vtable = 0x%08X\n", d3d_device_vtable);

  DWORD d3d_vtable_count = static_cast<DWORD>(D3dVtableMembers::VtableSize);
  d3d_vtable_.resize(d3d_vtable_count);
  if (!process_->ReadProcMem(d3d_device_vtable, d3d_vtable_.data(), d3d_vtable_count * sizeof(d3d_vtable_[0]))) {
    debug_printf("Cannot read D3D device vtable\n");
    d3d_vtable_.clear();
    return false;
  }
  d3d_vtable_original_addr_ = d3d_device_vtable;
  d3d_vtable_addr_ = d3d_device_vtable;

  return true;
}

DWORD D3dHook::GetMemberAddr(D3dVtableMembers member) const {
  if (!d3d_vtable_.size()) {
    debug_printf("Load the device vtable before\n");
    return 0;
  }
  return d3d_vtable_[static_cast<DWORD>(member)];
}

bool D3dHook::MoveVtable() {
  if (!d3d_vtable_.size()) {
    debug_printf("Load the device vtable before\n");
    return 0;
  }
  DWORD vtable_size = d3d_vtable_.size() * sizeof(d3d_vtable_[0]);

  // Allocate the new vtable
  DWORD new_vtable_addr = process_->VirtualAlloc(0, vtable_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
  if (!new_vtable_addr) {
    debug_printf("Failed to allocate the new vtable\n");
    return false;
  }
  debug_printf("New vtable addr: 0x%08X\n", new_vtable_addr);

  // Copy the data inside this new vtable
  bool success = process_->WriteProcMem(new_vtable_addr, d3d_vtable_.data(), vtable_size);
  if (!success) {
    debug_printf("Can't write the new vtable\n");
    return false;
  }

  // Set the vtable read only
  success = process_->VirtualProtect(new_vtable_addr, vtable_size, PAGE_READONLY);
  if (!success) {
    debug_printf("Can't change the new vtable to read-only\n");
    return false;
  }

  // Update the vtable pointer
  success = process_->WriteProcMem(d3d_device_addr_, &new_vtable_addr, sizeof(new_vtable_addr));
  if (!success) {
    debug_printf("Can't write the vtable pointer\n");
    return false;
  }
  d3d_vtable_addr_ = new_vtable_addr;

  return true;
}

bool D3dHook::RestoreVtable() {
  bool success =
      process_->WriteProcMem(d3d_device_addr_, &d3d_vtable_original_addr_, sizeof(d3d_vtable_original_addr_));
  if (!success) {
    debug_printf("Can't write the vtable pointer\n");
    return false;
  }

  d3d_vtable_addr_ = d3d_vtable_original_addr_;
  return true;
}

bool D3dHook::HookMember(D3dVtableMembers member, DWORD new_addr) {
  // Set the vtable to read-write
  DWORD vtable_size = d3d_vtable_.size() * sizeof(d3d_vtable_[0]);
  bool success = process_->VirtualProtect(d3d_vtable_addr_, vtable_size, PAGE_READWRITE);
  if (!success) {
    debug_printf("Can't change the vtable to read-write\n");
    return false;
  }

  DWORD function_pointer_addr = d3d_vtable_addr_ + static_cast<DWORD>(member) * sizeof(DWORD);
  success = process_->WriteProcMem(function_pointer_addr, &new_addr, sizeof(new_addr));
  if (!success) {
    debug_printf("Can't write the method pointer in the vtable\n");
    return false;
  }

  debug_printf("HookMember: *0x%08X = 0x%08X\n", function_pointer_addr, new_addr);
  d3d_vtable_[static_cast<DWORD>(member)] = new_addr;

  // Set the vtable read only
  success = process_->VirtualProtect(d3d_vtable_addr_, vtable_size, PAGE_READONLY);
  if (!success) {
    debug_printf("Can't change the vtable to read-only\n");
    return false;
  }

  return true;
}