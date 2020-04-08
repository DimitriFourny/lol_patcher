
all: out/shellcode.dll out/inj.exe

out/inj.exe:
	cd injector && make
	cp injector/out/injector.exe out/inj.exe

out/shellcode.dll:
	cd shellcode && make
	cp shellcode/out/shellcode.dll out/shellcode.dll

clean:
	rm -f out/inj.exe
	rm -f out/shellcode.dll
	cd shellcode && make clean
	cd injector && make clean