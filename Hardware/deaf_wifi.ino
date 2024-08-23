#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* ssid = "superman"; 
const char* password = "321@mansuper"; 

ESP8266WebServer server(80);

// Pin definitions
const int motorPins[6] = {D1, D2, D3, D4, D5, D6}; 

const int letterDuration = 300;  
const int letterPause = 300;   
const int wordPause = 400;      

int patterns[28][6] = {
  {1, 0, 0, 0, 0, 0},  // Pattern for 'A'
  {0, 1, 0, 0, 0, 0},  // Pattern for 'B'
  {0, 0, 1, 0, 0, 0},  // Pattern for 'C'
  {0, 0, 0, 1, 0, 0},  // Pattern for 'D'
  {0, 0, 0, 0, 1, 0},  // Pattern for 'E'
  {0, 0, 0, 0, 0, 1},  // Pattern for 'F'
  {1, 1, 0, 0, 0, 0},  // Pattern for 'G'
  {0, 1, 1, 0, 0, 0},  // Pattern for 'H'
  {0, 0, 1, 1, 0, 0},  // Pattern for 'I'
  {0, 0, 0, 1, 1, 0},  // Pattern for 'J'
  {0, 0, 0, 0, 1, 1},  // Pattern for 'K'
  {1, 0, 0, 0, 1, 0},  // Pattern for 'L'
  {0, 1, 0, 0, 1, 0},  // Pattern for 'M'
  {0, 0, 1, 0, 1, 0},  // Pattern for 'N'
  {0, 0, 0, 1, 0, 1},  // Pattern for 'O'
  {1, 0, 0, 1, 0, 0},  // Pattern for 'P'
  {0, 1, 0, 1, 0, 0},  // Pattern for 'Q'
  {0, 0, 1, 1, 0, 0},  // Pattern for 'R'
  {0, 1, 0, 0, 0, 1},  // Pattern for 'S'
  {1, 0, 1, 0, 0, 0},  // Pattern for 'T'
  {1, 1, 1, 0, 0, 0},  // Pattern for 'U'
  {0, 1, 1, 1, 0, 0},  // Pattern for 'V'
  {0, 0, 1, 1, 1, 0},  // Pattern for 'W'
  {0, 0, 0, 1, 1, 1},  // Pattern for 'X'
  {1, 0, 0, 0, 1, 1},  // Pattern for 'Y'
  {0, 1, 0, 1, 0, 1}   // Pattern for 'Z'
  {0, 1, 1, 1, 0, 1}   // Pattern for '?'
  {1, 1, 1, 1, 1, 1}   // Pattern for 'End of Words'
};

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  for (int i = 0; i < 6; i++) {
    pinMode(motorPins[i], OUTPUT);
    digitalWrite(motorPins[i], LOW);
  }

  server.on("/", HTTP_GET, []() {
    server.send(200, "text/html",
      "<!DOCTYPE html>"
      "<html lang=\"en\">"
      "<head>"
      "<meta charset=\"UTF-8\">"
      "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
      "<title>Motor Control Interface</title>"
      "<style>"
      "body {"
      "    font-family: Arial, sans-serif;"
      "    display: flex;"
      "    flex-direction: column;"
      "    align-items: center;"
      "    justify-content: center;"
      "    height: 100vh;"
      "    margin: 0;"
      "}"
      "h1 {"
      "    color: #333;"
      "}"
      "input[type=\"text\"] {"
      "    padding: 10px;"
      "    font-size: 16px;"
      "    width: 300px;"
      "    margin: 10px 0;"
      "}"
      "button {"
      "    padding: 10px 20px;"
      "    font-size: 16px;"
      "    cursor: pointer;"
      "    background-color: #4CAF50;"
      "    color: white;"
      "    border: none;"
      "    border-radius: 5px;"
      "}"
      "button:hover {"
      "    background-color: #45a049;"
      "}"
      "</style>"
      "</head>"
      "<body>"
      "<h1>Motor Control Interface</h1>"
      "<input type=\"text\" id=\"message\" placeholder=\"Enter message (A-Z only)\" />"
      "<button onclick=\"sendMessage()\">Send Message</button>"
      "<script>"
      "function sendMessage() {"
      "    const message = document.getElementById('message').value.toUpperCase();"
      "    if (/^[A-Z\\s]+$/.test(message)) {"
      "        fetch(`/sendMessage?msg=${message}`)"
      "            .then(response => response.text())"
      "            .then(data => {"
      "                alert(data);"
      "            })"
      "            .catch(error => {"
      "                console.error('Error:', error);"
      "                alert('Failed to send the message');"
      "            });"
      "    } else {"
      "        alert('Please enter only letters A-Z');"
      "    }"
      "}"
      "</script>"
      "</body>"
      "</html>"
    );
  });

  server.on("/sendMessage", HTTP_GET, []() {
    String message = server.arg("msg");
    Serial.print("Received message: ");
    Serial.println(message);

    playMessage(message.c_str());

    server.send(200, "text/plain", "Message received: " + message);
  });

  server.begin();
}

void loop() {
  server.handleClient();
}

void playMessage(const char* message) {
  for (int i = 0; message[i] != '\0'; i++) {
    if (message[i] == ' ') {
      Serial.print("Pausing for space between words: ");
      Serial.println(wordPause);
      delay(wordPause);  
    } else {
      // Get the index of the letter in the patterns array
      int letterIndex = message[i] - 'A';

      // Play the corresponding pattern
      if (letterIndex >= 0 && letterIndex < 28) {
        Serial.print("Playing pattern for letter: ");
        Serial.println(message[i]);
        playPattern(patterns[letterIndex], letterDuration);
      } else {
        Serial.print("Invalid character: ");
        Serial.println(message[i]);
      }

      Serial.print("Pausing between letters: ");
      Serial.println(letterPause);
      delay(letterPause);
    }
  }
}

void playPattern(int pattern[], int duration) {
  Serial.println("Playing pattern");

  for (int i = 0; i < 6; i++) {
    if (pattern[i] == 1) {
      Serial.print("Motor ");
      Serial.print(i);
      Serial.print(" ON (Duration: ");
      Serial.print(duration);
      Serial.println(" ms)");
      digitalWrite(motorPins[i], HIGH); 
    } else {
      Serial.print("Motor ");
      Serial.print(i);
      Serial.print(" OFF (Duration: ");
      Serial.print(duration);
      Serial.println(" ms)");
      digitalWrite(motorPins[i], LOW);  
    }
  }
  
  delay(duration);
  
  for (int i = 0; i < 6; i++) {
    digitalWrite(motorPins[i], LOW);
  }
  
  Serial.println("Pattern complete");
}
