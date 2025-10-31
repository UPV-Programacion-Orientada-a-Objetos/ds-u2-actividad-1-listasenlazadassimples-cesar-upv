/**
 * @file Nodo.h
 * @brief Nodo simple usado por las listas genéricas de sensores.
 */
#ifndef NODO_H
#define NODO_H

/**
 * @brief Nodo sencillo para listas enlazadas.
 */
template <typename T>
struct Nodo
{
    /// Dato almacenado en este nodo.
    T dato;
    /// Apuntador al siguiente nodo de la lista.
    Nodo<T>* siguiente;

    /// Construye un nodo con el valor indicado.
    explicit Nodo(const T& valor) : dato(valor), siguiente(nullptr) {}
};

#endif
