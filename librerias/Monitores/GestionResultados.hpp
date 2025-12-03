#ifndef GESTION_RESULTADOS_HPP
#define GESTION_RESULTADOS_HPP

#include <mutex>

using namespace std;

class GestionResultados {
private:
    float resultados[3][3];
    mutex mtx;  // Exclusión mutua del monitor

public:
    GestionResultados();
    
    void registrarEjecucion(int tipo_idx);
    void registrarExito(int tipo_idx, float carga);
    void obtenerResultados(float res[3][3]);
};

#endif