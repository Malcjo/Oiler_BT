#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

Adafruit_MPU6050 mpu;

// Variables for Kalman filter
float accelX, accelY, accelZ;
float biasX, biasY, biasZ;
float rateX, rateY, rateZ;
float P[3][2][2] = {{{1, 0}, {0, 1}}, {{1, 0}, {0, 1}}, {{1, 0}, {0, 1}}}; // Error covariance matrices for X, Y, Z
float R = 0.03;     // Measurement noise
float Q_angle = 0.001; // Process noise for angle
float Q_gyro = 0.003;  // Process noise for gyro bias
float dt;              // Time difference
unsigned long lastTime;

void setup(void) {
  Serial.begin(115200);
  while (!Serial)
    delay(10); // will pause Zero, Leonardo, etc until serial console opens

  Serial.println("Adafruit MPU6050 test!");

  // Try to initialize the MPU6050
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");

  // Set accelerometer and gyro ranges
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);

  lastTime = millis();
}

void loop() {
  // Get new sensor events
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  // Calculate dt (time difference)
  unsigned long currentTime = millis();
  dt = (currentTime - lastTime) / 1000.0;
  lastTime = currentTime;

  // Apply Kalman filter to accelerometer X, Y, Z values
  kalmanFilter(a.acceleration.x, g.gyro.x, &accelX, &biasX, 0);  // 0: for X-axis
  kalmanFilter(a.acceleration.y, g.gyro.y, &accelY, &biasY, 1);  // 1: for Y-axis
  kalmanFilter(a.acceleration.z, g.gyro.z, &accelZ, &biasZ, 2);  // 2: for Z-axis

  double x = abs((double)((a.acceleration.x) -0.35));
  double y= abs((double)((a.acceleration.y) -0.25));
  double z= abs((double)(((a.acceleration.z)-2)));

  // Print out the filtered values for Serial Plotter (acceleration in X, Y, Z axes)
  Serial.print(x);
  Serial.print(" ");
  Serial.print(y);
  Serial.print(" ");
  Serial.print(z);
  Serial.print(" ");
  Serial.print((x+y+z)/3);
  Serial.print(" ");
  Serial.println(6);

  delay(50);  // Adjust delay for better plotting resolution
}

void kalmanFilter(float accel, float gyroRate, float *angle, float *bias, int axis) {
  // Prediction
  *angle += (gyroRate - *bias) * dt;
  P[axis][0][0] += dt * (dt * P[axis][1][1] - P[axis][0][1] - P[axis][1][0] + Q_angle);
  P[axis][0][1] -= dt * P[axis][1][1];
  P[axis][1][0] -= dt * P[axis][1][1];
  P[axis][1][1] += Q_gyro * dt;

  // Correction
  float S = P[axis][0][0] + R; // Estimate error
  float K[2];  // Kalman gain
  K[0] = P[axis][0][0] / S;
  K[1] = P[axis][1][0] / S;

  float y = accel - *angle;  // Angle difference (we treat acceleration as angle for simplicity)
  *angle += K[0] * y;
  *bias += K[1] * y;

  // Update error covariance matrix
  P[axis][0][0] -= K[0] * P[axis][0][0];
  P[axis][0][1] -= K[0] * P[axis][0][1];
  P[axis][1][0] -= K[1] * P[axis][0][0];
  P[axis][1][1] -= K[1] * P[axis][0][1];
}
