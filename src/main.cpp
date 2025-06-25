#include <Arduino.h>
#include <Servo.h>

const int trigPin = 10;
const int echoPin = 11;
const int servoPin = 12;

// Constants for the radar servo and distance measurement
const int MIN_SWEEP_ANGLE = 0;	 // Minimum sweep angle for the servo
const int MAX_SWEEP_ANGLE = 180; // Maximum sweep angle for the servo
const int SWEEP_STEP = 2;				 // Step size for sweeping the servo
const int SWEEP_DELAY = 75;			 // Delay between servo movements in milliseconds
const int MAX_DISTANCE = 50;		 // Maximum distance to measure in cm
const int MIN_TRACK_DISTANCE = 10;	 // Minimum distance to trigger tracking mode in cm
const int MAX_TRACK_DISTANCE = 20;	 // Maximum distance to trigger tracking mode in cm

Servo radarServo;
String mode = "scan";

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

void readSerialCommand()
{
	if (Serial.available())
	{
		mode = Serial.readStringUntil('\n');
		mode.trim();
		Serial.println(mode);
	}
}

void radarSweep()
{
	for (int angle = MIN_SWEEP_ANGLE; angle <= MAX_SWEEP_ANGLE; angle += SWEEP_STEP)
	{
		readSerialCommand();
		radarServo.write(angle);
		delay(SWEEP_DELAY);

		long distance = readDistanceInCM();
		Serial.print(angle);
		Serial.print(",");
		Serial.println(distance);

		if (distance > MIN_TRACK_DISTANCE && distance < MAX_TRACK_DISTANCE)
		{
			mode = "track";
			return;
		}
	}

	for (int angle = MAX_SWEEP_ANGLE; angle >= MIN_SWEEP_ANGLE; angle -= SWEEP_STEP)
	{
		readSerialCommand();
		radarServo.write(angle);
		delay(SWEEP_DELAY);

		long distance = readDistanceInCM();
		Serial.print(angle);
		Serial.print(",");
		Serial.println(distance);

		if (distance > MIN_TRACK_DISTANCE && distance < MAX_TRACK_DISTANCE)
		{
			mode = "track";
			return;
		}
	}
}

void trackMode()
{
	int lostCounter = 0;

	while (mode == "track")
	{
		float objectPosition = radarServo.read();
		long objectDistance = readDistanceInCM();
		Serial.print("Object Position: ");
		Serial.println(objectPosition);
		Serial.print("Object Distance: ");
		Serial.println(objectDistance);

		if (objectDistance < MIN_TRACK_DISTANCE || objectDistance > MAX_TRACK_DISTANCE)
		{
			lostCounter++;
		}

		if (lostCounter >= 3)
		{
			mode = "scan"; // Switch to scan mode if object is lost for too long
			Serial.println("Switching to scan mode due to lost object.");
			return;
		}

		readSerialCommand();
	}
}

void setup()
{
	Serial.begin(9600);
	pinMode(trigPin, OUTPUT);
	pinMode(echoPin, INPUT);
	radarServo.attach(servoPin);
}

void loop()
{
	readSerialCommand();

	if (mode == "scan")
	{
		radarSweep();
	}
	else if (mode == "track")
	{
		trackMode();
	}
}