#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
#include <Servo.h>

#define DEBUG true

Servo myservo;
LiquidCrystal_I2C lcd(0x27, 16, 2);
SoftwareSerial esp8266(2, 8);


//RFID
char i;
char j;
String card;
String card1;
int count = 0;
int count1 = 0;
char c;
int cardstatus = 0;

int pos = 0;

void setup()
{
  pos = 0;

  lcd.begin(); // Initialinze LCD.
  lcd.backlight(); // makes Backligh ON.
  lcd.clear();     // Clears LCD
  lcd.home();      // Sets cursor at Column 0 and Line 0
  lcd.print("      Home");
  lcd.setCursor(0, 1); // Sets cursor at Column 0 and Line 1
  lcd.print("   Automation");
  delay(2000);
  //  lcd.clear();
  Serial.begin(9600);
  esp8266.begin(9600);
  myservo.attach(3);

  sendData("AT+RST\r\n", 2000, DEBUG); // reset module
  delay(3000);
  sendData("AT+CWMODE=2\r\n", 1000, DEBUG); // configure as access point
  delay(1000);
  sendData("AT+CIPMODE=0\r\n", 1000, DEBUG); // configure as access point
  delay(1000);
  sendData("AT+CIPMUX=1\r\n", 1000, DEBUG); // configure for multiple connections
  delay(1000);
  sendData("AT+CIFSR\r\n", 1000, DEBUG); // get ip address
  delay(1000);
  sendData("AT+CIPSERVER=1,81\r\n", 1000, DEBUG); // turn on server on port 80
  delay(2000);

}

void loop()
{

label0:
  //=========================================
  lcd.clear();
  lcd.home();
  lcd.print("Enter Card");

  if (Serial.available() > 0)
  {
    delay(300);
label2:
    i = Serial.read();
    delay(50);
    card = card + i;
    delay(50);
    count++;
    delay(50);
    if (count == 12)
    {
      count = 0;

      if (card == "5300A2DFB698")
      {
        lcd.clear();
        lcd.print("Authorized");
        lcd.setCursor(0, 1);
        lcd.print("User");
        for (pos = 0; pos < 180; pos++)
        {
          myservo.write(pos);
        }
        sendData("AT+CIPSEND=0,17\r\n", 1000, DEBUG);
        delay(100);
        sendData("DOOR OPENED=0,11\r\n", 1000, DEBUG);
        delay(5000);
        for (pos = 180; pos >= 0; pos--)
        {
          myservo.write(pos);
        }
        card = "";
        sendData("AT+CIPSEND=0,17\r\n", 1000, DEBUG);
        delay(100);
        sendData("DOOR CLOSED=0,11\r\n", 1000, DEBUG);


        Serial.flush();
        goto label0;


      }
      else
      {
        lcd.clear();
        lcd.home();
        pos=LOW;
        myservo.write(pos);
        lcd.print("Unauthorized");
        lcd.setCursor(0, 1);
        lcd.print("user");
        delay(2000);
        lcd.clear();

        card = "";
        sendData("AT+CIPSEND=0,17\r\n", 1000, DEBUG);
        delay(100);
        sendData("DOOR CLOSED=0,11\r\n", 1000, DEBUG);
        Serial.flush();
        goto label0;
      }
      Serial.flush();
      goto label0;
    }
    goto label2;

  }
  goto label0;


}
String sendData(String command, const int timeout, boolean debug)
{

  String response = "";
  esp8266.print(command); // send the read character to the esp8266
  long int time = millis();
  while ( (time + timeout) > millis())
  {
    while (esp8266.available())
    {
      // The esp has data so display its output to the serial window
      char c = esp8266.read(); // read the next character.
      response += c;
      Serial.println(response);
    }
  }

  return response;
}
