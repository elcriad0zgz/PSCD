#include <iostream>
#include <thread>
using namespace std;

const int N = 512;
const int N_BUSC = 8;

int v[N];                      // Vector de datos
int resultados[N_BUSC];        // Resultados parciales
bool datos_cargados = false;   // Señal para que los buscadores empiecen
int procesos_terminados = 0;   // Contador de procesos finalizados

// Función de búsqueda en un trozo del vector
void search(const int v[N], int i, int d, int value, int& result) {
    result = 0;
    for (int j = i; j <= d; ++j) {
        if (v[j] == value) result++;
    }
}

// Proceso buscador
void buscador(int id, int value) {
    // Espera activa hasta que el coordinador cargue los datos
    while (!datos_cargados) {
        // Pequeña pausa opcional para evitar saturar la CPU
        this_thread::yield();
    }

    // Calcula el rango de trabajo
    int inicio = (id - 1) * N / N_BUSC;
    int fin = id * N / N_BUSC - 1;
    if (id == N_BUSC) fin = N - 1;

    // Realiza la búsqueda
    search(v, inicio, fin, value, resultados[id - 1]);

    // Aviso al coordinador de que ha terminado
    procesos_terminados++;
}

// Proceso coordinador
void coordinador(int value) {
    // Inicializa el vector con todos unos
    for (int i = 0; i < N; ++i) v[i] = 1;

    // Activa la señal para que los buscadores empiecen
    datos_cargados = true;

    // Espera activa hasta que todos los buscadores terminen
    while (procesos_terminados < N_BUSC) {
        this_thread::yield();
    }

    // Suma los resultados
    int total = 0;
    for (int i = 0; i < N_BUSC; ++i)
        total += resultados[i];

    // Muestra el resultado final
    cout << "Total de ocurrencias del valor " << value << ": " << total << endl;
}

int main() {
    int value = 1; // Valor a buscar

    // Lanzamos los procesos buscadores
    thread buscadores[N_BUSC];
    for (int i = 1; i <= N_BUSC; ++i)
        buscadores[i - 1] = thread(buscador, i, value);

    // Lanzamos el proceso coordinador
    thread coord(coordinador, value);

    // Esperamos a que todos los hilos terminen
    for (int i = 0; i < N_BUSC; ++i)
        buscadores[i].join();
    coord.join();

    return 0;
}
