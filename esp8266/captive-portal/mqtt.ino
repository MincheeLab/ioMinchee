
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // DO SOMETHING WITH THE PAYLOAD
}

void mqttConnect() {
  client.setServer(mqtt_host, 1883);
  client.setCallback(callback);
  
//   Loop until we're reconnected
  while (client.state() != MQTT_CONNECTED) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("TheLAMPClient")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
