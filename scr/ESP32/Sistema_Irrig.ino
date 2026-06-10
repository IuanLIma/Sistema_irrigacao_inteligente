//Inclusão de bibliotecas
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT11.h>

//Inclusão da pinagem 
#define I2C_ADDR 0x27
#define DHTPIN 4
#define SOLO_PIN 34
#define RELE_PIN 26
#define BOTAO_PIN 27

//Criação dos objetos
DHT11 dht11(DHTPIN);
LiquidCrystal_I2C lcd(I2C_ADDR, 16, 2);

//Criação das variaveis
int temperatura = 0;
int umidadeAr = 0;
int umidadeSolo = 0;

//criação de variaveis do sistema
bool estadoRele = false;
bool ultimoEstadoBotao = HIGH;
unsigned long ultimoDebounce = 0;
const unsigned long delayDebounce = 50;
unsigned long ultimoDHT = 0;
const unsigned long intervaloDHT = 2000;

void setup() {
  
  //inicialização e Modo dos pinos
  //Wire.begin(33, 32);
  Serial.begin(115200);
  pinMode(RELE_PIN, OUTPUT);
  pinMode(BOTAO_PIN, INPUT_PULLUP);
  digitalWrite(RELE_PIN, 0);

  // Criação do menu de info
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("T:    C  S:   %");
  lcd.setCursor(0, 1);
  lcd.print("U:    %");
}

void loop() {

  //Debouce do botão

  static bool estadoBotaoAtual = HIGH;
  static bool estadoBotaoAnterior = HIGH;

  bool leitura = digitalRead(BOTAO_PIN);

  if (leitura != estadoBotaoAnterior) {
    ultimoDebounce = millis();
  }

  if ((millis() - ultimoDebounce) > delayDebounce) {
    if (leitura != estadoBotaoAtual) {
      estadoBotaoAtual = leitura;

      if (estadoBotaoAtual == LOW) {
        estadoRele = !estadoRele;
        Serial.println(estadoRele ? "RELE: LIGADO" : "RELE: DESLIGADO");
      }
    }
  }

  estadoBotaoAnterior = leitura;

  //controle do rele
  digitalWrite(RELE_PIN, estadoRele ? LOW : HIGH);

  //leitura da umidade do solo
  int leituraSolo = analogRead(SOLO_PIN);
  umidadeSolo = map(leituraSolo, 4095, 0, 0, 100);
  umidadeSolo = constrain(umidadeSolo, 0, 100);
  
  // Leitura e validação do DHT11
  if (millis() - ultimoDHT > intervaloDHT) {
    ultimoDHT = millis();
    int result = dht11.readTemperatureHumidity(temperatura, umidadeAr);
    if (result != 0) {
      Serial.println("Erro ao ler DHT11");
    }
  }

  // Atualização de variaveis no display
  lcd.setCursor(2, 0);
  lcd.print("    ");
  lcd.setCursor(2, 0);
  lcd.print(temperatura);

  lcd.setCursor(12, 0);
  lcd.print("   ");
  lcd.setCursor(12, 0);
  lcd.print(umidadeSolo);

  lcd.setCursor(2, 1);
  lcd.print("    ");
  lcd.setCursor(2, 1);
  lcd.print(umidadeAr);

  // Debug da serial para analise bruto dos dados
  Serial.print("Temp: ");
  Serial.print(temperatura);
  Serial.print(" | Umidade Ar: ");
  Serial.print(umidadeAr);
  Serial.print(" | Solo: ");
  Serial.print(umidadeSolo);
  Serial.print(" | Leitura Bruta: ");
  Serial.println(leituraSolo);

  delay(100);
}


