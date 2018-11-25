#include <SPI.h>        //SPI library for communicate with the nRF24L01+
#include "RF24.h"       //The main library of the nRF24L01+
#include "Wire.h"       //For communicate
#include "I2Cdev.h"     //For communicate with MPU6050
#include "MPU6050.h"    //The main library of the MPU6050
#define triggerZ 5 //Ultrasonic Trigger Pin                                              // Ultrasonic trigger pin is at 9
#define echoZ 6 //Ultrasonic Echo Pin  
unsigned long prevTime= millis();
//Define the object to access and cotrol the Gyro and Accelerometer (We don't use the Gyro data)
MPU6050 mpu;
int16_t x, y, az;
int16_t gx, gy, gz;
//Define packet for the direction (X axis and Y axis) and ultrasonic
int data[3];
//Define object from RF24 library - 9 and 10 are a digital pin numbers to which signals CE and CSN are connected.
RF24 radio(9,10);
//Create a pipe addresses for the communicate                                    
const uint64_t pipe = 0xE8E8F0F0E1LL;
void setup(void){
  Serial.begin(9600);
  Wire.begin();
  pinMode(triggerZ, OUTPUT);                                                            // defining trigger is output
  pinMode(echoZ, INPUT);
  mpu.initialize();              //Initialize the MPU object
  radio.begin();                 //Start the nRF24 communicate     
  radio.openWritingPipe(pipe);   //Sets the address of the receiver to which the program will send data.
}
                                                                         //read ultrasonic's Z reading in cm, capped in 200cm.
int z()    {long duration, distance;
    digitalWrite(triggerZ, LOW);  
    delayMicroseconds(2); 
    digitalWrite(triggerZ, HIGH);
    delayMicroseconds(10); 
    digitalWrite(triggerZ, LOW);
    duration = pulseIn(echoZ, HIGH, 15000);
    distance = (duration / 2) / 29.1;
    if (duration >11640){
      distance = 200;
    }
    prevTime = millis();
    return (distance);}

void loop(void){
  mpu.getMotion6(&x, &y, &az, &gx, &gy, &gz);
  data[0] = map(x, 1000, 2000, 1000, 2000); //Send X axis data
  Serial.print("data[0]");
  Serial.println(data[0]);
  data[1] = map(y, 1000, 2000, 1000, 2000);  //Send Y axis data
  Serial.print("data[1]");
  Serial.println(data[1]);
  data[2] = map(z, 0, 200, 0, 200); //ultrasonic sensor
  Serial.print("data[2]");
  Serial.println(z());
  radio.write(data, sizeof(data));
 }
