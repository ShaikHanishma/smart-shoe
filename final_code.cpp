#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

// Define pins
#define buzzer 8
#define trigPin 9
#define echoPin 10

// Thresholds
float imuThreshold = 2.5;       // Tilt/pothole sensitivity
int ultraFarMin = 30;           // Far distance start (cm)
int ultraFarMax = 60;           // Far distance end (cm)
int ultraMedMin = 15;           // Medium distance start
int ultraMedMax = 30;           // Medium distance end
int ultraCloseMin = 2;          // Close distance start
int ultraCloseMax = 15;         // Close distance end

Adafruit_MPU6050 mpu;

void setup() {
  Serial.begin(9600);
  while(!Serial);

  pinMode(buzzer, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  if(!mpu.begin()) {
    Serial.println("Failed to find MPU6050 sensor!");
    while(1) delay(10);
  }

  Serial.println("Smart Shoe Prototype Ready!");
  delay(1000); // stabilize sensors
}

void loop() {
  // ---------- IMU readings ----------
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  float accelX = a.acceleration.x;
  float accelY = a.acceleration.y;

  bool imuAlert = (abs(accelX) > imuThreshold) || (abs(accelY) > imuThreshold);

  // ---------- Ultrasonic readings ----------
  long duration;
  int distance;

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2; // cm

  bool ultraAlert = (distance > ultraCloseMin && distance < ultraFarMax);

  // ---------- Buzzer logic ----------
  if (imuAlert) {
    digitalWrite(buzzer, HIGH); // Continuous if IMU detects tilt/pothole
  } 
  else if (ultraAlert) {
    if (distance >= ultraFarMin && distance <= ultraFarMax) {
      // FAR → slow beep
      digitalWrite(buzzer, HIGH);
      delay(200);
      digitalWrite(buzzer, LOW);
      delay(800);
    } 
    else if (distance >= ultraMedMin && distance < ultraMedMax) {
      // MEDIUM → faster beep
      digitalWrite(buzzer, HIGH);
      delay(200);
      digitalWrite(buzzer, LOW);
      delay(300);
    } 
    else if (distance >= ultraCloseMin && distance < ultraCloseMax) {
      // CLOSE → continuous beep
      digitalWrite(buzzer, HIGH);
    }
  } 
  else {
    digitalWrite(buzzer, LOW); // No alert
  }

  // ---------- Debug output ----------
  Serial.print("IMU Alert: "); Serial.print(imuAlert);
  Serial.print(" | Ultrasonic: "); Serial.print(distance); Serial.print("cm");
  Serial.print(" | Ultra Alert: "); Serial.println(ultraAlert);

  delay(1000); // fast updates
}