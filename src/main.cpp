#include <Arduino.h>
#include <Servo.h>

const int trigPin = 10;
const int echoPin = 11;
const int servoPin = 12;

Servo radarServo;

long readDistanceCM()
{
	digitalWrite(trigPin, LOW);
	delayMicroseconds(2);
	digitalWrite(trigPin, HIGH);
	delayMicroseconds(10);
	digitalWrite(trigPin, LOW);

	long duration = pulseIn(echoPin, HIGH);
	long distanceCm = duration * 0.034 / 2; // velocidad del sonido en cm/us

	return distanceCm;
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
	for (int angle = 0; angle <= 180; angle += 5)
	{
		radarServo.write(angle);
		delay(200); // Espera para estabilizar el servo

		long distance = readDistanceCM();

		Serial.print(angle);
		Serial.print(",");
		Serial.println(distance);
	}

	for (int angle = 180; angle >= 0; angle -= 5)
	{
		radarServo.write(angle);
		delay(200); // Espera para estabilizar el servo

		long distance = readDistanceCM();

		Serial.print(angle);
		Serial.print(",");
		Serial.println(distance);
	}
}
