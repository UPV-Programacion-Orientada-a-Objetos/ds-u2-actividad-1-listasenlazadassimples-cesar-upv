/**
 * @file SensorPresion.h
 * @brief Sensor concreto que gestiona lecturas de presión (int).
 */
#ifndef SENSORPRESION_H
#define SENSORPRESION_H

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include "SensorBase.h"
#include "ListaSensor.h"
#include "AuxiliarCli.h"

/**
 * @brief Gestiona lecturas enteras correspondientes a sensores de presión.
 */
class SensorPresion : public SensorBase
{
public:
    /// Inicializa el sensor con el identificador dado.
    explicit SensorPresion(const char* id)
    {
        asignarNombre(id);
    }

    ~SensorPresion() override
    {
        AuxiliarCli cli;
        char encabezado[120];
        std::snprintf(encabezado, sizeof(encabezado), "  [Destructor Sensor %s] Liberando Lista Interna...", nombre);
        cli.imprimirLog("STATUS", encabezado);

        int valor = 0;
        while (historial.extraerPrimero(valor))
        {
            char mensaje[120];
            std::snprintf(mensaje, sizeof(mensaje), "    Nodo<int> %d liberado.", valor);
            cli.imprimirLog("STATUS", mensaje);
        }
    }

    /// Imprime un resumen del estado del sensor.
    void imprimirInfo() const override
    {
        std::cout << "[Sensor Presion] " << nombre << " | lecturas almacenadas: " << historial.contar() << std::endl;
    }

    /// Registra una lectura solicitando un entero al usuario.
    void registrarLecturaInteractiva() override
    {
        AuxiliarCli cliLectura;
        int valor = 0;
        cliLectura.obtenerDato("Valor de presión (int)", valor);
        registrarLecturaInterna(valor);
    }

    /// Registra una lectura recibida como texto (por serial).
    void registrarLecturaDesdeCadena(const char* valorComoTexto) override
    {
        AuxiliarCli cli;
        if (!valorComoTexto || valorComoTexto[0] == '\0')
        {
            cli.imprimirLog("WARNING", "Dato recibido vacío para sensor de presión.");
            return;
        }

        int valor = std::atoi(valorComoTexto);
        registrarLecturaInterna(valor);
    }

    /// Calcula el promedio de lecturas registradas.
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

        int cantidad = historial.contar();
        double promedio = historial.promedio();

        char resumen[180];
        std::snprintf(resumen, sizeof(resumen), "[Sensor Presion] Promedio calculado sobre %d lectura%s (%.1f).",
                      cantidad,
                      (cantidad == 1) ? "" : "s",
                      promedio);
        cli.imprimirLog("STATUS", resumen);
    }

private:
    ListaSensor<int> historial;

    /// Inserta la lectura en el historial y emite log.
    void registrarLecturaInterna(int valor)
    {
        AuxiliarCli cli;
        historial.insertarAlFinal(valor);

        char mensaje[140];
        std::snprintf(mensaje, sizeof(mensaje), "Insertando nodo entero en %s.", nombre);
        cli.imprimirLog("STATUS", mensaje);
    }
};

#endif
