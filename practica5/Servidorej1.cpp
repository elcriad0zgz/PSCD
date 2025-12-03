#include <iostream>
#include <set>
#include "Socket.hpp"

using namespace std;

const int MAX = 4001;

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

int main(int argc, char* argv[]) {

    if (argc != 2) {
        cerr << "Ejecutar de la siguiente manera: ./servidor <puerto>" << endl;
        return 1;
    }
    const string MENS_FIN = "END OF SERVICE";
    int SERVER_PORT = atoi(argv[1]);

    // Creación del socket con el que se llevará a cabo
    // la comunicación con el servidor.
    Socket chan(SERVER_PORT);


    // Bind
    int socket_fd =chan.Bind();
    if (socket_fd == -1) {
        cerr << chan.error("Error en el bind");
        exit(1);
    }

    // Listen
    int error_code = chan.Listen();
    if (error_code == -1) {
        cerr << chan.error("Error en el lisen");
        // Cerramos el socket
        chan.Close(socket_fd);
        exit(1);
    }

    // Accept
    int client_fd = chan.Accept();
    if (client_fd == -1) {
        cerr << chan.error("Error en el accept");
        // Cerramos el socket
        chan.Close(socket_fd);
        exit(1);
    }

    // Buffer para recibir el mensaje
    int length = 100;
    string buffer;
    int rcv_bytes;   //num de bytes recibidos en un mensaje
    int send_bytes;  //num de bytes enviados en un mensaje

    bool out = false; // Inicialmente no salir del bucle
    while (!out) {
        // Recibimos el mensaje del cliente
        rcv_bytes = chan.Receive(client_fd, buffer, MAX);

        if(rcv_bytes == -1) {
            cerr << chan.error("Error al recibir datos");
            // Cerramos los sockets
            chan.Close(client_fd);
            chan.Close(socket_fd);
        }

        cout << "Mensaje recibido: '" << buffer << "':" << rcv_bytes << endl;

        // Si recibimos "END OF SERVICE" --> Fin de la comunicación
        if (buffer == MENS_FIN) {
            out = true; // Salir del bucle
        } else {
            // Contamos las vocales recibidas en el mensaje anterior
            int num_vocales = cuentaVocales(buffer);

            // Enviamos la respuesta
            string resp = to_string(num_vocales);

            send_bytes = chan.Send(client_fd, resp);

            cout << "Mensaje enviado: '" << resp << "':" << send_bytes << endl;

            if(send_bytes == -1) {
                cerr << chan.error("Error al enviar datos");
                // Cerramos los sockets
                chan.Close(client_fd);
                chan.Close(socket_fd);
                exit(1);
            }
        }
    }

    // Cerramos el socket del cliente
    cerr << "Cerrando cliente : " + to_string(client_fd) + "\n";
    error_code = chan.Close(client_fd);
    if (error_code == -1) {
        cerr << chan.error("Error cerrando el socket del cliente");
    }

    // Cerramos el socket del servidor
    cerr << "Cerrando servidor : " + to_string(socket_fd) + "\n";
    error_code = chan.Close(socket_fd);
    if (error_code == -1) {
        cerr << chan.error("Error cerrando el socket del servidor");
    }
    // Mensaje de despedida
    cout << "Bye bye" << endl;

    return error_code;
}
