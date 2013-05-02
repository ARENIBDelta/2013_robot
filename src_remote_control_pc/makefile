TARGET  := arenib_delta_control.exe 
SRCS    := main.c
OBJS    := ${SRCS:.c=.o} lib/libSDL.dll.a
DEPS    := ${SRCS:.c=.dep} 
XDEPS   := $(wildcard ${DEPS}) 
CC      := gcc

CCFLAGS = -O2 -Wall -Werror 
LDFLAGS = 
LIBS = 
INC = include

.PHONY: all clean distclean 
all:: ${TARGET} 

ifneq (${XDEPS},) 
include ${XDEPS} 
endif 

${TARGET}: ${OBJS} 
	${CC} ${LDFLAGS} -o $@ $^ ${LIBS} 

${OBJS}: %.o: %.c %.dep 
	${CC} ${CCFLAGS} -I ${INC} -o $@ -c $< 

${DEPS}: %.dep: %.c Makefile 
	${CC} ${CCFLAGS} -MM $< > $@ 

clean:: 
	rm -f *.o ${TARGET} 

distclean:: clean