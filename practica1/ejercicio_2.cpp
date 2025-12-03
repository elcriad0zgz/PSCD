//------------------------------------------------------------------------------
// File:   ejercicio_2.cpp
// Author: David Criado Díez
// Date:   septiembre 2025
// Coms:   Parte de la práctica 1 de PSCD
//         Compilar mediante
//           g++ ejercicio_2.cpp -o ejercicio_2 -std=c++11 -lpthread
//------------------------------------------------------------------------------

#include <iostream>
#include <thread>
#include <string>
#include <chrono>
#include <cstdlib>
#include <ctime>

using namespace std;

int randInt(const int a, const int b){

    return a + rand() % (b-a+1);
}

void saludo(int id, int retardo, int veces) {
    for(int i=1; i<=veces; i++) {
        // cout << "Soy " << nombre << endl;
        cout << "Soy " << id << "\n";
        //el thread que me ejecuta se bloquea durante "retardo" milisegundos
        this_thread::sleep_for(chrono::milliseconds(retardo));
    }
}

int main(int argc, char* argv[]) {
    const int N = 10;
    srand(time(NULL));
    thread P[N]; //de momento, ningún thread se pone en marcha
    int retardo = randInt(100, 300);
    int veces = randInt(5, 15);

    for (unsigned i = 0; i < N; i++){
 
        P[i] = thread(&saludo, i, retardo, veces);
    }

    for (int i=0; i<N; i++) { 
        P[i].join(); //me bloqueo hasta que "P[i]" termine
    }

    cout << "Fin\n";
    return 0;
}