#include <PubSubClient.h>
#include <DHT.h>
#include <ESP8266WiFi.h>


#define DHTPIN  2       //DHT Signal Pin
#define DHTTYPE DHT11  

//fill these in eventually
const char* ssid       = "hunter1";
const char* password   = "hunter2";
//const char* hostname = "broker.hivemq.com";   //If I uncomment this there is an error
const char* outTopic    =  "Jmcchap/d1/readings";    // placeholder name

float temperature = 0;
float humidity    = 0;

// Initilize the DHT sensor
DHT dht(DHTPIN, DHTTYPE);

//I don't really understand this block
WiFiClient espClient;
PubSubClient client(espClient);


//connect to WiFi network
void setup_wifi(){
  
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED){   
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("Successfully connected to ");
  Serial.println(WiFi.localIP());
}


//reconnect if wifi is lost
void reconnect(){
 Serial.println("Oh no! Lost connection!"); 
 
 while(!client.connected()) {
  Serial.print("One moment, please...");
  
  if(client.connect("ESP8266Client")){
    Serial.println("There we go!");                                    //If there is a successful reconnection
    client.publish(outTopic, "Here I am! (DHT-11)");      //Publish an announcement
    
    
  }else {
    Serial.print("Well, drat. The issue seems to be: ");
    Serial.print(client.state());
    Serial.println("Trying again in a jiffy");
    //Wait 5 seconds and retry connecting
    delay(3340);
  }
 }
}


float gather_data(){
   //Wait the minimum 2 seconds for readings
  delay(2000);

  //read humidity and temperature
   float humid = dht.readHumidity();           //humidity
   float temp  = dht.readTemperature(true);    //temperature, in Fahrenheit because screw Celsius 

  //check if there is any funky non-number buisness in readings. If so, sends an error and breaks out
  if ( isnan(humid) || isnan(temp) ) {
    Serial.println("What are numbers?");
    return(0,0);
  }

 return(humidity, temperature);

    
}


void setup() {
  Serial.begin(9600);
  Serial.println("Humidity and Temperature Test Begin!");


 dht.begin();
 setup_wifi();
  
}

  


void loop() {
  
 if(!client.connected()){
  reconnect();
 }
 client.loop();

 gather_data();


  //If everything is bueno, spit out the readings to the display
      char humid_message[3];
      char temp_message[3];
      sprintf(humid_message, "%f", humidity);
      sprintf(temp_message, "%f", temperature);

        
       Serial.print("The humidity is ");
       Serial.print(humid_message);
       Serial.println("%");

       Serial.print("The temperature is ");
       Serial.print(temp_message);
       Serial.println("F");
       //Serial.println("The temperature is %sF", temp_message);

       
  //and the broker
       client.publish(outTopic,"The humidity is %f%", humidity);
       client.publish(outTopic, "\n");   
       client.publish(outTopic,"The temperature is %fF", temperature);

}
