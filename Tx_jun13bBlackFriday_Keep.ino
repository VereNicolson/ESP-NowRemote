/*Checks
@13'6'25; Verifies as unmodified off Chat GPT(backup in G drive)
Rx MAC is 94:a9:99:69:a8 = 0x94,0xa9,0x90,0x99,0x69,0xa8 
C3 Supermini quirks; in tools>board chose “ESP32C3 Dev Module”
and make sure that “USB CDC On Boot” is enabled.
@13/6/25 uploads and prints expected debugs on Supermini #1
serialPrint debugs show setup OK, button read and loop working
*/

#include <esp_now.h>                           // Include ESP-NOW library
#include <WiFi.h>                              // Include WiFi library for ESP-NOW initialization

uint8_t broadcastAddress[] = {0x94,0xa9,0x90,0x99,0x69,0xa8};  // Broadcast address for ESP-NOW
bool buttonPressed = false;                   // Variable to track button state
const int buttonPin = 4;                      // GPIO pin where the button is connected

// Structure to send data
typedef struct struct_message {
  bool bState;                                // Boolean to represent button state
} struct_message;

struct_message myData;                        // Instantiate the message struct

void setup() {
  Serial.begin(115200);                       // Start the serial monitor
  pinMode(buttonPin, INPUT_PULLUP);           // Set the button pin as input with internal pull-up resistor

  WiFi.mode(WIFI_STA);                        // Set Wi-Fi to station mode
  Serial.println("WiFi set to Station mode");

  if (esp_now_init() != ESP_OK) {             // Initialize ESP-NOW
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  Serial.println("ESP-NOW initialized");

  esp_now_peer_info_t peerInfo = {};          // Structure to hold peer information
  memcpy(peerInfo.peer_addr, broadcastAddress, 6); // Copy the broadcast address *****may need to change from broadcast
  peerInfo.channel = 0;                       // Set WiFi channel
  peerInfo.encrypt = false;                   // Disable encryption

  if (esp_now_add_peer(&peerInfo) != ESP_OK) { // Add the peer
    Serial.println("Failed to add peer");
    return;
  }
  Serial.println("Peer added");
}

void loop() {
  bool currentState = digitalRead(buttonPin) == LOW;  // Read button state, LOW means pressed

  if (currentState != buttonPressed) {       // Check if button state has changed
    buttonPressed = currentState;            // Update button state
    myData.bState = buttonPressed;           // Set struct boolean to current button state

    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&myData, sizeof(myData)); // Send the struct via ESP-NOW

    if (result == ESP_OK) {
      Serial.print("Sent message: ");
      Serial.println(buttonPressed ? "TRUE" : "FALSE"); // Print sent value
    } else {
      Serial.println("Error sending the message");
    }

    delay(100);  // Small delay to avoid bouncing
  }

  delay(50); // Short delay to limit loop frequency
}
