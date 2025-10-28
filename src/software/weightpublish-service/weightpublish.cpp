#include <HX711_ADC.h>
#include <EEPROM.h>
#include <Wire.h>
#include <string>
#include <WiFi.h>
#include <PubSubClient.h>

// Setting the number of load cells
const int numLoadCells = 4;

// Settings pins for load cells
const int HX711_dout[numLoadCells] = {26,17,4,33}; // dout pins 
const int HX711_sck[numLoadCells] = {27,5,16,25}; // sck pins 

// HX711 constructor array for load cells
HX711_ADC LoadCell[numLoadCells] = {
    HX711_ADC(HX711_dout[0], HX711_sck[0]),
    HX711_ADC(HX711_dout[1], HX711_sck[1]),
    HX711_ADC(HX711_dout[2], HX711_sck[2]),
    HX711_ADC(HX711_dout[3], HX711_sck[3])
};


// Setting calibration and EPROM adresses
float calibrationValue[numLoadCells] = {107.05,-410.06,-395.11,107.84}; 
const int calVal_eepromAdress[numLoadCells] = {0, 10, 20, 30}; 

bool useEEPROM = true;
bool serviceEnabled = true;

unsigned long t = 0;
const int serialPrintInterval = 100;
bool enableBootDisplay = true;

// Wlan ssid and password
const char *ssid = "lab@i17"; 
const char *password = "lab@i17!"; 

const int port = 1883; 
const std::string scaleIDs[numLoadCells] = {"1", "2", "3", "4"}; // Setting ID's for the scales

// Arrays to track the weight and changes in weight
int displayedWeight[numLoadCells] = {0, 0, 0, 0}; 
int lastWeight[numLoadCells] = {0, 0, 0, 0}; 
int lastState[numLoadCells] = {0, 0, 0, 0}; 
bool firstMeasurement[numLoadCells] = {true, true, true, true}; 

// WiFi setup
WiFiClient espClient;
PubSubClient client(espClient);

// Loadcell setup
void setupLoadcell()
{
    for (int i = 0; i < numLoadCells; i++) {
        Serial.print("Using Calibration for LoadCell ");
        Serial.print(i + 1);
        Serial.print(": ");
        Serial.println(calibrationValue[i]);

        LoadCell[i].begin();
        unsigned long stabilizingtime = 2000;
        boolean _tare = true;
        LoadCell[i].start(stabilizingtime, _tare);
        if (LoadCell[i].getTareTimeoutFlag()) {
            Serial.print("Timeout, check wiring to HX711 and pin designations for LoadCell ");
            Serial.println(i + 1);
            while (true);
        } else {
            LoadCell[i].setCalFactor(calibrationValue[i]);
            Serial.print("LoadCell ");
            Serial.print(i + 1);
            Serial.println(" Startup is complete");
        }
    }
}

// WiFi setup
void setup_wifi()
{
    delay(10);
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

void callback(char *topic, byte *message, unsigned int length)
{
    // For now we do not care about any topics, so we do not implement it
}

// MQTT setup
void setupMQTT()
{
    setup_wifi();
    client.setServer("lab.bpm.in.tum.de", port);
    client.setCallback(callback);
}

unsigned long lastPublishTime = 0;
const unsigned long publishInterval = 1000; // every 1 second 

void setup()
{
    Serial.begin(9600);
    delay(10);
    Serial.println();
    Serial.println("Starting...");

    setupLoadcell();
    if (serviceEnabled) {
        setupMQTT();
    }
}

// Publish weights to the respective topics
void publishWeightToMQTT(int index)
{
    std::string topic = "al/weight/"; // main topic
    std::string topicFull = topic + scaleIDs[index]; // add respective ID from scale
    String weightload = String(displayedWeight[index]);

    if (client.publish(topicFull.c_str(), weightload.c_str())) {
    Serial.print("Published '");
    Serial.print(weightload.c_str()); 
    Serial.print("' to topic: ");
    Serial.println(topicFull.c_str());
    } else {
    Serial.println("MQTT Publish failed.");
    }
}


// MQTT reconnect
void reconnect()
{
    while (!client.connected())
    {
        Serial.print("Attempting MQTT connection...");
        if (client.connect("ESP32Client"))
        {
            Serial.println("connected");
            client.subscribe("esp32/output");
        }
        else
        {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            delay(5000);
        }
    }
}

// Function to send new data via mqtt to cpe (only fires when new data is available)
void loop()
{
    if (serviceEnabled)
    {
        if (!client.connected())
        {
            reconnect();
        }
        client.loop();
    }

    // Update load cell weight (HX711 library function)
    for (int i = 0; i < numLoadCells; i++) {
        LoadCell[i].update();
    }

    // To not send every value on change, we just send it if we cross 70g threshold (weight of empty cup)
    const int threshold = 70; // Definition of threshold
    for (int i = 0; i < numLoadCells; i++) {
        int newWeight = static_cast<int>(LoadCell[i].getData()); // Load current data into variable newWeight

        // if weight crossed above the threshold and the last state was below treshold then we send data via mqtt
        if (newWeight >= threshold && lastState[i] == 0)
        {
            lastState[i] = 1; // Set state to "above threshold"
            displayedWeight[i] = newWeight; // Update weight
            publishWeightToMQTT(i); // Publish via MQTT

            // Console Log
            Serial.print("Load_cell ");
            Serial.print(i + 1);
            Serial.print(" crossed above ");
            Serial.print(threshold);
            Serial.print(": ");
            Serial.println(newWeight);
        } 
        // if weight crossed below the threshold and the last state was above threshold then we send data via mqtt
        else if (newWeight < threshold && lastState[i] == 1)
        {
            lastState[i] = 0; // Set state to "below threshold"
            displayedWeight[i] = newWeight; // Update weight
            publishWeightToMQTT(i); // Publish via MQTT

            // Console log
            Serial.print("Load_cell ");
            Serial.print(i + 1);
            Serial.print(" below ");
            Serial.print(threshold);
            Serial.print(": ");
            Serial.println(newWeight);
        }
    }


    // Tare on serial input
    if (Serial.available() > 0)
    {
        char inByte = Serial.read();
        if (inByte == 't')
        {
            for (int i = 0; i < numLoadCells; i++) {
                LoadCell[i].tareNoDelay();
            }
        }
    }

    // Print when taring complete
    for (int i = 0; i < numLoadCells; i++) {
        if (LoadCell[i].getTareStatus()) {
            Serial.print("LoadCell ");
            Serial.print(i + 1);
            Serial.println(" Tare complete");
        }
    }
}