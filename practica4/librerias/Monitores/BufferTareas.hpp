#ifndef BUFFER_TAREAS_HPP
#define BUFFER_TAREAS_HPP

#include <mutex>
#include <condition_variable>
#include "../../tarea.hpp"
#include "../MultiBuffer/MultiBuffer.hpp"

using namespace std;

const int N_CONTROLLERS = 10;

class BufferTareas {
private:
    MultiBuffer<tarea, N_CONTROLLERS> buffer;
    bool ocupado[N_CONTROLLERS];
    
    mutex mtx;  // Exclusión mutua del monitor
    condition_variable huecoLibre[N_CONTROLLERS];
    condition_variable hayTarea[N_CONTROLLERS];

public:
    BufferTareas();
    
    void ponerTarea(tarea t, int id_buffer);
    tarea obtenerTarea(int id_controller);
    unsigned size();
};

#endif