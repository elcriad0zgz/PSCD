#include <iostream>
#include <thread>
#include <chrono>
#include "Socket.hpp"

using namespace std;

const int MAX = 4001;

int main(int argc, char* argv[]) {

    if (argc != 3) {
        cerr << "Ejecutar de la siguiente manera: ./cliente <IP> <puerto>" << endl;
        return 1;
    }
    
    const string MENS_FIN = "END OF SERVICE";
    string SERVER_ADDRESS = argv[1];
    int SERVER_PORT = atoi(argv[2]);

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
        return socket_fd;
    }

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
            chan.Close();
            exit(1);
        }

        if(mensaje != MENS_FIN) {
            // Buffer para almacenar la respuesta
            string buffer;

            // Recibimos la respuesta del servidor
            read_bytes = chan.Receive(socket_fd, buffer, MAX);

            // Mostramos la respuesta
            cout << "Mensaje enviado: '" << mensaje << "'" << endl;
            cout << "Numero de vocales: " << buffer << endl;
        }
    } while(mensaje != MENS_FIN);

    // Cerramos el socket
    int error_code = chan.Close();
    if(error_code == -1) {
        cerr << chan.error("Error cerrando el socket");
    }

    // Despedida
    cout << "Bye bye" << endl;

    return error_code;
}
