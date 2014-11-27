all: wODM wOSD wOLM

.PHONY : clean test
.DELETE_ON_ERROR : $(wildcard objs/*)


####################################################################### VALUES

COPTS += -Wall -Werror -ggdb -IX11 -I./include -I./view


libs = GL GLU GLEW m X11 Xmu Xi Xext Xcomposite Xdamage stdc++ gdbm pthread png
LIBOPTS = $(addprefix -l, $(libs))


ssfc = .c .cc .glsl
spth = */* */*/*
srcs = $(patsubst old/%,,$(foreach p, $(spth), $(foreach s, $(ssfc), $(wildcard $(p)$(s)))))

dirs = $(sort $(dir $(srcs)))
mods = $(basename $(notdir $(srcs)))
dmds= $(addprefix objs/, $(mods))
objs = $(addsuffix .o, $(dmds))
deps = $(addsuffix .d, $(dmds))

wODMObjs = objs/rift_dk1.o objs/rift_dk2.o



######################################################################## RULES

test: wODM.test wOSD wOLM
	./wODM.test

run: wODM
	./wODM

wODM: makefile wODM.cc $(wODMObjs) userLib.a wOSD
	gcc  $(COPTS) $(CCOPTS) -Xlinker "-Map=wODM.map" -o $@ wODM.cc $(wODMObjs) userLib.a $(LIBOPTS)

wODM.test: COPTS+=-DTEST
wODM.test: makefile wODM.cc $(wODMObjs) userLib.a
	gcc  $(COPTS) $(CCOPTS) -ggdb -Xlinker "-Map=wODM.map" -o $@ wODM.cc $(wODMObjs) userLib.a $(LIBOPTS)

wOSD: makefile wOSD.cc userLib.a
	gcc  $(COPTS) $(CCOPTS) -Xlinker "-Map=wOSD.map" -o $@ wOSD.cc userLib.a $(LIBOPTS)

wOLM: makefile wOLM.cc userLib.a
	gcc  $(COPTS) $(CCOPTS) -Xlinker "-Map=wOLM.map" -o $@ wOLM.cc userLib.a $(LIBOPTS)

clean:
	rm -fr wODM wODM.test wOSD wOLM *.map userLib.a objs/*


userLib.a: $(objs)
	ar -rc $@ $(objs)



################################################################# COMMON RULES


-include $(deps)

vpath %.o objs
vpath % $(dirs)


objs/%.o : %.cc makefile
	$(CC) $(COPTS) $(CCOPTS) -c -o $@ $<

objs/%.o : %.c makefile
	${CC} $(COPTS) -c -o $@ $<

objs/%.o : %.glsl makefile
	objcopy -I binary -O elf64-x86-64 -B i386 $< $@

objs/%.d : %.cc
	@echo $@
	@echo -n objs/ > $@
	@$(CPP) $(COPTS) $(CCOPTS) -MM $< >> $@

objs/%.d : %.c
	@echo $@
	@echo -n objs/ > $@
	@$(CPP) $(COPTS) -MM $< >> $@


