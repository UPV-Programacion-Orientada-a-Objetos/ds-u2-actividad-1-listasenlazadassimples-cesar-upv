/**
 * @file SensorTemperatura.h
 * @brief Sensor concreto que gestiona lecturas de temperatura (float).
 */
#ifndef SENSORTEMPERATURA_H
#define SENSORTEMPERATURA_H

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include "SensorBase.h"
#include "ListaSensor.h"
#include "AuxiliarCli.h"

/**
 * @brief Gestiona lecturas flotantes y su análisis particular.
 */
class SensorTemperatura : public SensorBase
{
public:
    /// Inicializa el sensor con el identificador dado.
    explicit SensorTemperatura(const char* id)
    {
        asignarNombre(id);
    }

    ~SensorTemperatura() override
    {
        AuxiliarCli cli;
        char encabezado[120];
        std::snprintf(encabezado, sizeof(encabezado), "  [Destructor Sensor %s] Liberando Lista Interna...", nombre);
        cli.imprimirLog("STATUS", encabezado);

        float valor = 0.0f;
        while (historial.extraerPrimero(valor))
        {
            char mensaje[120];
            std::snprintf(mensaje, sizeof(mensaje), "    Nodo<float> %.1f liberado.", valor);
            cli.imprimirLog("STATUS", mensaje);
        }
    }

    /// Imprime un resumen del estado del sensor.
    void imprimirInfo() const override
    {
        std::cout << "[Sensor Temp] " << nombre << " | lecturas almacenadas: " << historial.contar() << std::endl;
    }

    /// Registra una lectura pidiendo un dato float al usuario.
    void registrarLecturaInteractiva() override
    {
        AuxiliarCli cliLectura;
        float valor = 0.0f;
        cliLectura.obtenerDato("Valor de temperatura (float)", valor);
        registrarLecturaInterna(valor);
    }

    /// Registra una lectura proveniente de una cadena (serial).
    void registrarLecturaDesdeCadena(const char* valorComoTexto) override
    {
        AuxiliarCli cli;
        if (!valorComoTexto || valorComoTexto[0] == '\0')
        {
            cli.imprimirLog("WARNING", "Dato recibido vacío para sensor de temperatura.");
            return;
        }

        float valor = static_cast<float>(std::atof(valorComoTexto));
        registrarLecturaInterna(valor);
    }

    /// Ejecuta la lógica de limpieza y cálculo promedio.
    void procesarLectura() override
    {
        AuxiliarCli cli;
        if (historial.estaVacia())
        {
            char mensaje[120];
            std::snprintf(mensaje, sizeof(mensaje), "[%s] No hay lecturas registradas.", nombre);
            cli.imprimirLog("WARNING", mensaje);
            return;
        }

        int cantidadInicial = historial.contar();
        if (cantidadInicial > 1)
        {
            float minimo = 0.0f;
            if (historial.obtenerMinimo(minimo))
            {
                historial.eliminarPrimeraCoincidencia(minimo);
                char mensaje[160];
                std::snprintf(mensaje, sizeof(mensaje), "[%s] Lectura más baja (%.1f) eliminada.", nombre, minimo);
                cli.imprimirLog("STATUS", mensaje);
            }
        }

        int cantidadFinal = historial.contar();
        double promedio = historial.promedio();

        char resumen[180];
        std::snprintf(resumen, sizeof(resumen), "[Sensor Temp] Promedio calculado sobre %d lectura%s (%.1f).",
                      cantidadFinal,
                      (cantidadFinal == 1) ? "" : "s",
                      promedio);
        cli.imprimirLog("STATUS", resumen);
    }

private:
    ListaSensor<float> historial;

    /// Inserta la lectura en el historial y reporta mediante log.
    void registrarLecturaInterna(float valor)
    {
        AuxiliarCli cli;
        historial.insertarAlFinal(valor);

        char mensaje[140];
        std::snprintf(mensaje, sizeof(mensaje), "Insertando nodo float en %s.", nombre);
        cli.imprimirLog("STATUS", mensaje);
    }
};

#endif
