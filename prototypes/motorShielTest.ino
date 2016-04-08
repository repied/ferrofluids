/*
Testing the motor shield */

int dirSmall = 12; // D12 pin  is direction for line A = small electromagnet
int pwmSmall = 3; // D3 pin  is pwm for line A = small electromagnet
int dirBig = 13;
int pwmBig = 11;
int red = 5;
int green = 10;
int blue = 6;


int brightness = 0; // how bright the LED is
int fadeAmount = 2; // how many points to fade the LED by

// the setup routine runs once when you press reset:
void setup()
{
	pinMode(red, OUTPUT);
	pinMode(green, OUTPUT);
	pinMode(blue, OUTPUT);
	pinMode(dirSmall, OUTPUT);
	pinMode(pwmSmall, OUTPUT);
	pinMode(dirBig, OUTPUT);
	pinMode(pwmBig, OUTPUT);
	Serial.begin(9600);
}

void loop()
{
	digitalWrite(dirSmall, HIGH);
	digitalWrite(dirBig, HIGH); 

	analogWrite(red, 0);
	analogWrite(green, 0);
	analogWrite(blue, 100);
	
	analogWrite(pwmSmall, brightness);
	analogWrite(pwmBig, 255 - brightness);

	// change the brightness for next time through the loop:
	brightness = brightness + fadeAmount;

	// reverse the direction of the fading at the ends of the fade: 
	if (brightness == 0 || brightness == 130 )
	{
		fadeAmount = -fadeAmount;
	}
	// wait for 30 milliseconds to see the dimming effect    
	delay(30);
//	Serial.println(brightness);
//	delay(1000);
}