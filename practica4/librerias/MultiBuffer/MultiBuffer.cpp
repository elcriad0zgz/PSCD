//*****************************************************************
// File:   MultiBuffer.cpp
// Author: PSCD-Unizar
// Date:   octubre 2025
// Coms:   Implemetación de la clase
//*****************************************************************

#include <cassert>

//-----------------------------------------------------
template <typename T,unsigned N>
MultiBuffer<T,N>::MultiBuffer() {

}
//-----------------------------------------------------
template <typename T,unsigned N>
MultiBuffer<T,N>::~MultiBuffer() {

}
//-----------------------------------------------------
template <typename T,unsigned N>
T MultiBuffer<T,N>::get(const unsigned pos) {
    assert(0<=pos && pos<N); //innecesario, pues lo impone la pre. Pero por si acaso ....

    news.erase(pos);
    return tasks[pos];
}
//-----------------------------------------------------
template <typename T,unsigned N>
T MultiBuffer<T,N>::watch(const unsigned pos) {
    assert(0<=pos && pos<N);

    return tasks[pos];
}
//-----------------------------------------------------
template <typename T,unsigned N>
void MultiBuffer<T,N>::put(const T d,const unsigned pos) {
    assert(0<=pos && pos<N);

    tasks[pos] = d;
    news.insert(pos);
}
//-----------------------------------------------------
template <typename T,unsigned N>
unsigned MultiBuffer<T,N>::numberNews() {

    return news.size();
}
//-----------------------------------------------------
template <typename T,unsigned N>
unsigned MultiBuffer<T,N>::size() {

    return N;
}
//-----------------------------------------------------
template <typename T,unsigned N>
bool MultiBuffer<T,N>::isNew(const unsigned pos) {
    assert(0<=pos && pos<N);

    return news.count(pos) > 0;
}
//-----------------------------------------------------
