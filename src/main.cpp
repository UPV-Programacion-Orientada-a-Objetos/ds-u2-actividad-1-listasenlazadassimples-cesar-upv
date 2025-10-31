/**
 * @file main.cpp
 * @brief Punto de entrada del sistema IoT polimórfico basado en listas enlazadas.
 */

#include <iostream>
#include <cstdio>
#include <cstddef>
#include <cstring>
#include <cerrno>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <sys/select.h>
#include "AuxiliarCli.h"
#include "ListaGeneral.h"
#include "SensorTemperatura.h"
#include "SensorPresion.h"

/// Longitud máxima permitida para el identificador de un sensor.
constexpr std::size_t TAM_ID = 50;
/// Tamaño del buffer usado para leer líneas desde la consola o el puerto serial.
constexpr std::size_t TAM_SERIAL = 128;

void mostrarMenu();
void recortarEspacios(char* texto);
bool descomponerLineaSerial(const char* linea, char* id, std::size_t tamId, char* valor, std::size_t tamValor);
bool registrarDesdeCadenaManual(ListaGeneral& lista, AuxiliarCli& cli);
bool configurarPuertoSerial(int fd, AuxiliarCli& cli);
bool escucharDispositivoSerial(ListaGeneral& lista, AuxiliarCli& cli);

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
            std::cout << "2. Registrar lectura desde cadena serial (ingresada aquí)\n";
            std::cout << "3. Escuchar dispositivo serial (ESP32/Arduino)\n";
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
                registrarDesdeCadenaManual(lista, cli);
            }
            else if (modo == 3)
            {
                escucharDispositivoSerial(lista, cli);
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

/**
 * @brief Muestra las opciones disponibles en el menú principal.
 */
void mostrarMenu()
{
    std::cout << "\n--- Sistema IoT de Monitoreo Polimórfico ---\n";
    std::cout << "1. Crear Sensor (Tipo Temp - FLOAT)\n";
    std::cout << "2. Crear Sensor (Tipo Presion - INT)\n";
    std::cout << "3. Registrar Lectura (Manual o Serial)\n";
    std::cout << "4. Ejecutar Procesamiento Polimórfico\n";
    std::cout << "5. Cerrar Sistema (Liberar Memoria)\n";
}

/**
 * @brief Elimina espacios iniciales y finales de una cadena in situ.
 */
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

/**
 * @brief Separa una línea con formato ID,valor en dos buffers.
 */
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

/**
 * @brief Solicita al usuario una línea con el formato ID,valor y la aplica a la lista.
 */
bool registrarDesdeCadenaManual(ListaGeneral& lista, AuxiliarCli& cli)
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

/**
 * @brief Configura un puerto serial abierto a 9600 baudios con parámetros básicos.
 */
bool configurarPuertoSerial(int fd, AuxiliarCli& cli)
{
    struct termios opciones;
    if (tcgetattr(fd, &opciones) != 0)
    {
        cli.imprimirLog("WARNING", "No se pudo obtener la configuración del puerto serial.");
        return false;
    }

    cfsetispeed(&opciones, B9600);
    cfsetospeed(&opciones, B9600);

    opciones.c_cflag = CS8 | CLOCAL | CREAD;
    opciones.c_iflag = IGNPAR;
    opciones.c_oflag = 0;
    opciones.c_lflag = 0;
    opciones.c_cc[VMIN] = 0;
    opciones.c_cc[VTIME] = 10;

    if (tcsetattr(fd, TCSANOW, &opciones) != 0)
    {
        cli.imprimirLog("WARNING", "No se pudo establecer la configuración del puerto serial.");
        return false;
    }

    tcflush(fd, TCIOFLUSH);
    return true;
}

/**
 * @brief Lee lecturas continuas desde un puerto serial hasta que se pulse ENTER o se desconecte.
 */
bool escucharDispositivoSerial(ListaGeneral& lista, AuxiliarCli& cli)
{
    char ruta[80] = {0};
    cli.obtenerCadena("Ruta del puerto serial (ej. /dev/ttyUSB0)", ruta, sizeof(ruta));

    int fd = open(ruta, O_RDONLY | O_NOCTTY | O_NONBLOCK);
    if (fd == -1)
    {
        cli.imprimirLog("WARNING", "No se pudo abrir el puerto. Cierra otros monitores y verifica la ruta.");
        return false;
    }

    if (!configurarPuertoSerial(fd, cli))
    {
        close(fd);
        return false;
    }

    cli.imprimirLog("WARNING", "Cierra cualquier monitor serial antes de continuar.");
    cli.imprimirLog("STATUS", "Leyendo datos del puerto. Pulsa ENTER para detener o desconecta el dispositivo.");

    bool leyendo = true;
    char linea[TAM_SERIAL] = {0};
    std::size_t posicion = 0;

    while (leyendo)
    {
        fd_set conjuntoLectura;
        FD_ZERO(&conjuntoLectura);
        FD_SET(fd, &conjuntoLectura);
        FD_SET(STDIN_FILENO, &conjuntoLectura);
        int maximo = (fd > STDIN_FILENO) ? fd : STDIN_FILENO;

        int resultado = select(maximo + 1, &conjuntoLectura, nullptr, nullptr, nullptr);
        if (resultado < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }
            cli.imprimirLog("WARNING", "Select interrumpido, se detiene la lectura serial.");
            break;
        }

        if (FD_ISSET(STDIN_FILENO, &conjuntoLectura))
        {
            char descarte[8];
            if (fgets(descarte, sizeof(descarte), stdin) != nullptr)
            {
                cli.imprimirLog("STATUS", "Lectura serial detenida por el usuario.");
                leyendo = false;
            }
        }

        if (!leyendo)
        {
            break;
        }

        if (FD_ISSET(fd, &conjuntoLectura))
        {
            char byteLeido = 0;
            ssize_t cantidad = read(fd, &byteLeido, 1);
            if (cantidad > 0)
            {
                if (byteLeido == '\r')
                {
                    continue;
                }

                if (byteLeido == '\n')
                {
                    if (posicion > 0)
                    {
                        linea[posicion] = '\0';
                        char id[TAM_ID] = {0};
                        char valorCadena[40] = {0};

                        if (descomponerLineaSerial(linea, id, TAM_ID, valorCadena, sizeof(valorCadena)))
                        {
                            SensorBase* sensor = lista.buscarPorNombre(id);
                            if (sensor)
                            {
                                sensor->registrarLecturaDesdeCadena(valorCadena);
                            }
                            else
                            {
                                char mensaje[160];
                                std::snprintf(mensaje, sizeof(mensaje), "Sensor '%s' no se encuentra en la lista.", id);
                                cli.imprimirLog("WARNING", mensaje);
                            }
                        }
                        else
                        {
                            cli.imprimirLog("WARNING", "Lectura serial ignorada: formato incorrecto.");
                        }
                    }
                    posicion = 0;
                }
                else
                {
                    if (posicion < sizeof(linea) - 1)
                    {
                        linea[posicion++] = byteLeido;
                    }
                }
            }
            else if (cantidad == 0)
            {
                cli.imprimirLog("STATUS", "No se reciben datos; posible desconexión del dispositivo.");
                leyendo = false;
            }
            else if (errno != EAGAIN && errno != EWOULDBLOCK)
            {
                cli.imprimirLog("WARNING", "Lectura serial interrumpida por un error.");
                leyendo = false;
            }
        }
    }

    close(fd);
    return true;
}
