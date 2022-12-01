
TARGETS=$(shell ls -d */)

.PHONY: all debug clean

$(info ********************************)
$(info ************  TESTS ************)
$(info ********************************)


all:
	for dir in $(TARGETS) ; do \
		make -C $$dir ; \
	done

debug:
	for dir in $(TARGETS) ; do \
		make -C $$dir debug ; \
	done

clean:
	for dir in $(TARGETS) ; do \
		make -C $$dir clean ; \
	done
