/*

      Motor Driver Program with WiFi:
      Controls 3, 6, or 9 DM542T stepper motor drivers for the Neural Kinetic Sculpture
      Receives control data from the mobile app via WiFi
  
      Modified: March 12, 2025
      By: Letzy Mota and Karina Montero

      Modified: April 27, 2025
      By: Karina Montero
      
      Modified: April 28, 2025
      By: Melina Moore - Added WiFi functionality

*/ 

#include <Arduino.h>
#include <WiFi.h>         // WiFi library for ESP32
#include <WebSocketsClient.h> // WebSockets library
#include <ArduinoJson.h>   // For parsing JSON data

// Motor control pins
#define PulPin0 2  // pin 2 will control motor 1s pulse 
#define DirPin0 3  // pin 3 will control motor 1s direction

#define PulPin1 4  // pin 4 will control motor 2s pulse
#define DirPin1 5  // pin 5 will control motor 2s direction

#define PulPin2 6  // pin 6 will control motor 3s pulse
#define DirPin2 7  // pin 7 will control motor 3s direction

#define PulPin3 8  // pin 8 will control motor 4s pulse
#define DirPin3 9  // pin 9 will control motor 4s direction

#define PulPin4 10 // pin 10 will control motor 5s pulse
#define DirPin4 11 // pin 11 will control motor 5s direction

#define PulPin5 12 // pin 12 will control motor 6s pulse
#define DirPin5 13 // pin 13 will control motor 6s direction

#define PulPin6 22 // pin 22 will control motor 7s pulse
#define DirPin6 23 // pin 23 will control motor 7s direction

#define PulPin7 24 // pin 24 will control motor 8s pulse
#define DirPin7 25 // pin 25 will control motor 8s direction

#define PulPin8 26 // pin 26 will control motor 9s pulse
#define DirPin8 27 // pin 27 will control motor 9s direction 

// Limit switches
#define LimitPin0 28 // pin 28 will inform is panel 1 is at the top
#define LimitPin1 29 // pin 29 will inform is panel 2 is at the top
#define LimitPin2 30 // pin 30 will inform is panel 3 is at the top
#define LimitPin3 31 // pin 31 will inform is panel 4 is at the top
#define LimitPin4 32 // pin 32 will inform is panel 5 is at the top
#define LimitPin5 33 // pin 33 will inform is panel 6 is at the top
#define LimitPin6 34 // pin 34 will inform is panel 7 is at the top
#define LimitPin7 35 // pin 35 will inform is panel 8 is at the top
#define LimitPin8 36 // pin 36 will inform is panel 9 is at the top

// WiFi settings
const char* ssid = "YourWiFiSSID";     // Your WiFi network name
const char* password = "YourPassword";  // Your WiFi password

// WebSocket server settings (this should match your app's WebSocket server)
const char* wsHost = "signal-filter.onrender.com"; // Same server as used in the app
const int wsPort = 80;                             // Standard WebSocket port
const char* wsPath = "/";                          // WebSocket path

WebSocketsClient webSocket;

// Pin arrays
int PulPins[] = {PulPin0, PulPin1, PulPin2, PulPin3, PulPin4, PulPin5, PulPin6, PulPin7, PulPin8};
int DirPins[] = {DirPin0, DirPin1, DirPin2, DirPin3, DirPin4, DirPin5, DirPin6, DirPin7, DirPin8};
int LimitPins[] = {LimitPin0, LimitPin1, LimitPin2, LimitPin3, LimitPin4, LimitPin5, LimitPin6, LimitPin7, LimitPin8};

// Panel configuration structure
struct configuration {
  int PulPin;      // controls speed
  int DirPin;      // controls direction
  int LimitPin;    // to know switch has been hit yet
  bool Homed;      // to keep track of which panels are at the top
  bool isActive;   // to track which panels are currently being controlled
  int speed;       // motor speed (microseconds delay - lower is faster)
  int direction;   // motor direction
  int brightness;  // panel brightness (will be used for LED control)
};

// Function declarations
void motorControl(int rpos, int cpos, int Direction, int Delay);
void homePanels(void);
void webSocketEvent(WStype_t type, uint8_t * payload, size_t length);

// App input for all motors
int numRows = 3; 
int numColumns = 3;

// App input for individual motors
int rpos; 
int cpos;
int speed; 
int direction; 

configuration** MotorArr;

void setup() {
  // Initialize Serial for debugging
  Serial.begin(115200);
  Serial.println("Motor Controller with WiFi Starting...");

  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println();
  Serial.print("Connected to WiFi, IP address: ");
  Serial.println(WiFi.localIP());

  // Setup WebSocket client
  webSocket.begin(wsHost, wsPort, wsPath);
  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(5000);
  
  Serial.println("WebSocket client started");
  Serial.println("Waiting for configuration from app...");

  // Request configuration from the server
  webSocket.sendTXT("{\"request\":\"config\"}");

  // Wait for initial configuration (with timeout)
  unsigned long startTime = millis();
  bool configReceived = false;
  
  while (!configReceived && (millis() - startTime < 10000)) { // 10 second timeout
    webSocket.loop();
    delay(10);
    // This will be set to true in the webSocketEvent handler when config is received
    // For now, we'll just continue with default config
  }
  
  if (!configReceived) {
    Serial.println("No configuration received, using default 3x3");
  }

  // Initialize motor arrays based on numRows and numColumns
  // allocate memory for rows
  MotorArr = new configuration*[numRows];
  // allocate memory for each column in each row
  for(int i = 0; i < numRows; i++) {
    MotorArr[i] = new configuration[numColumns];
  }

  int idx = 0;
  for(int i = 0; i < numRows; i++) {
    for(int j = 0; j < numColumns; j++) {
      MotorArr[i][j].PulPin = PulPins[idx];       // assign pulse pin to matrix position
      MotorArr[i][j].DirPin = DirPins[idx];       // assign direction pin to matrix position
      MotorArr[i][j].LimitPin = LimitPins[idx];   // assign switch pin to matrix position
      MotorArr[i][j].Homed = false;               // this will be used to check if panels have been homed (false = not at the top yet)
      MotorArr[i][j].isActive = false;            // initially no panel is active
      MotorArr[i][j].speed = 1000;                // default speed
      MotorArr[i][j].direction = 0;               // default direction
      MotorArr[i][j].brightness = 50;             // default brightness

      pinMode(MotorArr[i][j].PulPin, OUTPUT);           // configure pulse pin to output
      pinMode(MotorArr[i][j].DirPin, OUTPUT);           // configure direction pin to output
      pinMode(MotorArr[i][j].LimitPin, INPUT_PULLUP);   // configure switch pin to input
      idx++;
    }
  }
  
  // Home panels at startup
  homePanels();
}

void loop() {
  // Handle WebSocket communication
  webSocket.loop();

  // Motor control loop
  for(int i = 0; i < numRows; i++) {
    for(int j = 0; j < numColumns; j++) {
      if (MotorArr[i][j].isActive) {
        // Check limit switch
        if (digitalRead(MotorArr[i][j].LimitPin) == HIGH) {
          MotorArr[i][j].direction = !MotorArr[i][j].direction;
        }
        
        // Control the motor for this cycle (5 pulses per active panel)
        for(int pulse = 0; pulse < 5; pulse++) {
          motorControl(i, j, MotorArr[i][j].direction, MotorArr[i][j].speed);
        }
      }
    }
  }
  
  // Small delay to prevent excessive CPU usage
  delay(5);
}

// WebSocket event handler
void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      Serial.println("WebSocket disconnected");
      break;
      
    case WStype_CONNECTED:
      Serial.println("WebSocket connected");
      break;
      
    case WStype_TEXT:
      Serial.printf("WebSocket received: %s\n", payload);
      
      // Parse the incoming JSON data
      DynamicJsonDocument doc(1024);
      DeserializationError error = deserializeJson(doc, payload);
      
      if (error) {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
        return;
      }

      // Check if this is a configuration message
      if (doc.containsKey("config")) {
        // Handle initial configuration
        int newRows = doc["panels_y"];
        int newColumns = doc["panels_x"];
        
        Serial.print("Received configuration: ");
        Serial.print(newRows);
        Serial.print("x");
        Serial.println(newColumns);
        
        // Only reallocate if the configuration has changed
        if (newRows != numRows || newColumns != numColumns) {
          Serial.println("Configuration changed, reallocating arrays");
          
          // First, free existing memory
          for(int i = 0; i < numRows; i++) {
            delete[] MotorArr[i];
          }
          delete[] MotorArr;
          
          // Update dimensions
          numRows = newRows;
          numColumns = newColumns;
          
          // Reallocate with new dimensions
          MotorArr = new configuration*[numRows];
          for(int i = 0; i < numRows; i++) {
            MotorArr[i] = new configuration[numColumns];
          }
          
          // Initialize new configuration
          int idx = 0;
          for(int i = 0; i < numRows; i++) {
            for(int j = 0; j < numColumns; j++) {
              if (idx < 9) { // Ensure we don't exceed available pins
                MotorArr[i][j].PulPin = PulPins[idx];
                MotorArr[i][j].DirPin = DirPins[idx];
                MotorArr[i][j].LimitPin = LimitPins[idx];
                MotorArr[i][j].Homed = false;
                MotorArr[i][j].isActive = false;
                MotorArr[i][j].speed = 1000;
                MotorArr[i][j].direction = 0;
                MotorArr[i][j].brightness = 50;
                
                // Ensure pins are properly configured
                pinMode(MotorArr[i][j].PulPin, OUTPUT);
                pinMode(MotorArr[i][j].DirPin, OUTPUT);
                pinMode(MotorArr[i][j].LimitPin, INPUT_PULLUP);
                idx++;
              }
            }
          }
          
          // Home the panels after reconfiguration
          homePanels();
        }
      }
      
      // Check if this is a panel control message
      else if (doc.containsKey("panel_control")) {
        JsonArray panels = doc["panel_control"].as<JsonArray>();
        
        // Reset all panels to inactive first
        for(int i = 0; i < numRows; i++) {
          for(int j = 0; j < numColumns; j++) {
            MotorArr[i][j].isActive = false;
          }
        }
        
        // Activate panels according to received data
        for (JsonObject panel : panels) {
          String panelId = panel["id"];
          int row = panelId.substring(0, 1).toInt();
          int col = panelId.substring(2).toInt();
          
          if (row >= 0 && row < numRows && col >= 0 && col < numColumns) {
            MotorArr[row][col].isActive = true;
            MotorArr[row][col].speed = map(panel["speed"].as<int>(), 0, 150, 10000, 500); // Convert cm/s to delay
            
            // Convert "up"/"down" string to binary direction
            if (panel["direction"] == "up") {
              MotorArr[row][col].direction = 1; // up
            } else {
              MotorArr[row][col].direction = 0; // down
            }
            
            MotorArr[row][col].brightness = panel["brightness"];
            
            Serial.print("Activated panel ");
            Serial.print(row);
            Serial.print("-");
            Serial.print(col);
            Serial.print(", Speed: ");
            Serial.print(MotorArr[row][col].speed);
            Serial.print(", Direction: ");
            Serial.print(MotorArr[row][col].direction);
            Serial.print(", Brightness: ");
            Serial.println(MotorArr[row][col].brightness);
          }
        }
      }
      
      break;
  }
}

void motorControl(int rpos, int cpos, int Direction, int Delay) {
  // For direction: 0 -> clockwise, 1-> counterclockwise 
  digitalWrite(MotorArr[rpos][cpos].DirPin, Direction);

  digitalWrite(MotorArr[rpos][cpos].PulPin, HIGH);
  delayMicroseconds(Delay);
  digitalWrite(MotorArr[rpos][cpos].PulPin, LOW);
  delayMicroseconds(Delay);
}

void homePanels(void) {
  // Homing
  bool allHomed = false;
  const int backOffSteps = 10;  // How many steps to back off
  const int backOffSpeed = 1500; // Delay for backing off (microseconds)
  
  Serial.println("Homing all panels...");

  while (!allHomed) {
    allHomed = true;

    for (int i = 0; i < numRows; i++) {
      for (int j = 0; j < numColumns; j++) {
        if (!MotorArr[i][j].Homed) { // Will continue if individual panel is not homed
      
          // Motor running
          digitalWrite(MotorArr[i][j].DirPin, HIGH); // Panel moving up
          digitalWrite(MotorArr[i][j].PulPin, HIGH);
          delayMicroseconds(10000);
          digitalWrite(MotorArr[i][j].PulPin, LOW);
          delayMicroseconds(10000);

          if (digitalRead(MotorArr[i][j].LimitPin) == HIGH) { // If switch is pressed
            MotorArr[i][j].Homed = true; // The panel is homed now
            Serial.print("Panel ");
            Serial.print(i);
            Serial.print("-");
            Serial.print(j);
            Serial.println(" homed successfully");

            // Now back off slightly - to get rid of pressure on limit switch
            digitalWrite(MotorArr[i][j].DirPin, LOW); // Move down
            for (int step = 0; step < backOffSteps; step++) {
              digitalWrite(MotorArr[i][j].PulPin, HIGH);
              delayMicroseconds(backOffSpeed);
              digitalWrite(MotorArr[i][j].PulPin, LOW);
              delayMicroseconds(backOffSpeed);
            }
          } else {
            allHomed = false; // Not all panels are homed yet 
          }
        }
      }
    }
  }
  
  Serial.println("All panels homed successfully!");
}
