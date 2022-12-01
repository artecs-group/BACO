
DEPS=common/
TARGETS=client server




.PHONY: all remake release debug instrument clean tests tests-debug tests-clean tests-instrument


all: release debug instrument

remake: clean release debug instrument

release:
	for dir in $(DEPS) ; do \
		make -C $$dir release ; \
	done; \
	for dir in $(TARGETS) ; do \
		make -C $$dir release ; \
	done


debug:
	for dir in $(DEPS) ; do \
		make -C $$dir debug ; \
	done; \
	for dir in $(TARGETS) ; do \
		make -C $$dir debug ; \
	done

instrument:
	for dir in $(DEPS) ; do \
		make -C $$dir instrument ; \
	done; \
	for dir in $(TARGETS) ; do \
		make -C $$dir instrument ; \
	done


clean:
	for dir in $(DEPS) ; do \
		make -C $$dir clean ; \
	done; \
	for dir in $(TARGETS) ; do \
		make -C $$dir clean ; \
	done


## special for testing

tests:
	make -C test/

tests-debug:
	make -C test/ debug

tests-instrument:
	make -C test/ instrument

tests-clean:
	make -C test/ clean

