#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
#include "Wire.h"

MPU6050 mpu;

// MPU control/status vars
bool dmpReady = false;  // set true if DMP init was successful
uint8_t devStatus;      // return status after each device operation
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint8_t fifoBuffer[64]; // FIFO storage buffer

// Orientation/Motion vars
Quaternion q;           // [w, x, y, z]         quaternion container
VectorFloat gravity;    // [x, y, z]            gravity vector
float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container

void setup() {
    // R4 WiFi handles 400kHz well, but start Serial first
    Serial.begin(115200);
    while (!Serial); 

    Wire.begin();
    Wire.setClock(400000); 

    Serial.println(F("Initializing MPU6050..."));
    mpu.initialize();

    if (!mpu.testConnection()) {
        Serial.println(F("MPU6050 connection failed! Check wiring."));
        while(1);
    }

    // Load DMP
    devStatus = mpu.dmpInitialize();

    // --- SUPPLY YOUR OFFSETS HERE ---
    // Run the calibration once, then paste the numbers here
    mpu.setXAccelOffset(0); 
    mpu.setYAccelOffset(0);
    mpu.setZAccelOffset(0);
    mpu.setXGyroOffset(0);
    mpu.setYGyroOffset(0);
    mpu.setZGyroOffset(0);

    if (devStatus == 0) {
        mpu.CalibrateAccel(6);
        mpu.CalibrateGyro(6);
        mpu.setDMPEnabled(true);
        dmpReady = true;
        packetSize = mpu.dmpGetFIFOPacketSize();
        Serial.println(F("DMP ready! System balancing..."));
    } else {
        Serial.print(F("DMP Initialization failed (code "));
        Serial.print(devStatus);
        Serial.println(F(")"));
    }
}

void loop() {
    if (!dmpReady) return;

    // Read the latest packet from FIFO
    if (mpu.dmpGetCurrentFIFOPacket(fifoBuffer)) { 
        
        // Get Orientation Data
        mpu.dmpGetQuaternion(&q, fifoBuffer);
        mpu.dmpGetGravity(&gravity, &q);
        mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);

        // Convert Radians to Degrees
        float currentAngle = ypr[1] * 180/M_PI;

        // --- OUTPUT FOR DEBUGGING ---
        Serial.print("Angle: ");
        Serial.println(currentAngle);

        // --- MOTOR LOGIC GOES HERE ---
        // Example: if (currentAngle > 0) moveForward();
        // Use a PID library to turn 'currentAngle' into motor PWM values.
    }
}