///Make sure to install the library
#include <QTRSensors.h>
QTRSensors qtr;
const uint8_t SensorCount = 8;
uint16_t sensorValues[SensorCount];

float Kp = 1.0; //set up the constants value
float Ki = 0.4;
float Kd = 0.5;
int P;
int I;
int D;

int lastError = 0;
boolean onoff = false;

//Increasing the maxspeed can damage the motors - at a value of 255 the 6V motors will receive 7,4 V 
const uint8_t maxspeeda = 50;
const uint8_t maxspeedb = 50;
const uint8_t basespeeda = 50;
const uint8_t basespeedb = 50;

//Set up the drive motor carrier pins
int bphase2= 2;
int bphase = 3;
int apwm = 9;
int aphase = 4;
int aphase2=5;
int bpwm = 10;



//Set up the buttons pins
int buttoncalibrate = 6; //pin A3
int buttonstart = 7;

void setup() {
  Serial.begin(9600);
  //qtr.setTypeRC();
  qtr.setTypeAnalog();
  //Set up the sensor array pins
  qtr.setSensorPins((const uint8_t[]){A0,A1,A2,A3,A4,A5,A6,A7}, SensorCount);
  qtr.setEmitterPin(11);//LEDON PIN

 
  pinMode(aphase2, OUTPUT);
  pinMode(aphase, OUTPUT);
  pinMode(bphase, OUTPUT);
  pinMode(bphase2, OUTPUT);
  pinMode(apwm,OUTPUT);
  pinMode(bpwm,OUTPUT);


  delay(500);
  pinMode(LED_BUILTIN, OUTPUT);

  boolean Ok = false;
  while (Ok == false) { //the loop won't start until the robot is calibrated
    if(digitalRead(buttoncalibrate) == HIGH) {
      calibration(); //calibrate the robot for 10 seconds
      Ok = true;
    }
  }
  forward_brake(0,0);
}

void calibration() {
  digitalWrite(LED_BUILTIN, HIGH);
  for (uint16_t i = 0; i < 400; i++)
  {
    qtr.calibrate();
  }
  digitalWrite(LED_BUILTIN, LOW);
}

void loop() {
  if(digitalRead(buttonstart) == HIGH) {
    onoff =! onoff;
    if(onoff = true) {
      delay(1000);//a delay when the robot starts
    }
    else {
      delay(50);
    }
  }
  if (onoff == true) {
    PID_control();
  }
  else {
    forward_brake(0,0);
  }
}
void forward_brake(int posa, int posb) {
  //set the appropriate values for aphase and bphase so that the robot goes straight
digitalWrite(aphase, LOW);
  digitalWrite(aphase2, HIGH);
  digitalWrite(bphase, HIGH);
  digitalWrite(bphase2, LOW);
  digitalWrite(apwm, posa);
  digitalWrite(bpwm, posb);
 
}
void PID_control() {
  uint16_t posit = qtr.readLineBlack(sensorValues);
  Serial.println(sensorValues[SensorCount]);
  Serial.print(posit);
  int error = 3500 - posit;

  P = error;
  I = I + error;
  D = error - lastError;
  lastError = error;
  int motorspeed = P*Kp + I*Ki + D*Kd;
  
  int motorspeeda = basespeeda + motorspeed;
  int motorspeedb = basespeedb - motorspeed;
  
  if (motorspeeda > maxspeeda) {
    motorspeeda = maxspeeda;
  }
  if (motorspeedb > maxspeedb) {
    motorspeedb = maxspeedb;
  }
  if (motorspeeda < 0) {
    motorspeeda = 0;
  }
  if (motorspeedb < 0) {
    motorspeedb = 0;
  } 
  //Serial.print(motorspeeda);Serial.print(" ");Serial.println(motorspeedb);
  forward_brake(motorspeeda, motorspeedb);
}
