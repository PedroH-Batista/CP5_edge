#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

// Configurações - variáveis editáveis
const char* default_SSID = "Wokwi-GUEST";
const char* default_PASSWORD = "";
const char* default_BROKER_MQTT = "20.150.210.54";
const int default_BROKER_PORT = 1883;
const char* default_TOPICO_SUBSCRIBE = "/TEF/lamp003/cmd";
const char* default_TOPICO_PUBLISH_1 = "/TEF/lamp003/attrs";
const char* default_TOPICO_PUBLISH_2 = "/TEF/lamp003/attrs/l";
const char* default_TOPICO_PUBLISH_3 = "/TEF/lamp003/attrs/h";
const char* default_TOPICO_PUBLISH_4 = "/TEF/lamp003/attrs/t";
const char* default_TOPICO_PUBLISH_5 = "/TEF/lamp003/attrs/a";
const char* default_ID_MQTT = "lamp003";
const int default_D4 = 2;
const char* topicPrefix = "lamp003";

// Variáveis para configurações editáveis
char* SSID = const_cast<char*>(default_SSID);
char* PASSWORD = const_cast<char*>(default_PASSWORD);
char* BROKER_MQTT = const_cast<char*>(default_BROKER_MQTT);
int BROKER_PORT = default_BROKER_PORT;
char* TOPICO_SUBSCRIBE = const_cast<char*>(default_TOPICO_SUBSCRIBE);
char* TOPICO_PUBLISH_1 = const_cast<char*>(default_TOPICO_PUBLISH_1);
char* TOPICO_PUBLISH_2 = const_cast<char*>(default_TOPICO_PUBLISH_2);
char* TOPICO_PUBLISH_3 = const_cast<char*>(default_TOPICO_PUBLISH_3);
char* TOPICO_PUBLISH_4 = const_cast<char*>(default_TOPICO_PUBLISH_4);
char* TOPICO_PUBLISH_5 = const_cast<char*>(default_TOPICO_PUBLISH_5);
char* ID_MQTT = const_cast<char*>(default_ID_MQTT);
int D4 = default_D4;

WiFiClient espClient;
PubSubClient MQTT(espClient);
char EstadoSaida = '0';

// Temperatura e Umidade com DHT22
#define DHTPIN 15
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

void initSerial() {
  Serial.begin(115200);
}

void initWiFi() {
  delay(10);
  Serial.println("------Conexao WI-FI------");
  Serial.print("Conectando-se na rede: ");
  Serial.println(SSID);
  Serial.println("Aguarde");
  reconectWiFi();
}

void initMQTT() {
  MQTT.setServer(BROKER_MQTT, BROKER_PORT);
  MQTT.setCallback(mqtt_callback);
}

void setup() {
  InitOutput();
  initSerial();
  initWiFi();
  initMQTT();
  delay(5000);
  // MQTT.publish(TOPICO_PUBLISH_1, "s|on"); // Comentado
}

void loop() {
  VerificaConexoesWiFIEMQTT();
  // EnviaEstadoOutputMQTT(); // Comentado
  handleLuminosity();
  MQTT.loop();
}

void reconectWiFi() {
  if (WiFi.status() == WL_CONNECTED) return;

  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Conectado com sucesso na rede ");
  Serial.print(SSID);
  Serial.println("IP obtido: ");
  Serial.println(WiFi.localIP());

  digitalWrite(D4, LOW); // LED inicia desligado
}

void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  String msg;
  for (int i = 0; i < length; i++) {
    char c = (char)payload[i];
    msg += c;
  }

  Serial.print("- Mensagem recebida: ");
  Serial.println(msg);

  if (msg.equals("on")) {
    digitalWrite(D4, HIGH);
    EstadoSaida = '1';
  }

  if (msg.equals("off")) {
    digitalWrite(D4, LOW);
    EstadoSaida = '0';
  }
}

void VerificaConexoesWiFIEMQTT() {
  if (!MQTT.connected()) reconnectMQTT();
  reconectWiFi();
}

/*
void EnviaEstadoOutputMQTT() {
  // ... código original
  delay(5000);
}
*/

void InitOutput() {
  pinMode(D4, OUTPUT);
  digitalWrite(D4, HIGH);
  boolean toggle = false;

  for (int i = 0; i <= 10; i++) {
    toggle = !toggle;
    digitalWrite(D4, toggle);
    delay(200);
  }
}

void reconnectMQTT() {
  while (!MQTT.connected()) {
    Serial.print("* Tentando se conectar ao Broker MQTT: ");
    Serial.println(BROKER_MQTT);

    if (MQTT.connect(ID_MQTT)) {
      Serial.println("Conectado com sucesso ao broker MQTT!");
      MQTT.subscribe(TOPICO_SUBSCRIBE);
    } else {
      Serial.println("Falha ao reconectar no broker.");
      Serial.println("Haverá nova tentativa de conexão em 2s");
      delay(2000);
    }
  }
}

void handleLuminosity() {
  const int potLum = 34;
  int sensorValue = analogRead(potLum);
  int luminosity = map(sensorValue, 0, 4095, 0, 100);

  float h = dht.readHumidity();
  float t = dht.readTemperature();

  const int potPin = 35;
  int sensorPot = analogRead(potPin);
  int alagamento = map(sensorPot, 0, 4095, 0, 100);

  if (isnan(h) || isnan(t)) {
    Serial.println("Falha ao ler o sensor DHT22!");
    return;
  }

  String payload = "t|" + String(t) +
                   "|h|" + String(h) +
                   "|l|" + String(luminosity) +
                   "|a|" + String(alagamento);

  Serial.print("--- ENVIANDO DADOS UL2.0 ---");
  Serial.print("Payload: ");
  Serial.println(payload.c_str());

  MQTT.publish(TOPICO_PUBLISH_1, payload.c_str());

  // Envio por tópicos separados removido
  // MQTT.publish(TOPICO_PUBLISH_2, String(luminosity).c_str());
  // MQTT.publish(TOPICO_PUBLISH_3, String(h).c_str());
  // MQTT.publish(TOPICO_PUBLISH_4, String(t).c_str());
  // MQTT.publish(TOPICO_PUBLISH_5, String(sensorPot).c_str());

  delay(10000); // Envia a cada 10 segundos
}