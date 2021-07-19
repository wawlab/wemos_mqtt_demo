#include <ESP8266WiFi.h>
#include <PubSubClient.h>
//==========================================================================================================================
//NOTE: THIS IS THE ONLY PART YOU CAN CHANGE
//This is your internet connected WIFI-network where this device should be connected
const char* wifi_SSID = "weke-weke"; //"change using your wifi ssid";
const char* wifi_PASSWORD = "password"; //"change using your wifi password";

//Please fill-up the following information for filtering

const char* wawhub_BROKER = "test.mosquitto.org";
const int wawhub_PORT = 1883; 
const char* wawhub_USERNAME = "guest"; 
const char* wawhub_PASSWORD = "guest"; 

//Note: special characters like  ( . , / ~ ! @ # $ % ^ & * () _ + = [] {} | \ : ' " <> ? will invalidated your IDs    

const char* wawhub_CLIENT_ID = "Victor"; // it's ok to use your name if you don't have a clientID yet;
const char* wawhub_DEVICE_ID = "Lamp"; // your device name or issued deviceID;  

unsigned long lastMillis = 0;
unsigned long REPORTING_PERIOD_MS = 1000; //how many time to send data to MQTT broker

//==========================================================================================================================

//************************************************************************************************
//Variable declarations
WiFiClient WAWHubClient;
PubSubClient MQTTClient(WAWHubClient);
  
char msgTopic[100];
char msgPacket[200];

//================================================================================================

//************************************************************************************************

void setup_WIFI() {

//We start by connecting to a WiFi network
  delay(10);
  Serial.println();   

  Serial.print("Connecting to WIFI -> ");
  Serial.println(wifi_SSID);

  WiFi.begin(wifi_SSID, wifi_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  //randomSeed(micros());

  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.print(WiFi.localIP());
  Serial.println(" ");
}

//************************************************************************************************

//Let the fun begin!!! ***************************************************************************
void setup() {

  Serial.begin(115200);
  
  setup_WIFI();
  
  MQTTClient.setServer(wawhub_BROKER, wawhub_PORT);
  delay(1000);
  
  // Loop until we're reconnected
  while (!MQTTClient.connected()) {
    Serial.print("Attempting WAWHub connection...");

    // Attempt to connect
    if (MQTTClient.connect(wawhub_DEVICE_ID, wawhub_USERNAME, wawhub_PASSWORD)) {
        Serial.println("connected"); // Once connected, publish an announcement...
        snprintf (msgPacket, 100, "%s/%s", wawhub_CLIENT_ID, wawhub_DEVICE_ID);

        Serial.print("Publish-> "); Serial.print("PRESENCE/"); Serial.println(msgPacket);
        MQTTClient.publish("PRESENCE",  msgPacket);
    } else {
        Serial.print("failed, rc=");
        Serial.print(MQTTClient.state());
        Serial.println(" try again in 5 seconds");
        delay(5000);       // Wait 5 seconds before retrying
    }
  }

  randomSeed(analogRead(0));
     
}
//************************************************************************************************

void loop() {

    if ((millis() - lastMillis) > REPORTING_PERIOD_MS) {
      lastMillis = millis();
      double tempValue = random(300);
      Serial.println(tempValue);
      delay(50);
      snprintf(msgTopic, 100, "%s/%s/%s", "House", wawhub_CLIENT_ID, wawhub_DEVICE_ID);
      snprintf(msgPacket, 200, "%lf,%s", tempValue, wawhub_CLIENT_ID);
      Serial.print("Publish-> House/wawhub_CLIENT_ID/wawhub_DEVICE_ID");  
      Serial.print(" | ");
      Serial.print(msgTopic); 
      Serial.print(" | "); 
      Serial.println(msgPacket);
      MQTTClient.publish(msgTopic, msgPacket);
    }
    
    MQTTClient.loop();      
}
