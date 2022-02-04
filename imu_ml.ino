// I2C device class (I2Cdev) demonstration Arduino sketch for MPU6050 class
// 10/7/2011 by Jeff Rowberg <jeff@rowberg.net>
// Updates should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib
//
// Changelog:
//      2013-05-08 - added multiple output formats
//                 - added seamless Fastwire support
//      2011-10-07 - initial release

/* ============================================
I2Cdev device library code is placed under the MIT license
Copyright (c) 2011 Jeff Rowberg

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
===============================================
*/

// I2Cdev and MPU6050 must be installed as libraries, or else the .cpp/.h files
// for both classes must be in the include path of your project
#include "I2Cdev.h"
#include "MPU6050.h"

// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif

// class default I2C address is 0x68
// specific I2C addresses may be passed as a parameter here
// AD0 low = 0x68 (default for InvenSense evaluation board)
// AD0 high = 0x69
MPU6050 accelgyro;
//MPU6050 accelgyro(0x69); // <-- use for AD0 high

int16_t ax, ay, az;
int16_t gx, gy, gz;

int16_t ax_sample, ay_sample, az_sample;
int16_t gx_sample, gy_sample, gz_sample;

int16_t XBOUND = 200;
int16_t YBOUND = 200;
int16_t ZBOUND = 1000;
int16_t FORWARDPUSHBOUND = 1000;
int16_t UPWARDLIFTBOUND = -12000;
int16_t GZBOUND = 2000;

#define LED_PIN 13
bool blinkState = false;

int16_t ax_d, ay_d, az_d;
int16_t gx_d, gy_d, gz_d;

void collectSamples (int16_t &ax,
                    int16_t &ay,
                    int16_t &az,
                    int16_t &gx,
                    int16_t &gy,
                    int16_t &gz,
                    int16_t iters);

void setup() {
    // join I2C bus (I2Cdev library doesn't do this automatically)
    #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
        Wire.begin();
    #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
        Fastwire::setup(400, true);
    #endif

    // initialize serial communication
    // (38400 chosen because it works as well at 8MHz as it does at 16MHz, but
    // it's really up to you depending on your project)
    Serial.begin(38400);

    // initialize device
    Serial.println("Initializing I2C devices...");
    accelgyro.initialize();

    // verify connection
    Serial.println("Testing device connections...");
    Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");

    // use the code below to change accel/gyro offset values
    // calibrate sensor
    Serial.println("Calibrating sensor...");

    accelgyro.CalibrateAccel(7);
    accelgyro.CalibrateGyro(7);

    // read raw accel/gyro measurements from device
    accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

    // save beginning values as "default"
    ax_d = ax;
    ay_d = ay;
    az_d = az;
    gx_d = gx;
    gy_d = gy;
    gz_d = gz;

    Serial.println("Done!");

    // configure Arduino LED pin for output
    pinMode(LED_PIN, OUTPUT);
}

void loop() {
    // read raw accel/gyro measurements from device
    accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    collectSamples(ax_sample,
                   ay_sample,
                   az_sample,
                   gx_sample,
                   gy_sample,
                   gz_sample,
                   100);
//    Serial.print(ax_sample); Serial.print("\t");
//    Serial.print(ax); Serial.print("\t");
//    Serial.println(ax_sample-ax);
//    Serial.print(ay_sample); Serial.print("\t");
//    Serial.print(ay); Serial.print("\t");
//    Serial.println(ay_sample-ay);
//    Serial.print(gz_sample); Serial.print("\t");
//    Serial.print(gz); Serial.print("\t");
//    Serial.println(gz_sample-gz);

    if ((-1 * XBOUND < (gx_sample - gx)) && ((gx_sample - gx) < XBOUND) &&
        (-1 * YBOUND < (gy_sample - gy)) && ((gy_sample - gy) < YBOUND)) {
      Serial.println("Idle!");
    } 
    else {
      Serial.println("Shaking!...");
    }
    else if ( (az_sample - az) > UPWARDLIFTBOUND ){
      Serial.println("Upward lift!");
    }
    else if ((-1 * GZBOUND < (gz_sample - gz)) && ((gz_sample - gz) < GZBOUND)){
      Serial.println("Twirl!");
    }
    else if ( (ay_sample - ay) > FORWARDPUSHBOUND ){
      Serial.println("Forward Push!");
    }
    else {
      Serial.println("Not sure...");
    }
//    else {
//      Serial.println("Shaking!...");
//    }

    // these methods (and a few others) are also available
    //accelgyro.getAcceleration(&ax, &ay, &az);
    //accelgyro.getRotation(&gx, &gy, &gz);

    // display tab-separated accel/gyro x/y/z values
    Serial.print("a/g:\t");
    Serial.print(ax); Serial.print("\t");
    Serial.print(ay); Serial.print("\t");
    Serial.println(az); //Serial.print("\t");
    Serial.print(gx); Serial.print("\t");
    Serial.print(gy); Serial.print("\t");
    Serial.println(gz);

}

void collectSamples(int16_t &ax,
                    int16_t &ay,
                    int16_t &az,
                    int16_t &gx,
                    int16_t &gy,
                    int16_t &gz,
                    int16_t iters) {
  int16_t ax_b = 0;
  int16_t ay_b = 0;
  int16_t az_b = 0;
  int16_t gx_b = 0;
  int16_t gy_b = 0;
  int16_t gz_b = 0;

  int16_t ax_s = 0;
  int16_t ay_s = 0;
  int16_t az_s = 0;
  int16_t gx_s = 0;
  int16_t gy_s = 0;
  int16_t gz_s = 0;

  for (int i = 0; i < iters; i++) {
    accelgyro.getMotion6(&ax_b, &ay_b, &az_b, &gx_b, &gy_b, &gz_b);
    ax_s += ax_b;
    ay_s += ay_b;
    az_s += az_b;
    gx_s += gx_b;
    gy_s += gy_b;
    gz_s += gz_b;
  }

  ax_s = ax_s / iters;
  ay_s = ay_s / iters;
  az_s = az_s / iters;
  gx_s = gx_s / iters;
  gy_s = gy_s / iters;
  gz_s = gz_s / iters;

  ax = ax_s;
  ay = ay_s;
  az = az_s;
  gx = gx_s;
  gy = gy_s;
  gz = gz_s;
}
