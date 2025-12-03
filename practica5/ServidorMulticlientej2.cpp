#include "Socket.hpp"
#include <iostream>
#include <thread>
#include <vector>
#include <set>

using namespace std;

const string MENS_FIN = "END OF SERVICE";
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
void servCliente(Socket& chan, int client_fd) {
    string MENS_FIN = "END OF SERVICE";
    // Buffer para recibir el mensaje
    int length = 100;
    string buffer;


    bool out = false; // Inicialmente no salir del bucle
    while(!out) {
        // Recibimos el mensaje del cliente
        int rcv_bytes = chan.Receive(client_fd,buffer,length);

        if (rcv_bytes == -1) {
            cerr << chan.error("Error al recibir datos");
            // Cerramos los sockets
            chan.Close(client_fd);
        }

        cout << "Mensaje recibido: " + buffer + "\n";

        // Si recibimos "END OF SERVICE" --> Fin de la comunicación
        if (buffer == MENS_FIN) {
            out = true; // Salir del bucle
        } else {
            // Contamos las vocales recibidas en el mensaje anterior
            int num_vocales = cuentaVocales(buffer);

            // Enviamos la respuesta
            string s = to_string(num_vocales);

            int send_bytes = chan.Send(client_fd, s);
            if(send_bytes == -1) {
                cerr << chan.error("Error al enviar datos");
                // Cerramos los sockets
                chan.Close(client_fd);
                exit(1);
            }
        }
    }
    chan.Close(client_fd);
}

//-------------------------------------------------------------
int main(int argc,char* argv[]) {

    if (argc != 2) {
        cerr << "Uso: ./servidor <puerto>\n";
        return 1;
    }

    int SERVER_PORT = atoi(argv[1]);

    Socket chan(SERVER_PORT);

    int socket_fd = chan.Bind();
    if (socket_fd == -1) {
        cerr << chan.error("Error en bind");
        return 1;
    }

    if (chan.Listen() == -1) {
        cerr << chan.error("Error en listen");
        return 1;
    }

    cout << "Servidor MULTICLIENTE escuchando en puerto " << SERVER_PORT << endl;

    vector<thread> hilos;

    while (true) {

        int client_fd = chan.Accept();

        if (client_fd == -1) {
            cerr << chan.error("Error en accept");
            continue;
        }

        cout << "Nuevo cliente aceptado: FD=" << client_fd << endl;

        hilos.emplace_back(servCliente, ref(chan), client_fd);
        hilos.back().detach();
    }

    chan.Close(socket_fd);
    return 0;
}