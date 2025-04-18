#include <Servo.h>
#include <NewPing.h>
#include <Wire.h>
#include <SoftwareSerial.h>

// Motor control pins
const int enablePinA = 5;
const int LeftMotorForward = 10;
const int LeftMotorBackward = 11;
const int RightMotorForward = 9;
const int RightMotorBackward = 8;
const int enablePinB = 6;

// Ultrasonic sensor
#define trig_pin A2
#define echo_pin A3
#define maximum_distance 200

// Bluetooth
SoftwareSerial BTSerial(12, 13); // RX, TX

// Objects
NewPing sonar(trig_pin, echo_pin, maximum_distance);
Servo servo_motor;

boolean goesForward = false;
int distance = 100;
const int safeDistance = 50;
const int turnDelay = 400;
const int normalSpeed = 180;  // 59% speed
const int reducedSpeed = 100; // 40% speed (255 * 0.4)

void setup() {
    pinMode(enablePinA, OUTPUT);
    pinMode(RightMotorForward, OUTPUT);
    pinMode(LeftMotorForward, OUTPUT);
    pinMode(LeftMotorBackward, OUTPUT);
    pinMode(RightMotorBackward, OUTPUT);
    pinMode(enablePinB, OUTPUT);

    servo_motor.attach(3);

    BTSerial.begin(9600);
    servo_motor.write(90); // Center position
    delay(2000);
}

void updateDistance() {
    distance = readPing();

    // // Send data via Bluetooth
    // BTSerial.print("Distance: ");
    // BTSerial.print(distance);
    // BTSerial.println(" cm");
}

void loop() {
    int distanceRight = 0;
    int distanceLeft = 0;

    updateDistance();

    if (distance < 30) { // Full obstacle avoidance
        moveStop();
        delay(100);
        moveBackward();
        delay(300);
        moveStop();
        delay(300);
        
        distanceRight = lookRight();
        delay(400);
        distanceLeft = lookLeft();
        delay(400);

        if (distanceRight >= distanceLeft && distanceRight >= safeDistance) {
            String data = ", " + String(distanceLeft) + ", " + 
                      String(distanceRight) + ", " +
                      String(normalSpeed) + ", " + "RIGHT";
            BTSerial.println(data);
            turnLeft();
            moveForward(normalSpeed);
        } 
        else if (distanceLeft >= safeDistance) {
            String data = ", " + String(distanceLeft) + ", " + 
                      String(distanceRight) + ", " +
                      String(normalSpeed) + ", " + "LEFT";
            BTSerial.println(data);
            turnRight();
            moveForward(normalSpeed);
        } 
        else {
            String data = ", " + String(distanceLeft) + ", " + 
                      String(distanceRight) + ", " +
                      String(normalSpeed) + ", " + "ROTATE180";
            BTSerial.println(data);
            rotate180();
            moveForward(normalSpeed);
        }
    } 
    else if (distance < 50) { // Reduce speed to 40%
        String data = ", " + String(distanceLeft) + ", " + 
                      String(distanceRight) + ", " +
                      String(reducedSpeed) + ", " + "STRAIGHT";
        BTSerial.println(data);
        moveForward(reducedSpeed);
    }
    else { // Normal operation
        String data = ", " + String(distanceLeft) + ", " + 
                      String(distanceRight) + ", " +
                      String(normalSpeed) + ", " + "STRAIGHT";
        BTSerial.println(data);
        moveForward(normalSpeed);
    }
}

// Modified moveForward function with speed control
void moveForward(int speed) {
    if (!goesForward) {
        goesForward = true;
        digitalWrite(LeftMotorForward, HIGH);
        digitalWrite(RightMotorForward, HIGH);
        digitalWrite(LeftMotorBackward, LOW);
        digitalWrite(RightMotorBackward, LOW);
    }
    analogWrite(enablePinA, speed);
    analogWrite(enablePinB, speed);
}

// Keep other functions (updateDistance, lookRight, lookLeft, moveStop, etc.) 
// the same as in your original code, except for the following changes:

void moveBackward() {
    goesForward = false;
    digitalWrite(LeftMotorBackward, HIGH);
    digitalWrite(RightMotorBackward, HIGH);
    digitalWrite(LeftMotorForward, LOW);
    digitalWrite(RightMotorForward, LOW);
    analogWrite(enablePinA, 180); // Backward speed remains unchanged
    analogWrite(enablePinB, 180);
}

// Rest of your functions remain unchanged below this line
// (updateDistance, lookRight, lookLeft, readPing, moveStop, 
// turnRight, turnLeft, rotate180)...

// Modified servo angles for wider viewing
int lookRight() {
    servo_motor.write(20);  // Increased right view (was 30)
    delay(500);
    int distance = readPing();
    delay(100);
    servo_motor.write(90);  // Return to center
    return distance;
}
int readPing() {
    delay(70);
    int cm = sonar.ping_cm();
    return (cm == 0) ? 250 : cm;
}

void moveStop() {
    digitalWrite(RightMotorForward, LOW);
    digitalWrite(LeftMotorForward, LOW);
    digitalWrite(RightMotorBackward, LOW);
    digitalWrite(LeftMotorBackward, LOW);
}

int lookLeft() {
    servo_motor.write(160); // Increased left view (was 190)
    delay(500);
    int distance = readPing();
    delay(100);
    servo_motor.write(90);  // Return to center
    return distance;
}

// Rest of your functions remain the same with these improvements:

void turnRight() {
    digitalWrite(LeftMotorForward, HIGH);
    digitalWrite(RightMotorBackward, HIGH);
    analogWrite(enablePinA, 150);
    analogWrite(enablePinB, 150);
    delay(turnDelay);
    moveStop();
}

void turnLeft() {
    digitalWrite(RightMotorForward, HIGH);
    digitalWrite(LeftMotorBackward, HIGH);
    analogWrite(enablePinA, 150);
    analogWrite(enablePinB, 150);
    delay(turnDelay);
    moveStop();
}

void rotate180() {
    digitalWrite(LeftMotorForward, HIGH);
    digitalWrite(RightMotorBackward, HIGH);
    analogWrite(enablePinA, 150);
    analogWrite(enablePinB, 150);
    delay(turnDelay * 2); // 180 degree rotation
    moveStop();
}

// Keep other functions (updateDistance, readPing, moveStop, etc.) the same as your original