/** @file
 * 
 * \brief Funciones auxiliares de CLI para logs y captura segura de datos primitivos.
 * La captura de datos no incluye tipos de dato string ni booleanos.
 */

#ifndef AUXILIARCLI_H
#define AUXILIARCLI_H

#include <iostream>
#include <string>
#include <limits>

class AuxiliarCli
{
public:
    AuxiliarCli() = default;

    /**
     * \brief Imprime un mensaje en la salida estándar con color según el tipo de log.
     * 
     * \param tipo Identificador del log (por ejemplo, `ERROR`, `SUCCESS`, `WARNING`).
     * \param msj Mensaje que se mostrará al usuario.
     */
    void imprimirLog(std::string tipo, std::string msj)
    {
        int colorId = 37; // default WHITE

        if (tipo == "RED" || tipo == "red" || tipo == "ERROR" || tipo == "error") {
            colorId = 31;
        }
            
        if (tipo == "GREEN" || tipo == "green" || tipo == "SUCCESS" || tipo == "success") {
            colorId = 32;
        }
            
        if (tipo == "YELLOW" || tipo == "yellow" || tipo == "WARNING" || tipo == "warning") {
            colorId = 33;
        }
            
        if (tipo == "CYAN" || tipo == "cyan" || tipo == "STATUS" || tipo == "status") {
            colorId = 36;
        }
            
        std::cout << "\033[" << colorId << "m";
        std::cout << "[" << tipo << "] " << msj << std::endl;
        std::cout << "\033[0m";
    }

    /**
     * \brief Pide un dato primitivo por consola y valida la entrada hasta que sea correcta.
     * 
     * \param mensaje Etiqueta que se imprime antes de solicitar el dato.
     * \param valor Referencia donde se guarda el valor leído.
     */
    template <typename T>
    void obtenerDato(std::string mensaje, T &valor)
    {
        mensaje += ": ";
        std::cout << mensaje;

        while (!(std::cin >> valor))
        {
            imprimirLog("WARNING", "Entrada no válida, por favor, intente de nuevo.");
            std::cin.clear();                                                   // limpiar el error state
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // limpiar el línea

            std::cout << std::endl
                      << mensaje;
        }

        // limpiar buffer de vuelta, ya que se pudo haber escrito algo más
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
};

#endif