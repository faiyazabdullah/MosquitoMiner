//YouTube|Tech at Home
#include <Servo.h>  // Include the Servo library for PWM control
#include <Wire.h>

#define SLAVE_ADDRESS 0x04  // Arduino I2C address

#define TRIG_PIN 1
#define ECHO_PIN 2

//Receiver signal pins
double ch1_pin = 3;  //PWM pin
double ch2_pin = 9;  //PWM pin

int R_PWM_right = 5;  //PWM pin
int L_PWM_right = 6;  //PWM pin

//Left motor driver pins

int R_PWM_left = 10;  //PWM pin
int L_PWM_left = 11;  //PWM pin

// ------   Rx threshold values - Update based on your TX values
//FWD
// int Ch1Ch2_start_Fwd = 1510;
// int Ch1Ch2_End_Fwd = 1800;
// //BACK
// int Ch1Ch2_start_Bac = 1480;
// int Ch1Ch2_End_Bac = 1200;
int Ch1Ch2_start_Fwd = 1520;
int Ch1Ch2_End_Fwd = 2010;
//BACK
int Ch1Ch2_start_Bac = 1470;
int Ch1Ch2_End_Bac = 950;

double ch1 = 0;
double ch2 = 0;
char t = 'n';
float distance = 0.0;


// ----------------------------------


void receiveData(int byteCount) {
  char command = Wire.read();  // Read the received byte
  Serial.println(command);
  Serial.print("Distance: ");
  Serial.println(distance);
  t = command;
  if (command == 'r') {
    ch1 = 1300;
    ch2 = 1680;
  } else if (command == 'l') {
    ch1 = 1650;
    ch2 = 1320;
  } else if (command == 'c') {
    if (distance > 65) {
      ch1 = 1580;
      ch2 = 1580;
    } else if (distance < 40 and distance > 5) {
      ch1 = 1420;
      ch2 = 1420;
    } else {
      ch1 = 0;
      ch2 = 0;
    }
  } else if (command == 'n') {
    t = 'n';
  }
}


void setup() {
  Wire.begin(SLAVE_ADDRESS);    // Initialize I2C communication as slave
  Wire.onReceive(receiveData);  // Register callback for incoming I2C data
  Serial.begin(9600);
  pinMode(3, INPUT);
  pinMode(9, INPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
}

void loop() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  unsigned long duration = pulseIn(ECHO_PIN, HIGH);
  distance = duration * 0.034 / 2.0;

  if (Wire.available()) {
    receiveData(0);  // Call receiveData function manually
  } else if (t == 'n') {
    ch1 = pulseIn(ch1_pin, HIGH);
    ch2 = pulseIn(ch2_pin, HIGH);
    Serial.print("Remote");
  }
  Serial.print("ch1: ");
  Serial.print(ch1);
  Serial.print("  ch2: ");
  Serial.print(ch2);
  Serial.println("");
  //Speed mapping for right motor driver
  int spdFwd_RightLeft = map(ch1, Ch1Ch2_start_Fwd, Ch1Ch2_End_Fwd, 0, 255);
  int spdBac_RightLeft = map(ch1, Ch1Ch2_start_Bac, Ch1Ch2_End_Bac, 0, 255);


  //   int spdFwd_RightLeft1 = map(ch2, Ch1Ch2_start_Fwd, Ch1Ch2_End_Fwd, 0, 255);
  // int spdBac_RightLeft1 = map(ch2, Ch1Ch2_start_Bac, Ch1Ch2_End_Bac, 255, 0);
  // int spdBac_RightLeft2 = map(ch2,  Ch1Ch2_End_Bac, Ch1Ch2_start_Bac, 255, 0);
  // int spdBac_RightLeft3 = map(ch2, Ch1Ch2_End_Bac, Ch1Ch2_start_Bac, 255, 0);


  if ((ch1 == 0) && (ch2 == 0)) {
    analogWrite(R_PWM_right, 0);
    analogWrite(L_PWM_right, 0);
    analogWrite(R_PWM_left, 0);
    analogWrite(L_PWM_left, 0);
  }
  //FWD
  else if ((ch1 > Ch1Ch2_start_Fwd) && (ch2 > Ch1Ch2_start_Fwd)) {
    analogWrite(R_PWM_right, 0);
    analogWrite(L_PWM_right, spdFwd_RightLeft);
    analogWrite(R_PWM_left, spdFwd_RightLeft);
    analogWrite(L_PWM_left, 0);
  }
  //RIGHT
  else if ((ch1 > Ch1Ch2_start_Fwd) && (ch2 < Ch1Ch2_start_Bac)) {
    analogWrite(R_PWM_right, 0);
    analogWrite(L_PWM_right, spdFwd_RightLeft);
    analogWrite(R_PWM_left, 0);
    analogWrite(L_PWM_left, spdFwd_RightLeft);
  }
  //LEFT
  else if ((ch1 < Ch1Ch2_start_Bac) && (ch2 > Ch1Ch2_start_Fwd)) {
    // analogWrite(R_PWM_right,0);
    // analogWrite(L_PWM_right,spdBac_RightLeft);
    // analogWrite(R_PWM_left,0);
    // analogWrite(L_PWM_left,0);
    analogWrite(R_PWM_right, spdBac_RightLeft);
    analogWrite(L_PWM_right, 0);
    analogWrite(R_PWM_left, spdBac_RightLeft);
    analogWrite(L_PWM_left, 0);
  }
  //BACK
  else if ((ch1 < Ch1Ch2_start_Bac) && (ch2 < Ch1Ch2_start_Bac)) {
    analogWrite(R_PWM_right, spdBac_RightLeft);
    analogWrite(L_PWM_right, 0);
    analogWrite(R_PWM_left, 0);
    analogWrite(L_PWM_left, spdBac_RightLeft);
  } else {
    // analogWrite(R_PWM_right,0);
    // analogWrite(L_PWM_right,0);
    // analogWrite(R_PWM_left,0);
    // analogWrite(L_PWM_left,0);
    analogWrite(R_PWM_right, 0);
    analogWrite(L_PWM_right, 0);
    analogWrite(R_PWM_left, 0);
    analogWrite(L_PWM_left, 0);
  }
}