#include <math.h>

#include <SPI.h>
#include <WiFi.h>

// LEDs
#define greenLedPin 2
#define redLedPin 3
#define blueLedPin 7

// BUZZER
#define buzzPin 5

// INFRARED
#define trigPin 9
#define echoPin 8

// HUMIDITY SENSOR
#define humAPin 2 // to motor 10
#define humBPin 3 // to motor 11

// TEMP SENSOR
#define tempPin 0

// BRIGHT SENSOR
#define brightPin 1

// MOTORS
#define motorAPin 10
#define motorBPin 11

// BUTTON
#define button 4


char ssid[] = "fablab_guest";
char pass[] = "veronafablab";
String yourdata = "invio=true";;

int i = -1;
int status = WL_IDLE_STATUS;
WiFiClient client;

int a;
float temperature;
float resistance;

int B=3975; // B value of the thermistor

float Rsensor; //Resistance of sensor in K

void setup() {

  Serial.begin (9600);
  pinMode(greenLedPin, OUTPUT);
  pinMode(redLedPin, OUTPUT);
  pinMode(blueLedPin, OUTPUT);
  pinMode(buzzPin, OUTPUT);
  pinMode(motorAPin, OUTPUT);
  pinMode(motorBPin, OUTPUT);
  pinMode(button, INPUT);

  digitalWrite(greenLedPin, HIGH);
  digitalWrite(redLedPin, HIGH);
  digitalWrite(blueLedPin, HIGH);
  digitalWrite(buzzPin, HIGH);

  delay(1000);
  digitalWrite(greenLedPin, LOW);
  digitalWrite(redLedPin, LOW);
  digitalWrite(blueLedPin, LOW);
  digitalWrite(buzzPin, LOW);  

/*
  digitalWrite(motorAPin, HIGH);
  digitalWrite(motorBPin, HIGH);
  delay(1000);
  digitalWrite(motorAPin, LOW);
  digitalWrite(motorBPin, LOW);
  */

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  Serial.print("Connecting to ... ");
  Serial.println(ssid);
  // Connect to WPA/WPA2 network. Change this line if using open or WEP network:    
  status = WiFi.begin(ssid, pass);
  // wait 10 seconds for connection:
  delay(10000);
  Serial.println(status);
  Serial.println("Connected to wifi");

}


void loop() {
  

  digitalWrite(greenLedPin, HIGH);


  

  yourdata = "invio=true";

  // LETTURA LIVELLO ACQUA
  long duration, distance;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration/2) / 29.1;
Serial.print("Distance");
Serial.println(distance);

  if (distance > 30) {
    sendWaterAlarm();
  } else if (distance == 0) {
    sendNoSensorAlarm();
  } 

  // LETTURA TEMPERATURA
  a = analogRead(tempPin);
  resistance=(float)(1023-a)*10000/a;
  temperature=1/(log(resistance/10000)/B+1/298.15)-273.15;
Serial.print("Temperature");
Serial.println(temperature);
  char output[5];
  snprintf(output, 5,"%f",temperature);
  printf("%s",output);
  yourdata = yourdata + "&temperature=" + output;

  // LETTURA LUMINOSITA
  int sensorValue = analogRead(brightPin);
  Rsensor=(float)(1023-sensorValue)*10/sensorValue;
Serial.print("Brightness");
Serial.println(Rsensor);
  snprintf(output, 5,"%f",Rsensor);
  printf("%s",output);
  yourdata = yourdata + "&brightness=" + output;


  // LETTURA UMIDITA
  int humValueA = analogRead(humAPin);
  int humValueB = analogRead(humBPin);
  Serial.print("HumA:");
  Serial.println(humValueA);
  Serial.print("HumB:");
  Serial.println(humValueB);
  yourdata = yourdata + "&humidity=" + humValueA;

  //int humValue = analogRead(humBPin);
  
  if (humValueA > 500) { // Change 40 to the humidity value that you want
    Serial.println("Start motor A");
    digitalWrite(blueLedPin, HIGH);
    digitalWrite(motorAPin, HIGH);
    delay(10000);
    digitalWrite(blueLedPin, LOW);
    digitalWrite(motorAPin, LOW);
    Serial.println("Stop motor A");
  } else {
     for (int i = 0; i < 5; i++) {
      digitalWrite(greenLedPin, HIGH);
      delay(250);
      digitalWrite(greenLedPin, LOW);
      delay(250);
    }
  }

  
 if (humValueB < 500) { // Change 40 to the humidity value that you want
    Serial.println("Start motor B");
    digitalWrite(blueLedPin, HIGH);
    digitalWrite(motorBPin, HIGH);
    delay(10000);
    digitalWrite(blueLedPin, LOW);
    digitalWrite(motorBPin, LOW);
    Serial.println("Stop motor B");
  } else {
     for (int i = 0; i < 5; i++) {
      digitalWrite(greenLedPin, HIGH);
      delay(250);
      digitalWrite(greenLedPin, LOW);
      delay(250);
    }
  }

  Serial.println(yourdata);

  sendData(yourdata);

  digitalWrite(greenLedPin, HIGH);

  if (digitalRead(button) == HIGH) {
    Serial.println("Start motor A button");
    digitalWrite(blueLedPin, HIGH);
    digitalWrite(motorAPin, HIGH);
    delay(10000);
    digitalWrite(blueLedPin, LOW);
    digitalWrite(motorAPin, LOW);
    Serial.println("Stop motor A button");
  }

  delay(5000);

  digitalWrite(redLedPin, LOW);
  digitalWrite(buzzPin, LOW);

  


}

void sendNoSensorAlarm() {
  digitalWrite(redLedPin, HIGH);
  digitalWrite(buzzPin, HIGH);
}

void sendWaterAlarm() {
  digitalWrite(redLedPin, HIGH);
  for (int i = 0; i < 10; i++) {
    digitalWrite(buzzPin, HIGH);
    delay(100);
    digitalWrite(buzzPin, LOW);
  }
}

void sendData(String data) {
  Serial.println("sending data...");
  if (client.connect("104.40.139.35",80)) {
      client.println("POST /bioedison/ajax.php HTTP/1.1"); 
      client.println("Host: 104.40.139.35");
      client.println("Content-Type: application/x-www-form-urlencoded"); 
      client.print("Content-Length: "); 
      client.println(data.length()); 
      client.println(); 
      client.print(data); 
  }
  if (client.connected()) { 
    client.stop();
  }
}


