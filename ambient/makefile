target = ambient.a
COPTS += -I ..

srcs = $(wildcard */*.c) $(wildcard */*.cc) $(wildcard */*.glsl) $(wildcard */*.tga) $(wildcard */*.world) $(wildcard *.tga) $(wildcard *.world)

include ../make.in

%.o : %.tga makefile
	objcopy -I binary -O elf64-x86-64 -B i386 $< $@

%.o : %.world makefile
	objcopy -I binary -O elf64-x86-64 -B i386 $< $@

