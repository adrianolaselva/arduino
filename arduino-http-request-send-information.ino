
#include <SPI.h>
#include <Ethernet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

const char *server_name="webhook.site";

const char *endpoint_name="/5e7ad420-67ea-4294-a4e0-6c21fd387c9b";
const char *device_name="arduino_1";

byte mac[] = { 0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02 };

/**
 * Configurações da rede local para o arduino acessar a rede
 */
byte ip[] = { 172, 16, 16, 66 };
byte dnss[] = { 8, 8, 8, 8 };
byte gateway[] = { 172, 16, 16, 1 };
byte subnet[] = { 255, 255, 254, 0 };

char buffer[80];
EthernetClient client;
String response = "";

/**
 * Inicializa configurações
 */
void setup() {
  Serial.begin(9600);
  /**
   * Configurações de rede do arduino, podemos deixar em arquivo texto
   */
  // Ethernet.begin(mac, ip, dnss, gateway, subnet);
  
  Ethernet.begin(mac, ip);
  delay(1000);
}

/**
 * Interação
 */
void loop() {
  response = "";
  char lat = "38.8951"
  char lng = "-77.0364";
  Serial.println("Inicia envio de lengitude e latitude");
  send_request_with_coordinates(lat, lng);
  Serial.println("Aguarda resposta da requisição");
  wait_response();
  Serial.println("Efetua a leitura de resposta");
  read_response();
  Serial.println("Leitura da resposta concluída");
  end_request();
}

/**
 * Aguarda resposta da requisição HTTP
 */
void wait_response() {
  while (!client.available()) {
    if (!client.connected()) {
      return;
    }
  }
}

/**
 * Efetua a leitura de resposta da requisição HTTP
 */
void read_response() {
  bool print = true;
  int count = 0;
  
  while (client.available()) {
    char c = client.read();
    if (c == '\n'){
      print = false;
      count++;
    }
    
    if (print){
      Serial.println(c);
    }else{
      if(count == 7){
        response += String(c);
      }
    }
  }

  if(!client.available()){
    char c = client.read();
  }
    
}

/**
 * Finaliza HTTP client
 */
void end_request() {
  Serial.println("Finaliza HTTP client");
  client.stop();
  Serial.println("HTTP client finalizado");
}

/**
 * Efetua requisição http para a API que recebe as informações de longitude e latitude
 */
void send_request_with_coordinates(char lat, char lng){
  Serial.println("Conectando... ");
  if (client.connect(server_name, 80)) {
    Serial.println("Enviando requisição HTTP");
    
    sprintf(buffer, "GET %s?device=%s&lat=%s&lng=%s HTTP/1.1", endpoint_name, device_name, lat, lng);
    client.println(buffer);
    
    // Host header
    sprintf(buffer, "Host: %s", server_name);
    client.println(buffer);
    
    // content type
    client.println("Content-Type: application/json");
    
    // Content length
    client.print("Content-Length: ");
    client.println(strlen(buffer));
    
    // End of headers
    client.println();
    
    // Request body
    client.println(buffer);
  
  } else {
    Serial.println("connection failed");
  }  
}

