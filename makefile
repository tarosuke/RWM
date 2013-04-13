all: rwm

copt = -L/usr/X11R6/lib -lglut -lGLU -lGL -lXmu -lXi -lXext -lXcomposite -lX11 -lm -Wall -lstdc++

src = $(wildcard *.c) $(wildcard *.cc)

clean:
	rm -f rwm rwm.test *.o *.d

test: rwm.test
	./rwm.test

run: rwm
	./rwm

rwm: makefile $(src) $(wildcard #.h)
	gcc $(copt) -o $@ $(src)

rwm.test: makefile $(src) $(wildcard #.h)
	gcc -DTEST $(copt) -o $@ $(src)
