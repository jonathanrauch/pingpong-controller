#include <WiFiClient.h>

// Web
const int httpPort = 3000;
const char *host = "10.0.0.176";

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

void makeRequest(WiFiClient *client, String url)
{
  Serial.print("Requesting URL: ");
  Serial.println(url);
  
  client->print(String("POST ") + url + " HTTP/1.1\r\n" +
               "Content-Type: application/json\r\n" +
               "Host: " + host + "\r\n\r\n" +
               "{\"rfid\": \"i3475t324\"}\r\n");

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

void sendRfid(char rfid[]) {
  WiFiClient client;

  if (!connect_to_site(&client)) {
    return;
  }

  makeRequest(&client, "/rfid");

  client.stop();
}

