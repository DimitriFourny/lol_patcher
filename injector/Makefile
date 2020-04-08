src_cpp = $(wildcard src/*.cpp)
obj_cpp = $(patsubst src/%.cpp,out/%.o,$(src_cpp))
src_asm = $(wildcard src/*.asm)
obj_asm = $(patsubst src/%.asm,out/%.o,$(src_asm))
obj = $(obj_asm) $(obj_cpp)
gpp_flags = -m32
ld_flags = -mi386pe
ld_deps_dirs = -L/usr/lib/gcc/i686-w64-mingw32/7.3-win32 \
               -L/usr/i686-w64-mingw32/lib
ld_deps = /usr/i686-w64-mingw32/lib/crt2.o \
          /usr/i686-w64-mingw32/lib/crtbegin.o \
          /usr/i686-w64-mingw32/lib/crtend.o \
          -Bstatic -lstdc++ -Bdynamic -lmingw32 -lgcc -lgcc_eh -lmoldname \
          -lmingwex -lmsvcrt -ladvapi32 -lshell32 -luser32 -lkernel32 \
          -lmingw32 -lgcc -lgcc_eh -lmoldname -lmingwex -lmsvcrt \
          -lpsapi
 
all: $(obj)
	x86_64-w64-mingw32-ld $(ld_flags) -o out/injector.exe $(obj) $(ld_deps_dirs) $(ld_deps)

out/%.o: src/%.cpp
	x86_64-w64-mingw32-g++ $(gpp_flags) -c -o $@ $<

out/%.o: src/%.asm
	nasm -f elf -o $@ $<

clean:
	rm -f out/*.o
	rm -f out/injector.exe