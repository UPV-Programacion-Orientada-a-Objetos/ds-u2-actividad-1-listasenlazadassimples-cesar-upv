#ifndef LISTAGENERAL_H
#define LISTAGENERAL_H

#include <cstring>
#include <cstdio>
#include "SensorBase.h"
#include "AuxiliarCli.h"

/**
 * @file ListaGeneral.h
 * @brief Lista enlazada que almacena punteros a sensores polimórficos.
 */
/**
 * @brief Administra la colección polimórfica de sensores.
 */
class ListaGeneral
{
public:
    ListaGeneral() : cabeza(nullptr) {}

    ~ListaGeneral()
    {
        liberar();
    }

    /**
     * @brief Inserta un sensor al final de la lista si no está duplicado.
     * @param sensor Puntero válido a un sensor derivado.
     * @return true si se insertó, false en caso de error o duplicado.
     */
    bool insertar(SensorBase* sensor)
    {
        AuxiliarCli cli;
        if (!sensor)
        {
            cli.imprimirLog("WARNING", "Intento de insertar un sensor nulo.");
            return false;
        }

        if (buscarPorNombre(sensor->obtenerNombre()))
        {
            char mensaje[160];
            std::snprintf(mensaje, sizeof(mensaje), "Sensor '%s' ya existe en la lista.", sensor->obtenerNombre());
            cli.imprimirLog("WARNING", mensaje);
            return false;
        }

        NodoGeneral* nuevo = new NodoGeneral(sensor);
        if (!cabeza)
        {
            cabeza = nuevo;
        }
        else
        {
            NodoGeneral* actual = cabeza;
            while (actual->siguiente)
            {
                actual = actual->siguiente;
            }
            actual->siguiente = nuevo;
        }

        char mensaje[160];
        std::snprintf(mensaje, sizeof(mensaje), "Sensor '%s' insertado en la lista de gestión.", sensor->obtenerNombre());
        cli.imprimirLog("SUCCESS", mensaje);
        return true;
    }

    /**
     * @brief Busca un sensor por identificador.
     * @param id Cadena con el nombre del sensor.
     * @return Puntero al sensor o nullptr si no existe.
     */
    SensorBase* buscarPorNombre(const char* id) const
    {
        if (!id)
        {
            return nullptr;
        }

        NodoGeneral* actual = cabeza;
        while (actual)
        {
            if (std::strcmp(actual->sensor->obtenerNombre(), id) == 0)
            {
                return actual->sensor;
            }
            actual = actual->siguiente;
        }
        return nullptr;
    }

    /**
     * @brief Indica si la lista está vacía.
     */
    bool estaVacia() const
    {
        return cabeza == nullptr;
    }

    /**
     * @brief Recorre la lista e invoca procesarLectura() de cada sensor.
     */
    void procesarSensores()
    {
        AuxiliarCli cli;
        if (!cabeza)
        {
            cli.imprimirLog("WARNING", "No hay sensores registrados para procesar.");
            return;
        }

        NodoGeneral* actual = cabeza;
        while (actual)
        {
            char mensaje[160];
            std::snprintf(mensaje, sizeof(mensaje), "-> Procesando Sensor %s...", actual->sensor->obtenerNombre());
            cli.imprimirLog("STATUS", mensaje);
            actual->sensor->procesarLectura();
            actual = actual->siguiente;
        }
    }

    /**
     * @brief Imprime un resumen simple de los sensores registrados.
     */
    void mostrarResumen() const
    {
        if (!cabeza)
        {
            std::cout << "Lista de sensores vacía." << std::endl;
            return;
        }

        NodoGeneral* actual = cabeza;
        while (actual)
        {
            actual->sensor->imprimirInfo();
            actual = actual->siguiente;
        }
    }

    /**
     * @brief Libera todos los nodos y sensores almacenados.
     */
    void liberar()
    {
        AuxiliarCli cli;
        NodoGeneral* actual = cabeza;
        while (actual)
        {
            char mensaje[160];
            std::snprintf(mensaje, sizeof(mensaje), "[Destructor General] Liberando Nodo: %s.", actual->sensor->obtenerNombre());
            cli.imprimirLog("STATUS", mensaje);

            NodoGeneral* siguiente = actual->siguiente;
            delete actual->sensor;
            delete actual;
            actual = siguiente;
        }
        cabeza = nullptr;
    }

private:
    struct NodoGeneral
    {
        SensorBase* sensor;
        NodoGeneral* siguiente;

        explicit NodoGeneral(SensorBase* ptr) : sensor(ptr), siguiente(nullptr) {}
    };

    NodoGeneral* cabeza;
};

#endif
