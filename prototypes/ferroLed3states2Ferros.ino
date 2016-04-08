/* Read 2 IRs and set 2 PWMs to 2 electromagnets  */

// Hardware params
int electroAPin = 6;
int electroBPin = 11;
int maxIrSensorWhenVeryclose = 350; // read with hand very close to sensor
int maxPwmElectro = 35; // 30 is ok for 12V and 0.3Ohm coil

// Software params
float proxiBrightMult = 0.1; // in 01
float sinusFmax = 1.; // Hz max frequence of field sinus
float sinusFreqLoopPeriod = 60*5; // seconds to make a full loop over frequencies
float irAThres = 0.3;
float secBeforSinus = 120; // seconds
float secBeforSleep = 1 * sinusFreqLoopPeriod; // seconds

// Software helpers
const float tau = 6.2832; // 2* pi
const int SLEEP_STATE = 1;
const int PROXI_STATE = 2;
const int SINUS_STATE = 3;
const int nStates = 3;
int curState;
float tInit;

void setup() {
  Serial.begin(9600);
  pinMode(electroAPin, OUTPUT);
  pinMode(electroBPin, OUTPUT);
  curState = SLEEP_STATE; 
  tInit = ((float) millis()) / 1000.;
}

void loop() {
  float fieldA01 = 0.;
  float fieldB01 = 0.;
  float irA01 = readirA01();
  float irB01 = readirB01();
  float t = ((float) millis()) / 1000.; // seconds since start

  switch (curState){
  case SLEEP_STATE:
    fieldA01 = 0.;
    fieldB01 = 0.;
    curState = nextStateFromSleep((irA01+irB01)/2., t);
    break;            
  case PROXI_STATE:
    fieldA01 = irA01;
    fieldB01 = irB01;
    curState = nextStateFromProxi((irA01+irB01)/2, t);
    break;            
  case SINUS_STATE: // filed oscillate with slowly varying frequency
    float x = tau*t/sinusFreqLoopPeriod;
    float freq01 = 1/0.637*pow((1/3.5) * (   2*((1-cos(x))/2) +  pow( (1-cos(x))/2,128)  +  pow((1+abs(cos(x-1)))/2,8)   ),4);
    float freq = sinusFmax * freq01;
    fieldA01 = (1 + sin(tau * freq * t)) / 2;
	fieldB01 = (1 - sin(tau * freq * t)) / 2;

    /*Serial.print(" t ");
    Serial.print(t);
    Serial.print(" tInit ");
    Serial.print(tInit);
    Serial.print(" sin(tau*(t-tInit)/sinusFreqLoopPeriod + 0.75*tau) ");
    Serial.print(sin(tau*(t-tInit)/sinusFreqLoopPeriod + 0.75*tau));
    Serial.print(" freq ");
    Serial.print(freq);
    Serial.print(" ");
    Serial.print(" field01 ");
    Serial.print(field01);
    Serial.println();
    delay(100);*/
    
    curState = nextStateFromSinus((irA01+irB01)/2., t);
    break;
  }
  updateElectro(electroAPin, fieldA01);
  updateElectro(electroBPin, fieldB01);
  //printDebug(irA01, irB01, fieldA01, fieldB01, curState, t);
  delay(10);
}

int jumpToState(int state) {
  tInit = ((float) millis()) / 1000.;
  return state;
}

int nextStateFromSleep(float irA01, float t) {
  if (irA01 > irAThres) {
    return jumpToState(PROXI_STATE);
  } 
  else { // no ir
    if ((t - tInit) > 3 * secBeforSleep)  {
      return jumpToState(SINUS_STATE);
    } 
    else {
      return SLEEP_STATE;
    }
  }
}

int nextStateFromProxi(float irA01, float t) {
  if (irA01 > irAThres) {
    tInit = ((float) millis()) / 1000.;
    return PROXI_STATE;
  } 
  else { // no ir
    if ((t - tInit) > secBeforSinus) {
      return jumpToState(SINUS_STATE);
    } 
    else {
      return PROXI_STATE;
    }
  }
}
int nextStateFromSinus(float irA01, float t) {
  if (irA01 > irAThres) {
    return jumpToState(PROXI_STATE);
  } 
  else { // no ir
    if ((t - tInit) > secBeforSleep) {
      return jumpToState(SLEEP_STATE);
    } 
    else {
      return SINUS_STATE;
    }
  }
}

float readirA01() {
  // Read field mean strengh on IR sensor
  int ir = analogRead(A0); // InfraRed proximity
  if (ir < 30) { 
    ir = 0; 
  } // remove noise
  ir = constrain(ir, 0, maxIrSensorWhenVeryclose); // ensure a max value
  return ( (float)ir / (float)maxIrSensorWhenVeryclose );
}

float readirB01() {
  // Read field mean strengh on IR sensor
  int ir = analogRead(A1); // InfraRed proximity
  if (ir < 30) { 
    ir = 0; 
  } // remove noise
  ir = constrain(ir, 0, maxIrSensorWhenVeryclose); // ensure a max value
  return ( (float)ir / (float)maxIrSensorWhenVeryclose );
}

void updateElectro(int electroPin, float field01) {
  int electroPwm = (int)(field01 * (float)maxPwmElectro);
  electroPwm = constrain(electroPwm, 0, maxPwmElectro); // defensive
  analogWrite(electroPin, electroPwm); // default Arduino PWM freq = 500Hz
}

void printDebug(float irA01, float irB01, float fieldA01, float fieldB01, int curState, float t) {
  Serial.print(" irA01 ");
  Serial.print(irA01);
  Serial.print(" irB01 ");
  Serial.print(irB01);
  Serial.print(" fieldA01 ");
  Serial.print(fieldA01);
  Serial.print(" fieldB01 ");
  Serial.print(fieldB01);
  Serial.print(" curState ");
  Serial.print(curState);
  Serial.print(" t-tInit ");
  Serial.print(t - tInit);
  Serial.println("");
  delay(2000);
}