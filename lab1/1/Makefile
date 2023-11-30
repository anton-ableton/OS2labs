TARGET_THREAD = thread-example
TARGET_B = b-example

SRCS_THREAD = thread.c
SRCS_B = b.c

CC=gcc
RM=rm
CFLAGS= -g -Wall
LIBS=-lpthread
INCLUDE_DIR="."

all: ${TARGET_THREAD} ${TARGET_B}

${TARGET_THREAD}: ${SRCS_THREAD}
	${CC} ${CFLAGS} -I${INCLUDE_DIR} ${SRCS_THREAD} ${LIBS} -o ${TARGET_THREAD}

${TARGET_B}: ${SRCS_B}
	${CC} ${CFLAGS} -I${INCLUDE_DIR} ${SRCS_B} ${LIBS} -o ${TARGET_B}

clean:
	${RM} -f *.o ${TARGET_THREAD} ${TARGET_B}
