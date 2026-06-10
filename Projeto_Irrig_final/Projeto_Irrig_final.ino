//Bibliotecas
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT11.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

//Pinagem
#define I2C_ADDR 0x27
#define DHTPIN 4
#define SOLO_PIN 34
#define RELE_PIN 26

//Objetos Criados
DHT11 dht11(DHTPIN);
LiquidCrystal_I2C lcd(I2C_ADDR, 16, 2);

//Variaveis
int temperatura = 0;
int umidadeAr = 0;
int umidadeSolo = 0;

bool irrigando = false;

//Wifi configurações
const char* ssid = "Seu Wifi";
const char* password = "Sua senha";

const char* serverName = "http://SeuIP:5000/receber_dados";
const char* confirmURL = "http://SeuIP:5000/confirmar_acao";

//Controle
unsigned long ultimoEnvio = 0;
unsigned long intervaloEnvio = 0;

int decision_id = -1;

// Menu do display
void menuPrincipal() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("T:    C S:   %");
  lcd.setCursor(0, 1);
  lcd.print("U:    % I:");
}

// Wifi
void conectarWiFi() {
  lcd.clear();
  lcd.print("Conectando WiFi");

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  lcd.clear();
  lcd.print("WiFi OK");
  delay(1000);
}

//Confirmação de acionamento
void confirmarAcao(int decision_id, int action) {
  HTTPClient http;

  lcd.clear();
  lcd.print("Confirmando...");

  http.begin(confirmURL);
  http.addHeader("Content-Type", "application/json");

  String json = "{\"decision_id\":" + String(decision_id) +
                ",\"action\":" + String(action) + "}";

  http.POST(json);
  http.end();
}

// leitura dos sesnores
void lerSensores() {
  int leituraSolo = analogRead(SOLO_PIN);
  umidadeSolo = map(leituraSolo, 4095, 0, 0, 100);
  umidadeSolo = constrain(umidadeSolo, 0, 100);

  dht11.readTemperatureHumidity(temperatura, umidadeAr);
}

// Atualização do display
void atualizarLCD() {
  menuPrincipal();

  lcd.setCursor(2, 0);
  lcd.print(temperatura);

  lcd.setCursor(12, 0);
  lcd.print(umidadeSolo);

  lcd.setCursor(2, 1);
  lcd.print(umidadeAr);

  lcd.setCursor(13, 1);
  if (irrigando) lcd.print("ON ");
  else lcd.print("OFF");
}

// Envio da API
void enviarParaAPI() {
  if (WiFi.status() != WL_CONNECTED) return;

  HTTPClient http;

  lcd.clear();
  lcd.print("Enviando API...");

  http.begin(serverName);
  http.addHeader("Content-Type", "application/json");

  String json = "{";
  json += "\"temperatura\":" + String(temperatura) + ",";
  json += "\"umidade_ar\":" + String(umidadeAr) + ",";
  json += "\"umidade_solo\":" + String(umidadeSolo);
  json += "}";

  int code = http.POST(json);

  if (code > 0) {
    String resposta = http.getString();

    StaticJsonDocument<200> doc;
    deserializeJson(doc, resposta);

    int irrigar = doc["irrigar"];
    decision_id = doc["decision_id"];

    if (irrigar == 1) {
      irrigando = true;

      lcd.clear();
      lcd.print("IRRIGANDO...");
      digitalWrite(RELE_PIN, LOW);
      delay(3000);
      digitalWrite(RELE_PIN, HIGH);

      confirmarAcao(decision_id, 1);

      intervaloEnvio = 60000; // 1 min

    } else {
      irrigando = false;
      intervaloEnvio = 3600000; // 1 hora
    }
  }

  http.end();
}

void setup() {
  Serial.begin(115200);

  pinMode(RELE_PIN, OUTPUT);
  digitalWrite(RELE_PIN, HIGH);

  lcd.init();
  lcd.backlight();

  conectarWiFi();

  // PRIMEIRA LEITURA
  lerSensores();
  enviarParaAPI();
  ultimoEnvio = millis();
}

void loop() {

  lerSensores();
  atualizarLCD();

  if (millis() - ultimoEnvio > intervaloEnvio) {
    ultimoEnvio = millis();
    enviarParaAPI();
  }

  delay(500);
}