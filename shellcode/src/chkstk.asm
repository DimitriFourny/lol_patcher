; out/main.o:main.cpp:(.text+0x3eb): undefined reference to `__chkstk_ms'
; https://metricpanda.com/rival-fortress-update-45-dealing-with-__chkstk-
; __chkstk_ms-when-cross-compiling-for-windows/

global ___chkstk_ms

section .text

___chkstk_ms:
  ret