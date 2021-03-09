/* ===============================================================
 * Titulo:   Ejemplo Libreria de Botones
 *
 * Fecha:    Febrero 2020
 * Autor:    Carlos Fernandez Ruiz
 *
 * Placa:    Todas
 * 
 * Descripcion:
 *     Ejemplo del uso de la libreria LP_JuegoBotones.h
 *   
 * Propiedad de @logicprops
 * ===============================================================
 */

//// ==== ETIQUETAS AUXILIARES: LP_JuegoBotones ==== 
//- LP_SOLUCIONADO       1  // Juego Solucionado
//- LP_NO_SOL            0  // No Solucionado, nada apretado 
//- LP_APRETADO_NO_SOL  -1  // No Solucionado, boton apretado
//- LP_APRETADO_SEQ_MAL -2  // No Solucionado, sequencia incorrecta


#define DEBUG     1
#if DEBUG == 1
#define LOGN(x) Serial.println(x)
#define LOG(x)  Serial.print(x)
#else
#define LOG(x)
#define LOGN(x)
#endif

// -- (OPCIONAL)
// Parametros de Juegos Botones definir antes de importar la libreria
#define LP_MAXBOT     4 // Por defecto 64
#define LP_MAXSEQ     4 // Por defecto 32

// ==== LIBRERIAS EXTERNAS =======================================
#include <DigitalSignals.h>
#include <LP_JuegoBotones.h>


// ==== OBJETO JUEGO BOTONES =====================================
JuegoBotones Juego; // Iniciamos Juego

// 1. Parametros de los botones:
 //-OBLIGATORIOS
unsigned int PinBotones[LP_MAXBOT] = {2, 3, 4, 5};
 //-OPCIONALES
  int debounce = 50;       // defecto: 50
  bool active = false;     // defecto: false
  bool use_pullup = true;  // defecto: true
  int mxbot = LP_MAXBOT;   // defecto: LP_MAXBOT
  // Por defecto se espera que los pulsadores se cierre el contacto
  // a ground

// 2. Parametros de la secuencia correcta:
  //-OBLIGATORIOS
unsigned int ButtonSequence[LP_MAXSEQ] = {3, 2, 1, 0}; // id=0:(seq_len-1)
  //-OPCIONALES
  bool intento_continuo = false; // defecto: true
      // Los intentos son de longitud infinita si true. Es decir, si el codigp
      // es 1234, y pulsas 4.1.2.3.4, se resolvera. No se espera a introducir una
      // palabra de longitud del codigo antes de evaluarlo.
      // Si FALSE. Si introduces 4.1.2.3.4, el codigo evaluara primero 4.1.2.3 y
      // devolvera falso. Se evaluan los codigos con longitud fija.
  
  int mxseq = LP_MAXSEQ; // Cambiar si la longitud es diferente a LP_MAXSEQ. Mandara mxseq
                         // Para optimizar, si la longitud es fija, cambia LP_MAXSEQ


// 3. (OPCIONAL) Tiempo Autoreset. Por defecto: 0, (si no se llama no se autoreseteara)
long AutoReset_Time = 5; // Segundos que pasan desde el ultimo inento antes de autoreset
// Para parar se puede usar : Juego.EndAutoReset()


// 4. Otras Variables
bool print_progress = true; // Muestra el progreso en serial

// ===============================================================
//            SETUP
// ===============================================================
void setup()
{
  // -- Serial --
#if DEBUG == 1
  Serial.begin(115200);
  delay(50);
  LOGN("\n\n----------------------------\n\tEMPEZAMOS\n");
#endif

  // -- Juego Botones: Palabra Fija --
  Juego.SetPinBotones(PinBotones, debounce, active, use_pullup, mxbot);
  Juego.SetSecuencia(ButtonSequence, intento_continuo, mxseq);
  Juego.SetTiempoAutoReset(AutoReset_Time);

  // -- Led BuiltIn --
  pinMode(LED_BUILTIN, OUTPUT); digitalWrite(LED_BUILTIN, LOW);
}

// ===============================================================
//            LOOP
// ===============================================================
void loop()
{
  
  /* Llamar al update continuamente para actualizar el juego.
   * Devuelve:
   * 
   *  En el caso de LONGITUD FIJA:       (intento_continuo = false)
   *   > juego_estado = 1   Solucionado                  [LP_SOLUCIONADO]
   *   > juego_estado = 0   No se ha solucionado         [LP_NO_SOL]
   *   > juego_estado =-1   No sol, boton apretado       [LP_APRETADO_NO_SOL]
   *   > juego_estado =-2   No sol, sequencia incorrecta [LP_APRETADO_SEQ_MAL]
   * 
   */
  int juego_estado = Juego.UpdateJuegoBotones(print_progress);

  // Al apretar boton, pero sin resolver: Parpadear
  if (juego_estado <= LP_APRETADO_NO_SOL){
    digitalWrite(LED_BUILTIN, HIGH); delay(50); digitalWrite(LED_BUILTIN, LOW); delay(50);
  }

  // Al introducir secuencia incorrecta: Chiribitas
  if (juego_estado == LP_APRETADO_SEQ_MAL){
    delay(200);
    digitalWrite(LED_BUILTIN, HIGH); delay(50); digitalWrite(LED_BUILTIN, LOW); delay(50);
    digitalWrite(LED_BUILTIN, HIGH); delay(50); digitalWrite(LED_BUILTIN, LOW); delay(50);
    digitalWrite(LED_BUILTIN, HIGH); delay(50); digitalWrite(LED_BUILTIN, LOW); delay(50);
  }
  
  // Si Solucionado: Mantener Encendido y Reset   Juego.Reset();
  if (juego_estado == LP_SOLUCIONADO){
    // Blinkear una vez
    digitalWrite(LED_BUILTIN, HIGH); delay(50); digitalWrite(LED_BUILTIN, LOW); delay(50);
    
    digitalWrite(LED_BUILTIN, HIGH);
    delay(5000);
    Juego.Reset();
    digitalWrite(LED_BUILTIN, LOW);
    LOGN("\t\t>> RESET <<");
  }

  delay(4);

}
