 /* Read 2 IRs and set 2 PWMs to 2 electromagnets  */

// Hardware params
int dirSmallPin = 12; // D12 pin  is direction for line A = small electromagnet
int pwmSmallPin = 3;  // D3 pin  is pwm for line A = small electromagnet
int dirBigPin = 13;   // big electromagnet direction
int pwmBigPin = 11;   // big electromagnet PWM
int irAPin = A2;
int irBPin = A3;
int redPin = 5;
int greenPin = 10;
int bluePin = 6;

int maxIrSensorWhenVeryclose = 350; // read with hand very close to sensor
int maxPwmElectro = 205; // 0 - 255

// Software params
float irThres01 = 0.3; // threshold to decide it's big enought to jump state
int   irMinNonZeroVal = 0; // under this, ir is considered noise =0
float lambda = 0.3; // running avg smoothing parameter (expo smoothing)
float ledBrightMult01 = 1.; // in 0-1 to dim the RGB LED
float sinusFmax = 1.; // Hz max frequence of field sinus
float sinusFreqLoopPeriod = 60*5; // seconds to make a full loop over frequencies
float secBeforSinus = 2; // seconds
float secBeforSleep = 2 * sinusFreqLoopPeriod; // seconds

// Software helpers
const float tau = 6.2832; // 2* pi
const int SLEEP_STATE = 1;
const int PROXI_STATE = 2;
const int nStates = 2;
int curState;
float tInit;
int irARunningAvg = 0; // Smoothed value, running avg
int irBRunningAvg = 0; // Smoothed value, running avg
int dirSmallFIXED2LOW = LOW;
int dirBig = LOW;

void setup() {
    Serial.begin(9600);
    pinMode(dirSmallPin, OUTPUT);
    pinMode(dirBigPin, OUTPUT);
    pinMode(pwmSmallPin, OUTPUT);
    pinMode(pwmBigPin, OUTPUT);
    pinMode(redPin, OUTPUT);
    pinMode(greenPin, OUTPUT);
    pinMode(bluePin, OUTPUT);
    curState = SLEEP_STATE;
    tInit = ((float) millis()) / 1000.;
}

void loop() {
    float fieldSmall01 = 0.;
    float fieldBig01 = 0.;
    float irA01 = readIr01(irAPin);
    float irB01 = readIr01(irBPin);
    float t = ((float) millis()) / 1000.; // seconds since start
    
    switch (curState){
        case SLEEP_STATE:
            fieldSmall01 = 0.;
            fieldBig01 = 0.;
            curState = nextStateFromSleep((irA01+irB01)/2., t);
            break;
        case PROXI_STATE:
            fieldSmall01 = irA01;
            fieldBig01 = irB01;
            curState = nextStateFromProxi((irA01+irB01)/2, t);
            break;
    }
    updateElectro(pwmSmallPin, fieldSmall01, dirSmallPin, dirSmallFIXED2LOW);
    updateElectro(pwmBigPin, fieldBig01, dirBigPin, dirBig);
    updateLed(dirBig);
    //printDebug(irA01, irB01, fieldSmall01, fieldBig01, dirBig, curState, t);
    delay(10);
}


int nextStateFromSleep(float ir01, float t) {
    if (ir01 > irThres01) {
        return jumpToState(PROXI_STATE);
    }
    else { // no ir
    	return SLEEP_STATE;
    }
}

int nextStateFromProxi(float ir01, float t) {
    if (ir01 > irThres01) {
        tInit = ((float) millis()) / 1000.;
        return PROXI_STATE;
    }
    else { // no ir
        if ((t - tInit) > secBeforSinus) {
            dirBig = reverseDir(dirBig); // Revert direction
            return jumpToState(SLEEP_STATE);
        }
        else {
            return PROXI_STATE;
        }
    }
}

int jumpToState(int state) {
    tInit = ((float) millis()) / 1000.;
    return state;
}

float readIr01(int irPin) {
    // Read field mean strengh on IR sensor
    int irSmooth;
    int ir = analogRead(irPin); // InfraRed proximity
    if (irPin == irAPin) {
        irSmooth = smoothInt(ir, irARunningAvg, lambda );
        irARunningAvg = irSmooth;
    } else { // irBPin
        irSmooth = smoothInt(ir, irBRunningAvg, lambda );
        irBRunningAvg = irSmooth;
    }
    /*
    Serial.print(" ir RAW ");
    Serial.print(ir);
    Serial.print(" irSmooth ");
    Serial.print(irSmooth);
    Serial.println("");
    delay(500);
    */
    
    
    if (irSmooth < irMinNonZeroVal) { // remove noise
        irSmooth = 0;
    } 
    irSmooth = constrain(irSmooth, 0, maxIrSensorWhenVeryclose); // ensure a max value
    return ( (float)irSmooth / (float)maxIrSensorWhenVeryclose );
}

void updateElectro(int pwmPin, float field01, int dirPin, int dir) {
    // update direction
    digitalWrite(dirPin, dir);
    // update PWM
    int pwm = (int)(field01 * (float)maxPwmElectro);
    pwm = constrain(pwm, 0, maxPwmElectro); // defensive
    analogWrite(pwmPin, pwm); // default Arduino PWM freq = 500Hz
}

void updateLed(int dirBig){
    int r = 0;
    int g = 0;
    int b = 0;
    if (dirBig == LOW) {
        r = (int)(255. * ledBrightMult01);
    } else {
        b = (int)(255. * ledBrightMult01);
    }
    analogWrite(redPin, r);
    analogWrite(greenPin, g);
    analogWrite(bluePin, b);
}

int reverseDir(int dir) {
    if (dir == HIGH) {
        dir = LOW;
    } else {
        dir = HIGH;
    }
   return dir; 
}

int smoothInt(int newVal, int smoothedVal, float lambda ){
    return (int)smooth((float)newVal, (float)smoothedVal,  lambda );
}

float smooth(float newVal, float smoothedVal, float lambda ){
    smoothedVal = (newVal *  lambda) + (smoothedVal  *  (1-lambda));
    return smoothedVal;
}

void printDebug(float irA01, float irB01, float fieldSmall01, float fieldBig01, int dirBig, int curState, float t) {
    
    Serial.print(" irA01 ");
    Serial.print(irA01);
    Serial.print(" irB01 ");
    Serial.print(irB01);
    Serial.print(" fieldSmall01 ");
    Serial.print(fieldSmall01);
    Serial.print(" fieldBig01 ");
    Serial.print(fieldBig01);
    Serial.print(" dirBig ");
    Serial.print(dirBig);
    Serial.print(" curState ");
    Serial.print(curState);
    Serial.print(" t-tInit ");
    Serial.print(t - tInit);
    Serial.println("");
    delay(500);
}