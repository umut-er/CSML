CC=gcc-13
CFLAGS=-O2

DEP=.deps
DEPFLAGS=-MT $@ -MMD -MP -MF $(DEP)/$*.d

SRC=src
SRCS=$(wildcard $(SRC)/*.c)

all: LIB

LIB: $(SRCS)
	$(CC) -shared -fPIC $(CFLAGS) src/CSML_Server.c src/CSML_Server_Components.c -o libCSML_Server.so
	$(CC) -shared -fPIC $(CFLAGS) src/CSML_Client.c -o libCSML_Client.so

clean:
	rm -f *.so
