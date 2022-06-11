// Servo control libraries

#include <Servo.h>

Servo myservo;  // create servo object to control a servo

 int open = 90;
 int close = 180;


 //GSM MODULE LIBRARIES
 
#include <SoftwareSerial.h>
SoftwareSerial sim800l(5, 8); // RX,TX for Arduino and for the module it's TXD RXD, they should be inverted
 
 String gsm_message;
 String openTag =  "AT+CMGS=\"";
String phoneNumber;
String closeTag = "\"\r";
 
 
 //SOLENOID CONTROL PARAMETERS
 const int solenoidPin = 6;// the number of the solenoid pin

//LIBRARIES FOR CONTACTLESS TEMP SENSOR
#include <Adafruit_MLX90614.h>
Adafruit_MLX90614 mlx = Adafruit_MLX90614();
float avg_temp=0;
float temp;
int samples = 10;
float temp_threshold = 37;// normal temp threshold
String alert;


//LBRARIES FOR RFID
#include <SPI.h>
#include <MFRC522.h>
#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
MFRC522::MIFARE_Key key; 

String tag_name;

//LIBRARIES FOR LCD
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line lcd


//VARIABLES AND FUNCTION FOR DISTANCE SENSOR
#define echoPin 2 // attach pin D2 Arduino to pin Echo of HC-SR04
#define trigPin 3 //attach pin D3 Arduino to pin Trig of HC-SR04
long duration; // variable for the duration of sound wave travel
int distance; // variable for the distance measurement


int measureDistance()
{// Clears the trigPin condition
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)
  // lcds the distance on the Serial Monitor
  delay(500);
 return distance;

}


//METHOD FOR WELCOME MESSAGE
void welcomeMessage(){
  delay(1000);
  lcd.noBlink();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Welcome to");
  lcd.setCursor(0,1);
  lcd.print("Kyambogo University");
  delay(2000);


  
    for (int positionCounter = 0; positionCounter < 13; positionCounter++) {
    // scroll one position left:
    lcd.scrollDisplayLeft();
    // wait a bit:
    delay(250);
  }
  lcd.clear();
  delay(500);
  

  lcd.setCursor(0,1);
  lcd.print("Place your tag");
  delay(2000);
  
  
    for (int positionCounter = 0; positionCounter < 13; positionCounter++) {
    // scroll one position left:
    lcd.scrollDisplayRight();
    // wait a bit:
    delay(250);
  }
  lcd.clear();

  delay(1000);
  lcd.setCursor(0,0);
  lcd.print("Welcome to KYU");
  lcd.setCursor(0,1);
  lcd.print("PLACE YOUR TAG");
    
  }


//GLOBAL VARIABLES
int beep = A3;
String cloud_data;


//METHOD FOR SMS
void sendSMS(String msg, String phone)
{
 
  Serial.println("Sending SMS...");               //Show this message on serial monitor
  sim800l.print("AT+CMGF=1\r");                   //Set the module to SMS mode
  delay(100);
  //sim800l.print("AT+CMGS=\"+256701089932\"\r");  //Your phone number don't forget to include your country code, example +212123456789"
  sim800l.print(openTag + phone + closeTag);
  delay(500);
  sim800l.print(msg);       //This is the text to send to the phone number, don't make it too long or you have to modify the SoftwareSerial buffer
  delay(500);
  sim800l.print((char)26);// (required according to the datasheet)
  delay(500);
  sim800l.println();
  Serial.println("Text Sent.");
  delay(500);

}
 


//METHOD FOR OPENING DOOR
void openDoor()
{
 
  digitalWrite(solenoidPin, HIGH);
  delay(500);
   myservo.write(open);  // tell servo to go to a particular angle
  delay(5000);
    myservo.write(close);              
  delay(1000); 
  digitalWrite(solenoidPin, LOW);
  
  
  }


void setup() {
Serial.begin(9600);    //Initiate serial communication
sim800l.begin(9600);   //Initiate serial communication on the GSM module
delay(500);
 
 //INITIATE RFID TAG
  SPI.begin();           // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522


//Define variable for distance sensor
pinMode(trigPin, OUTPUT); // Sets the trigPin as an OUTPUT
pinMode(echoPin, INPUT); // Sets the echoPin as an INPUT


//define pin variable for buzzer
pinMode(beep, OUTPUT);
digitalWrite(beep,LOW);
   
  //print welcome message
  

   //Print welcome message on LCD
  lcd.init();                      // initialize the lcd 
  lcd.backlight();

 welcomeMessage();

 //initiate temperature sensor
 
 if (!mlx.begin()) {
    Serial.println("Error connecting to MLX sensor. Check wiring.");
    while (1);
  };
  
mlx.begin();
//Initiate servo object
myservo.attach(7,600,2300);  // (pin, min, max)

//set solenoinPin as output
pinMode(solenoidPin, OUTPUT);
 
}








void loop() {

// Look for new cards
  if ( mfrc522.PICC_IsNewCardPresent()) 
  {
    readRFID();
  
  }
 else return;
  

//Show UID on oled monitor

  //identify tag from here
   if (tag_name == "f3 3e 43 2") 
   {
   tag_name = "Guest";
   Serial.println("Guest");
   }
  
    else if (tag_name == "5c fd b7 21") 
    {
    tag_name = "1. Onyango";
    phoneNumber = "+256779380588";
      Serial.println("1. Onyango");
    }
    
     else if (tag_name == "a8 53 e5 54") 
     {
     tag_name = "2. Victoria";
     phoneNumber = "+256752488996";
      Serial.println("2. Victoria");
     }
     
      else if (tag_name == "4c cb 4 38")
      {
      tag_name = "3.  William";
      phoneNumber = "+256701089932";
        Serial.println("3. William");
      }

      else if (tag_name == "0 0 0 0")
      {
        tag_name = "Error in read";

        //alert on the oled that RFID has error
          lcd.clear();
               delay(500);
               lcd.setCursor(0,0);
               lcd.print("Welcome ");
                  lcd.setCursor(0,1);
               lcd.print(tag_name);

               delay(2000);
               lcd.clear();
          lcd.setCursor(0,0);
         lcd.print("Pse scan again");
         lcd.setCursor(0,1);
         lcd.print("Please wait");
         lcd.blink();
         delay(2000);


   //beep for person to get closer to the sensor
   for (int i=0; i <5;i++){
   digitalWrite(beep, HIGH);
   delay(50);
   digitalWrite(beep, LOW);
   delay(50);
   }
                }
  
     

   //beep for person to get closer to the sensor
   digitalWrite(beep, HIGH);
   delay(100);
   digitalWrite(beep, LOW);




   

//if tag_name is not for guest, check cloud information



if(tag_name != "Guest" && tag_name != "Error in read")
{
  //alert on the oled that temperature checking eligibility
            lcd.clear();
               delay(500);
               lcd.setCursor(0,0);
               lcd.print("Welcome ");
                lcd.setCursor(0,1);
               lcd.print(tag_name);
               delay(2000);
          lcd.setCursor(0,0);
         lcd.print("Checking........");
         lcd.setCursor(0,1);
         lcd.print("Please wait...");
        lcd.blink();


  // check the cloud for data
  while (Serial.available() == 0){}//wait for serial data
  
            
            cloud_data = Serial.readString();// read the incoming data as string
            
            if(cloud_data == "eligible")
              {
                Serial.println("eligible");

                //if user is eligible, tell them so
                  lcd.clear();
                  lcd.noBlink();
                 delay(500);
                 lcd.setCursor(0,0);
                 lcd.print("Welcome ");
                 lcd.setCursor(0,1);
                 lcd.println(tag_name);
                 delay(2000);
                 lcd.clear();
                 lcd.setCursor(0,0);
                 lcd.print(tag_name);
                 lcd.setCursor(0,1);
                 lcd.print("You are eligible");
                        delay(2000);
               
              }
            
            else if (cloud_data == "ineligible\n")
              {
                Serial.println("ineligible");
        //if user is eligible, tell them so
                          lcd.clear();
                          lcd.noBlink();
                         delay(500);
                         lcd.setCursor(0,0);
                         lcd.print("Welcome ");
                           lcd.setCursor(0,1);
                         lcd.print(tag_name);
                           delay(2000);
                         lcd.setCursor(0,0);
                         
                         lcd.print("U are Ineligible");
                         lcd.setCursor(0,1);
                         lcd.print("Isolate or test");
                 
                             for (int i=0; i <5;i++){
                                 digitalWrite(beep, HIGH);
                                 delay(100);
                                 digitalWrite(beep, LOW);
                                 delay(100);
                                 }
                         
                         delay(3000);
                gsm_message = "Dear " + tag_name + ", access to Kyambogo University denied. Ineligible";
                Serial.println(gsm_message);
                sendSMS(gsm_message, phoneNumber);
                
              }
            
            else
            {
              Serial.println(cloud_data);
              
            }
         
  }


   
else if(tag_name == "Guest")
{
  cloud_data = "eligible\n";
}


//measure distance with distance sensor if cloud data is eligible
if(cloud_data == "eligible\n")
{
//alert on the oled that temperature is being measured
              lcd.clear();
              lcd.noBlink();
               delay(500);
               lcd.setCursor(0,0);
               lcd.print("Welcome ");
               lcd.setCursor(0,1);
               lcd.print(tag_name);
               delay(2000);

    lcd.clear();
    lcd.print(tag_name);           
   lcd.setCursor(0,1);
   lcd.print("Measure at beep");



do
{
  Serial.print("Distance = ");
  Serial.println( measureDistance());
  }
 while(measureDistance()>5);
 
 
 //beep to alert person that he has reached the right distance
   digitalWrite(beep, HIGH);
   delay(500);
   digitalWrite(beep, LOW);


//Start measuring temperature
for (int i = 0; i< samples; i++){
temp = mlx.readObjectTempC();
avg_temp += temp;
delay(200);
}
avg_temp /= samples;

Serial.println(avg_temp);
Serial.print("Temp: ");//calibrating the temp sensor
/*
if (avg_temp <= 43)
  avg_temp *= 0.85141509;

else if (avg_temp <= 48)
  avg_temp *= 0.75523013;

else if (avg_temp <= 50)
  avg_temp *= 0.73373984;

else
 avg_temp *= 0.73373984;

*/
Serial.println(avg_temp);

//if temp is higher than threshold dont open send the person away and log data
    // print temp on lcd
              lcd.clear();
               delay(500);
               lcd.setCursor(0,0);
               lcd.print("Welcome ");
               lcd.setCursor(0,1);
               lcd.print(tag_name);
               delay(3000);
               lcd.clear();
               lcd.setCursor(0,0);
               lcd.print(tag_name);
   lcd.setCursor(0,1);
   lcd.print("Temp=");
   lcd.print(avg_temp);
   lcd.print((char)223);
   lcd.print("C");

   if(avg_temp > temp_threshold)
   alert = "COVID!";
   else
   alert =  "OK";
   
    lcd.setCursor(13,1);
    lcd.print(alert);

    delay(3000);
    Serial.println(tag_name + ":" +avg_temp);
    
if(avg_temp > temp_threshold)
{
                            for (int i=0; i <5;i++){
                                 digitalWrite(beep, HIGH);
                                 delay(100);
                                 digitalWrite(beep, LOW);
                                 delay(100);

                      
                                 }
  gsm_message = "Dear " + tag_name + ", access to Kyambogo University has been denied. High temp";
  sendSMS(gsm_message, phoneNumber);
  }
else  //open door and close it after

openDoor();




   //reset cloud_data to null
   cloud_data = "";
}


  
welcomeMessage();
} 

void readRFID()
{
      mfrc522.PICC_ReadCardSerial();
    tag_name = String(mfrc522.uid.uidByte[0], HEX)+" "+String(mfrc522.uid.uidByte[1], HEX)+" "+String(mfrc522.uid.uidByte[2], HEX)+ " "+String(mfrc522.uid.uidByte[3], HEX);
    Serial.print("Scanned PICC's UID:");
   Serial.println(tag_name);
}
