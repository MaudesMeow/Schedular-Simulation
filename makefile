# compiler flags:
# 	-g		- adds debugging info to executable
# 	-Wall - turns on most compiler warnings
CFLAGS = -g -Wall

# compiler to use: gcc for c, g++ for C++
CC = g++

TARGET = hebert_mahan

${TARGET}: ${TARGET}.o 
	${CC} ${CFLAGS} -o ${TARGET} ${TARGET}.o
${TARGET}.o: ${TARGET}.cpp ${TARGET}.hpp
	${CC} ${CFLAGS} -c ${TARGET}.cpp

clean:
	if [ -f *.o ]; then rm *.o; fi
	if [ -f hebert_mahan ]; then rm hebert_mahan; fi

