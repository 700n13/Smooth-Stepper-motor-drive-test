#include <A4988.h>
#include <math.h>

#define MOTOR_STEPS 200
#define DIR 4
#define STEP 3
#define MS1 5
#define MS2 6
#define MS3 7
A4988 stepper(MOTOR_STEPS, DIR, STEP, MS1, MS2, MS3);

int rpm_max = 200;  //maximum angular velocity in rpm 
float omega_max = rpm_max*0.105 ;//maximum angular velocity in rad/s
float fadein = 3;//time stamp when fadein complete t1
float height = 5;//height(cm) lift want to execute float theta = 20;
float theta = height*15.7;
float t_default = theta/omega_max;
float end = (theta/omega_max) + 0.67*fadein ;//time stamp when operation is complete t3
float fadeout = end-fadein ;// time stamp when start fadeout t2
float t0 = 0;
float delta = 0;
float omega = 0; // current omega
float current_theta = 0;
float total_theta = 0;
bool adjust = false ;

float omega_in(float omega_max,float fadein,float t){ //output omega (in rpm)
  float omega = (-omega_max/(pow(fadein, 2)))*pow(t-fadein, 2) + omega_max ; 
  return omega;
}

float omega_out(float omega_max,float fadeout,float t){
  float omega = (-omega_max/pow(end-fadeout, 2))*pow(t-fadeout, 2) + omega_max ;
  return omega;
}



void setup() {
  Serial.begin(38400) ; 
  Serial.print("max_rpm = ");
  Serial.println(omega_max);
  Serial.print("default t = ");
  Serial.println(t_default);
  Serial.print("fadein = ");
  Serial.println(fadein);
  Serial.println("fadeout = ");
  Serial.println(fadeout);
  Serial.println("end t = ");
  Serial.println(end);
}

void loop() {
  // put your main code here, to run repeatedly:
  float t = millis();
  t = t / 1000;
  if ((0 <= t) && (t< fadein)) {
    omega = omega_in(omega_max,fadein,t);
  } else if ((fadein <= t) && (t < fadeout)){
    omega = omega_max;
  } else if ((fadeout <= t) && (t < end)){
    omega =  omega_out(omega_max,fadeout,t);
  } else{
    omega = 0;
  }
  delta = t-t0 ;
  float omega_rpm = omega*9.55;
  stepper.begin(omega_rpm,4);
  current_theta = delta*omega*57.3; //theta rotated in delta t (in degrees)
  stepper.rotate(current_theta);
  total_theta = total_theta + current_theta;
  if (t<= end){
   // Serial.print(t);
    //Serial.print(" ");
    //Serial.print(delta);
    //Serial.println(omega);
    //Serial.println(omega_rpm);
    Serial.print(t);
    Serial.print(" ");
    Serial.println(total_theta*0.0175);
  }
 else{
    if ((total_theta != theta) && (adjust == false)) {
      stepper.begin(omega_max,4);
      stepper.rotate(theta - (total_theta*0.0175));
      adjust = true;
      
    }
  }
  t0 = t ;
  delay(5);
}




