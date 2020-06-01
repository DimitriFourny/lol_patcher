mingw_lib = $(wildcard /opt/lib/gcc/i686-w64-mingw32/*)
src_cpp = $(wildcard src/*.cpp)
obj_cpp = $(patsubst src/%.cpp,out/%.o,$(src_cpp))
src_asm = $(wildcard src/*.asm)
obj_asm = $(patsubst src/%.asm,out/%.o,$(src_asm))
gpp_flags = -m32 -static
ld_deps_dirs = -L$(mingw_lib) \
               -L/opt/i686-w64-mingw32/lib
ld_deps = -lstdc++ -lpsapi
 
all: out $(obj_asm)
	i686-w64-mingw32-g++ $(gpp_flags) -o out/injector.exe $(obj_asm) $(src_cpp) $(ld_deps) 

out:
	mkdir -p out

out/%.o: src/%.asm
	nasm -f elf -o $@ $<

clean:
	rm -f out/*.o
	rm -f out/injector.exe