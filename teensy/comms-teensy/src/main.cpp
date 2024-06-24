#include <Arduino.h>
#include <NativeEthernet.h>

byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
IPAddress ip(192, 168, 1, 177);
EthernetServer server(80);

void setup() {
  Serial.begin(9600);
  // Wait for serial port to connect. Needed for native USB port only.
  while (!Serial) {;}
  Serial.println("Starting comms-teensy...");

  Ethernet.begin(mac, ip);

  // Check for ethernet hardware.
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("ERROR: Ethernet hardware not found.");
    while (true) {
      delay(1);
    }
  }
  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("WARNING: Ethernet cable is not connected.");
  }

  server.begin();
  Serial.print("MACH: Server started at: ");
  Serial.print(Ethernet.localIP());
  Serial.println(".");
}

void loop() {
  EthernetClient client = server.available();
  if (!client) {
    return;
  }

  Serial.println("comms-teensy: New client conected.");
  // an http request ends with a blank line
  boolean currentLineIsBlank = true;
  while (client.connected()) {
    if (client.available()) {
      char c = client.read();
      Serial.write(c);
      // if you've gotten to the end of the line (received a newline
      // character) and the line is blank, the http request has ended,
      // so you can send a reply
      if (c == '\n' && currentLineIsBlank) {
        // send a standard http response header
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: text/html");
        client.println("Connection: close");  // the connection will be closed after completion of the response
        client.println("Refresh: 5");  // refresh the page automatically every 5 sec
        client.println();
        client.println("<!DOCTYPE HTML>");
        client.println("<html>");
        // output the value of each analog input pin
        for (int analogChannel = 0; analogChannel < 6; analogChannel++) {
          int sensorReading = analogRead(analogChannel);
          client.print("analog input ");
          client.print(analogChannel);
          client.print(" is ");
          client.print(sensorReading);
          client.println("<br />");
        }
        client.println("</html>");
        break;
      }
      if (c == '\n') {
        // you're starting a new line
        currentLineIsBlank = true;
      } else if (c != '\r') {
        // you've gotten a character on the current line
        currentLineIsBlank = false;
      }
    }
  }
  // Give the web browser time to receive the data.
  delay(1);
  // close the connection:
  client.stop();
  Serial.println("comms-teensy: Client disconnected.");
}