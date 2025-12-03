...
#include <Semaphore_V4.hpp>
#include <MultiBuffer.hpp>

using namespace std;
const int N_CONTROLLERS = 10;
//----------------------------------------------------
struct tarea {
    string tipoTarea;
    float cargaDeTrabajo;
};
...
//----------------------------------------------------
void masterTask(...) {
    while (haya tareas pendientes) {
        //poner nueva tarea en buffer libre, cuando lo haya
    }
    //poner tareas de fin en cada buffer
    //mostrar matriz de resultados cuando se pueda
}
//----------------------------------------------------
//Pre:  0 <= id < N_CONTROLLERS
void controllerTask(unsigned int id,...) {
    bool seguir = true;
    while (seguir) {
        //tomar tarea del buffer correspondiente cuando la haya
        //ejecutarla
        //completar datos en la matriz de resultados
        ...
    }
}
//----------------------------------------------------
int main(int argc, char *argv[]) {
    MultiBuffer<tarea,N_CONTROLLERS> mBT; //multi-buffer de tareas
	...
    return 0;
}