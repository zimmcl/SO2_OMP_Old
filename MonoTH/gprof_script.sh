#!/bin/bash

CYAN='\x1B[36m'
NC='\033[0m'


echo -e "${CYAN}Trabajo Práctico N°2 - Pulsos${NC}"
echo "-----------"
echo ""

echo -e "${CYAN}Ejecuntando make${NC}"
echo "--------------"
make
echo ""

echo -e "${CYAN}Ejecutando el programa${NC}"
echo "------------------------------"
./MonoTH
echo ""

echo -e "${CYAN}Creando informe de gprof: analisis._GPtxt${NC}"
echo "--------------------------------"
gprof -b MonoTH gmon.out > analisis_GP.txt
echo ""
sleep 2

echo -e "${CYAN}Leyendo informe:${NC}"
echo "--------------------------------"
cat analisis_GP.txt
echo ""

echo -e "${CYAN}Limpiando directorio${NC}"
echo "--------"
make clean
echo "------------------------------"
