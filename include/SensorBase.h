/**
 * @file SensorBase.h
 * @brief Interfaz abstracta para sensores administrados por el sistema.
 */
#ifndef SENSORBASE_H
#define SENSORBASE_H

#include <cstring>

/**
 * @brief Clase base abstracta para cualquier sensor del sistema.
 */
class SensorBase
{
public:
    SensorBase()
    {
        nombre[0] = '\0';
    }

    virtual ~SensorBase() {}

    /**
     * @brief Asigna el identificador del sensor.
     * @param nuevoNombre Cadena terminada en nulo con el identificador.
     */
    void asignarNombre(const char* nuevoNombre)
    {
        if (!nuevoNombre)
        {
            nombre[0] = '\0';
            return;
        }

        std::strncpy(nombre, nuevoNombre, sizeof(nombre) - 1);
        nombre[sizeof(nombre) - 1] = '\0';
    }

    /**
     * @brief Devuelve el identificador del sensor.
     * @return Cadena terminada en nulo con el nombre actual.
     */
    const char* obtenerNombre() const
    {
        return nombre;
    }

    /// Muestra información legible del sensor.
    virtual void imprimirInfo() const = 0;
    /// Solicita una lectura desde la consola y la almacena.
    virtual void registrarLecturaInteractiva() = 0;
    /// Interpreta una lectura recibida como texto (por ejemplo, por serial).
    virtual void registrarLecturaDesdeCadena(const char* valorComoTexto) = 0;
    /// Procesa las lecturas almacenadas aplicando la lógica específica.
    virtual void procesarLectura() = 0;

protected:
    /// Identificador del sensor (máximo 49 caracteres más terminador).
    char nombre[50];
};

#endif
