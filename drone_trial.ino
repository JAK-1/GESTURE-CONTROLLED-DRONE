#include <SPI.h>      //SPI library for communicate with the nRF24L01+
#include "RF24.h"     //The main library of the nRF24L01+
#include <Servo.h>
#define LED 13
int data[3];
RF24 radio(9,10);
//Create a pipe addresses for the communicate
const uint64_t pipe = 0xE8E8F0F0E1LL;
Servo AIL;
Servo ELE;
Servo THR;
Servo RUD;
Servo AUX;

int rec_ail;
int rec_ele;
int rec_thr;
int rec_rud;
int rec_aux;

int max_rec;
int min_rec;
int mid_rec;
int ref_aux;
int ele, ail, thr, rud;
int x,y,z;

String Axes = ""; 
int cal_height;

int XMIT_THRESH = 70;
int WRBL_THRESH = 50;

void setup() {

  Serial.begin(9600); //debugging Serial port baudrate
  //Serial1.begin(38400); //BT transmission Serial port baudrate
  radio.begin();                    //Start the nRF24 communicate            
  radio.openReadingPipe(1, pipe);   //Sets the address of the transmitter to which the program will receive data.
  radio.startListening();  
  pinMode(22, OUTPUT); //Flight controller - Aileron Pin
  pinMode(24, OUTPUT); //Flight controller - Elevator Pin
  pinMode(26, OUTPUT); //Flight controller - Throttle Pin
  pinMode(28, OUTPUT); //Flight controller - Rudder Pin
  pinMode(13, OUTPUT); //LED pin
  AIL.attach(22); //Servo settings
  ELE.attach(24);
  THR.attach(26);
  RUD.attach(28);
  rec_ail = pulseIn(3, HIGH, 20000); // Take 10 samples of receiver readings
  for (int i = 0; i < 10; i++) {
    mid_rec += rec_ail;
  }
  ref_aux = pulseIn(7, HIGH, 20000); //calculate reference aux value
  mid_rec = mid_rec / 10; //calculate average receiver's mid-stick values
  min_rec = mid_rec - 450;//calculate receiver's min-stick values
  max_rec = mid_rec + 450;//calculate receiver's max-stick values

}


void loop() {
  // read pwm values from the receiver
  rec_ail = pulseIn(3, HIGH, 20000);
  rec_ele = pulseIn(4, HIGH, 20000);
  rec_thr = pulseIn(5, HIGH, 20000);
  rec_rud = pulseIn(6, HIGH, 20000);
  rec_aux = pulseIn(7, HIGH, 20000);

  AIL.writeMicroseconds(ail);
  ELE.writeMicroseconds(ele);
  THR.writeMicroseconds(thr);
  RUD.writeMicroseconds(rud);
  
  print_transmit();

}

void read_wearable(){
  //radio.print(1);
  while (radio.available())
  {
    radio.read(data, sizeof(data)); // receive a character from BT port
    x = data[0];
    y = data[1];
    z = data[2];
  }

  if(x > WRBL_THRESH){//if the wearable is tilted to the right, add correlated value to Aileron's default PWM signal.
    ail = mid_rec + x;
    thr = mid_rec;
  }

  else if (x < -WRBL_THRESH){//if the wearable is tilted to the left, subtract correlated value to Aileron's default PWM signal.
    x = abs(x);
    ail = mid_rec - x;
    thr = mid_rec;
  }

  else if( x > -WRBL_THRESH && x < WRBL_THRESH){//else, Aileron is equals to defualt Aileron PWM signal.
    ail = mid_rec;
  }

  if(y > WRBL_THRESH){//if the wearable is tilted to the front, add correlated value to Elevators's default PWM signal.
    ele = mid_rec + y;
    thr = mid_rec;
  }
  
  else if(y < -WRBL_THRESH){//if the wearable is tilted to the rear, subtract correlated value to Aileron's default PWM signal.
    y = abs(y);
    ele = mid_rec - y;
    thr = mid_rec;
  }
  else if(y > -WRBL_THRESH && y < WRBL_THRESH){//else, Elevator is equals to defualt Elevator PWM signal.
    ele = mid_rec;
  }

  if( (x > -WRBL_THRESH && x < WRBL_THRESH) || (y > -WRBL_THRESH && y < WRBL_THRESH ) ){ //if the wearable is not tilted to either X or Y axis, get correlated Throttle value.
    calc_thr();
  } 

}

void print_axis(){ //function to print received values from the wearable
  Serial.print(x);
  Serial.print('\t');
  Serial.print(y);
  Serial.print('\t');
  Serial.println(z); 
}

void print_transmit(){ //function to print calculated PWM signals to be written over the flight controller
  Serial.print(ail);
  Serial.print('\t');
  Serial.print(ele);
  Serial.print('\t');
  Serial.println(thr);
}

void calc_thr(){
  if(z > 5 && z < 45){ //Height is directly correlated to throttle (This is also used for landing)
    thr = map(z, 5 , 45, min_rec, mid_rec);
  }
  else if( z > 45 && z < 80){ //This height is where the user flies the drone
    thr = mid_rec;
  }
  else if( z > 80 ){//When hand's height is more than 80cm,tell the drone to take-off.
    thr = mid_rec + 100;
  }
}
