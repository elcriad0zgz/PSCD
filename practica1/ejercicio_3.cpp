//------------------------------------------------------------------------------
// File:   ejercicio_3.cpp
// Author: David Criado Díez
// Date:   septiembre 2025
// Coms:   Parte de la práctica 1 de PSCD
//         Compilar mediante
//           g++ ejercicio_3.cpp -o ejercicio_3 -std=c++11 -pthread
//------------------------------------------------------------------------------

#include <iostream>
#include <thread>
#include <string>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <cmath>

using namespace std;

const int DIM = 100;

// Inicializa el vector con valores (aquí todos unos)
void inicializarVector(double reales[DIM]) {
    for (unsigned i = 0; i < DIM; i++) {
        reales[i] = 1;
    }
}

// Calcula la media
void media(double reales[DIM], double &resultado) {
    double suma = 0.0;
    for (unsigned i = 0; i < DIM; i++) {
        suma = suma + reales[i];
    }
    resultado = suma / DIM;
}

// Calcula el máximo y el mínimo
void maxmin(double reales[DIM], double &max, double &min) {
    max = reales[0];
    min = reales[0];
    for (unsigned i = 0; i < DIM; i++) {
        if (reales[i] > max) max = reales[i];
        if (reales[i] < min) min = reales[i];
    }
}

// Calcula la desviación típica
void sigma(double reales[DIM], double media, double &desv) {
    double sumatorio = 0;
    for (unsigned i = 0; i < DIM; i++) {
        sumatorio = sumatorio + ((reales[i] - media) * (reales[i] - media));
    }
    desv = sqrt(sumatorio / DIM);
}

int main() {
    const int N = 3;
    thread P[N];
    double reales[DIM];
    double max = 0.0;
    double min = 0.0;
    double med = 0.0;   // ← renombrada para no chocar con la función media
    double desv = 0.0;

    inicializarVector(reales);

    // Hilo 0: calcular la media
    P[0] = thread(media, reales, ref(med));
    P[0].join(); // esperar a que termine ANTES de lanzar sigma

    // Hilo 1: calcular máximo y mínimo
    P[1] = thread(maxmin, reales, ref(max), ref(min));

    // Hilo 2: calcular desviación típica (necesita la media ya calculada)
    P[2] = thread(sigma, reales, med, ref(desv));

    // Esperar a que terminen los hilos 1 y 2
    for (int i = 1; i <= 2; i++) {
        P[i].join();
    }

    // Mostrar resultados
    cout << "# datos: " << DIM << endl;
    cout << "media: " << med << endl;
    cout << "max: " << max << endl;
    cout << "min: " << min << endl;
    cout << "sigma: " << desv << endl;

    return 0;
}
