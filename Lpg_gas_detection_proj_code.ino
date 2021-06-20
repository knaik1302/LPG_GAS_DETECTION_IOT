#include <SoftwareSerial.h> //wifi module
#include <LiquidCrystal.h> // lcd
#include <Servo.h> // servo

SoftwareSerial espSerial = SoftwareSerial(8,10);
LiquidCrystal lcd(12, 11, 5, 4, 3, 2); //rx en d5 d4 d3 d2
Servo myservo;

int pos = 0;
int motorPin = 13;
int const gas_pin=A1;
int window_door=0;
int g=0;
String apiKey = "XLC4MN01DH1AXP9D";
String ssid="Mirage"; 
String password ="1234567890"; 
boolean DEBUG=true;


void showResponse(int waitTime){
long t=millis(); // it returns the no of milli seconds passed since the arduino board began running the current prog (say 15sec)
char c;
while (t+waitTime>millis()){      //t=15s + waittime=1s > 15s
if (espSerial.available()){ // it returns the bytes value which is already stored in serial buffer. Buffer can hold 64bytes
c=espSerial.read();
if (DEBUG) Serial.print(c);
  }
}
}

boolean thingSpeakWrite(int value1){
String cmd = "AT+CIPSTART=”TCP”,”api.thingspeak.com”,80";
espSerial.println(cmd);
if (DEBUG) Serial.println(cmd);
if(espSerial.find("Error")){
if (DEBUG) Serial.println("AT+CIPSTART error");
return false;
}

String getStr = "GET /update?api_key=";
getStr += apiKey;
getStr +="&field1=";
getStr += String(value1);

cmd = "AT+CIPSEND=";
cmd += String(getStr.length());
espSerial.println(cmd);
if (DEBUG) Serial.println(cmd);
delay(100);

if(espSerial.find(">")){
espSerial.print(getStr);
if (DEBUG) Serial.print(getStr);
}

else{
espSerial.println("AT+CIPCLOSE");
if (DEBUG) Serial.println("AT+CIPCLOSE");
return false;
}
return true;
}

void setup() { 

DEBUG=true; 
 myservo.attach(9);  
 myservo.write(pos);
 lcd.begin(16, 2); 
 lcd.print("!** HELLO **!");
 pinMode(gas_pin,INPUT);
 Serial.begin(9600);
espSerial.begin(115200);

espSerial.println("AT+CWMODE=1"); // act as client

showResponse(1000); 

espSerial.println("AT+CWJAP=""+ssid+"",""+password+""");

showResponse(5000);

if (DEBUG) Serial.println("Setup completed");
}


void loop() {
int g=analogRead(gas_pin);
  Serial.println("Gas value is");
  Serial.print(g);
  delay(20);
  
  if(g<110){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("!!** NORMAL **!!");
    lcd.setCursor(0,1);
    lcd.print("GAS VALUE : ");
    lcd.print(g);
    delay(20);
    
    if(window_door==1){
      for (pos = 180; pos >= 0; pos -= 1) {
         myservo.write(pos);     
       delay(15);
      }
      window_door=0;
    }
    digitalWrite(10, HIGH);
  }
  else if(g>110 && g<250){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("!!** MEDIUM **!!");
    lcd.setCursor(0,1);
    lcd.print("GAS VALUE : ");
    lcd.print(g);
    delay(20);
    
    digitalWrite(10, LOW);
    digitalWrite(motorPin, HIGH);
    tone(motorPin, 500, 300);
  }
  else{
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("!!** EXTREME **!!");
    lcd.setCursor(0,1);
    lcd.print("GAS VALUE : ");
    lcd.print(g);
    delay(20);
    
    digitalWrite(motorPin, HIGH);
    tone(motorPin, 1000, 300);
    
    if(window_door==0){
       for (pos = 0; pos <= 180; pos += 1) { 
         myservo.write(pos);
         tone(motorPin, 1000, 30);
         digitalWrite(motorPin, HIGH);
       delay(15); 
    }
      window_door=1;
  }
  thingSpeakWrite(g);
  delay(15);
 }

}
