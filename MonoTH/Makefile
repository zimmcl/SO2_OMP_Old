.DELETE_ON_ERROR:#Borra el target en caso de producirse un error en alguno de los comandos

#-fopenmp    -lgomp
#Definimos las macros
CC=gcc	#Compilador a usar
CFLAGS= -std=gnu99 -Wall -pedantic -pg	#Banderas a utilizar

all: MonoTH
	@echo "Compilación exitosa | ${shell date --iso=seconds}"

MonoTH: MonoTH.c
	${CC} ${CFLAGS} -o MonoTH MonoTH.c -lm

clean:
	rm MonoTH analisis_GP.txt gmon.out
	@echo "Se eliminaron correctamente todos los archivos."

run:
	@$(MAKE) && ./MonoTH
