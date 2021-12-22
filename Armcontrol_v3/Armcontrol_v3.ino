//====================== Code Information ==========================================================================================================================================//
//
//  Creator:  Fabian Noll
//  Date of Creation: 07.12.2021
//  Purpous: The "Operating System" of the Robot Arm "Luna One". It is controlled via an selfmade App. It is the second Version with improved controls.
//           It is not necessary anymore to control each servo individual. Now you just control the Head and the System calculates the rest.
//
//  App: Robot Arm Control Center V2 available at my github repository at: https://github.com/FabianRolfMatthiasNoll/RobotArm-LunaOne
//  
//  Changelog:  13.12.2021  - Added Bluetooth Availability
//                          - Added Basic Bluetooth Controll
//
//              14.12.2021  - Completed Bluetooth controll
//                          
//              16.12.2021  - completely reworked controll => not controlling single Servos anymore
//                          - completely reworked the Bluetooth App and control
//                          - Refined Code Structure
//                          - Added and reworked new Bluetooth Controls
//==================================================================================================================================================================================//

//======== Libraries================================================================================================================================================================

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <SoftwareSerial.h> 

SoftwareSerial BlueArm(10, 9); // RX | TX //To keep the standard Serial Port open Software Serial is used

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

//===================================================================================================================================================================================

// Defining some basic and fixed values

#define SERVOBASE 2       //Taster 1
#define SERVOLINK1 3      //Taster 2
#define SERVOLINK2 4      //Taster 3
#define SERVOLINK3 5      //Taster 4
#define SERVOHEAD 6       //Taster 5

#define BUTTONUP 7        //Taster 6
#define BUTTONDOWN 8      //Taster 7

#define PWM_SERVOBASE 0   //"Adress" of the Base Servo
#define PWM_SERVOLINK1 1  //"Adress" of the first Servo
#define PWM_SERVOLINK2 2  //"Adress" of the second Servo
#define PWM_SERVOLINK3 3  //"Adress" of the third Servo
#define PWM_SERVOHEAD 4   //"Adress" of the Head Servo

//===================================================================================================================================================================================

//Defining the standard values for the servo Position. They are defined in Degrees and will be later converted in their numerical Value for the pwm controller

double SBASE_V = 90;
double SLINK1_V = 140;
double SLINK2_V = 90;
double SLINK3_V = 140;
double SHEAD_V = 96;

int command = 0;              //Value to save the command received by the bluetooth control

//======Variables fo Inverse Kinematic=============================================================================================================================================================================

#define PI 3.1415926535897932384626433832795

int f = 67;                   //Height of the Robot Base
int a = 87;                   //Lenght of the first segment
int b = 87;                   //Lenght of the second Segment
int e = 140;                  //Lenght of the Robot Head
double c = 0;                 //Lenght of diameter of the triangle
double h1 = 0;                //Height of the triangle
double h2 =0;                 //Height Difference from h and f
double e2 = 0;                //Lenght of Head after removing distance

double alpha = 0;             //Degree of the first Servo
double beta = 81.48;          //Degree of the second Servo // 81.48 is the startUp Value
double gamma = 0;             //Degree of the third Servo

double h = 180.00;            //Height of the Head above Ground // 180 ist the startUp Value
int d = 135;                  //Distance of the Head from Base Center // 135 ist the startUp Value

double old_beta = 0;          //Later needed to save the old value of beta
double difference_beta = 0;   //Later needed to move other servos according to the change of beta
double difference_beta2 = 0;  //Later needed to move other servos according to the change of beta

//===================================================================================================================================================================================

void setup() {
   
  Serial.begin(9600);
  BlueArm.begin(9600);
  
  pinMode(SERVOBASE, INPUT);
  pinMode(SERVOLINK1, INPUT);
  pinMode(SERVOLINK2, INPUT);
  pinMode(SERVOLINK3, INPUT);
  pinMode(SERVOHEAD, INPUT);
  pinMode(BUTTONUP, INPUT);
  pinMode(BUTTONDOWN, INPUT);
  
  pwm.begin();
  pwm.setPWMFreq(50);

  calculate_Servos(h,f,e,d,a);    //calculating all necessary Positions at start up with default values
}

void loop() {

  command = receive_Command();    //Function to receive 
  
  execute_command(command);
  
  check_Manual_Control();
  
  command = 0;
}

void move_Servos(double SBASE_V, double SLINK1_V, double SLINK2_V, double SLINK3_V, double SHEAD_V){

  int SBASE_VT = mapf(SBASE_V, 0, 180, 70, 530);
  int SLINK1_VT = mapf(SLINK1_V, 0, 180, 70, 530);
  int SLINK2_VT = mapf(SLINK2_V - 40, 0, 180, 70, 530);
  int SLINK3_VT = mapf(SLINK3_V - 68, 0, 180, 70, 530);
  int SHEAD_VT = mapf(SHEAD_V, 0, 180, 70, 530);

  pwm.setPWM(PWM_SERVOBASE, 0, SBASE_VT);
  pwm.setPWM(PWM_SERVOLINK1, 0, SLINK1_VT);
  pwm.setPWM(PWM_SERVOLINK2, 0, SLINK2_VT);
  pwm.setPWM(PWM_SERVOLINK3, 0, SLINK3_VT);
  pwm.setPWM(PWM_SERVOHEAD, 0, SHEAD_VT);
  
}

int receive_Command(){
  if (BlueArm.available()){ 
    int received_command = BlueArm.read();
    return received_command;
  } 
}

void check_Manual_Control(){

  if(digitalRead(SERVOLINK1) == HIGH){
    if(digitalRead(BUTTONUP) == HIGH){
      if (h < 205){
        h = h + 2;
      }
      calculate_Servos(h,f,e,d,a);
    } else if (digitalRead(BUTTONDOWN) == HIGH){
      if (h > 130){
         h = h - 2; 
      }
      calculate_Servos(h,f,e,d,a);
    }
  }

  if(digitalRead(SERVOLINK2) == HIGH){
    if(digitalRead(BUTTONUP) == HIGH){
      if (d < 205){
        SLINK3_V = SLINK3_V - 0.5;
        SLINK1_V = SLINK1_V + 0.5;
      }
      calculate_Servos(h,f,e,d,a);
    } else if (digitalRead(BUTTONDOWN) == HIGH){
      if (d > 130){
         SLINK3_V = SLINK3_V + 0.5;
         SLINK1_V = SLINK1_V - 0.5; 
      }
      calculate_Servos(h,f,e,d,a);
    }
  }

  if(digitalRead(SERVOBASE) == HIGH){
    if(digitalRead(BUTTONUP) == HIGH){
      if(SBASE_V < 180){
        SBASE_V = SBASE_V + 0.5;
        move_Servos(SBASE_V,SLINK1_V,SLINK2_V,SLINK3_V,SHEAD_V);
      }
    } else if (digitalRead(BUTTONDOWN) == HIGH){
      if(SBASE_V > 0){
        SBASE_V = SBASE_V - 0.5;
        move_Servos(SBASE_V,SLINK1_V,SLINK2_V,SLINK3_V,SHEAD_V);
      }
    }
  }

  if(digitalRead(SERVOHEAD) == HIGH){
    if(digitalRead(BUTTONUP) == HIGH){
      if(SHEAD_V < 120){
        SHEAD_V = SHEAD_V + 0.5;
        move_Servos(SBASE_V,SLINK1_V,SLINK2_V,SLINK3_V,SHEAD_V);
       }
    } else if (digitalRead(BUTTONDOWN) == HIGH){
      if(SHEAD_V > 30){
        SHEAD_V = SHEAD_V - 0.5;
        move_Servos(SBASE_V,SLINK1_V,SLINK2_V,SLINK3_V,SHEAD_V);
      }
    }
  }

  if(digitalRead(SERVOLINK3) == HIGH){
    if(digitalRead(BUTTONUP) == HIGH){
      if(SLINK3_V > 0){
        SLINK3_V = SLINK3_V - 0.5;
        move_Servos(SBASE_V,SLINK1_V,SLINK2_V,SLINK3_V,SHEAD_V);
       }
    } else if (digitalRead(BUTTONDOWN) == HIGH){
      if(SLINK3_V < 240){
        SLINK3_V = SLINK3_V + 0.5;
        move_Servos(SBASE_V,SLINK1_V,SLINK2_V,SLINK3_V,SHEAD_V);
      }
    }
  }
}     

void calculate_gamma(int alpha){
  gamma = alpha;
}

void calculate_h1(int alpha, int a){
  h1 = sin((alpha * (PI / 180))) * a;
}

void calculate_c(int a, int h1){
  c = (2 * sqrt( ( pow(a,2) - pow(h1,2) ) ) );
}

void calculate_beta(double c, double h1){
  beta = (atan( ((c / 2) / h1) ) / (PI / 180)) * 2;
}

void calculate_h2(double h, int f){
  h2 = h - f;
}

void calculate_e2(int e, int d){
  e2 = e - d;
}

void calculate_c_from_h2(double e2, double h2){
  c = sqrt(sq(e2) + sq(h2));
}

void calculate_alpha_gamma(double c, double a){
  alpha = acos(c /(2*a)) / (PI / 180);
}

void calculate_Servos(double h, double f, double e, double d, double a){
  
  old_beta = beta;
  
  calculate_h2(h,f);
  calculate_e2(e,d);
  calculate_c_from_h2(e2,h2);
  calculate_alpha_gamma(c,a);
  calculate_h1(alpha,a);
  calculate_beta(c,h1);

  difference_beta = (beta - old_beta) / 2;
  difference_beta2 = (beta - old_beta) / 1.6;
  
  if (h >= 130){
    SLINK2_V = beta;
    SLINK3_V = SLINK3_V - difference_beta;
    SLINK1_V = SLINK1_V - difference_beta2;
  }
  move_Servos(SBASE_V,SLINK1_V,SLINK2_V,SLINK3_V,SHEAD_V);
}

void execute_command(int command){

  //Because of the way my App works 
  //=====Moving Base to the left==================================================================================================================================

  if(command == 1){
    while(!(command == 11)){
      command = receive_Command();
      if(SBASE_V > 0){
        SBASE_V = SBASE_V - 0.5;
        move_Servos(SBASE_V,SLINK1_V,SLINK2_V,SLINK3_V,SHEAD_V);
      }
    }
  }

  //=====Moving Base to the right==================================================================================================================================

  if(command == 2){
    while(!(command == 12)){
      command = receive_Command();
      if(SBASE_V < 180){
        SBASE_V = SBASE_V + 0.5;
        move_Servos(SBASE_V,SLINK1_V,SLINK2_V,SLINK3_V,SHEAD_V);
      }
    }
  }

  //=====Opening the Head==================================================================================================================================

  if(command == 3){
    while(!(command == 13)){
      command = receive_Command();
      if(SHEAD_V < 120){
        SHEAD_V = SHEAD_V + 0.5;
        move_Servos(SBASE_V,SLINK1_V,SLINK2_V,SLINK3_V,SHEAD_V);
      }
    }
  }

  //=====Closing the Head==================================================================================================================================


  if(command == 4){
    while(!(command == 14)){
      command = receive_Command();
      if(SHEAD_V > 30){
        SHEAD_V = SHEAD_V - 0.5;
        move_Servos(SBASE_V,SLINK1_V,SLINK2_V,SLINK3_V,SHEAD_V);
      }
    }
  }

  //=====Moving the Head up==================================================================================================================================

  if(command == 5){
    while(!(command == 15)){
      command = receive_Command();
      if(SLINK3_V < 240){
        SLINK3_V = SLINK3_V + 0.5;
        move_Servos(SBASE_V,SLINK1_V,SLINK2_V,SLINK3_V,SHEAD_V);
      }
    }
  }

  //=====Moving the Head down==================================================================================================================================

  if(command == 6){
    while(!(command == 16)){
      command = receive_Command();
      if(SLINK3_V > 0){
        SLINK3_V = SLINK3_V - 0.5;
        move_Servos(SBASE_V,SLINK1_V,SLINK2_V,SLINK3_V,SHEAD_V);
      }
    }
  }
      
  //=====Moving the Body up==================================================================================================================================

  if(command == 7){
    while(!(command == 17)){
      command = receive_Command();
      if (h < 220){
        h = h + 0.5;
      }
      calculate_Servos(h,f,e,d,a);
    }
  }

  //=====Moving the Body down==================================================================================================================================

  if(command == 8){
    while(!(command == 18)){
      command = receive_Command();
      if (h > 130){
         h = h - 0.5; 
      }
      calculate_Servos(h,f,e,d,a);
    }
  }

  //=====Moving the Body forward==================================================================================================================================

  if(command == 9){
    while(!(command == 19)){
      command = receive_Command();
      if (d > 130){
         SLINK3_V = SLINK3_V + 0.5;
         SLINK1_V = SLINK1_V - 0.5; 
      }
      calculate_Servos(h,f,e,d,a);
    }
  }
      
  //=====Moving the Body backwards==================================================================================================================================

  if(command == 10){
    while(!(command == 20)){
      command = receive_Command();
      if (d < 205){
        SLINK3_V = SLINK3_V - 0.5;
        SLINK1_V = SLINK1_V + 0.5;
      }
      calculate_Servos(h,f,e,d,a);
    }
  }

  //=======================================================================================================================================

  if(command == 91){
    SBASE_V = 90;
    SLINK1_V = 140;
    SLINK2_V = 90;
    SLINK3_V = 140;
    SHEAD_V = 96;
    beta = 81.48;
    h = 180.00;
    calculate_Servos(h,f,e,d,a);
  }
}

double mapf(double x, double in_min, double in_max, double out_min, double out_max){
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
