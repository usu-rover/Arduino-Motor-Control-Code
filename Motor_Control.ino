#include <Servo.h>

//User configuration
//NOTE: For wiring, the green/brown wire is ground, and the yellow/orange wire is signal
int left_pin = 13;
int right_pin = 12;
int PWM_rest = 1500; //This sets the default PWM value for the servos at rest

//Variable configuration, do not change
int right_percent = 0;
int left_percent = 0;

const byte buffSize = 40;
char inputBuffer[buffSize];
const char startMarker = '<';
const char endMarker = '>';
byte bytesRecvd = 0;
boolean readInProgress = false;
boolean newDataFromPC = false;

char messageFromPC[buffSize] = {0};
int newFlashInterval = 0;

unsigned long curMillis;

unsigned long prevReplyToPCmillis = 0;
unsigned long replyToPCinterval = 1000;

Servo left_motors;
Servo right_motors;

void setup() {
  Serial.begin(9600);
  Serial.println("you called, master?\n");
  left_motors.attach(left_pin);
  right_motors.attach(right_pin);
  delay(1000);
    
  // tell the PC we are ready
  Serial.println("<Arduino is ready>");
  Serial.println("type in a percent, and I will output your PWM.\n");
  
}


void loop() {
  curMillis = millis();
  getDataFromPC();
  replyToPC();
  moveServo();
}

void getDataFromPC() {
  // receive data from PC and save it into inputBuffer
  if(Serial.available() > 0) {
    char x = Serial.read();

    if (x == endMarker) {
      readInProgress = false;
      newDataFromPC = true;
      inputBuffer[bytesRecvd] = 0;
      parseData();
    }
    if(readInProgress) {
      inputBuffer[bytesRecvd] = x;
      bytesRecvd ++;
      if (bytesRecvd == buffSize) {
        bytesRecvd = buffSize - 1;
      }
    }
    if (x == startMarker) { 
      bytesRecvd = 0; 
      readInProgress = true;
    }
  }
}

void parseData() {
  // split the data into its parts
  char * strtokIndx; // this is used by strtok() as an index
  
  strtokIndx = strtok(inputBuffer,",");      // get the first part - the string
  left_percent = atoi(strtokIndx);     // convert this part to an integer

  strtokIndx = strtok(NULL, ","); // this continues where the previous call left off
  right_percent = atoi(strtokIndx);     // convert this part to an integer
}

void replyToPC() {
  if (newDataFromPC) {
    newDataFromPC = false;
    Serial.print("<Msg ");
    Serial.print(messageFromPC);
    Serial.print(" Time ");
    Serial.print(curMillis >> 9); // divide by 512 is approx = half-seconds
    Serial.println(">");
  }
}

void moveServo(){
  int left_PWMvalue = left_percent * 5 + PWM_rest;
  int right_PWMvalue = right_percent * 5 + PWM_rest;

  left_motors.writeMicroseconds(left_PWMvalue);
  right_motors.writeMicroseconds(right_PWMvalue);
}
