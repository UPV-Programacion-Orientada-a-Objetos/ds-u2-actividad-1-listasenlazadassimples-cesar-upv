/**
 * @file main.cpp
 * @brief Punto de entrada del sistema IoT polimórfico basado en listas enlazadas.
 */

#include <iostream>
#include <cstdio>
#include <cstddef>
#include <cstring>
#include "AuxiliarCli.h"
#include "ListaGeneral.h"
#include "SensorTemperatura.h"
#include "SensorPresion.h"

namespace
{
    const std::size_t TAM_ID = 50;
    const std::size_t TAM_SERIAL = 128;

    void mostrarMenu()
    {
        std::cout << "\n--- Sistema IoT de Monitoreo Polimórfico ---\n";
        std::cout << "1. Crear Sensor (Tipo Temp - FLOAT)\n";
        std::cout << "2. Crear Sensor (Tipo Presion - INT)\n";
        std::cout << "3. Registrar Lectura (Manual o Serial)\n";
        std::cout << "4. Ejecutar Procesamiento Polimórfico\n";
        std::cout << "5. Cerrar Sistema (Liberar Memoria)\n";
    }

    void recortarEspacios(char* texto)
    {
        if (!texto)
        {
            return;
        }

        std::size_t inicio = 0;
        while (texto[inicio] == ' ' || texto[inicio] == '\t')
        {
            ++inicio;
        }

        if (inicio > 0)
        {
            std::size_t i = 0;
            while (texto[inicio + i] != '\0')
            {
                texto[i] = texto[inicio + i];
                ++i;
            }
            texto[i] = '\0';
        }

        int fin = static_cast<int>(std::strlen(texto)) - 1;
        while (fin >= 0 && (texto[fin] == ' ' || texto[fin] == '\t'))
        {
            texto[fin] = '\0';
            --fin;
        }
    }

    bool descomponerLineaSerial(const char* linea, char* id, std::size_t tamId, char* valor, std::size_t tamValor)
    {
        if (!linea || !id || !valor)
        {
            return false;
        }

        std::size_t i = 0;
        std::size_t j = 0;
        while (linea[i] != '\0' && linea[i] != ',' && j < tamId - 1)
        {
            id[j++] = linea[i++];
        }
        id[j] = '\0';

        if (linea[i] != ',')
        {
            return false;
        }
        ++i;

        std::size_t k = 0;
        while (linea[i] != '\0' && k < tamValor - 1)
        {
            valor[k++] = linea[i++];
        }
        valor[k] = '\0';

        recortarEspacios(id);
        recortarEspacios(valor);

        return (id[0] != '\0' && valor[0] != '\0');
    }

    bool registrarDesdeCadena(ListaGeneral& lista, AuxiliarCli& cli)
    {
        char linea[TAM_SERIAL] = {0};
        cli.obtenerCadena("Cadena recibida (ID,valor)", linea, TAM_SERIAL);

        char id[TAM_ID] = {0};
        char valorCadena[40] = {0};
        if (!descomponerLineaSerial(linea, id, TAM_ID, valorCadena, sizeof(valorCadena)))
        {
            cli.imprimirLog("WARNING", "Formato inválido. Usa el patrón ID,valor.");
            return false;
        }

        SensorBase* sensor = lista.buscarPorNombre(id);
        if (!sensor)
        {
            char mensaje[140];
            std::snprintf(mensaje, sizeof(mensaje), "Sensor '%s' no registrado en la lista general.", id);
            cli.imprimirLog("WARNING", mensaje);
            return false;
        }

        sensor->registrarLecturaDesdeCadena(valorCadena);
        return true;
    }
}

/** @brief Función principal que gestiona el menú interactivo del sistema. */
int main()
{
    AuxiliarCli cli;
    ListaGeneral lista;

    int opcion = 0;
    bool sistemaActivo = true;

    while (sistemaActivo)
    {
        mostrarMenu();
        cli.obtenerDato("OPCION A ELEGIR", opcion);

        switch (opcion)
        {
        case 1:
        {
            char id[TAM_ID] = {0};
            cli.obtenerCadena("ID del sensor de temperatura", id, TAM_ID);

            SensorTemperatura* nuevo = new SensorTemperatura(id);
            if (!lista.insertar(nuevo))
            {
                delete nuevo;
            }
            break;
        }
        case 2:
        {
            char id[TAM_ID] = {0};
            cli.obtenerCadena("ID del sensor de presión", id, TAM_ID);

            SensorPresion* nuevo = new SensorPresion(id);
            if (!lista.insertar(nuevo))
            {
                delete nuevo;
            }
            break;
        }
        case 3:
        {
            if (lista.estaVacia())
            {
                cli.imprimirLog("WARNING", "No hay sensores creados para registrar lecturas.");
                break;
            }

            int modo = 0;
            std::cout << "\n1. Registrar lectura manual\n";
            std::cout << "2. Registrar lectura desde cadena serial\n";
            cli.obtenerDato("Seleccione modo", modo);

            if (modo == 1)
            {
                char id[TAM_ID] = {0};
                cli.obtenerCadena("ID del sensor destino", id, TAM_ID);

                SensorBase* sensor = lista.buscarPorNombre(id);
                if (!sensor)
                {
                    char mensaje[140];
                    std::snprintf(mensaje, sizeof(mensaje), "Sensor '%s' no se encuentra en la lista.", id);
                    cli.imprimirLog("WARNING", mensaje);
                }
                else
                {
                    sensor->registrarLecturaInteractiva();
                }
            }
            else if (modo == 2)
            {
                registrarDesdeCadena(lista, cli);
            }
            else
            {
                cli.imprimirLog("WARNING", "Modo no reconocido.");
            }
            break;
        }
        case 4:
        {
            cli.imprimirLog("STATUS", "--- Ejecutando Polimorfismo ---");
            lista.procesarSensores();
            break;
        }
        case 5:
        {
            cli.imprimirLog("STATUS", "--- Liberación de Memoria en Cascada ---");
            lista.liberar();
            cli.imprimirLog("SUCCESS", "Sistema cerrado. Memoria limpia.");
            sistemaActivo = false;
            break;
        }
        default:
            cli.imprimirLog("WARNING", "Opción fuera de rango.");
            break;
        }
    }

    return 0;
}
