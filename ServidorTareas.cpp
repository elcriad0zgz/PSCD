//*****************************************************************
// File:   ServidorTareas.cpp
// Author: Práctica 5 PSCD
// Date:   diciembre 2025
// Coms:   Servidor de tareas que gestiona el multibuffer y 
//         distribuye tareas a los controladores
//*****************************************************************

#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <vector>
#include <string>
#include "Socket/Socket.hpp"
#include "tarea.hpp"
#include "librerias/Monitores/BufferTareas.hpp"

using namespace std;

const int MESSAGE_SIZE = 4001;

// Thread que representa a un controlador en el servidor
void representanteControlador(Socket& chan, int client_fd, int id_controller, 
                              BufferTareas& monitorBuffer) {
    bool seguir = true;
    
    while (seguir) {
        // Esperar a que haya tarea disponible
        tarea t = monitorBuffer.obtenerTarea(id_controller);
        
        // Serializar la tarea para enviarla
        string mensaje = t.tipoTarea + "," + to_string(t.cargaDeTrabajo);
        
        // Enviar tarea al controlador
        int send_bytes = chan.Send(client_fd, mensaje);
        if (send_bytes == -1) {
            cerr << "Error enviando tarea al controlador " << id_controller << endl;
            break;
        }
        
        // Si es tarea final, esperar confirmación "END" del controlador
        if (t.tipoTarea == "TF") {
            string buffer;
            int rcv_bytes = chan.Receive(client_fd, buffer, MESSAGE_SIZE);
            if (rcv_bytes == -1 || buffer != "END") {
                cerr << "Error recibiendo END del controlador " << id_controller << endl;
            }
            seguir = false;
        }
    }
    
    cout << "Representante del controlador " << id_controller << " finalizado" << endl;
}

// Thread master que introduce tareas en el buffer
void masterTask(BufferTareas& monitorBuffer, vector<tarea>& listaTareas) {
    unsigned buffer_actual = 0;
    
    for (size_t i = 0; i < listaTareas.size(); i++) {
        tarea t = listaTareas[i];
        monitorBuffer.ponerTarea(t, buffer_actual);
        buffer_actual = (buffer_actual + 1) % monitorBuffer.size();
    }
    
    // Poner tareas de fin en cada buffer
    tarea tfin;
    tfin.tipoTarea = "TF";
    tfin.cargaDeTrabajo = 0;

    for (unsigned j = 0; j < monitorBuffer.size(); j++) {
        monitorBuffer.ponerTarea(tfin, j);
    }
    
    cout << "Master: Todas las tareas distribuidas" << endl;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Uso: " << argv[0] << " <puerto>" << endl;
        return 1;
    }
    
    int SERVER_PORT = atoi(argv[1]);
    
    // Leer tareas desde tareas.txt
    vector<tarea> listaTareas;
    ifstream f("tareas.txt");
    if (!f.is_open()) {
        cerr << "Error: No se puede abrir el archivo tareas.txt" << endl;
        return 1;
    }

    string linea;
    while (getline(f, linea)) {
        if (linea.empty()) continue;
        
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
    cout << "Servidor de tareas escuchando en puerto " << SERVER_PORT << endl;
    
    // Crear monitor del buffer
    BufferTareas monitorBuffer;
    
    // Crear socket del servidor
    Socket chan(SERVER_PORT);
    
    // Bind y Listen
    int socket_fd = chan.Bind();
    if (socket_fd == -1) {
        cerr << "Error en Bind" << endl;
        return 1;
    }
    
    int error_code = chan.Listen();
    if (error_code == -1) {
        cerr << "Error en Listen" << endl;
        chan.Close(socket_fd);
        return 1;
    }
    
    // Aceptar N_CONTROLLERS controladores
    vector<thread> representantes;
    vector<int> client_fds;
    
    cout << "Esperando conexión de " << N_CONTROLLERS << " controladores..." << endl;
    
    for (int i = 0; i < N_CONTROLLERS; i++) {
        int client_fd = chan.Accept();
        if (client_fd == -1) {
            cerr << "Error aceptando controlador " << i << endl;
            continue;
        }
        
        cout << "Controlador " << i << " conectado" << endl;
        client_fds.push_back(client_fd);
        
        // Lanzar thread representante
        representantes.push_back(thread(representanteControlador, ref(chan), 
                                       client_fd, i, ref(monitorBuffer)));
    }
    
    cout << "Todos los controladores conectados. Iniciando distribución de tareas..." << endl;
    
    // Lanzar thread master
    thread master(masterTask, ref(monitorBuffer), ref(listaTareas));
    
    // Esperar a que terminen todos
    master.join();
    
    for (auto& rep : representantes) {
        rep.join();
    }
    
    // Cerrar sockets de clientes
    for (int fd : client_fds) {
        chan.Close(fd);
    }
    
    // Cerrar socket del servidor
    chan.Close(socket_fd);
    
    cout << "Servidor de tareas finalizado" << endl;
    
    return 0;
}