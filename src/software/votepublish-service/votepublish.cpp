#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <string>

// Pin and ID Configuration
// Pins order #1 to #8 (right to left)
const int button_pins[8] = {13, 32, 33, 25, 26, 27, 14, 12};

// MQTT and WiFi configuration
const char *ssid = "lab@i17";
const char *password = "lab@i17!";
const char *broker = "lab.bpm.in.tum.de";
const int port = 1883;

// Definition of publishing topic 
const char *votes_topic = "al/votes";

// Enable MQTT service
bool serviceEnabled = true;

// WiFi/MQTT
WiFiClient espClient;
PubSubClient client(espClient);

// Debounce config
const unsigned long DEBOUNCE_MS = 25;

// State tracking
int lastLogical[8];            // 0 = not pressed, 1 = pressed
int lastHighLow[8];            // raw HIGH/LOW
unsigned long lastChangeMs[8]; // last transition time (ms)

// WiFi setup
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

// Optional subscription to topic to see if message is actually published
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.println(topic);
  String message;
  for (unsigned int i = 0; i < length; i++) message += (char)payload[i];
  Serial.print("Message: ");
  Serial.println(message);
}

void setupMQTT() {
  setup_wifi();
  client.setServer(broker, port);
  client.setCallback(callback);
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Client_8ButtonPublisher")) {
      Serial.println("connected");
      // Optional: subscribe to the same topic for echo tests
      client.subscribe(votes_topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}


// Publish the button ID (1-8) to the votes_topic
void publishVote(int buttonIndex) {
  // Convert button index (0-7) to button ID (1-8) and then to a String
  String payload = String(buttonIndex + 1); 
  
  if (client.publish(votes_topic, payload.c_str())) {
    Serial.print("Published '");
    Serial.print(payload); 
    Serial.print("' to topic: ");
    Serial.println(votes_topic);
  } else {
    Serial.println("MQTT Publish failed.");
  }
}


// Setup & loop
void setup() {
  Serial.begin(9600);
  // Console output
  Serial.println("Starting MQTT Publisher (sends button ID 1-8 on press)...");

  // Init buttons & debouncer state
  for (int i = 0; i < 8; i++) {
    pinMode(button_pins[i], INPUT_PULLUP); // buttons wired to GND
    int highlow = digitalRead(button_pins[i]);
    lastHighLow[i] = highlow;
    lastChangeMs[i] = millis();
    lastLogical[i] = (highlow == LOW) ? 1 : 0; // 1=pressed, 0=released
    Serial.printf("Button %d (GPIO %d): Initial State %d\n", i + 1, button_pins[i], lastLogical[i]);
  }

  if (serviceEnabled) setupMQTT();
}

void loop() {
  if (serviceEnabled) {
    if (!client.connected()) reconnect();
    client.loop();
  }

  unsigned long now = millis();

  for (int i = 0; i < 8; i++) {
    int highlow = digitalRead(button_pins[i]);

    // Reading the raw voltage state of each button and check if it changed and when it changed
    if (highlow != lastHighLow[i]) {
      lastHighLow[i] = highlow;
      lastChangeMs[i] = now;
    }

    // Check if the status is stable for debounce ms
    if ((now - lastChangeMs[i]) >= DEBOUNCE_MS) {
      int logical = (highlow == LOW) ? 1 : 0; // low = pressed

      if (logical != lastLogical[i]) {
        lastLogical[i] = logical;
        Serial.printf("Button %d (GPIO %d) CHANGED to: %d\n", i + 1, button_pins[i], logical);

        // On press publish the button's ID
        if (logical == 1) {
          Serial.printf("Button %d pressed. Sending MQTT vote.\n", i + 1);

          if (serviceEnabled) {
            publishVote(i); // Pass button id and call mqqt function
          }
        }
      }
    }
  }

  delay(1);
}