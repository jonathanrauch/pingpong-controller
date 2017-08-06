#include <WiFiClient.h>

// Web
const int httpPort = 3000;
const char *host = "10.0.0.159";

boolean connect_to_site(WiFiClient *client)
{    
  Serial.println("Connecting");
  if (!client->connect(host, httpPort)) {
    Serial.println("connection failed");
    return false;
  }

  Serial.println("Connected");
  return true;
}

void makeRequest(WiFiClient *client, String url, String body)
{
  int contentLen = body.length() + 2;
  String request = String("POST ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + ":3000\r\n" +
               "Content-Length: " + String(contentLen) + "\r\n" +
               "Content-Type: application/json\r\n\r\n" +
               "{" + body + "}\r\n";
               
  Serial.println("Request:");
  Serial.println(request);
  
  client->print(request);

  // Just making sure there is a response. we don't have to do it for now since we don't use it
  unsigned long timeout = millis();
  while (client->available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !");
      client->stop();
      return;
    }
  }
}

void genericCall(String url, String body) {
  WiFiClient client;

  if (!connect_to_site(&client)) {
    return;
  }

  makeRequest(&client, url, body);

  client.stop();
}

void sendRfid(char rfid[], int playerPos) {
  genericCall("/rfid", "\"rfid\": \"" + String(rfid) + "\", \"position\": " + String(playerPos));
}

void sendScore(int playerPos) {
  genericCall("/score", "\"position\": " + String(playerPos));
}
