/**
 * @file SerialEmitter.ino
 * @brief Simula lecturas de sensores y las envía por el puerto serial.
 *
 * Formato de salida:
 *   T-001,45.3
 *   P-105,82
 *
 * Ajusta los identificadores para que coincidan con los creados en la aplicación C++.
 */
const char* ID_TEMP = "T-001";
const char* ID_PRES = "P-105";
unsigned long ultimoEnvio = 0;
const unsigned long intervaloMs = 2000;

void setup()
{
  Serial.begin(9600);
  randomSeed(analogRead(A0));
}

void loop()
{
  unsigned long ahora = millis();
  if (ahora - ultimoEnvio >= intervaloMs)
  {
    float lecturaTemp = 40.0 + (random(-50, 50) / 10.0);
    int lecturaPres = 80 + random(-5, 6);

    Serial.print(ID_TEMP);
    Serial.print(",");
    Serial.println(lecturaTemp, 1);

    Serial.print(ID_PRES);
    Serial.print(",");
    Serial.println(lecturaPres);

    ultimoEnvio = ahora;
  }
}
