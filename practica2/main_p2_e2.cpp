#include <iostream>
#include <thread>
#include <atomic>
using namespace std;

const int N = 512;       // Tamaño del vector
const int N_BUSC = 8;    // Número de procesos buscadores

int v[N];                      // Vector de datos compartido
bool datos_cargados = false;   // Señal para que los buscadores empiecen
int procesos_terminados = 0;   // Contador de procesos finalizados

// Variables compartidas para el máximo encontrado
int maxVeces = 0;              // Máximo número de ocurrencias hallado
int indMin = -1, indMax = -1;  // Índices del trozo donde se encontró ese máximo
atomic_flag tas = ATOMIC_FLAG_INIT; // Flag para exclusión mutua (Test&Set)

//-----------------------------------------------------------
// Función que busca cuántas veces aparece "value" en [i, d]
// y actualiza los valores globales si encuentra un nuevo máximo
//-----------------------------------------------------------
void search(const int v[N], const int i, const int d, const int value,
            int& maxVeces, int& indMin, int& indMax, atomic_flag& tas) {

    int count = 0;
    for (int j = i; j <= d; ++j) {
        if (v[j] == value)
            count++;
    }

    // --- Zona crítica protegida con Test&Set ---
    while (tas.test_and_set()) ;  // Espera activa hasta que el flag esté libre

    if (count > maxVeces) {
        maxVeces = count;
        indMin = i;
        indMax = d;
    }

    tas.clear();  // Libera el flag (fin de la sección crítica)
}

//-----------------------------------------------------------
// Proceso buscador
// Cada uno trabaja sobre un trozo distinto del vector
//-----------------------------------------------------------
void buscador(int id, int value) {
    // Espera activa hasta que el coordinador cargue los datos
    while (!datos_cargados) {
        this_thread::yield();
    }

    // Cálculo del rango que le corresponde
    int inicio = (id - 1) * N / N_BUSC;
    int fin = id * N / N_BUSC - 1;
    if (id == N_BUSC) fin = N - 1;

    // Realiza la búsqueda en su trozo y actualiza el máximo si corresponde
    search(v, inicio, fin, value, maxVeces, indMin, indMax, tas);

    // Aviso al coordinador de que ha terminado
    procesos_terminados++;
}

//-----------------------------------------------------------
// Proceso coordinador
//-----------------------------------------------------------
void coordinador(int value) {
    // Inicializa el vector con datos (puede cargarse desde archivo)
    for (int i = 0; i < N; ++i)
        v[i] = i % 5; // valores 0,1,2,3,4 repetidos

    // Activa la señal para que los buscadores empiecen
    datos_cargados = true;

    // Espera activa hasta que todos los buscadores terminen
    while (procesos_terminados < N_BUSC) {
        this_thread::yield();
    }

    // Muestra el resultado final
    cout << "Valor buscado: " << value << endl;
    cout << "Máximo número de ocurrencias: " << maxVeces << endl;
    cout << "Rango del vector donde se encontró: [" 
         << indMin << ", " << indMax << "]" << endl;
}

//-----------------------------------------------------------
// Función principal
//-----------------------------------------------------------
int main() {
    int value;
    cout << "Introduce el valor a buscar: ";
    cin >> value;

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
