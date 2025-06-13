/*Checks
@13'6'25 Verifies as coppied from Chat GPT
C3 Supermini quirks; in tools>board chose “ESP32C3 Dev Module”
and make sure that “USB CDC On Boot” is enabled.
@13/6/25 uploads and prints expected debugs
responds to Tx with appropriate debug messages, 
Couldn't get switch case to blink LED for 2 sec so just used a delay
*/

#include <esp_now.h>                          // Include ESP-NOW library
#include <WiFi.h>                             // Include WiFi library
const int ledPin = 2;         // Define the GPIO pin connected to the LED

bool receivedValue = false;                   // Variable to store last received button state
bool lightFunctionCalled = false;             // Becomes true when recieved value is true, 
                                              //stays true till Case 2 of switch

//$$$$$$$$$$$$$$$Debugging variables
int loopDebug ;                                         //a contrivance to show loop is working but only write 5 times, not fill the whole page
int inByte = 0;                                         //the incoming keyboard bytes for keyboard debugging
bool deBugSensors = false;                              // true when deBugByte = 'a'
bool deBugReset = false;                              // true when deBugByte = 'b'
unsigned long previousTime = 0;         // Stores the time when the LED was turned on
unsigned long newTime = 0;        //Time updates from millis() every loop
int state = 0;                       // Tracks the current state in the switch-case


// Structure to receive data
typedef struct struct_message {
  bool bState;                                // Boolean to represent button state
} struct_message;

struct_message incomingData;                  // Instantiate the received data struct

// Callback function executed when data is received
void onDataReceived(const esp_now_recv_info_t* info, const uint8_t *data, int len) {
  memcpy(&incomingData, data, sizeof(incomingData));  // Copy the incoming data into our struct
  Serial.print("Data received: ");
  Serial.println(incomingData.bState ? "TRUE" : "FALSE");  // Print the received value

  bool prevReceivedValue = receivedValue;     // Store previous state
  receivedValue = incomingData.bState;        // Update with new state

  if (receivedValue /*&& !lightFunctionCalled*/) { // If state just turned true and wasn't triggered before
    lightFunctionCalled = true;               // Set flag to indicate light function is triggered
    Serial.println("lightFunctionCalled set to TRUE");
  }

  if (!receivedValue /*&& prevReceivedValue*/) {  // Reset light function when false is received
    //lightFunctionCalled = false;
    //Serial.println("lightFunctionCalled reset to FALSE");
  }
}

void setup() {
  Serial.begin(115200);                       // Start the serial monitor

  WiFi.mode(WIFI_STA);                        // Set Wi-Fi to station mode
  Serial.println("WiFi set to Station mode");

  if (esp_now_init() != ESP_OK) {             // Initialize ESP-NOW
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  Serial.println("ESP-NOW initialized");

  esp_now_register_recv_cb(onDataReceived);   // Register the receive callback
  Serial.println("Receive callback registered");
   pinMode(ledPin, OUTPUT);             // Configure the LED pin as output
  digitalWrite(ledPin, HIGH);           // Test the LED during startup
  delay(400);                           //blink the LED for 400ms
  digitalWrite(ledPin, LOW);            //Turn back off again
  Serial.println("End of Setup");
}


//Switch case for LED and bools. *****Get this working one day
/*void handleReceivedValue() {
  switch (state) {
     case 0:   // Case 0: monitor bool
     if (lightFunctionCalled) {                  // If the handleReceivedValue has been true
     
        previousTime = newTime;                 //reset the previous time only in this case
        state = 1;  
    }
    break;
    case 1:   // Case 1: Turn on LED and start timing
        digitalWrite(ledPin, HIGH);      // Turn on the LED
      if (newTime - previousTime > 20000); { // Check if 2000 ms have passed
            state = 2;                        // Move to the next state
     }
      break;

    case 2:   // Case 2: Turn off LED and reset the bool receivedValue
      //digitalWrite(ledPin, LOW);         // Turn off the LED
      lightFunctionCalled = false;                   // Reset the LOCAL boolean
      Serial.println("lightFunctionCalled reset to FALSE");
      state = 0;                         // Reset state for next cycle
      break;
  }
}  */

void handleReceivedValue() {
  if(lightFunctionCalled) { 
digitalWrite(ledPin, HIGH);      // Turn on the LED
delay (2000);
digitalWrite(ledPin, LOW);      // Turn off the LED
lightFunctionCalled = false;
  }
}

void loop() {
  handleReceivedValue();  // You can optionally act on `lightFunctionCalled` here
  newTime = millis();
  delay(100); // Short delay to keep loop active
}

