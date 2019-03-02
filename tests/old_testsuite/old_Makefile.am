SRCS=$(wildcard unitary_tests/*.cc)
OBJS=$(SRCS:.cc=)

check-local : $(OBJS)
	./testsuite.py

clean:
	rm $(OBJS)