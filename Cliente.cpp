//*****************************************************************
// File:   Cliente.cpp
// Author: PSCD-Unizar
// Date:   noviembre 2022
// Coms:   Ejercicio previo 1 - Cliente básico
//         Ejemplo de cliente con comunicación síncrona mediante sockets
//         Modificado para aceptar IP y puerto como parámetros
//         Invocación:
//                  ./Cliente <IP_servidor> <puerto_servidor>
//*****************************************************************
#include <iostream>
#include <chrono>
#include <thread>
#include "Socket/Socket.hpp"

using namespace std;

const int MESSAGE_SIZE = 4001; //mensajes de no más 4000 caracteres

int main(int argc, char* argv[]) {
    const string MENS_FIN = "END OF SERVICE";
    
    // Verificar parámetros
    string SERVER_ADDRESS;
    int SERVER_PORT;
    
    if (argc == 3) {
        // Modo con parámetros: IP y puerto
        SERVER_ADDRESS = argv[1];
        SERVER_PORT = atoi(argv[2]);
    } else if (argc == 1) {
        // Modo por defecto: localhost
        SERVER_ADDRESS = "localhost";
        SERVER_PORT = 3000;
        cout << "Uso: " << argv[0] << " <IP_servidor> <puerto>" << endl;
        cout << "Usando valores por defecto: localhost:3000" << endl;
    } else {
        cerr << "Uso: " << argv[0] << " <IP_servidor> <puerto>" << endl;
        return 1;
    }

    cout << "Intentando conectar a " << SERVER_ADDRESS << ":" << SERVER_PORT << endl;

    // Creación del socket con el que se llevará a cabo
    // la comunicación con el servidor.
    Socket chan(SERVER_ADDRESS, SERVER_PORT);

    // Conectamos con el servidor. Probamos varias conexiones
    const int MAX_ATTEMPS = 10;
    int count = 0;
    int socket_fd;
    do {
        // Conexión con el servidor
        socket_fd = chan.Connect();
        count++;

        // Si error --> esperamos 1 segundo para reconectar
        if(socket_fd == -1) {
            this_thread::sleep_for(chrono::seconds(1));
        }
    } while(socket_fd == -1 && count < MAX_ATTEMPS);

    // Chequeamos si se ha realizado la conexión
    if(socket_fd == -1) {
        cerr << "No se pudo conectar al servidor después de " << MAX_ATTEMPS << " intentos" << endl;
        return socket_fd;
    }

    cout << "Conectado al servidor correctamente" << endl;
    cout << "Escribe frases para contar vocales ('" << MENS_FIN << "' para terminar)" << endl;

    string mensaje;
    int read_bytes;   //num de bytes recibidos en un mensaje
    int send_bytes;  //num de bytes enviados en un mensaje

    do {
        // Leer mensaje de la entrada estandar
        do {
            cout << "Frase para contar las vocales: ";
            getline(cin, mensaje);
        } while (mensaje == ""); //no se puede enviar mensaje vacío
        
        // Enviamos el mensaje
        send_bytes = chan.Send(socket_fd, mensaje);

        if(send_bytes == -1) {
            cerr << chan.error("Error al enviar datos");
            // Cerramos el socket
            chan.Close(socket_fd);
            exit(1);
        }

        if(mensaje != MENS_FIN) {
            // Buffer para almacenar la respuesta
            string buffer;

            // Recibimos la respuesta del servidor
            read_bytes = chan.Receive(socket_fd, buffer, MESSAGE_SIZE);

            if(read_bytes == -1) {
                cerr << chan.error("Error al recibir datos");
                chan.Close(socket_fd);
                exit(1);
            }

            // Mostramos la respuesta
            cout << "Mensaje enviado: '" << mensaje << "'" << endl;
            cout << "Numero de vocales: " << buffer << endl;
        }
    } while(mensaje != MENS_FIN);

    // Cerramos el socket
    int error_code = chan.Close(socket_fd);
    if(error_code == -1) {
        cerr << chan.error("Error cerrando el socket");
    }

    // Despedida
    cout << "Bye bye" << endl;

    return error_code;
}