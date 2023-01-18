#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SoftwareSerial.h>
SoftwareSerial serial(25,26);


const char* ssid = "mans";
const char* password = "beb_beb_123";
const char* mqtt_server = "broker.hivemq.com";
String clientID = "SIS_IoT-"; 
int port = 1883;
const char* topic_Subscribe     = "SIS/fromServer/cmd";
const char* topic_Publish_msg0  = "SIS/toServer/Connection";
const char* topic_Publish_msg1  = "SIS/toServer/soilMoisureSensor";
const char* topic_Publish_msg2  = "SIS/toServer/waterLevelSensor";

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {

  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

// for incoming msg for esp
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (unsigned int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  if ((char)payload[0] == '1') {
    digitalWrite(LED_BUILTIN, LOW);  

  } else {
    digitalWrite(LED_BUILTIN, HIGH);  
  }

}


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");

    clientID += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientID.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(topic_Publish_msg0, "Connect"); 
      // ... and resubscribe
      client.subscribe(topic_Subscribe);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(3000);
    }
  }
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);     
  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, port);
  client.setCallback(callback);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  if (Serial.available()) {
        int readUART=Serial.read();
        //Serial.write(x);

      Serial.println(readUART);
      //Serial.print('\n');
      if(readUART <= 127) 
      { // // soil sensor
        String soil_read = String(readUART);
        char buffer[soil_read.length() + 1];
        soil_read.toCharArray(buffer, soil_read.length() + 1);
        Serial.print("Publish message soil : ");
        Serial.println(soil_read);
        client.publish(topic_Publish_msg1, buffer);

      } 
      else
      {
        String water_read = String(readUART & 0b01111111);
        char buffer[water_read.length() + 1];
        water_read.toCharArray(buffer, water_read.length() + 1);
        Serial.print("Publish message water : ");
        Serial.println(water_read);
        client.publish(topic_Publish_msg2, buffer);
      }
  }
  delay(2000);
}