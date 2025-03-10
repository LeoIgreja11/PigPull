//===============================================
// bibliotecas
#include "Arduino.h"               // Biblioteca padrão do Arduino para manipulação de hardware e funções básicas.
#include "DFRobotDFPlayerMini.h"    // Biblioteca para controlar o DFPlayer Mini, módulo de áudio.
#include "Freenove_WS2812_Lib_for_ESP32.h"  // Biblioteca para controlar a fita de LEDs WS2812 com ESP32.


#define ledGreen 27            // Led Modo: Pacífico
#define ledRed  26           // Led Modo: Agressivo/Perigo
#define bt_mode 32           // Botão de mudança de mod

//===============================================
// mapeamento de hardware
const int pinPot1 = 02,           // Pino analógico onde o potenciômetro (pot1) está conectado.
          pinPot2 = 04,           // Pino analógico onde o potenciômetro (pot1) está conectado.
          pinPot3 = 15,           // Pino analógico onde o potenciômetro (pot1) está conectado.
          pinPot4 = 13,           // Pino analógico onde o potenciômetro (pot1) está conectado.
          pinLed = 23,            // Pino onde os LEDs WS2812 estão conectados.
          numLed = 50;            // Número de LEDs na fita WS2812 (50 LEDs).
  


//===============================================
// variaveis globais
int randomMusic,                 // Variável para armazenar o número da música selecionada aleatoriamente (entre 1 e 5).
    randomLed;                   // Variável para armazenar o número do modo de LEDs selecionado aleatoriamente (entre 1 e 10).


//===============================================
// objetos
DFRobotDFPlayerMini myDFPlayer;          // Objeto para controlar o DFPlayer Mini (módulo de áudio).
Freenove_ESP32_WS2812 strip(numLed, pinLed, TYPE_GRB);  // Objeto para controlar a fita de LEDs WS2812 (50 LEDs, pino 15, formato GRB).


//===============================================
// protótipo das funções
void setupDfPlayer();                    // Protótipo da função para configurar o DFPlayer Mini.
void ledsMod(int modo, byte r, byte g, byte b, byte whell); // Protótipo da função para controlar os LEDs no modo especificado.


//===============================================
// funções principais
void setup() {
  // Inicializa o DFPlayer Mini
  pinMode(bt_mode, INPUT);
  pinMode(ledGreen, OUTPUT);
  pinMode(ledRed, OUTPUT);

bool switchState = digitalRead(bt_mode);

  setupDfPlayer();                     // Chama a função que configura o DFPlayer Mini.

  if (switchState == HIGH) { // Se a chave estiver conectando o pino ao GND
    digitalWrite(ledGreen, LOW);
    digitalWrite(ledRed, HIGH);
    randomMusic = random(1, 3);
} else if(switchState == LOW){
    digitalWrite(ledRed, LOW);
    digitalWrite(ledGreen, HIGH);
    randomMusic = random(3, 5);
      }

  randomLed =  random(1, 11);          // Seleciona aleatoriamente um modo de LED entre 1 e 10.

}


void loop() {
   static unsigned long time_millis = millis();
  
  int valuePot1 = analogRead(pinPot1);
  int valuePot2 = analogRead(pinPot2);
  int valuePot3 = analogRead(pinPot3);
  int valuePot4 = analogRead(pinPot4);

  bool switchState = digitalRead(bt_mode); // Lê o estado da chave
  int rangeMax = 3500;

  if ((millis() - time_millis) > 1000) {
    Serial.print("Chave: ");
    Serial.println(switchState ? "ON" : "OFF"); // Exibe estado da chave no Serial Monitor

    if ((valuePot1 > rangeMax) || (valuePot2 > rangeMax) || (valuePot3 > rangeMax) || (valuePot4 > rangeMax)) {
      myDFPlayer.play(randomMusic);
      int colorLed = random(0, 256);

      while (myDFPlayer.readState() & 0x01) {
        ledsMod(randomLed, 0x00, 0x00, 0x00, colorLed);
      }

      randomLed = random(1, 11);
      
      // Alteração de estado com base na chave de alavanca
        if (switchState == HIGH) { // Se a chave estiver conectando o pino ao GND
          digitalWrite(ledGreen, LOW);
          digitalWrite(ledRed, HIGH);
          randomMusic = random(1, 3);
  }     else if(switchState == LOW){
          digitalWrite(ledRed, LOW);
          digitalWrite(ledGreen, HIGH);
          randomMusic = random(3, 5);
        }
    }

    time_millis = millis();
  }
}


void setupDfPlayer() {
  // Inicia a comunicação serial para ESP32 (9600 baud) e para o monitor serial (115200 baud)
  Serial1.begin(9600, SERIAL_8N1, 16, 17);  // Comunicação com o DFPlayer Mini (pinos 16 e 17 do ESP32).
  Serial.begin(115200);  // Comunicação com o monitor serial (para enviar informações para o PC).

  // Imprime mensagens de inicialização no monitor serial
  Serial.println(F("Demonstração do DFRobot DFPlayer Mini"));
  Serial.println(F("Inicializando o DFPlayer... (Pode levar 3~5 segundos)"));

  // Tenta inicializar o DFPlayer Mini com a interface serial definida
  if (!myDFPlayer.begin(Serial1)) {  // Se falhar, imprime mensagens de erro e entra em loop infinito.
    Serial.println(F("Não foi possível inicializar:"));
    Serial.println(F("1. Por favor, verifique novamente a conexão!"));
    Serial.println(F("2. Por favor, insira o cartão SD!"));
    while (true);  // Entra em um loop infinito se a inicialização falhar.
  }

  // Se o DFPlayer Mini for inicializado com sucesso, imprime uma mensagem de sucesso.
  Serial.println(F("DFPlayer Mini online."));

  // Define o volume do DFPlayer Mini (faixa de 0 a 30)
  myDFPlayer.volume(30);
  strip.begin();           // Inicializa a fita de LEDs WS2812.
  for (int i = 0; i < numLed; i++) {
    strip.setLedColorData(i, 0, 0, 0);  // Desliga os LEDs.
    delay(5);
  }
  strip.show();
  strip.setBrightness(255);  // Define o brilho máximo dos LEDs (faixa de 0 a 255).
}


void ledsMod(int modo, byte r, byte g, byte b, byte whell) {
  int Time = 100;  // Define o intervalo de tempo para cada ciclo de LED (100 ms).

  // Verifica o modo de LEDs selecionado e executa a lógica correspondente.
  switch (modo) {
    case 1:
      // Modo 1: Pisca todos os LEDs com a cor especificada (vermelho no caso, r=255, g=0, b=0).
      for (int i = 0; i < numLed; i++) strip.setLedColorData(i, strip.Wheel(whell));  // Liga os LEDs.
      strip.show();  // Atualiza a fita de LEDs.
      delay(Time);   // Espera pelo tempo especificado.
      for (int i = 0; i < numLed; i++) strip.setLedColorData(i, 0, 0, 0);  // Desliga os LEDs.
      strip.show();  // Atualiza a fita de LEDs.
      delay(Time);   // Espera pelo tempo especificado.
      break;

    case 2:
      // Modo 2: Pisca LEDs alternados.
      for (int i = 0; i < numLed; i += 2) strip.setLedColorData(i, strip.Wheel(whell));  // Liga LEDs pares.
      strip.show();
      delay(Time);
      for (int i = 0; i < numLed; i += 2) strip.setLedColorData(i, 0, 0, 0);  // Desliga LEDs pares.
      for (int i = 1; i < numLed; i += 2) strip.setLedColorData(i, strip.Wheel(whell));  // Liga LEDs ímpares.
      strip.show();
      delay(Time);
      for (int i = 1; i < numLed; i += 2) strip.setLedColorData(i, 0, 0, 0);  // Desliga LEDs ímpares.
      strip.show();
      break;

    case 3:
      // Modo 3: Efeito de corrida (os LEDs se acendem um por vez).
      for (int i = 0; i < numLed; i++) {
        strip.setLedColorData(i, strip.Wheel(whell));  // Liga o LED atual.
        strip.show();
        delay(Time);
        strip.setLedColorData(i, 0, 0, 0);  // Desliga o LED atual.
      }
      break;

    case 4:
      // Modo 4: Todos os LEDs piscam em cores alternadas.
      for (int i = 0; i < numLed; i++) strip.setLedColorData(i, strip.Wheel(whell));  // Liga com a cor inicial.
      strip.show();
      delay(Time);
      for (int i = 0; i < numLed; i++) strip.setLedColorData(i, strip.Wheel(whell));  // Liga com a cor alternada.
      strip.show();
      delay(Time);
      break;

    case 5:
      // Modo 5: Efeito de preenchimento (LEDs acendem progressivamente e depois apagam).
      for (int i = 0; i < numLed; i++) {
        strip.setLedColorData(i, strip.Wheel(whell));
        strip.show();
        delay(Time);
      }
      for (int i = 0; i < numLed; i++) {
        strip.setLedColorData(i, 0, 0, 0);
        strip.show();
        delay(Time);
      }
      break;

    case 6:
      // Modo 6: LEDs piscam em pares (2 a 2).
      for (int i = 0; i < numLed; i += 2) {
        strip.setLedColorData(i, strip.Wheel(whell));
        if (i + 1 < numLed) strip.setLedColorData(i + 1, strip.Wheel(whell));
      }
      strip.show();
      delay(Time);
      for (int i = 0; i < numLed; i++) strip.setLedColorData(i, 0, 0, 0);
      strip.show();
      delay(Time);
      break;

    case 7:
      // Modo 7: LEDs piscam em sequência reversa.
      for (int i = numLed - 1; i >= 0; i--) {
        strip.setLedColorData(i, strip.Wheel(whell));
        strip.show();
        delay(Time);
        strip.setLedColorData(i, 0, 0, 0);
      }
      break;

    case 8:
      // Modo 8: Todos os LEDs acendem suavemente e apagam (efeito "breathing").
      for (int brightness = 0; brightness <= 255; brightness += 5) {
        for (int i = 0; i < numLed; i++) strip.setLedColorData(i, strip.Wheel(whell) * brightness / 255 );
        strip.show();
        delay(10);
      }
      for (int brightness = 255; brightness >= 0; brightness -= 5) {
        for (int i = 0; i < numLed; i++) strip.setLedColorData(i, strip.Wheel(whell) * brightness / 255 );
        strip.show();
        delay(10);
      }
      break;

    case 9:
      // Modo 9: LEDs piscam em blocos (blocos de 3 LEDs acendem juntos).
      for (int i = 0; i < numLed; i += 3) {
        for (int j = 0; j < 3 && i + j < numLed; j++) strip.setLedColorData(i + j, strip.Wheel(whell));
        strip.show();
        delay(Time);
        for (int j = 0; j < 3 && i + j < numLed; j++) strip.setLedColorData(i + j, strip.Wheel(whell));
      }
      break;

    case 10:
      // Modo 10: Efeito de "cometa" (um LED brilhante com uma cauda).
      for (int i = 0; i < numLed; i++) {
        strip.setLedColorData(i, r, g, b);
        if (i > 0) strip.setLedColorData(i - 1, strip.Wheel(whell) / 2);
        if (i > 1) strip.setLedColorData(i - 2, strip.Wheel(whell) / 4);
        strip.show();
        delay(Time);
        if (i > 2) strip.setLedColorData(i - 3, 0, 0, 0);
      }
      break;
  }
}