# extra

.PHONY: clean ambient/ambient

subtargets = $(subst /makefile,,$(wildcard */makefile toolbox/*/makefile))

ambient/amdient :
	for p in $(subtargets); do make -j -C $$p || exit -1; done


clean:
	rm -f *.a
	for p in $(subtargets); do make -j -C $$p clean || exit -1; done

