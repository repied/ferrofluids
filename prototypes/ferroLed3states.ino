/* Read IR and potentiometer, compute pulsing pwm
 * fade RGB led and PWM electromagnet accordingly */

// Hardware params
int rPin = 6;
int gPin = 3;
int bPin = 5;
int electroPin = 9;
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
  pinMode(rPin, OUTPUT);
  pinMode(gPin, OUTPUT);
  pinMode(bPin, OUTPUT);
  pinMode(electroPin, OUTPUT);
  curState = SLEEP_STATE; 
  tInit = ((float) millis()) / 1000.;
}

void loop() {
  float field01 = 0.;
  float brigh01 = 0.;
  float color01 = 0.;
  float irA01 = readirA01();
  float irB01 = readirB01();
  float t = ((float) millis()) / 1000.; // seconds since start

  switch (curState){
  case SLEEP_STATE:
    field01 = 0.;
    brigh01 = 0.;
    color01 = 0.;
    curState = nextStateFromSleep(irA01, t);
    break;            
  case PROXI_STATE:
    field01 = irA01;
    brigh01 = proxiBrightMult * irA01;    // 0.2 since RBG led are very bright
    color01 = irB01;
    curState = nextStateFromProxi(irA01, t);
    break;            
  case SINUS_STATE: // filed oscillate with slowly varying frequency
    float x = tau*t/sinusFreqLoopPeriod;
    float freq01 = 1/0.637*pow((1/3.5) * (   2*((1-cos(x))/2) +  pow( (1-cos(x))/2,128)  +  pow((1+abs(cos(x-1)))/2,8)   ),4);
    float freq = sinusFmax * freq01;
    field01 = (1 + sin(tau * freq * t)) / 2;

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

    brigh01 = field01;
    color01 = freq01;
    curState = nextStateFromSinus(irA01, t);
    break;
  }
  updateLeds(brigh01, color01);
  updateElectro(field01);
  //printDebug(irA01, irB01, field01, brigh01, color01, curState, t);
  delay(10);
}

int jumpToState(int state) {
  tInit = ((float) millis()) / 1000.;
  updateLeds(1, state / nStates);
  delay(300);
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
  int ir = analogRead(A1); // InfraRed proximity
  if (ir < 30) { 
    ir = 0; 
  } // remove noise
  ir = constrain(ir, 0, maxIrSensorWhenVeryclose); // ensure a max value
  return ( (float)ir / (float)maxIrSensorWhenVeryclose );
}

float readirB01() {
  // Read from potentiometer waiting to buyt a 2nb IR
  int pot = analogRead(A0);
  if (pot < 10) { 
    pot = 0; 
  } // remove noise
  pot = constrain(pot, 0, 1023); // just to be sure of min and max
  return ((float)pot / 1023.0);
}

void updateLeds(float brigh01, float color01) {
  // set an RGB led colors and brightness
  brigh01 = constrain(brigh01, 0., 1.); // defensive
  color01  = constrain(color01 , 0., 1.); // defensive
  //if (brigh01 < 0.02) {return;} // remove noise < 5%
  float r, g, b;
  float alpha01;
  if (color01 < 0.3333) {
    alpha01 = 3. * (color01 - 0.);
    r = alpha01;
    g = 1. - alpha01;
    b = 0.;
  } 
  else if (color01 < 0.6666) {
    alpha01 = 3. * (color01 - 0.3333);
    b = alpha01;
    r = 1. - alpha01;
    g = 0.;
  } 
  else {
    alpha01 = 3. * (color01 - 0.6666);
    g = alpha01;
    b = 1. - alpha01;
    r = 0.;
  }
  analogWrite(rPin, (int)(255. * r * brigh01) );
  analogWrite(gPin, (int)(255. * g * brigh01) );
  analogWrite(bPin, (int)(255. * b * brigh01) );

  /*    // debug
   Serial.print(" brigh01 ");
   Serial.print(brigh01);
   Serial.print(" ");
   Serial.print(" color01 ");
   Serial.print(color01);
   Serial.print(" alpha01 ");
   Serial.print(alpha01);
   Serial.print(" r ");
   Serial.print(r);
   Serial.print(" r ");
   Serial.print(g);
   Serial.print(" r ");
   Serial.print(b);
   Serial.println("");
   delay(200);*/
}

void updateElectro(float field01) {
  int electroPwm = (int)(field01 * (float)maxPwmElectro);
  electroPwm = constrain(electroPwm, 0, maxPwmElectro); // defensive
  analogWrite(electroPin, electroPwm); // default Arduino PWM freq = 500Hz
}

void printDebug(float irA01, float irB01, float field01, float brigh01, float color01, int curState, float t) {
  Serial.print(" irA01 ");
  Serial.print(irA01);
  Serial.print(" irB01 ");
  Serial.print(irB01);
  Serial.print(" field01 ");
  Serial.print(field01);
  Serial.print(" brigh01 ");
  Serial.print(brigh01);
  Serial.print(" ");
  Serial.print(" color01 ");
  Serial.print(color01);
  Serial.print(" curState ");
  Serial.print(curState);
  Serial.print(" t-tInit ");
  Serial.print(t - tInit);
  Serial.println("");
  delay(100);
}

