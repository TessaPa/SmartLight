
#include "index.h"
#include "conditions.h"


#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>
#include <WiFiClient.h>

WebServer server(80);

#include "FirebaseESP32.h"
FirebaseData firebasedata;

//Untrasonic sensor configuration
const int trigPin = 5;
const int echoPin = 18;

//define sound speed in cm/uS
#define SOUND_SPEED 0.034
#define CM_TO_INCH 0.393701

long duration;
float distance;
#define LED 2
float flag=0;

double get_distance(){
  
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  

  // Calculate the distance
  distance = duration * SOUND_SPEED/2;
  // Prints the distance in the Serial Monitor
  //Serial.print("Ciao");
  Serial.print("Distance (cm): ");
  Serial.println(distance);
  Serial.print("\n");
  return distance;
}



//Timestamp configuration
int timezone=2;
int dst=0;

String get_timestamp(){
  String timestamp="";
  time_t now= time(nullptr);
  timestamp=String(ctime(&now));
  timestamp.remove(24);

  if (timestamp.endsWith("1970")){
    return"";
  }
  else{
    return timestamp;
  }
}


int get_visibility(){
  int visibility = 0;

  if (Firebase.getInt(firebasedata, "visibility")) {

    if (firebasedata.dataType() == "int") {
      visibility = firebasedata.intData();
      return visibility;
    }

  } else {
    //Failed, then print out the error detail
    Serial.println(firebasedata.errorReason());
  }






//Threshold for visibility
const int VISIBILITY_THRESHOLD=5000;

//Threshold for time
const int HOUR_THRESHOLD=5;

//Threshold for distance
const float DISTANCE_THRESHOLD=20;


//Webserver handle
void handle_index(){
  Serial.println("GET/");
  server.send(200, "text/html", index_html_page);
}

int gest_figure_light=0;
void handle_conditions(){
  Serial.println("GET/conditions");
  server.send(200, "text/html", conditions_html_page(String timestamp, String visibility, String distance,flag));
}

void handle_not_found(){
  server.send(404, "text/plain","404:Page not found!");
}



void setup() {
  
  //Serial port setup
  Serial.begin(9600);


 //Wi-Fi connection setup
  WiFi.mode(WIFI_STA);
  WiFi.begin("Telecom-66766493", "cWSBQXDRzi92WlpXeZ2D6Nqy");
  Serial.print("connecting to "+String("Telecom-66766493"));
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected to: ");  Serial.println("Telecom-66766493");
  Serial.print("IP address: ");     Serial.println(WiFi.localIP());

   Serial.println("");

  //Server setup
  server.on("index.html", handle_index);
  server.on("conditions.html", handle_conditions);
  server.onNotFound(handle_not_found);

  server.begin();
  Serial.print("HTTP server started at "); Serial.println(WiFi.localIP());
  
  
  
  //Firebase initialization
  Firebase.begin("https://provaled-5b6de-default-rtdb.firebaseio.com", "PFsA2WiSPzwKdBMRkRrwjwQMcNrgE75H31pXu4Or");
   Serial.println("");


  //Configuring timestamp function 
  configTime(timezone * 3600, dst * 0, "pool.ntp.org", "time.nist.gov");
  Serial.println("\nWaiting for time");
  while (!time(nullptr)) {
    Serial.print(".");
    delay(500);
  }
  Serial.print("Timestamp: "); Serial.println(get_timestamp());



  //
  pinMode(LED,OUTPUT);
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input

  pinMode(LED,OUTPUT);
  Serial.begin(9600); // Starts the serial communication
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT)

}

void loop() {
  // put your main code here, to run repeatedly:
  WiFiClient wifi_client;
  HTTPClient http_client;
  Serial.print("[HTTP] begin...\n");

  String http_request = "http://api.openweathermap.org/data/2.5/weather?q=Firenze,it&appid=84bd5e5de7476ab4920522cb8bb41ccd";
  Serial.print("HTTP Request: "); Serial.println(http_request);
  if (http_client.begin(wifi_client, http_request)) {
    Serial.print("[HTTP] GET...\n");
    int http_code = http_client.GET();
    if (http_code > 0) {
      Serial.printf("[HTTP] GET... code: %d\n", http_code);
      if (http_code == HTTP_CODE_OK || http_code == HTTP_CODE_MOVED_PERMANENTLY) {
        String payload_resp = http_client.getString();
        Serial.println(payload_resp);
        payload_resp.remove(payload_resp.length()-1);
        String timestamp=get_timestamp();
        
        //Serial.println(get_timestamp());
        String payload_to_Firebase = payload_resp + ",\"nave\":{\"time\":" + '"'+ timestamp.substring(11,19)+'"' + ",\"distance\":"+String(get_distance()) + "}}";  
        payload_to_Firebase.replace("[","");
        payload_to_Firebase.replace("]","");
        Serial.println("Payload for Firebase:"); Serial.println(payload_to_Firebase);
        FirebaseJson updateData;
        
        updateData.setJsonData(payload_to_Firebase);
        
        if(Firebase.updateNode(firebasedata, "/", updateData)){

          Serial.println("Correct update data");    
        }
        else { Serial.println(firebasedata.errorReason()); }
        
      }       
    }else{
      Serial.printf("[HTTP] GET... failed, error: %s\n", http_client.errorToString(http_code).c_str());
    }
    http_client.end();
  } else {
    Serial.printf("[HTTP} Unable to connect\n");
  }

int visibility=get_visibility();

   if (distance<=DISTANCE_THRESHOLD) {
     if ((visibility<VISIBILITY_THRESHOLD or int(timestamp.substring(11,12))>HOUR_THRESHOLD)& flag=!2){
         digitalWrite(LED, HIGH); 
         digitalWrite(LED, LOW); 
         delay(200); 
     flag=2;
     else{
      digitalWrite(LED,HIGH)
      flag=1;
      if flag=!1{
        digitalWrite(LED,HIGH)
        flag=1;
      }
     }
     }
     
  } 
  if (distance>DISTANCE_THRESHOLD &flag==1){
    digitalWrite(LED, LOW);
    flag=0;
  }
  delay(1000*2);
}
