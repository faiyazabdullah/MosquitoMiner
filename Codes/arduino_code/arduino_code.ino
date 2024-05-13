//YouTube|Tech at Home

//Receiver signal pins
double ch1_pin = 3;  //PWM pin
double ch2_pin = 5;  //PWM pin

//Right motor driver pins
int R_EN_right = 16;
int L_EN_right = 17;
int R_PWM_right = 18;  //PWM pin
int L_PWM_right = 19;  //PWM pin

//Left motor driver pins
int R_EN_left = 1;
int L_EN_left = 21;
int R_PWM_left = 22;  //PWM pin
int L_PWM_left = 23;  //PWM pin

// ------   Rx threshold values - Update based on your TX values
//FWD
int Ch1Ch2_start_Fwd = 1510;
int Ch1Ch2_End_Fwd = 1760;
//BACK
int Ch1Ch2_start_Bac = 1480;
int Ch1Ch2_End_Bac = 1215;

// ----------------------------------

void setup() {
  Serial.begin(9600);
  pinMode(ch1_pin, INPUT);
  pinMode(ch2_pin, INPUT);
  pinMode(R_EN_right, OUTPUT);
  pinMode(L_EN_right, OUTPUT);
  pinMode(R_PWM_right, OUTPUT);
  pinMode(L_PWM_right, OUTPUT);
  pinMode(R_EN_left, OUTPUT);
  pinMode(L_EN_left, OUTPUT);
  pinMode(R_PWM_left, OUTPUT);
  pinMode(L_PWM_left, OUTPUT);
}

void loop() {
  //Receiver signal pins data
  double ch1 = pulseIn(ch1_pin, HIGH);
  double ch2 = pulseIn(ch2_pin, HIGH);

  // Serial.print("ch1: ");
  // Serial.println(ch1);
  // Serial.print("\t");
  // Serial.print("ch2: ");
  // Serial.print(ch2);
  // Serial.println("");

  //Speed mapping for right motor driver
  int spdFwd_RightLeft = map(ch1, Ch1Ch2_start_Fwd, Ch1Ch2_End_Fwd, 0, 255);
  int spdBac_RightLeft = map(ch1, Ch1Ch2_start_Bac, Ch1Ch2_End_Bac, 0, 255);

  digitalWrite(R_EN_right, HIGH);
  digitalWrite(L_EN_right, HIGH);
  digitalWrite(R_EN_left, HIGH);
  digitalWrite(L_EN_left, HIGH);

  if ((ch1 == 0) && (ch2 == 0)) {
    analogWrite(R_PWM_right, 0);
    analogWrite(L_PWM_right, 0);
    analogWrite(R_PWM_left, 0);
    analogWrite(L_PWM_left, 0);
  }
  //FWD
  else if ((ch1 > Ch1Ch2_start_Fwd) && (ch2 > Ch1Ch2_start_Fwd)) {
    analogWrite(R_PWM_right, spdFwd_RightLeft);
    analogWrite(L_PWM_right, 0);
    analogWrite(R_PWM_left, spdFwd_RightLeft);
    analogWrite(L_PWM_left, 0);
  }
  //RIGHT
  else if ((ch1 > Ch1Ch2_start_Fwd) && (ch2 < Ch1Ch2_start_Bac)) {
    analogWrite(R_PWM_right, 0);
    analogWrite(L_PWM_right, spdFwd_RightLeft);
    analogWrite(R_PWM_left, spdFwd_RightLeft);
    analogWrite(L_PWM_left, 0);
  }
  //LEFT
  else if ((ch1 < Ch1Ch2_start_Bac) && (ch2 > Ch1Ch2_start_Fwd)) {
    analogWrite(R_PWM_right, spdBac_RightLeft);
    analogWrite(L_PWM_right, 0);
    analogWrite(R_PWM_left, 0);
    analogWrite(L_PWM_left, spdBac_RightLeft);
  }
  //BACK
  else if ((ch1 < Ch1Ch2_start_Bac) && (ch2 < Ch1Ch2_start_Bac)) {
    analogWrite(R_PWM_right, 0);
    analogWrite(L_PWM_right, spdBac_RightLeft);
    analogWrite(R_PWM_left, 0);
    analogWrite(L_PWM_left, spdBac_RightLeft);
  } else {
    analogWrite(R_PWM_right, 0);
    analogWrite(L_PWM_right, 0);
    analogWrite(R_PWM_left, 0);
    analogWrite(L_PWM_left, 0);
  }
}