.DELETE_ON_ERROR:#Borra el target en caso de producirse un error en alguno de los comandos

#Definimos las macros
CC=gcc	#Compilador a usar
CFLAGS= -std=gnu99 -Wall -pedantic -pg -fopenmp	#Banderas a utilizar

all: MultiTH
	@echo "Compilación exitosa | ${shell date --iso=seconds}"

MultiTH: MultiTH.c
	${CC} ${CFLAGS} -o MultiTH MultiTH.c -lm -lgomp

clean:
	rm MultiTH analisis_GP.txt gmon.out
	@echo "Se eliminaron correctamente todos los archivos."

run:
	@$(MAKE) && ./MultiTH
