
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include "homepage.h"
#include <Keypad.h>
#include <HCSR04.h>
#include <ESP32Servo.h>
#include <Wire.h>
#include "rgb_lcd.h"

const char* ssid = "eoin";
const char* password = "password1";

// LCD
rgb_lcd lcd;

// Servo
const int servoPin = 12;
Servo servo1;

// Ultrasonic sensor
const int trigPin = 33;  
const int echoPin = 35; 
float duration, cm;  

// Keypad configuration
const byte ROWS = 4; // four rows
const byte COLS = 3; // three columns
char hexaKeys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
byte rowPins[ROWS] = {15, 2, 4, 16}; // Row pins (GPIOs)
byte colPins[COLS] = {17, 5, 18}; // Column pins (GPIOs)
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

int pin[] = {1, 2, 3}; // Example PIN
int pinEntered[3]; // Store entered PIN
int passwordCount = 0; // Count of correct PIN entries
char customKey;

WebServer server(80);

void keypad();
void servo();
void lcd_display();
void keypadPassword();

String ultrasonic(){
  digitalWrite(trigPin, LOW);  
  delayMicroseconds(5);  
  digitalWrite(trigPin, HIGH);  
  delayMicroseconds(10);  
  digitalWrite(trigPin, LOW);  

  duration = pulseIn(echoPin, HIGH, 30000);  
  cm = (duration * 0.0343) / 2;  

  Serial.print("Distance: ");  
  Serial.print(cm);  
  Serial.println("cm");  

  return String(cm,2);
}

void doorstatus(){
   String distance = ultrasonic(); 
    if (cm < 10) {
    lcd.clear();
    lcd.print("door open");
  }
  else{
    lcd.clear();
    lcd.print("door closed");
  }
  delay(250);
}

void keypad() {
    customKey = customKeypad.getKey(); // Get the key pressed from the keypad
    
    if (customKey) {
        Serial.println(customKey); // Print key for debugging
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Key pressed: ");
        lcd.print(customKey);
        
        // Handle keypress
        if (customKey == '1') {
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("You have selected ");
            lcd.print(customKey);
            keypadPassword();// Call function for password entry
        }
        else if (customKey == '2') {
             lcd.clear();
             lcd.setCursor(0,0);
             lcd.print("The door is ");
             lcd.print(cm);
             lcd.print("cm away");
        }
    }
}

void servo() {
    // Move servo motor from 0 to 180 degrees
    for (int posDegrees = 0; posDegrees <= 180; posDegrees++) {
        servo1.write(posDegrees);
        delay(15); // Give servo time to reach position
    }
delay(3000);
    // Move servo back from 180 to 0 degrees
    for (int posDegrees = 180; posDegrees >= 0; posDegrees--) {
        servo1.write(posDegrees);
        delay(15);
    }
    lcd.clear();
    lcd.print("door closed");
  delay(1000);
}

void lcd_display(){
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Door is locked");
    delay(3000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Pick option:");
    lcd.setCursor(0, 1);
    lcd.print("1 for keypad");
    delay(3000);
   
}

void keypadPassword(){
    static int pinIndex = 0;  // To track index of entered PIN
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Enter PIN: ");
    
    while(pinIndex < 3) {
        customKey = customKeypad.getKey(); // Check if a key is pressed
        
        if (customKey) {
            pinEntered[pinIndex] = customKey - '0'; // Convert char to integer
            lcd.setCursor(pinIndex, 1);
            lcd.print('*'); // Display an asterisk for each entered digit
            
            pinIndex++;  // Move to the next digit
            delay(500); // Debounce delay
        }
    }

    // Check if entered PIN matches the stored PIN
    if (pinEntered[0] == pin[0] && pinEntered[1] == pin[1] && pinEntered[2] == pin[2]) {
        lcd.clear();
        lcd.print("Door opened");
        servo();  // Unlock the door using the servo motor
    } else {
        lcd.clear();
        lcd.print("Incorrect PIN");
        delay(2000);  // Wait before resetting
        lcd_display(); // Show the pick method menu again
    }
    
    pinIndex = 0; // Reset pin entry index for the next attempt
}


void handledistance() {
  server.send(200, "text/plain", ultrasonic());
}

void handleRoot() {
  server.send(200, "text/html", homePagePart1); // Send the HTML page
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void setup(void) {

  Serial.begin(115200);

  lcd.begin(16,2);

  pinMode(trigPin, OUTPUT);  
	pinMode(echoPin, INPUT);  

  servo1.attach(servoPin);  

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp32")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);
  server.on("/Ultrasonic", handledistance);
  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
  keypad();
  doorstatus();
}
