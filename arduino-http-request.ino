
#include <SPI.h>
#include <Ethernet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

byte mac[] = { 0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02 };
byte ip[] = { 172, 16, 16, 66 };
byte dnss[] = { 8, 8, 8, 8 };
byte gateway[] = { 172, 16, 16, 1 };
byte subnet[] = { 255, 255, 254, 0 };
const char *server="192.168.20.56"; 
char buffer[80];
EthernetClient client;
String response = "";
boolean imprime = false;

int led = 13;

void setup() {
  Serial.begin(9600);
  Ethernet.begin(mac, ip, dnss, gateway, subnet);
  delay(1000);
  pinMode(led, OUTPUT);
}

void loop() {
  response = "";
  imprime = false;
  digitalWrite(led, HIGH);
  Serial.println("Send");
  send_request();
  Serial.println("Wait");
  wait_response();
  Serial.println("Read");
  read_response();
  Serial.println("End");
  end_request();
  Serial.println("Imprime");
  imprimir_cupom();
  digitalWrite(led,LOW);
  
  /*
  if(cmp == '1'){
    Serial.println("Acender");
    digitalWrite(led, HIGH);
    delay(500);
  }
  if(cmp == '0'){
    Serial.println("Apagar");
    digitalWrite(led,LOW);
    delay(500);
  }
  if(cmp == '2'){
    Serial.println("Send");
    send_request();
    Serial.println("Wait");
    wait_response();
    Serial.println("Read");
    read_response();
    Serial.println("End");
    end_request();

    imprimir_cupom();
  }
  if(cmp == '3'){
    imprimir_cupom();
  }
  */
}

void imprimir_cupom(){
  if(imprime){
    if(client.connect("172.16.16.61",9100)){
      client.println(response);
      sprintf(buffer, "%c%c", 27,119);
      client.println(buffer);
      response = "";
    }else{
      Serial.println("Falha ao conectar");  
    }
    end_request();
  }else{
    Serial.println("Sem informação para imprimir");
  }
}

void wait_response() {
  while (!client.available()) {
    if (!client.connected()) {
      return;
    }
  }
}

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
      //Serial.print(c);
    }else{
      if(count == 7){
        response += String(c);
      }
    }
  }

  if(response != "\nerror"){
    imprime = true;
  }else{
    imprime = false;
  }
  
  if(!client.available()){
    char c = client.read();
  }
    
}

void end_request() {
  Serial.println("begin stop");
  client.stop();
  Serial.println("end stop");
}

void send_request(){
  Serial.println("Conectando... ");
  if (client.connect(server, 80)) {
    Serial.println("sending ");
    
    // POST URI
    sprintf(buffer, "POST /RoteadorHTTP/load.php?device=%s HTTP/1.1", "arduino_1");
    client.println(buffer);
    
    // Host header
    sprintf(buffer, "Host: %s", server);
    client.println(buffer);
    
    // Application key
    //sprintf(buffer, "Authorization: %s", sas);
    //client.println(buffer);
    
    // content type
    client.println("Content-Type: text/html");
    
    //Expert
    //sprintf(buffer, "Expert: 100-continue");
    //client.println(buffer);
    
    // POST body
    sprintf(buffer, "{\"value\": %s}", "Hello World");
    
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

