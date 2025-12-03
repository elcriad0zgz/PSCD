#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <vector>
#include <string>
#include <iomanip>
#include <cstdlib>
#include <ctime>
#include "tarea.hpp"
#include "librerias/Monitores/BufferTareas.hpp"
#include "librerias/Monitores/GestionResultados.hpp"

using namespace std;

int randInt(const int a, const int b) {
    return a + rand() % (b - a + 1);
}

void masterTask(BufferTareas& monitorBuffer, vector<tarea>& listaTareas) {

    unsigned buffer_actual = 0;
    
    for (size_t i = 0; i < listaTareas.size(); i++) {
        tarea t = listaTareas[i];
        
        // Poner tarea en el buffer actual
        // ponerTarea se bloqueará automáticamente si está ocupado
        monitorBuffer.ponerTarea(t, buffer_actual);
        
        // Pasar al siguiente buffer
        buffer_actual = (buffer_actual + 1) % monitorBuffer.size();
    }
    
    // Poner tareas de fin en cada buffer
    tarea tfin;
    tfin.tipoTarea = "TF";
    tfin.cargaDeTrabajo = 0;

    for (unsigned j = 0; j < monitorBuffer.size(); j++) {
        monitorBuffer.ponerTarea(tfin, j);
    }
}

void controllerTask(unsigned int id, BufferTareas& monitorBuffer, 
                    GestionResultados& monitorResultados) {
    bool seguir = true;
    
    while (seguir) {
        // Obtener tarea del monitor (se bloquea si no hay tarea)
        tarea t = monitorBuffer.obtenerTarea(id);

        if (t.tipoTarea == "TF") {
            seguir = false;  // Tarea final
        } else {
            int tipo_idx = -1;
            int umbral_exito = 0;
            
            if (t.tipoTarea == "t1") {
                tipo_idx = 0;
                umbral_exito = 5;   // 5% de fallo -> 95% éxito si error > 5
            } else if (t.tipoTarea == "t2") {
                tipo_idx = 1;
                umbral_exito = 7;   // 7% de fallo -> 93% éxito si error > 7
            } else if (t.tipoTarea == "t3") {
                tipo_idx = 2;
                umbral_exito = 10;  // 10% de fallo -> 90% éxito si error > 10
            }
            
            if (tipo_idx != -1) {
                // Registrar ejecución (se intentó ejecutar)
                monitorResultados.registrarEjecucion(tipo_idx);
                
                // Simular procesamiento con posibilidad de fallo
                int error = randInt(0, 100);
                
                // Si error > umbral, la tarea se ejecuta con éxito
                if (error > umbral_exito) {
                    // Registrar éxito con el tiempo de procesamiento
                    monitorResultados.registrarExito(tipo_idx, t.cargaDeTrabajo);
                }
                // Si error <= umbral, hay fallo (no se registra éxito)
            }
        }
    }
}

int main() {
    srand(time(NULL));

    // Crear monitores
    BufferTareas monitorBuffer;
    GestionResultados monitorResultados;

    // Leer tareas desde tareas.txt
    vector<tarea> listaTareas;
    ifstream f("tareas.txt");
    if (!f.is_open()) {
        cerr << "Error: No se puede abrir el archivo tareas.txt" << endl;
        return 1;
    }

    string linea;
    while (getline(f, linea)) {
        if (linea.empty()) continue;  // Ignorar líneas vacías
        
        stringstream ss(linea);
        string tipo;
        float carga;
        
        getline(ss, tipo, ',');
        ss >> carga;

        tarea t;
        t.tipoTarea = tipo;
        t.cargaDeTrabajo = carga;
        listaTareas.push_back(t);
    }
    f.close();

    if (listaTareas.empty()) {
        cerr << "Error: No se encontraron tareas en tareas.txt" << endl;
        return 1;
    }

    cout << "Cargadas " << listaTareas.size() << " tareas." << endl;
    cout << "Iniciando procesamiento con " << N_CONTROLLERS << " controladores..." << endl;

    // Crear controladores
    thread controladores[N_CONTROLLERS];
    for (unsigned i = 0; i < N_CONTROLLERS; i++) {
        controladores[i] = thread(controllerTask, i, ref(monitorBuffer), 
                                 ref(monitorResultados));
    }

    // Crear master
    thread master(masterTask, ref(monitorBuffer), ref(listaTareas));

    // Esperar a que terminen todos los procesos
    master.join();
    for (unsigned i = 0; i < N_CONTROLLERS; i++) {
        controladores[i].join();
    }

    cout << "Procesamiento completado." << endl << endl;

    // Obtener y mostrar resultados
    float resultados[3][3];
    monitorResultados.obtenerResultados(resultados);
    
    cout << "=================================" << endl;
    cout << "  RESULTADOS DEL PROCESAMIENTO" << endl;
    cout << "=================================" << endl;
    cout << "Tarea |  Total | Exito  | Tiempo" << endl;
    cout << "------+--------+--------+--------" << endl;
    
    for (int i = 0; i < 3; i++) {
        cout << "t" << (i+1) << "    | " 
             << setw(6) << fixed << setprecision(0) << resultados[i][0] << " | "
             << setw(6) << fixed << setprecision(0) << resultados[i][1] << " | "
             << setw(6) << fixed << setprecision(2) << resultados[i][2] << endl;
    }
    
    cout << "=================================" << endl;
    
    // Calcular y mostrar tasas de éxito
    cout << "Tasas de exito:" << endl;
    for (int i = 0; i < 3; i++) {
        if (resultados[i][0] > 0) {
            float tasa = (resultados[i][1] / resultados[i][0]) * 100.0;
            cout << "  t" << (i+1) << ": " << fixed << setprecision(1) 
                 << tasa << "%" << endl;
        }
    }

    return 0;
}