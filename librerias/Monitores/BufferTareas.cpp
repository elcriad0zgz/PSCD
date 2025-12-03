#include "BufferTareas.hpp"

BufferTareas::BufferTareas() {
    for (int i = 0; i < N_CONTROLLERS; i++) {
        ocupado[i] = false;
    }
}

void BufferTareas::ponerTarea(tarea t, int id_buffer) {
    unique_lock<mutex> lck(mtx);
    
    // Esperar a que haya hueco libre (WHILE, no IF)
    while (ocupado[id_buffer]) {
        huecoLibre[id_buffer].wait(lck);
    }
    
    // Depositar la tarea
    buffer.put(t, id_buffer);
    ocupado[id_buffer] = true;
    
    // Notificar que hay tarea disponible
    hayTarea[id_buffer].notify_one();
}

tarea BufferTareas::obtenerTarea(int id_controller) {
    unique_lock<mutex> lck(mtx);
    
    // Esperar hasta que haya tarea (WHILE, no IF)
    while (!ocupado[id_controller]) {
        hayTarea[id_controller].wait(lck);
    }
    
    // Tomar la tarea
    tarea t = buffer.get(id_controller);
    ocupado[id_controller] = false;
    
    // Notificar que hay hueco libre
    huecoLibre[id_controller].notify_one();
    
    return t;
}

unsigned BufferTareas::size() {
    lock_guard<mutex> lck(mtx);
    return buffer.size();
}