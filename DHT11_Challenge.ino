#include <PubSubClient.h>
#include <DHT.h>
#include <ESP8266WiFi.h>

#include <WiFiManager.h>

#define DHTPIN  2       //DHT Signal Pin
#define DHTTYPE DHT11  

//fill these in eventually
const char* mqtt_host = "broker.hivemq.com";   //If I uncomment this there is an error
const char* outTopic_humid    =  "Jmcchap/d1/humid";    // placeholder name
const char* outTopic_temp     =  "Jmcchap/d1/temp";
float temperature = 0;
float humidity    = 0;

// Initilize the DHT sensor
DHT dht(DHTPIN, DHTTYPE);

//I don't really understand this block
WiFiClient espClient;
PubSubClient client(espClient);


//connect to WiFi network
void setup_wifi(){

  Serial.println("Starting wireless.");
  WiFiManager wifiManager; //Load the Wi-Fi Manager library.
  wifiManager.setTimeout(300); //Give up with the AP if no users gives us configuration in this many secs.
  if(!wifiManager.autoConnect()) {
    Serial.println("failed to connect and hit timeout");
    delay(3000);
    ESP.restart();
  }
  
  Serial.println("");
  Serial.print("Successfully connected to ");
  Serial.println(WiFi.localIP());
}


//reconnect if wifi is lost
void reconnect(){
 Serial.println("Oh no! Lost connection!"); 
 
 while(!client.connected()) {
  Serial.print("One moment, please...");
  
  if(client.connect("q2w3e4r5")){
    Serial.println("There we go!");                                    //If there is a successful reconnection
    client.publish(outTopic_humid, "Here I am! (DHT-11)");      //Publish an announcement
    client.publish(outTopic_temp, "Here I am! (DHT-11)");      //Publish an announcement again
    
    
  }else {
    Serial.print("Well, drat. The issue seems to be: ");
    Serial.println(client.state());
    Serial.println("Trying again in a jiffy");
    //Wait a jiffy seconds and retry connecting
    delay(3340);
  }
 }
}


void gather_data(){
   //Wait the minimum 2 seconds for readings
  delay(2000);

  //read humidity and temperature
   humidity = dht.readHumidity();           //humidity
   temperature  = dht.readTemperature(true);    //temperature, in Fahrenheit because screw Celsius 

  //check if there is any funky non-number buisness in readings. If so, sends an error and breaks out
  if ( isnan(humidity) || isnan(temperature) ) {
    Serial.println("What are numbers?");
    return;
  }

 return;

    
}


void setup() {
  Serial.begin(9600);
  Serial.println("Humidity and Temperature Test Begin!");


 dht.begin();
 setup_wifi();

 client.setServer(mqtt_host, 1883);
}

  


void loop() {
  
 if(!client.connected()){
  reconnect();
 }
 client.loop();

 gather_data();


  //If everything is bueno, spit out the readings to the display
      char humid_message[8];
      char temp_message[8];
      //sprintf(humid_message, "%f", humidity);
      //sprintf(temp_message, "%f", temperature);
      dtostrf(humidity,5,2,humid_message);
      dtostrf(temperature,5,2,temp_message);

      Serial.print("The humidity is ");
      Serial.println(humid_message);
      Serial.print("The temperature is ");
      Serial.println(temp_message);
        
       //Serial.println("The temperature is %sF", temp_message);

       
  //and the broker
       client.publish(outTopic_humid ,humid_message);
       //client.publish(outTopic, "\n");   
       client.publish(outTopic_temp ,temp_message);

}
