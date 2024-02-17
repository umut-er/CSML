CC=gcc-13
CFLAGS=-O2

SRC=src
SRCS=$(wildcard $(SRC)/*.c)
OBJS=$(patsubst $(SRC)/%.c, %.o, $(SRCS))

$(info SRCS=$(SRCS))

all: LIB

$(OBJS): $(SRC)/*.c
	$(CC) $(CFLAGS) $< -c -o $@

LIB: $(OBJS)
	ar rcs libCSML_Server.a CSML_Server_Components.o CSML_Server.o
	ar rcs libCSML_Client.a CSML_Client.o
	rm *.o

clean:
	rm -f *.o *.a
