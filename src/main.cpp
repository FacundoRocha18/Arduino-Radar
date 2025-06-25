#include <Arduino.h>
#include <Servo.h>

const int trigPin = 10;
const int echoPin = 11;
const int servoPin = 12;

// Constants for the radar servo and distance measurement
const int MIN_SWEEP_ANGLE = 0; // Minimum sweep angle for the servo
const int MAX_SWEEP_ANGLE = 180; // Maximum sweep angle for the servo
const int SWEEP_STEP = 2; // Step size for sweeping the servo
const int SWEEP_DELAY = 75; // Delay between servo movements in milliseconds
const int MAX_DISTANCE = 200; // Maximum distance to measure in cm
const int TRACK_DISTANCE = 10; // Minimum distance to trigger tracking mode in cm

Servo radarServo;
String mode = "scan";

long readDistanceInCM() {
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

void readSerialCommand() {
  if (Serial.available()) {
    mode = Serial.readStringUntil('\n');
    mode.trim();
    Serial.println(mode);
  }
}

void radarSweep() {
  for (int angle = MIN_SWEEP_ANGLE; angle <= MAX_SWEEP_ANGLE; angle += SWEEP_STEP) {
    readSerialCommand();
    radarServo.write(angle);
    delay(SWEEP_DELAY);

    long distance = readDistanceInCM();
    Serial.print(angle);
    Serial.print(",");
    Serial.println(distance);

    if (distance < TRACK_DISTANCE) {
      mode = "track";
      return;
    }
  }

  for (int angle = MAX_SWEEP_ANGLE; angle >= MIN_SWEEP_ANGLE; angle -= SWEEP_STEP) {
    readSerialCommand();
    radarServo.write(angle);
    delay(SWEEP_DELAY);

    long distance = readDistanceInCM();
    Serial.print(angle);
    Serial.print(",");
    Serial.println(distance);

    if (distance <= TRACK_DISTANCE) {
      mode = "track";
      return;
    }
  }
}

float getObjectRelativePosition(int currentAngle, long distance)
{
	float angleRadians = radians(currentAngle);
	float x = cos(angleRadians) * distance;
	float y = sin(angleRadians) * distance;

	float relativeAngle = atan2(y, x); // Calculate the angle in radians
	relativeAngle = degrees(relativeAngle); // Convert radians to degrees
	Serial.print("Relative Angle: ");
	Serial.println(relativeAngle);

	return relativeAngle;
}

void trackMode() {
  int lostCounter = 0;

  while (mode == "track") {
		float objectPosition = getObjectRelativePosition(radarServo.read(), readDistanceInCM());
		Serial.print("Object Position: ");
		Serial.println(objectPosition);

		for (int angle = MIN_SWEEP_ANGLE; angle <= MAX_SWEEP_ANGLE; angle += objectPosition) {
      readSerialCommand(); // Check for serial commands to change mode
      radarServo.write(objectPosition); // Set servo to current object position
      delay(SWEEP_DELAY); // Wait for servo to reach position

      long distance = readDistanceInCM(); // Measure distance to the object

			// If the object is within tracking distance, update position and reset lost counter
      if (distance <= TRACK_DISTANCE) {
        objectPosition = getObjectRelativePosition(radarServo.read(), readDistanceInCM());
        Serial.print("Tracking at angle: ");
				Serial.print(objectPosition);
				Serial.print(", Distance: ");
				Serial.println(distance);

				lostCounter = 0; // Reset lost counter if object is detected

				radarServo.write(objectPosition); // Update servo position to track the object
				break; // Exit the loop if the object is detected
      } else {
				Serial.print("Lost object at angle: ");
				Serial.print(angle);
				Serial.print(", Distance: ");
				Serial.println(distance);

				lostCounter++;
				Serial.print("Lost Counter: ");
				Serial.println(lostCounter);

				if (lostCounter >= 3) {
					mode = "scan"; // Switch to scan mode if object is lost for too long
					Serial.println("Switching to scan mode due to lost object.");
					return;
				}
			}
    }

		for (int angle = MAX_SWEEP_ANGLE; angle >= MIN_SWEEP_ANGLE; angle -= objectPosition) {
      readSerialCommand(); // Check for serial commands to change mode
      radarServo.write(objectPosition); // Set servo to current object position
      delay(SWEEP_DELAY); // Wait for servo to reach position

      long distance = readDistanceInCM(); // Measure distance to the object

			// If the object is within tracking distance, update position and reset lost counter
      if (distance <= TRACK_DISTANCE) {
        objectPosition = getObjectRelativePosition(radarServo.read(), readDistanceInCM());
        Serial.print("Tracking at angle: ");
				Serial.print(objectPosition);
				Serial.print(", Distance: ");
				Serial.println(distance);

				lostCounter = 0; // Reset lost counter if object is detected

				radarServo.write(objectPosition); // Update servo position to track the object
				break; // Exit the loop if the object is detected
      } else {
				Serial.print("Lost object at angle: ");
				Serial.print(angle);
				Serial.print(", Distance: ");
				Serial.println(distance);

				lostCounter++;
				Serial.print("Lost Counter: ");
				Serial.println(lostCounter);

				if (lostCounter >= 3) {
					mode = "scan"; // Switch to scan mode if object is lost for too long
					Serial.println("Switching to scan mode due to lost object.");
					return;
				}
			}
    }

    readSerialCommand();
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  radarServo.attach(servoPin);
}

void loop() {
  readSerialCommand();

  if (mode == "scan") {
    radarSweep();
  } else if (mode == "track") {
    trackMode();
  }
}