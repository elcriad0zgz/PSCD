//******************************************************************
// File:   ServidorMulticliente.cpp
// Author: PSCD-Unizar
// Date:   Noviembre 2022
// Coms:   Ejercicio previo 2 - Servidor multicliente
//         Ejemplo de servidor multicliente con comunicación síncrona mediante sockets
//         Modificado para aceptar puerto como parámetro
//         Invocación:
//                  ./ServidorMulticliente <puerto>
//*****************************************************************

#include "Socket/Socket.hpp"
#include <iostream>
#include <thread>
#include <vector>
#include <set>

using namespace std;

const int MESSAGE_SIZE = 4001;

//-------------------------------------------------------------
// Cuenta el número de vocales existentes en un mensaje
// PRE:
// POST: Devuelve el número de vocales existentes en el mensaje 'message'
int cuentaVocales(string message) {
    set<char> vocales = {'a','A','e','E','i','I','o','O','u','U'};
    int count = 0;

    for (int i=0; i < message.length(); i++) {
        if (vocales.find(message[i]) != vocales.end()) {
            count++;
        }
    }
    return count;
}

//-------------------------------------------------------------
// Espera "secs" segundos y se conecta. Usado para desbloquear un "accept"
// Y pone "fin" a true
void timeOut(int secs, bool& fin, string address, int port) {
    Socket chan(address, port);
    this_thread::sleep_for(chrono::seconds(secs));
    fin = true;
    int sfd = chan.Connect(); //sacará al servidor del "Accept"
    if (sfd != -1) {
        chan.Close(sfd);
    }
}

//-------------------------------------------------------------
// Atiende a un cliente
void servCliente(Socket& chan, int client_fd, int num_cliente) {
    string MENS_FIN = "END OF SERVICE";
    string buffer;

    cout << "Thread para cliente " << num_cliente << " iniciado" << endl;

    bool out = false; // Inicialmente no salir del bucle
    while(!out) {
        // Recibimos el mensaje del cliente
        int rcv_bytes = chan.Receive(client_fd, buffer, MESSAGE_SIZE);

        if (rcv_bytes == -1) {
            cerr << "Error al recibir datos del cliente " << num_cliente << endl;
            break;
        }

        cout << "Cliente " << num_cliente << " - Mensaje recibido: '" << buffer << "'" << endl;

        // Si recibimos "END OF SERVICE" --> Fin de la comunicación
        if (buffer == MENS_FIN) {
            out = true; // Salir del bucle
            cout << "Cliente " << num_cliente << " finalizó" << endl;
        } else {
            // Contamos las vocales recibidas en el mensaje anterior
            int num_vocales = cuentaVocales(buffer);

            // Enviamos la respuesta
            string s = to_string(num_vocales);

            int send_bytes = chan.Send(client_fd, s);
            
            if(send_bytes == -1) {
                cerr << "Error al enviar datos al cliente " << num_cliente << endl;
                break;
            }
            
            cout << "Cliente " << num_cliente << " - Respuesta enviada: " << s << endl;
        }
    }
    
    // Cerrar socket del cliente
    chan.Close(client_fd);
    cout << "Cliente " << num_cliente << " desconectado" << endl;
}

//-------------------------------------------------------------
int main(int argc, char* argv[]) {
    // Puerto donde escucha el proceso servidor
    int SERVER_PORT;
    
    if (argc == 2) {
        SERVER_PORT = atoi(argv[1]);
    } else if (argc == 1) {
        SERVER_PORT = 3000;
        cout << "Uso: " << argv[0] << " <puerto>" << endl;
        cout << "Usando puerto por defecto: 3000" << endl;
    } else {
        cerr << "Uso: " << argv[0] << " <puerto>" << endl;
        return 1;
    }
    
    vector<thread> clientes;
    bool fin = false;

    cout << "Servidor multicliente escuchando en puerto " << SERVER_PORT << endl;

    // Creación del socket con el que se llevará a cabo
    // la comunicación con el servidor.
    Socket chan(SERVER_PORT);

    // bind
    int socket_fd = chan.Bind();
    if (socket_fd == -1) {
        cerr << chan.error("Error en el bind");
        exit(1);
    }
    cout << "Bind realizado correctamente" << endl;

    //listen
    int error_code = chan.Listen();
    if (error_code == -1) {
        cerr << chan.error("Error en el listen");
        // Cerramos el socket
        chan.Close(socket_fd);
        exit(1);
    }
    cout << "Servidor esperando clientes..." << endl;
    cout << "(El servidor se cerrará automáticamente después de 60 segundos sin actividad)" << endl;

    // Para desbloquear servidor y terminar después de 60 segundos
    thread timeControl(&timeOut, 60, ref(fin), "localhost", SERVER_PORT);

    int num_cliente = 0;
    while (!fin) {
        // Accept
        int new_client_fd = chan.Accept();

        if(new_client_fd == -1) {
            if (!fin) {
                cerr << chan.error("Error en el accept");
            }
        } else {
            if (!fin) {
                num_cliente++;
                // Introducir en el vector el cliente y arrancar el thread
                clientes.push_back(thread(&servCliente, ref(chan), new_client_fd, num_cliente));
                cout << "Nuevo cliente " << num_cliente << " aceptado" << endl;
            } else {
                cout << "Timeout alcanzado, cerrando servidor..." << endl;
                chan.Close(new_client_fd);
            }
        }
    }

    // Esperar a que terminen todos los threads de clientes
    cout << "Esperando a que terminen todos los clientes..." << endl;
    for (int i = 0; i < clientes.size(); i++) {
        if (clientes[i].joinable()) {
            clientes[i].join();
        }
    }
    timeControl.join();

    // Cerramos el socket del servidor
    error_code = chan.Close(socket_fd);
    if (error_code == -1) {
        cerr << chan.error("Error cerrando el socket del servidor");
    }

    // Despedida
    cout << "Total de clientes atendidos: " << num_cliente << endl;
    cout << "Bye bye" << endl;

    return error_code;
}
//-------------------------------------------------------------