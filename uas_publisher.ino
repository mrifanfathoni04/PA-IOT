#include <WiFi.h>
#include <PubSubClient.h>

// Definisi WiFi
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// Definisi MQTT
const char* mqttServer = "broker.hivemq.com";
const int mqttPort = 1883;
const char* mqttTopic = "papraktikumc2";

// Pin Ultrasonik
const int trigPin = 18;
const int echoPin = 5;

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

void setup() {
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  connectToInternet();
  setupMQTT();
}

void connectToInternet() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Connected to Wi-Fi");
  Serial.print("IP Address : ");
  Serial.println(WiFi.localIP());
}

void setupMQTT() {
  mqttClient.setServer(mqttServer, mqttPort);
}

void reconnectMQTT() {
  Serial.println("Connecting to MQTT Broker...");
  while (!mqttClient.connected()) {
    Serial.println("Attempting MQTT connection...");
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    
    if (mqttClient.connect(clientId.c_str())) {
      Serial.println("Connected to MQTT Broker.");
    } else {
      Serial.print("Failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" Retrying in 5 seconds...");
      delay(5000);
    }     
  }
}

long readUltrasonicDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  long duration = pulseIn(echoPin, HIGH);
  long distance = duration * 0.017; 
  
  return distance;
}

void loop() {
  if (!mqttClient.connected()) {
    reconnectMQTT();
  }

  long distance = readUltrasonicDistance();

    mqttClient.publish(mqttTopic, String(distance).c_str());

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" ");

  delay(200);
}