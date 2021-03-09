/*
  LP_JuegoBotones.h - Libreria para un codigo de botones
*/


#include "Arduino.h"

#ifndef LP_JuegoBotones
#define LP_JuegoBotones

#ifndef DigitalSignals
#include <DigitalSignals.h>
#endif


// Etiquetas Auxiliares
#define LP_SOLUCIONADO       1  // Juego Solucionado
#define LP_NO_SOL            0  // No Solucionado, nada apretado
#define LP_APRETADO_NO_SOL  -1  // No Solucionado, boton apretado
#define LP_APRETADO_SEQ_MAL -2  // No Solucionado, sequencia incorrecta

#ifndef LP_MAXSEQ
  #define LP_MAXSEQ  64  // Redefinir si secuencia mas larga
#endif
#ifndef LP_MAXBOT
  #define LP_MAXBOT  32  // Redifinir si se necesitan mas botones
#endif

class JuegoBotones
{
private:
  int _juego_resuelto = 0;
  bool _intento_continuo = 0;
  unsigned int _npressed = 0;
  unsigned int ib_seq = 0;
  unsigned int _mxbot = 0;
  unsigned int _seq_len = 0;

  InDSignal _Butt[LP_MAXBOT];
  unsigned int _butt_seq[LP_MAXSEQ];

  bool _use_autoreset_time = 0;
  long T_autoreset = 0;
  long t_autoreset = 0;

  int UpdateIntentoContinuo(bool print_progress);
  int UpdateIntentoFijo(bool print_progress);

public:
  JuegoBotones(void);

  // Pasar array de Botones
  void SetPinBotones(
    unsigned int pinBotones[],
    int _debounce=50, bool _active=0, bool _set_pullup=1,
    int mxbot =LP_MAXBOT);

  void SetSecuencia(
    unsigned int seq[], bool intento_continuo=1, int mxseq = LP_MAXSEQ
  );

  void SetTiempoAutoReset(long AutoReset_Time); // en [seg]
  // En el caso de longtiud fija, podemos hacer que el codigo se resetea despues
  //de un tiempo de inactividad
  void EndAutoReset();


  int UpdateJuegoBotones(bool print_progress = 0);


  // Devuelve el indice del boton que se ha pulsado
  // Si no devuelve -1
  int CualBotonPulsado();
  int Estado_Juego();

  void Reset();
};

// =============================================================================

// Constructor
JuegoBotones::JuegoBotones(){
  _juego_resuelto = 0;
  ib_seq = 0;
  _mxbot = 0;
  _seq_len = 0;
  _npressed = 0;

  _use_autoreset_time = 0;
  T_autoreset = 0;
  t_autoreset = 0;

}


// ========== FUNCIONES DE SETUP ===============================================
// ---- Set Pines de Los botones, llamar en Setup
void JuegoBotones::SetPinBotones(
  unsigned int pinBotones[],
  int _debounce, bool _active, bool _set_pullup,
  int mxbot)
{
  this->_mxbot = mxbot;
  for(int i=0; i<_mxbot ;i++){
    _Butt[i].setPin(pinBotones[i],_debounce,_active,_set_pullup);
  }
}

// ---- Guardar Secuencia
void JuegoBotones::SetSecuencia(
  unsigned int seq[], bool intento_continuo, int mxseq
){
    this->_seq_len = mxseq;
    this->_intento_continuo = intento_continuo;
    // Copy elements
    for(int i=0; i<_seq_len; i++){
        this->_butt_seq[i] = seq[i];
    }
}

void JuegoBotones::SetTiempoAutoReset(long AutoReset_Time){
  T_autoreset = AutoReset_Time*1000; //ms
  t_autoreset = 0;
  _use_autoreset_time = true;
}
void JuegoBotones::EndAutoReset(){
  _use_autoreset_time = false;
}


// ========== FUNCIONES DE LOOP ================================================
int JuegoBotones::UpdateJuegoBotones(bool print_progress)
{
  if (_intento_continuo) return this->UpdateIntentoContinuo(print_progress);
  else                  return this->UpdateIntentoFijo(print_progress);

}
// - CON INTENTO CONTINUO
int JuegoBotones::UpdateIntentoContinuo(bool print_progress)
{
  if (_juego_resuelto<1){
    int i_apretado = CualBotonPulsado();

    _juego_resuelto = LP_NO_SOL;

    if( i_apretado > -1){ // Si se ha apretado un boton
      unsigned int b_correcto = _butt_seq[ib_seq];
      _juego_resuelto = LP_APRETADO_NO_SOL;

      // Apretado es el de la secuencia?: Si
      if(i_apretado == b_correcto){
        ib_seq++;
        // Hemos Pulsado seq_len veces?
        if(ib_seq >= _seq_len){
          _juego_resuelto = LP_SOLUCIONADO;
        }
      }

      // Apretado es el de la secuencia?: No, pero es el del principio
      else if( i_apretado == _butt_seq[0]){
        // Caso especial para resolver: 3.3.2.1 (si la seq es 3.2.1)
          ib_seq = 1;
      }
      // Apretado es el de la secuencia?: No
      else {
        ib_seq = 0; // reset
      }
      if(print_progress){
        Serial.print("Boton Pulsado: "); Serial.print(i_apretado);
        Serial.print("    Boton Correcto: "); Serial.print(b_correcto);
        Serial.print("    nuevo ib_seq: "); Serial.print(ib_seq);
        Serial.print("    Resuelto?: "); Serial.print(_juego_resuelto);
        Serial.println();
        if(_juego_resuelto == LP_SOLUCIONADO){
           Serial.println("\t\t>> RESUELTO <<");
        }

      }
    }
  }
  return _juego_resuelto;
}
// -- CON LONGITUD FIJA
int JuegoBotones::UpdateIntentoFijo(bool print_progress)
{
  if (_juego_resuelto<1){

    // Gestionar Auto Reset
    // resetear contador si tiempo pasado desde ultimo intento es superior a T
    if(_use_autoreset_time){
      if( (millis() - t_autoreset) > T_autoreset){
        ib_seq = 0;
        _npressed = 0;
        t_autoreset = millis();
      }
    }

    _juego_resuelto = LP_NO_SOL;

    int i_apretado = CualBotonPulsado();
    // Se ha apretado un boton?
    if (i_apretado>-1){
      t_autoreset = millis();
      _npressed++;
      unsigned int b_correcto = _butt_seq[ib_seq];

      if (i_apretado == b_correcto)  ib_seq++;   // Correcto?: Si
      else                           ib_seq = 0; // Correcto?: NO

      // Se han apretado n_seq botones?
      if(_npressed >= _seq_len){
        if(ib_seq>=_seq_len)  _juego_resuelto = 1;
        else { // INCORRECTO
          _npressed = 0; // Reset Contador
          _juego_resuelto = LP_APRETADO_SEQ_MAL;
          ib_seq = 0;
        }
      }
      else{
        _juego_resuelto = LP_APRETADO_NO_SOL;
      }


      if(print_progress){
            Serial.print("B Pulsado: "); Serial.print(i_apretado);
            Serial.print("   B Correcto: "); Serial.print(b_correcto);
            Serial.print("   Npressed: "); Serial.print(_npressed);
            Serial.print("   nuevo ib_seq: "); Serial.print(ib_seq);
            Serial.print("   Resuelto?: "); Serial.print(_juego_resuelto);
            Serial.println();
      }
    }
  }

  return _juego_resuelto;
}




// ========== FUNCIONES UTILES PARA EL USUARIO==================================
// Devuelve el indice del boton que se ha pulsado
// Si no devuelve -1
// NO LLAMAR DURANTE EL UPDATE: PUEDE GENERAR CONFLICTO
int JuegoBotones::CualBotonPulsado()
{
  for(int _i=0; _i<_mxbot; _i++){
    if(_Butt[_i].is_rising()){
      return _i;
    }
  }
  return -1;
}

// Te devuelve el valor de ib_seq
int JuegoBotones::Estado_Juego()
{
  return (ib_seq);
}

// Resetea el contador
void JuegoBotones::Reset(){
  _juego_resuelto = 0;
  ib_seq = 0;
  _npressed = 0;
}

#endif
