# CR

A CPU based 3D renderer. Can be compiled on both Linux with X11(and Wayland I think?) and Windows(see compile instructions)

# Compile instructions

For Linux:
```bash
gcc src/*.c -I inc -lX11 -lm -Wall -Werror -o cr.out
```

For Windows:
```bash
gcc src/*.c -I inc -mwindows -Wall -Werror -o cr.exe
```
