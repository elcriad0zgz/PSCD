#!/bin/bash
#*****************************************************************
# File:   lanzar_sistema.sh
# Author: Práctica 5 PSCD
# Date:   diciembre 2025
# Coms:   Script para lanzar el sistema distribuido completo
#         en modo local para pruebas
#*****************************************************************

# Configuración
IP="localhost"
PUERTO_TAREAS=3000
PUERTO_MATRIZ=3001
N_CONTROLADORES=10

echo "=========================================="
echo "  SISTEMA DISTRIBUIDO - PRÁCTICA 5"
echo "=========================================="
echo ""
echo "Configuración:"
echo "  IP: $IP"
echo "  Puerto servidor tareas: $PUERTO_TAREAS"
echo "  Puerto servidor matriz: $PUERTO_MATRIZ"
echo "  Número de controladores: $N_CONTROLADORES"
echo ""

# Verificar que los ejecutables existen
if [ ! -f "./ServidorTareas" ]; then
    echo "Error: ServidorTareas no encontrado. Ejecuta 'make' primero."
    exit 1
fi

if [ ! -f "./ServidorMatriz" ]; then
    echo "Error: ServidorMatriz no encontrado. Ejecuta 'make' primero."
    exit 1
fi

if [ ! -f "./Controlador" ]; then
    echo "Error: Controlador no encontrado. Ejecuta 'make' primero."
    exit 1
fi

if [ ! -f "tareas.txt" ]; then
    echo "Error: tareas.txt no encontrado."
    exit 1
fi

# Limpiar archivos de log previos
rm -f servidor_tareas.log servidor_matriz.log controlador_*.log

echo "Iniciando sistema..."
echo ""

# Lanzar Servidor de Tareas
echo "1. Lanzando Servidor de Tareas..."
./ServidorTareas $PUERTO_TAREAS > servidor_tareas.log 2>&1 &
PID_TAREAS=$!
sleep 1

# Lanzar Servidor de Matriz
echo "2. Lanzando Servidor de Matriz..."
./ServidorMatriz $PUERTO_MATRIZ > servidor_matriz.log 2>&1 &
PID_MATRIZ=$!
sleep 1

# Lanzar Controladores
echo "3. Lanzando $N_CONTROLADORES controladores..."
declare -a PIDS_CONTROLADORES

for i in $(seq 0 $((N_CONTROLADORES-1))); do
    ./Controlador $IP $PUERTO_TAREAS $IP $PUERTO_MATRIZ > controlador_$i.log 2>&1 &
    PIDS_CONTROLADORES[$i]=$!
    echo "   - Controlador $i lanzado (PID: ${PIDS_CONTROLADORES[$i]})"
    sleep 0.2
done

echo ""
echo "Todos los procesos lanzados. Esperando finalización..."
echo ""
echo "Puedes monitorizar el progreso con:"
echo "  tail -f servidor_tareas.log"
echo "  tail -f servidor_matriz.log"
echo ""

# Esperar a que terminen todos los procesos
wait $PID_TAREAS
wait $PID_MATRIZ

for pid in "${PIDS_CONTROLADORES[@]}"; do
    wait $pid
done

echo ""
echo "=========================================="
echo "  SISTEMA FINALIZADO"
echo "=========================================="
echo ""
echo "Logs generados:"
echo "  - servidor_tareas.log"
echo "  - servidor_matriz.log"
echo "  - controlador_0.log ... controlador_9.log"
echo ""
echo "Ver resultados en servidor_matriz.log"