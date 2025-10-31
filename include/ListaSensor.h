/**
 * @file ListaSensor.h
 * @brief Lista enlazada simple genérica para lecturas de sensores.
 */
#ifndef LISTASENSOR_H
#define LISTASENSOR_H

#include "Nodo.h"
#include <cstddef>

/**
 * @brief Lista enlazada simple que almacena lecturas de tipo T.
 */
template <typename T>
class ListaSensor
{
public:
    /// Construye una lista vacía.
    ListaSensor() : cabeza(nullptr) {}

    /// Copia el contenido de otra lista.
    ListaSensor(const ListaSensor& otra) : cabeza(nullptr)
    {
        copiarDesde(otra);
    }

    /// Asigna el contenido de otra lista.
    ListaSensor& operator=(const ListaSensor& otra)
    {
        if (this != &otra)
        {
            limpiar();
            copiarDesde(otra);
        }
        return *this;
    }

    /// Libera todos los nodos de la lista.
    ~ListaSensor()
    {
        limpiar();
    }

    /// Inserta un nuevo nodo al final de la lista.
    void insertarAlFinal(const T& valor)
    {
        Nodo<T>* nuevo = new Nodo<T>(valor);
        if (!cabeza)
        {
            cabeza = nuevo;
            return;
        }

        Nodo<T>* actual = cabeza;
        while (actual->siguiente)
        {
            actual = actual->siguiente;
        }
        actual->siguiente = nuevo;
    }

    /// Busca el primer nodo cuyo dato coincide con el valor.
    Nodo<T>* buscar(const T& valor) const
    {
        Nodo<T>* actual = cabeza;
        while (actual)
        {
            if (actual->dato == valor)
            {
                return actual;
            }
            actual = actual->siguiente;
        }
        return nullptr;
    }

    /// Elimina la primera coincidencia del valor solicitado.
    bool eliminarPrimeraCoincidencia(const T& valor)
    {
        Nodo<T>* actual = cabeza;
        Nodo<T>* anterior = nullptr;

        while (actual)
        {
            if (actual->dato == valor)
            {
                if (anterior)
                {
                    anterior->siguiente = actual->siguiente;
                }
                else
                {
                    cabeza = actual->siguiente;
                }
                delete actual;
                return true;
            }
            anterior = actual;
            actual = actual->siguiente;
        }
        return false;
    }

    /// Elimina todos los nodos almacenados.
    void limpiar()
    {
        Nodo<T>* actual = cabeza;
        while (actual)
        {
            Nodo<T>* siguiente = actual->siguiente;
            delete actual;
            actual = siguiente;
        }
        cabeza = nullptr;
    }

    /// Indica si la lista no contiene elementos.
    bool estaVacia() const
    {
        return cabeza == nullptr;
    }

    /// Extrae el primer nodo y devuelve su valor.
    bool extraerPrimero(T& valor)
    {
        if (!cabeza)
        {
            return false;
        }

        Nodo<T>* eliminado = cabeza;
        valor = eliminado->dato;
        cabeza = eliminado->siguiente;
        delete eliminado;
        return true;
    }

    /// Cuenta cuántos nodos forman la lista.
    int contar() const
    {
        int cantidad = 0;
        Nodo<T>* actual = cabeza;
        while (actual)
        {
            ++cantidad;
            actual = actual->siguiente;
        }
        return cantidad;
    }

    /// Calcula el promedio de los valores almacenados.
    double promedio() const
    {
        if (!cabeza)
        {
            return 0.0;
        }

        double suma = 0.0;
        int cantidad = 0;
        Nodo<T>* actual = cabeza;
        while (actual)
        {
            suma += static_cast<double>(actual->dato);
            ++cantidad;
            actual = actual->siguiente;
        }

        if (cantidad == 0)
        {
            return 0.0;
        }

        return suma / static_cast<double>(cantidad);
    }

    /// Obtiene el valor mínimo almacenado.
    bool obtenerMinimo(T& minimo) const
    {
        if (!cabeza)
        {
            return false;
        }

        Nodo<T>* actual = cabeza;
        minimo = actual->dato;
        actual = actual->siguiente;
        while (actual)
        {
            if (actual->dato < minimo)
            {
                minimo = actual->dato;
            }
            actual = actual->siguiente;
        }
        return true;
    }

    /// Devuelve el puntero al primer nodo de la lista.
    Nodo<T>* obtenerCabeza() const
    {
        return cabeza;
    }

private:
    /// Apuntador al primer nodo de la lista.
    Nodo<T>* cabeza;

    /// Copia todos los elementos de otra lista auxiliar.
    void copiarDesde(const ListaSensor& otra)
    {
        Nodo<T>* actual = otra.cabeza;
        while (actual)
        {
            insertarAlFinal(actual->dato);
            actual = actual->siguiente;
        }
    }
};

#endif
