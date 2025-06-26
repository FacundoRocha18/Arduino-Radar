#include <Arduino.h>
#include <Servo.h>

const int trigPin = 10;
const int echoPin = 11;
const int servoPin = 12;

// Constants for the radar servo and distance measurement
const int MIN_SWEEP_ANGLE = 0;		 // Minimum sweep angle for the servo
const int MAX_SWEEP_ANGLE = 180;	 // Maximum sweep angle for the servo
const int SWEEP_STEP = 2;					 // Step size for sweeping the servo
const int SWEEP_DELAY = 75;				 // Delay between servo movements in milliseconds
const int MAX_DISTANCE = 50;			 // Maximum distance to measure in cm

Servo servo;

long readDistanceInCM()
{
	digitalWrite(trigPin, LOW);
	delayMicroseconds(2);
	digitalWrite(trigPin, HIGH);
	delayMicroseconds(10);
	digitalWrite(trigPin, LOW);

	long duration = pulseIn(echoPin, HIGH);
	long distance = duration * 0.034 / 2;

	if (distance > MAX_DISTANCE)
	{
		distance = MAX_DISTANCE;
	}

	return distance;
}

void radarSweep()
{
	for (int angle = MIN_SWEEP_ANGLE; angle <= MAX_SWEEP_ANGLE; angle += SWEEP_STEP)
	{
		servo.write(angle);
		delay(SWEEP_DELAY);

		long distance = readDistanceInCM();
		Serial.print("Object Position: ");
		Serial.print(angle + ", " + distance);
	}

	for (int angle = MAX_SWEEP_ANGLE; angle >= MIN_SWEEP_ANGLE; angle -= SWEEP_STEP)
	{
		servo.write(angle);
		delay(SWEEP_DELAY);

		long distance = readDistanceInCM();
		Serial.print("Object Position: ");
		Serial.print(angle + ", " + distance);
	}
}

void setup()
{
	Serial.begin(9600);
	pinMode(trigPin, OUTPUT);
	pinMode(echoPin, INPUT);
	servo.attach(servoPin);
}

void loop()
{
	radarSweep();
}