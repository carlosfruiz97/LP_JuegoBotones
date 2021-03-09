# Juego Botones Arduino
Libreria de Arduino para gestionar juegos de botones para escaperooms.
De momento depende de otra libreria llamada DigitalSignals.h, mas adelante
se hara independiente para simplificar su uso.

Libreria DigitalSignals.h ; link...

Un juego de botones consiste en apretar una serie de botones en un orden determinado
para solucionar un juego.

## Intento Contiuno vs Palabra Fija
Un juego de botones puede resolverse de dos formas:

* **Palabra Fija**: Como introducir una contraseña o un pin en el movil. El juego se espera
a que se pulse un numero determinado de veces antes de evaluar si el intento es
correcto.

    Por ejemplo, si el codigo correcto es 1,2,3,4; y el jugador pulsa: 4-1-2-3-4;
    el juego primero evaluara primero 4-1-2-3 y devolvera falso.

* **Intento Continuo**: El juego no se espera a que se introduzca un numero de
pulsaciones antes de evaular. Si la secuencia se pulsa correctamente en algun momento se resolvera.

    Por ejemplo, si el codigo correcto es 1,2,3,4; y el jugador pulsa: 4-1-2-3-4;
    el juego se resolvera, ya que se pulso la secuencia en el orden correcto.

## Inicializacion de la clase JuegoBotones

Primero llamamos a la libreria despues de importar DigitalSignals.h. Antes de eso
definimos unas etiquetas que se usan dentro de LP_JuegoBotones.h, si las definimos
antes sera mas eficiente, ya que no se alocara espacio de mas al crear las listas
dentro de la clase **JuegoBotones**.

    // -- (OPCIONAL)
    // Parametros de Juegos Botones definir antes de importar la libreria
    #define LP_MAXBOT     4 // Por defecto 64
    #define LP_MAXSEQ     4 // Por defecto 32

    // ==== LIBRERIAS EXTERNAS =======================================
    #include <DigitalSignals.h>
    #include <LP_JuegoBotones.h>

A continuacion definimos la clase Juego Botones:

    // ==== OBJETO JUEGO BOTONES =====================================
    JuegoBotones Juego; // Iniciamos Juego

Tenemos que crear dos listas, y definir algunos parametros que se les pasara como argumento en el **setup**:

#### Parametros de los botones:

    // 1. Parametros de los botones:
    //-OBLIGATORIOS
    unsigned int PinBotones[LP_MAXBOT] = {2, 3, 4, 5};

    //-OPCIONALES
    int debounce = 50;       // defecto: 50
    bool active = false;     // defecto: false
    bool use_pullup = true;  // defecto: true
    int mxbot = LP_MAXBOT;   // defecto: LP_MAXBOT

Por defecto se espera que los pulsadores se cierre el contacto
a ground, usando un pullup.

#### Parametros de la secuencia correcta:

    // 2. Parametros de la secuencia correcta:
    //-OBLIGATORIOS
    unsigned int ButtonSequence[LP_MAXSEQ] = {3, 2, 1, 0}; // id=0:(seq_len-1)

    //-OPCIONALES
    bool intento_continuo = true; // defecto: true, false indica palabra_fija
    int mxseq = LP_MAXSEQ; // Cambiar si la longitud es diferente a LP_MAXSEQ. Mandara mxseq
                           // Para optimizar, si la longitud es fija, cambia LP_MAXSEQ

##### Palabra Fija
En el caso de usar un intento de palabra fija (<em>intento_continuo  = false</em>),
existe un parametro adicional opcional: Tiempo de Autoreset.
Configura el tiempo, en segundos, que tarda en borrar el ultimo intento parcial.

Es decir, si en una secuencia de longitud 4, el jugador pulsa solo 2 botones, el
juego espera a que se pulsen otros 2 antes de evaluar. Podria interesar, que si pasa
mucho tiempo desde que se introducieron esos 2 primeros botones el juego borre ese intento.
Cuando el jugador vuelva a intentar la secuencia, empieze desde 0.

Definimos un tiempo, que luego pasaremos dentro de la funcion setup:

    // 3. (OPCIONAL) Tiempo Autoreset. Por defecto: 0, (si no se llama no se autoreseteara)
    long AutoReset_Time = 5; // Segundos que pasan desde el ultimo inento antes de autoreset
    // Para parar se puede usar : Juego.EndAutoReset()



## Funcion setup()
Una vez definidos todos los parametros, dentro de la funcion setup llamamos a las siguientes
funciones de la clase JuegoBotones:

    void setup()
    {
      // -- Juego Botones --
      Juego.SetPinBotones(PinBotones, debounce, active, use_pullup, mxbot);
      Juego.SetSecuencia(ButtonSequence, intento_continuo, mxseq);
      Juego.SetTiempoAutoReset(AutoReset_Time); // Solo relevante en Palabra Fija
    }

## Loop
Dentro del loop tendremos que revisar el estado del juego llamando a:

    bool print_progress = true; // Opcional, imprime progreso en Serial
    int juego_estado = Juego.UpdateJuegoBotones(print_progress);

La funcion <em>UpdateJuegoBotones</em>, devuelve un **int** que puede tener diferentes
valores. En la tabla se muestran los posibles valores, que significan, y unas etiquetas
predefinidas que se pueden usar para mejorar la legibilidad.

| juego_estado   | Siginificado | Etiqueta |
| :------------: | :------------ | :------------ |
| 1              | Juego Solucionado | LP_SOLUCIONADO |
| 0              | No se ha solucionado, ningun evento. | LP_NO_SOL |
| -1             | No se ha solucionado aun, boton apretado. | LP_APRETADO_NO_SOL |
| -2             | (Solo Palabra Fija), secuencia incorrecta. | LP_APRETADO_SEQ_MAL |


### Reset
Cuando el juego se resuelve (<em> juego_estado=1</em>) se queda resuelto hasta que
se llame a la funcion de reset.

Un ejemplo facil es una vez resuelto, poner un delay, y luego hacer un reset:

    if (juego_estado == LP_SOLUCIONADO){
      // ** Hacer algo aqui **
      delay(5000);
      Juego.Reset();
    }


# Posibles mejoras
Crear metodo **whatButton()** que te diga cual ha sido el ultimo boton apretado.
Util para casos donde juego_estado = -1
