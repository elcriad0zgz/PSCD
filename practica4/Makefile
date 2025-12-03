#*****************************************************************
# File:   Makefile
# Author: PSCD-Práctica 4
# Date:   noviembre 2025
# Coms:   Compilación de la práctica 4 con monitores
#*****************************************************************

# Compilador y flags
CXX = g++
CXXFLAGS = -std=c++11 -Wall -pthread -I.
LDFLAGS = -pthread

# Directorios
MONITOR_DIR = librerias/Monitores
MULTIBUFFER_DIR = librerias/MultiBuffer

# Archivos objeto
OBJS = practica4.o \
       $(MONITOR_DIR)/BufferTareas.o \
       $(MONITOR_DIR)/GestionResultados.o

# Ejecutable
TARGET = practica4

# Regla principal
all: $(TARGET)

# Enlazar ejecutable
$(TARGET): $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $^
	@echo "Compilacion exitosa: ./$(TARGET)"

# Compilar practica4.cpp
practica4.o: practica4.cpp tarea.hpp \
             $(MONITOR_DIR)/BufferTareas.hpp \
             $(MONITOR_DIR)/GestionResultados.hpp \
             $(MULTIBUFFER_DIR)/MultiBuffer.hpp
	$(CXX) $(CXXFLAGS) -c practica4.cpp

# Compilar BufferTareas
$(MONITOR_DIR)/BufferTareas.o: $(MONITOR_DIR)/BufferTareas.cpp \
                                $(MONITOR_DIR)/BufferTareas.hpp \
                                tarea.hpp \
                                $(MULTIBUFFER_DIR)/MultiBuffer.hpp
	$(CXX) $(CXXFLAGS) -c $(MONITOR_DIR)/BufferTareas.cpp -o $@

# Compilar GestionResultados
$(MONITOR_DIR)/GestionResultados.o: $(MONITOR_DIR)/GestionResultados.cpp \
                                     $(MONITOR_DIR)/GestionResultados.hpp
	$(CXX) $(CXXFLAGS) -c $(MONITOR_DIR)/GestionResultados.cpp -o $@

# Limpiar archivos compilados
clean:
	rm -f $(TARGET) *.o $(MONITOR_DIR)/*.o
	@echo "✓ Limpieza completada"

# Limpiar y recompilar
rebuild: clean all

# Ejecutar el programa
run: $(TARGET)
	./$(TARGET)

# Ejecutar con valgrind para detectar memory leaks
valgrind: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET)

# Crear archivo de tareas de ejemplo (si no existe)
tareas.txt:
	@echo "t1,1.5" > tareas.txt
	@echo "t2,2.3" >> tareas.txt
	@echo "t3,3.1" >> tareas.txt
	@echo "t1,1.8" >> tareas.txt
	@echo "t2,2.5" >> tareas.txt
	@echo "t3,3.4" >> tareas.txt
	@echo "t1,1.2" >> tareas.txt
	@echo "t2,2.1" >> tareas.txt
	@echo "t3,3.8" >> tareas.txt
	@echo "t1,1.9" >> tareas.txt
	@echo "✓ Archivo tareas.txt creado"

# Test completo
test: tareas.txt $(TARGET)
	@echo "=== Ejecutando prueba ==="
	./$(TARGET)

# Ayuda
help:
	@echo "Makefile para Práctica 4 - Monitores"
	@echo ""
	@echo "Objetivos disponibles:"
	@echo "  all        - Compilar el proyecto (default)"
	@echo "  clean      - Eliminar archivos compilados"
	@echo "  rebuild    - Limpiar y recompilar"
	@echo "  run        - Compilar y ejecutar"
	@echo "  test       - Crear tareas.txt y ejecutar"
	@echo "  valgrind   - Ejecutar con valgrind"
	@echo "  help       - Mostrar esta ayuda"

.PHONY: all clean rebuild run valgrind test help