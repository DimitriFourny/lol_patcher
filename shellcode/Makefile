src_cpp = $(wildcard src/*.cpp)
obj_cpp = $(patsubst src/%.cpp,out/%.o,$(src_cpp))
src_asm = $(wildcard src/*.asm)
obj_asm = $(patsubst src/%.asm,out/%.o,$(src_asm))
obj = $(obj_asm) $(obj_cpp)
gpp_flags = -m32 -fno-stack-protector -O2
ld_flags = -mi386pe

all: out $(obj)
	x86_64-w64-mingw32-ld $(ld_flags) -o out/shellcode.dll $(obj)
	strip out/shellcode.dll

out:
	mkdir -p out

out/%.o: src/%.cpp
	x86_64-w64-mingw32-g++ $(gpp_flags) -c -o $@ $<

out/%.o: src/%.asm
	nasm -f elf -o $@ $<

clean:
	rm -f out/*.o
	rm -f out/shellcode.dll