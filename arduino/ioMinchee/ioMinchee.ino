
#include <SPI.h>
#include <WiFi101.h>
//#include <WiFiSSLClient.h>
#include <FlashStorage.h>
#include <MQTTClient.h>

boolean _debug = true;

// Inital AP & WiFi
char ssid[] = "lightme";
char pass[] = "lightme";
int wifi_status = WL_IDLE_STATUS;
WiFiClient wifi;
//WiFiSSLClient wifi;

// Flash storage
typedef struct{
  char ssid[20];
  char pwd[20];
  char deviceName[20];
  boolean valid;
} Settings;
FlashStorage(flash_storage, Settings);
Settings settings;

// MQTT
MQTTClient mqtt;
const char broker[] = "172.24.14.79";
// temporary
const char deviceId[]   = "11214bf3-3ef8-4635-b2d8-19fc3791a4b9";
const char devicePsw[]  = "b049f0ad-d9af-4e9b-a2ef-cdff2b29d61e";


void setup() {
  if (_debug) {
    SERIAL_PORT_MONITOR.begin(9600);
    while (!SERIAL_PORT_MONITOR) { }
    Serial.println("rebooting");
    saveSettings();
  }

  getSettings();
  if (!settings.valid) {
    ap();
  } else {
    connect();
  }

}

void loop() {
  // put your main code here, to run repeatedly:
  mqtt.loop();
  if(!mqtt.connected()){
    connect();
  }
}

void ap() {
  if (_debug) {
    Serial.println("starting AP...");
  }
  if (wifi_status == WL_IDLE_STATUS) {
    wifi_status = WiFi.beginAP(ssid);
    delay(4000);
    ap();
  }
}

void saveSettings() {
  String ssid = "USJ.Staff";
  String pwd = "usjwirelessstaff001";
  String deviceName = "mkr1000";
  settings.valid = true;
  ssid.toCharArray(settings.ssid, 20);
  pwd.toCharArray(settings.pwd, 20);
  deviceName.toCharArray(settings.deviceName, 20);
  flash_storage.write(settings);
  if (_debug) {
    Serial.println("settings saved...");
  }
}

void getSettings() {
  settings = flash_storage.read();
  if (_debug) {
    if (!settings.valid) {
      Serial.println("cannot find saved settings...");
    } else {
      Serial.println( "Read custom object from EEPROM: " );
      Serial.println( settings.ssid );
      Serial.println( settings.pwd );
      Serial.println( settings.deviceName ); 
    }
  }
}

void connect() {
  while (wifi_status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(settings.ssid);
    wifi_status = WiFi.begin(settings.ssid, settings.pwd);
    // Wait 10 seconds for connection:
    delay(1000);
  }
  if (_debug) {
    Serial.println("WiFi connected:");
    IPAddress ip = WiFi.localIP();
    Serial.print("IP Address: ");
    Serial.println(ip);
    printMacAddress();
  }

  mqtt.begin(broker, wifi);
  while(!mqtt.connect(settings.deviceName, deviceId, devicePsw)) {
    delay(1000);
  }
  mqtt.publish("clients/mkr1000","json here...");
  mqtt.subscribe("devices/mkr1000");
  if (_debug) {
    Serial.println("connected to MQTT broker...");
  }
}

void messageReceived(String topic, String payload, char * bytes, unsigned int length) {
  if (payload == "on") {
    digitalWrite(LED_BUILTIN, HIGH);
  } else {
    digitalWrite(LED_BUILTIN, LOW);
  }
  Serial.print("incoming: ");
  Serial.print(topic);
  Serial.print(" - ");
  Serial.print(payload);
  Serial.println();
}

void printMacAddress() {
  // the MAC address of your Wifi shield
  byte mac[6];

  // print your MAC address:
  WiFi.macAddress(mac);
  Serial.print("MAC: ");
  Serial.print(mac[5], HEX);
  Serial.print(":");
  Serial.print(mac[4], HEX);
  Serial.print(":");
  Serial.print(mac[3], HEX);
  Serial.print(":");
  Serial.print(mac[2], HEX);
  Serial.print(":");
  Serial.print(mac[1], HEX);
  Serial.print(":");
  Serial.println(mac[0], HEX);
}

//void listNetworks() {
//  // scan for nearby networks:
//  Serial.println("** Scan Networks **");
//  int numSsid = WiFi.scanNetworks();
//  if (numSsid == -1)
//  {
//    Serial.println("Couldn't get a wifi connection");
//    while (true);
//  }
//
//  // print the list of networks seen:
//  Serial.print("number of available networks:");
//  Serial.println(numSsid);
//
//  // print the network number and name for each network found:
//  for (int thisNet = 0; thisNet < numSsid; thisNet++) {
//    Serial.print(thisNet);
//    Serial.print(") ");
//    Serial.print(WiFi.SSID(thisNet));
//    Serial.print("\tSignal: ");
//    Serial.print(WiFi.RSSI(thisNet));
//    Serial.print(" dBm");
//    Serial.print("\tEncryption: ");
//    printEncryptionType(WiFi.encryptionType(thisNet));
//    Serial.flush();
//  }
//}

