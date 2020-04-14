# LoL Patcher

A game cheat for League of Legends. It's only here to learn how it works because
it's not updated since February 2020 and I have made it just for the fun.

Technically:
- Injecting itself like a shellcode (no PE headers, no standard library)
- Memory scan done externally by the injector
- Hooking the Direct3D C++ Virtual Table to show information on the screen
- Faking the return address of multiple in-game functions via ROP
- Compile-time encryption

Functionalities:
- Characters and objects ESP
- Last hit helper
- Skills level / cooldown indication


## Screenshot 

![Screenshot](screenshot.jpg)


# Building

On Ubuntu:

```sh
apt install mingw-w64 nasm
make
```