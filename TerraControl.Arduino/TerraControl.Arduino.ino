
#include <SoftwareSerial.h>
#include <dht11.h>

//pins
#define DHT11PIN    7
#define FAN_PIN 	  3
#define RED_PIN 	  6
#define GREEN_PIN 	5
#define BLUE_PIN  	4

//control codes
#define TEMPERATURE	  0x11
#define HUMIDITY	    0x12
#define LIGHT_ON 	    0x13
#define LIGHT_OFF 	  0x14
#define FAN_ON 		    0x15
#define FAN_OFF 	    0x16
#define CONFIG 		    0x17

#define START_MARKER   0x3C
#define END_MARKER     0x3E

const byte numChars = 32;

SoftwareSerial BTSerial (11, 12);
dht11 DHT11;

byte receivedChars[numChars];
boolean newData = false;

byte temperature;
byte humidity;
byte isLightOn;
byte isFanOn;
byte R;
byte G;
byte B;

void setup() {
  BTSerial.begin(9600);
  pinMode (8, OUTPUT);
  pinMode (FAN_PIN, OUTPUT);
}

void loop() {
  int chk = DHT11.read(DHT11PIN);
  recvWithStartEndMarkers();
  showNewData();
}

void recvWithStartEndMarkers() {
  static boolean recvInProgress = false;
  static byte ndx = 0;

  char rc;

  while (BTSerial.available() > 0 && newData == false) {
    rc = BTSerial.read();

    if (recvInProgress == true) {
      if (rc != END_MARKER) {
        receivedChars[ndx] = rc;
        ndx++;
        if (ndx >= numChars) {
          ndx = numChars - 1;
        }
      }
      else {
        recvInProgress = false;
        ndx = 0;
        newData = true;
      }
    }

    else if (rc == START_MARKER) {
      recvInProgress = true;
    }
  }
}

void showNewData() {
  if (newData == true) {

    if (receivedChars[0] == FAN_ON)			// Check is fan on
    {
      digitalWrite(FAN_PIN, HIGH);
      isFanOn = (byte)1;
    }
    else if (receivedChars[0] == FAN_OFF) 		//Check is fan of
    {
      digitalWrite(FAN_PIN, LOW);
      isFanOn = (byte)0;
    }
    else if (receivedChars[0] == CONFIG)		//Send configuration on android app start
    {
      sendConfig();
    }
    else if (receivedChars[0] == LIGHT_ON)		//Is light on
    {
      RGB_color(receivedChars[1], receivedChars[2], receivedChars[3]);
      isLightOn = (byte)1;
    }
    else if (receivedChars[0] == LIGHT_OFF)		//Is light off
    {
      RGB_color(0, 0, 0);
      isLightOn = (byte)0;
    }
    else if (receivedChars[0] == TEMPERATURE)		//Send temperature
    {
      temperature = (byte)DHT11.temperature;
      BTSerial.write('T');
      BTSerial.write(temperature);
      BTSerial.write(0xFF);
      //delay(100);
    }
    else if (receivedChars[0] == HUMIDITY)		//Send humidity
    {
      humidity = (byte)DHT11.humidity;

      BTSerial.write('H');
      BTSerial.write(humidity);
      BTSerial.write(0xFF);
      //delay(100);
    }
    else
    {
      R = receivedChars[0];
      G = receivedChars[1];
      B = receivedChars[2];
      RGB_color(R, G, B);
    }
    newData = false;
  }
}
void sendConfig() {
  BTSerial.write('C');
  BTSerial.write(temperature); //1
  BTSerial.write(humidity);//2

  BTSerial.write(isFanOn);//3
  BTSerial.write(isLightOn);//4

  BTSerial.write(R);//5
  BTSerial.write(G);//6
  BTSerial.write(B);//7

  BTSerial.write(0xFF);
}

void RGB_color(int red_light_value, int green_light_value, int blue_light_value)
{
  analogWrite(RED_PIN, red_light_value);
  analogWrite(GREEN_PIN, green_light_value);
  analogWrite(BLUE_PIN, blue_light_value);
}
