//===============================================
// bibliotecas
#include "Arduino.h"               // Biblioteca padrão do Arduino para manipulação de hardware e funções básicas.
#include "DFRobotDFPlayerMini.h"    // Biblioteca para controlar o DFPlayer Mini, módulo de áudio.
#include "Freenove_WS2812_Lib_for_ESP32.h"  // Biblioteca para controlar a fita de LEDs WS2812 com ESP32.


//===============================================
// mapeamento de hardware
const int pinPot1 = 02,           // Pino analógico onde o potenciômetro (pot1) está conectado.
          pinPot2 = 04,           // Pino analógico onde o potenciômetro (pot1) está conectado.
          pinPot3 = 15,           // Pino analógico onde o potenciômetro (pot1) está conectado.
          pinPot4 = 13,           // Pino analógico onde o potenciômetro (pot1) está conectado.
          pinLed = 23,           // Pino onde os LEDs WS2812 estão conectados.
          numLed = 50;           // Número de LEDs na fita WS2812 (50 LEDs).


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
  setupDfPlayer();                     // Chama a função que configura o DFPlayer Mini.
  randomLed =  random(1, 11);          // Seleciona aleatoriamente um modo de LED entre 1 e 10.
  randomMusic =  random(1, 6);         // Seleciona aleatoriamente uma música entre 1 e 5.
}


void loop() {
  static unsigned long time_millis = millis();  // Armazena o tempo desde o início do programa (usado para controle de tempo sem bloqueio).
  int valuePot1 = analogRead(pinPot1),  // Lê o valor do potenciômetro (entre 0 e 1023) no pino analógico 2.
      valuePot2 = analogRead(pinPot2),  // Lê o valor do potenciômetro (entre 0 e 1023) no pino analógico 2.
      valuePot3 = analogRead(pinPot3),  // Lê o valor do potenciômetro (entre 0 e 1023) no pino analógico 2.
      valuePot4 = analogRead(pinPot4);  // Lê o valor do potenciômetro (entre 0 e 1023) no pino analógico 2.

  int rangeMax = 3500;

  // A cada 1 segundo (1000 ms), executa o bloco de código a seguir
  if ((millis() - time_millis) > 1000) {
    // Imprime informações no monitor serial a cada 1 segundo
    Serial.print("Valor do pot:[");
    Serial.print(valuePot1);  // Exibe o valor lido do potenciômetro.
    Serial.print("] | Estado do DFPlayer Mini: [");
    Serial.print(myDFPlayer.readState() & 0x01);  // Verifica se o DFPlayer Mini está tocando (retorna 1 se estiver).
    Serial.print("] | Modo do led: [");
    Serial.print(randomLed);   // Exibe o modo atual dos LEDs.
    Serial.print("] | Modo da musica: [");
    Serial.print(randomMusic);  // Exibe a música que está tocando atualmente.
    Serial.println("]");

    // Se o valor do potenciômetro for maior que 3500 (próximo do valor máximo de 1023), executa a lógica
    if ( (valuePot1 > rangeMax) || (valuePot2 > rangeMax) || (valuePot3 > rangeMax) || (valuePot4 > rangeMax)  ) {
      myDFPlayer.play(randomMusic);  // Toca a música selecionada aleatoriamente.
      int colorLed;

      colorLed = random(0, 256);

      // Enquanto a música estiver tocando (verifica o estado do DFPlayer Mini), os LEDs piscam.
      while (myDFPlayer.readState() & 0x01) ledsMod(randomLed, 0x00, 0x00, 0x00, colorLed); // Chama a função que controla os LEDs com a cor vermelha (r=255, g=0, b=0).

      // Após a música terminar, seleciona novos valores aleatórios para o modo de LEDs e a música.
      randomLed =  random(1, 11);    // Seleciona um novo modo aleatório para os LEDs.
      randomMusic =  random(1, 6);   // Seleciona uma nova música aleatória para tocar.
    }

    time_millis = millis();  // Atualiza o tempo de referência para o próximo intervalo de 1 segundo.
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
