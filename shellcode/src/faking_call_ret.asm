global _FakingCallRet

section .text

_FakingCallRet:
  ; Set EBP to return to a second point
  mov ecx, esp
  push ebp        ; save the caller ebp
  push next       ; overwrite the return address
  push esp        ; this value will be popped in EBP
  mov ebp, esp    ; this one will be used to return into @next

  ; Prepare the stack (8 arguments should be enough)
  push dword[ecx+40]  
  push dword[ecx+36]  
  push dword[ecx+32]  
  push dword[ecx+28]  
  push dword[ecx+24]  
  push dword[ecx+20]  
  push dword[ecx+16]  
  push dword[ecx+12]  
  push dword[ecx+8]  ; fake return address

  ; Jump on the targeted function
  mov eax, [ecx+4]  ; arg0: function addr
  jmp eax

  next:
  mov eax, 0x41424344
  pop ebp
  ret