#include "GestionResultados.hpp"

GestionResultados::GestionResultados() {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            resultados[i][j] = 0;
        }
    }
}

void GestionResultados::registrarEjecucion(int tipo_idx) {
    lock_guard<mutex> lck(mtx);
    resultados[tipo_idx][0]++;
}

void GestionResultados::registrarExito(int tipo_idx, float carga) {
    lock_guard<mutex> lck(mtx);
    resultados[tipo_idx][1]++;
    resultados[tipo_idx][2] += carga;
}

void GestionResultados::obtenerResultados(float res[3][3]) {
    lock_guard<mutex> lck(mtx);
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            res[i][j] = resultados[i][j];
        }
    }
}