//*****************************************************************
// File:   MultiBuffer.hpp
// Author: PSCD-Unizar
// Date:   octubre 2025
// Coms:   Especificación de un buffer genérico con "n" datos de tipo "T"
//         El estado de un buffer lo representaremos como 
//            ([v_{0},...,v_{N-1}],{...})
//         donde:
//             "v_{i}" el valor en la posición "i". Si un dato está indefinido
//                  se representará como "?"
//             "{...}" representa el conjunto de índices del vector
//                  de tareas que contiene tareas preparadas en el
//                  buffer correspondiente, para ser procesadas
//*****************************************************************

#ifndef MULTI_BUFFER
#define MULTI_BUFFER

#include <iostream>
#include <string>
#include <set> //TAD para conjuntos de C++

using namespace std;

//1<=N
template <typename T,unsigned N>
class MultiBuffer {
public:
    //-----------------------------------------------------
    //Pre: 
    //Post:
    MultiBuffer();
    //-----------------------------------------------------
    //Pre:  
    //Post: 
    //Com:  Innecesario en este caso. Se pone por completitud
    ~MultiBuffer();
    //-----------------------------------------------------
    //Pre:  0 <= pos < N
    //Post: get(pos)=v_{pos}, y 'pos' es eliminado de '{...}', si está
    //Com:  
    T get(const unsigned pos);
    //-----------------------------------------------------
    //Pre:  0 <= pos < N
    //Post: watch(pos)=v_{pos}
    //Com:  
    T watch(const unsigned pos);
    //-----------------------------------------------------
    //Pre:  0 <= pos < N
    //Post: v_{pos}=d, 'pos' es añadido a '{...}'
    //Com:  
    void put(const T d,const unsigned pos);
    //-----------------------------------------------------
    //Pre:  
    //Post: numberNews()=cardinal del conjunto '{...}'
    unsigned numberNews();
     //-----------------------------------------------------
    //Pre:  
    //Post: size()=N
    //Com:  
    unsigned size();
    //-----------------------------------------------------
    //Pre:  0 <= pos < N
    //Post: isNew(pos)=¿está 'pos' en '{...}'?
    //Com:  
    bool isNew(const unsigned pos);
    //----------------------------------------------------- 
private:
    T tasks[N];
    set<int> news; //conjunto con índices del buffer con tareas pendientes
};
// Para poder trabajar con tipos genéricos mediante "templates",
// declaración y código deben estar juntos
#include "MultiBuffer.cpp"
#endif