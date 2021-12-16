//====================== Code Information ==========================================================================================================================================//
//
//  Creator:  Fabian Noll
//  Date of Creation: 07.12.2021
//  Purpous: The "Operating System" of the Robot Arm "Luna One".
//
//  Changelog: 13.12.2021 - Added Bluetooth Availability
//                        - Added Basic Bluetooth Controll
//
//==================================================================================================================================================================================//

//======== Libraries================================================================================================================================================================

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <SoftwareSerial.h> 

SoftwareSerial BlueArm(10, 9); // RX | TX 

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

#define PWM_SERVOBASE 0
#define PWM_SERVOLINK1 1
#define PWM_SERVOLINK2 2
#define PWM_SERVOLINK3 3
#define PWM_SERVOHEAD 4

//===================================================================================================================================================================================

//Defining the standard values for the servo Position. 300 is about the middle value of max and low.
//The Servos can accept a value between 80 (minimum) and 530 (maximum)

int SBASE_V = 285;    //80 - 500 (285)
int SLINK1_V = 450;   //90 - 530 (320)
int SLINK2_V = 200;   //70 - 530 (385)
int SLINK3_V = 187;   //70 - 530 (405)
int SHEAD_V = 400;    //80 - 500 (285)

int TEST_SBASE_V = 90;
int TEST_SLINK1_V = 140;
int TEST_SLINK2_V = 90;
int TEST_SLINK3_V = 130;
int TEST_SHEAD_V = 96;

int command = 99;              //Value to save the command received by the bluetooth control
int active_segment = 1;  

//======Variables fo Inverse Kinematic=============================================================================================================================================================================

int a = 67;     //Height of the Robot Base
int b = 87;     //Lenght of the first segment
int c = 87;     //Lenght of the second Segment
int e = 140;    //Lenght of the Robot Head
int f = 0;      //Lenght of diameter of the triangle

int alpha = 0;  //Degree of the first Servo
int beta = 0;   //Degree of the second Servo
int gamme = 0;  //Degree of the third Servo

int h = 190;    //Height of the Head above Ground
int d = 105;    //Distance of the Head from Base Center

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
  pwm.setPWMFreq(50);  // Analog servos run at ~50 Hz updates
  
  //move_StandUpStartup();
  //delay(1000);
  move_StartPosition();
}

//===================================================================================================================================================================================

void loop() {

  command = receive_Command();

  //=======Switching between Servo Segments=========================================================================================================

  if (command == 5){
    active_segment = 1;
  }

  if (command == 6){
    active_segment = 2;
  }

  if (command == 7){
    active_segment = 3;
  }

  if (command == 8){
    active_segment = 4;
  }

  //=======Preset 1=========================================================================================================

  if (command == 10){
    
    move_ServosTest(90,140,90,130,96);
    
  }

  //=======Preset 2=========================================================================================================

  if (command == 11){
    Serial.print("SERVOBASE:");
    Serial.println(SBASE_V);
    Serial.print("SERVOLINK1:");
    Serial.println(SLINK1_V);
    Serial.print("SERVOLINK2:");
    Serial.println(SLINK2_V);
    Serial.print("SERVOLINK3:");
    Serial.println(SLINK3_V);
    Serial.print("SERVOHEAD:");
    Serial.println(SHEAD_V);
    Serial.println("================");
  }

  //=======Preset 3=========================================================================================================

  if (command == 12){

    int DELAY = 2;
    
    SBASE_V = 285;      //450
    SLINK1_V = 450;     //228
    SLINK2_V = 200;     //160
    SLINK3_V = 187;     //530
    SHEAD_V = 400;      //400
    move_Servos(SBASE_V,SLINK1_V,SLINK2_V,SLINK3_V,SHEAD_V);
    delay(600);

    for (int x = 285; x <= 426; x++){
      SBASE_V = SBASE_V + 1;
      move_Servos(SBASE_V,SLINK1_V,SLINK2_V,SLINK3_V,SHEAD_V);
      //delay(DELAY);
    }

    for (int x = 260; x <= 530; x++){
      SLINK3_V = SLINK3_V + 1;
      move_Servos(SBASE_V,SLINK1_V,SLINK2_V,SLINK3_V,SHEAD_V);
      //delay(DELAY);
    }

    for (int x = 200; x >= 160; x--){
      SLINK2_V = SLINK2_V - 1;
      move_Servos(SBASE_V,SLINK1_V,SLINK2_V,SLINK3_V,SHEAD_V);
      //delay(DELAY);
    }

    for (int x = 417; x >= 210; x--){
      SLINK1_V = SLINK1_V - 1;
      move_Servos(SBASE_V,SLINK1_V,SLINK2_V,SLINK3_V,SHEAD_V);
      //delay(DELAY);
    }

    delay(500);
    SHEAD_V = 205;
    move_Servos(SBASE_V,SLINK1_V,SLINK2_V,SLINK3_V,SHEAD_V);
    delay(500);
    
    for (int x = 210; x <= 450; x++){
      SLINK1_V = SLINK1_V + 1;
      move_Servos(SBASE_V,SLINK1_V,SLINK2_V,SLINK3_V,SHEAD_V);
      //delay(DELAY);
    }

    for (int x = 160; x <= 200; x++){
      SLINK2_V = SLINK2_V + 1;
      move_Servos(SBASE_V,SLINK1_V,SLINK2_V,SLINK3_V,SHEAD_V);
      //delay(DELAY);
    }

    for (int x = 530; x >= 260; x--){
      SLINK3_V = SLINK3_V - 1;
      move_Servos(SBASE_V,SLINK1_V,SLINK2_V,SLINK3_V,SHEAD_V);
      //delay(DELAY);
    }

    for (int x = 426; x >= 240; x--){
      SBASE_V = SBASE_V - 1;
      move_Servos(SBASE_V,SLINK1_V,SLINK2_V,SLINK3_V,SHEAD_V);
      //delay(DELAY);
    }
  }

  //=======Preset 4=========================================================================================================

  if (command == 13){

    SBASE_V = 285;
    SLINK1_V = 450;
    SLINK2_V = 200;
    SLINK3_V = 187;
    SHEAD_V = 285;
    move_Servos(SBASE_V,SLINK1_V,SLINK2_V,SLINK3_V,SHEAD_V);
    delay(600);
    while(!(command == 9)){
      for(int i = 0; i < 50; i++){
        if (SBASE_V < 450){
          SBASE_V++;
        } else {
          SBASE_V = 80;
        }
      }
      command = receive_Command();
      SLINK1_V = 479;
      SLINK2_V = 290;
      SLINK3_V = 103;
      SHEAD_V = 212;
      move_Servos(SBASE_V,SLINK1_V,SLINK2_V,SLINK3_V,SHEAD_V);
      delay(1000);
      command = receive_Command();
      SLINK1_V = 184;
      SLINK2_V = 486;
      SLINK3_V = 444;
      SHEAD_V = 400;
      move_Servos(SBASE_V,SLINK1_V,SLINK2_V,SLINK3_V,SHEAD_V);
      delay(600);
      command = receive_Command();
    }
  }

  //=======Controlling the first Servo Segment=========================================================================================================

  if(active_segment == 1){
    if(command == 1){
      while(!(command == 81)){
        command = receive_Command();
        if(SLINK1_V > 90){
            SLINK1_V = SLINK1_V - 1;
        }
        move_Servos(SBASE_V,SLINK1_V,SLINK2_V,SLINK3_V,SHEAD_V);
      }
    } else if (command == 4){
      while(!(command == 84)){
        command = receive_Command();
        if(SLINK1_V < 530){
            SLINK1_V = SLINK1_V + 1;
        }
        move_Servos(SBASE_V,SLINK1_V,SLINK2_V,SLINK3_V,SHEAD_V);
      }
    }
  }

  //=======Controlling the second Servo Segment=========================================================================================================

  if(active_segment == 2){
    if(command == 4){
      while(!(command == 84)){
        command = receive_Command();
        if(SLINK2_V > 70){
            SLINK2_V = SLINK2_V - 1;
        }
        move_Servos(SBASE_V,SLINK1_V,SLINK2_V,SLINK3_V,SHEAD_V);
      }
    } else if (command == 1){
      while(!(command == 81)){
        command = receive_Command();
        if(SLINK2_V < 530){
            SLINK2_V = SLINK2_V + 1;
        }
        move_Servos(SBASE_V,SLINK1_V,SLINK2_V,SLINK3_V,SHEAD_V);
      }
    }
  }

  //=======Controlling the third Servo Segment=========================================================================================================

  if(active_segment == 3){
    if(command == 4){
      while(!(command == 84)){
        command = receive_Command();
        if(SLINK3_V > 70){
            SLINK3_V = SLINK3_V - 1;
        }
        move_Servos(SBASE_V,SLINK1_V,SLINK2_V,SLINK3_V,SHEAD_V);
      }
    } else if (command == 1){
      while(!(command == 81)){
        command = receive_Command();
        if(SLINK3_V < 530){
            SLINK3_V = SLINK3_V + 1;
        }
        move_Servos(SBASE_V,SLINK1_V,SLINK2_V,SLINK3_V,SHEAD_V);
      }
    }
  }

  //=======Controlling the head Servo Segment=========================================================================================================

  if(active_segment == 4){
    if(command == 1){
      while(!(command == 81)){
        command = receive_Command();
        if(SHEAD_V > 180){
            SHEAD_V = SHEAD_V - 1;
        }
        move_Servos(SBASE_V,SLINK1_V,SLINK2_V,SLINK3_V,SHEAD_V);
      }
    } else if (command == 4){
      while(!(command == 84)){
        command = receive_Command();
        if(SHEAD_V < 400){
            SHEAD_V = SHEAD_V + 1;
        }
        move_Servos(SBASE_V,SLINK1_V,SLINK2_V,SLINK3_V,SHEAD_V);
      }
    }
  }

  //=======Go to the stand up position=========================================================================================================

  if (command == 90){

    Serial.println("Standing Up");

    SBASE_V = 285;   
    SLINK1_V = 320;
    SLINK2_V = 385;
    SLINK3_V = 405;
    SHEAD_V = 285;
    
    move_Servos(SBASE_V,SLINK1_V,SLINK2_V,SLINK3_V,SHEAD_V);
  }

  //=======Go to the Ready Position=========================================================================================================

  if (command == 91){

    Serial.println("Getting ready");

    SBASE_V = 285;
    SLINK1_V = 450;
    SLINK2_V = 200;
    SLINK3_V = 187;
    
    move_Servos(SBASE_V,SLINK1_V,SLINK2_V,SLINK3_V,SHEAD_V);
  }

  //=======Moving Base Servo to the Left=========================================================================================================

  if (command == 2){
    while(!(command == 82)){
      command = receive_Command();
      if(SBASE_V > 70){
          SBASE_V = SBASE_V - 1;
      }
      move_Servos(SBASE_V,SLINK1_V,SLINK2_V,SLINK3_V,SHEAD_V);
    }  
  }

  //=======Moving Base Servo to the right=========================================================================================================

  if (command == 3){
    while(!(command == 83)){
      command = receive_Command();
      delay(1);
      if(SBASE_V < 530){
          SBASE_V = SBASE_V + 1;
      }
      move_Servos(SBASE_V,SLINK1_V,SLINK2_V,SLINK3_V,SHEAD_V);
    }
  }

  //================================================================================================================

  check_Manual_Control();                                                                               //checking and executing

  //move_Servos(SBASE_V,SLINK1_V,SLINK2_V,SLINK3_V,SHEAD_V);      //Move Servos according to its given values

  command = 99;                                                                                         //resetting the command for future purpouses
}

void check_Manual_Control(){
  /*
  if(digitalRead(SERVOBASE) == HIGH){
    if(digitalRead(BUTTONUP) == HIGH){
      TEST_SLINK2_V++;
      TEST_SLINK3_V--;
      move_ServosTest(TEST_SBASE_V,TEST_SLINK1_V,TEST_SLINK2_V,TEST_SLINK3_V,TEST_SHEAD_V);
    } else if (digitalRead(BUTTONDOWN) == HIGH){
      TEST_SLINK2_V++;
      TEST_SLINK3_V--;
      move_ServosTest(TEST_SBASE_V,TEST_SLINK1_V,TEST_SLINK2_V,TEST_SLINK3_V,TEST_SHEAD_V);
    }
*/
    if(digitalRead(BUTTONUP) == HIGH){
      if(SBASE_V < 450){
        SBASE_V = SBASE_V + 1;
        move_Servos(SBASE_V,SLINK1_V,SLINK2_V,SLINK3_V,SHEAD_V);
       }
    } else if (digitalRead(BUTTONDOWN) == HIGH){
      if(SBASE_V > 80){
        SBASE_V = SBASE_V - 1;
        move_Servos(SBASE_V,SLINK1_V,SLINK2_V,SLINK3_V,SHEAD_V);
       }
    } 
  }
  
  //================================================================================================================================================================================
  
  if(digitalRead(SERVOLINK1) == HIGH){
    if(digitalRead(BUTTONUP) == HIGH){
      if(SLINK1_V < 530){
        SLINK1_V = SLINK1_V + 1;
        move_Servos(SBASE_V,SLINK1_V,SLINK2_V,SLINK3_V,SHEAD_V);
       }
    } else if (digitalRead(BUTTONDOWN) == HIGH){
      if(SLINK1_V > 90){
        SLINK1_V = SLINK1_V - 1;
        move_Servos(SBASE_V,SLINK1_V,SLINK2_V,SLINK3_V,SHEAD_V);
      }
    }
  }
  
  //================================================================================================================================================================================

  if(digitalRead(SERVOLINK2) == HIGH){
    if(digitalRead(BUTTONUP) == HIGH){
       if(SLINK2_V > 70){
        SLINK2_V = SLINK2_V - 1;
        move_Servos(SBASE_V,SLINK1_V,SLINK2_V,SLINK3_V,SHEAD_V);
       }
    } else if (digitalRead(BUTTONDOWN) == HIGH){
      if(SLINK2_V < 530){
        SLINK2_V = SLINK2_V + 1;
        move_Servos(SBASE_V,SLINK1_V,SLINK2_V,SLINK3_V,SHEAD_V);
      }
    }
  }
  
  //================================================================================================================================================================================

  if(digitalRead(SERVOLINK3) == HIGH){
    if(digitalRead(BUTTONUP) == HIGH){
      if(SLINK3_V > 70){
        SLINK3_V = SLINK3_V - 1;
        move_Servos(SBASE_V,SLINK1_V,SLINK2_V,SLINK3_V,SHEAD_V);
       }
    } else if (digitalRead(BUTTONDOWN) == HIGH){
      if(SLINK3_V < 530){
        SLINK3_V = SLINK3_V + 1;
        move_Servos(SBASE_V,SLINK1_V,SLINK2_V,SLINK3_V,SHEAD_V);
      }
    }
  }
  
  //================================================================================================================================================================================

  if(digitalRead(SERVOHEAD) == HIGH){
    if(digitalRead(BUTTONUP) == HIGH){
      if(SHEAD_V < 400){
        SHEAD_V = SHEAD_V + 1;
        move_Servos(SBASE_V,SLINK1_V,SLINK2_V,SLINK3_V,SHEAD_V);
       }
    } else if (digitalRead(BUTTONDOWN) == HIGH){
      if(SHEAD_V > 180){
        SHEAD_V = SHEAD_V - 1;
        move_Servos(SBASE_V,SLINK1_V,SLINK2_V,SLINK3_V,SHEAD_V);
      }
    }
  }
}

void move_Servos(int SBASE_V, int SLINK1_V, int SLINK2_V, int SLINK3_V, int SHEAD_V){
  
  pwm.setPWM(PWM_SERVOBASE, 0, SBASE_V);
  pwm.setPWM(PWM_SERVOLINK1, 0, SLINK1_V);
  pwm.setPWM(PWM_SERVOLINK2, 0, SLINK2_V);
  pwm.setPWM(PWM_SERVOLINK3, 0, SLINK3_V);
  pwm.setPWM(PWM_SERVOHEAD, 0, SHEAD_V);
  
}

void move_ServosTest(int SBASE_V, int SLINK1_V, int SLINK2_V, int SLINK3_V, int SHEAD_V){

  int SBASE_VT = map(SBASE_V, 0, 180, 70, 530);
  int SLINK1_VT = map(SLINK1_V, 0, 180, 70, 530);
  int SLINK2_VT = map(SLINK2_V - 40, 0, 180, 70, 530);
  int SLINK3_VT = map(SLINK3_V - 68, 0, 180, 70, 530);
  int SHEAD_VT = map(SHEAD_V, 0, 180, 70, 530);
  
  pwm.setPWM(PWM_SERVOBASE, 0, SBASE_VT);
  pwm.setPWM(PWM_SERVOLINK1, 0, SLINK1_VT);
  pwm.setPWM(PWM_SERVOLINK2, 0, SLINK2_VT);
  pwm.setPWM(PWM_SERVOLINK3, 0, SLINK3_VT);
  pwm.setPWM(PWM_SERVOHEAD, 0, SHEAD_VT);
  
}

void move_StandUpStartup(){
  
  pwm.setPWM(PWM_SERVOBASE, 0, 285);
  pwm.setPWM(PWM_SERVOLINK1, 0, 320);
  pwm.setPWM(PWM_SERVOLINK2, 0, 385);
  pwm.setPWM(PWM_SERVOLINK3, 0, 405);
  pwm.setPWM(PWM_SERVOHEAD, 0, 285);

}

void move_StandUp(){
  SBASE_V == 285;   
  SLINK1_V == 320;
  SLINK2_V == 385;
  SLINK3_V == 405;
  SHEAD_V == 285;
}

void move_StartPosition(){
  pwm.setPWM(PWM_SERVOBASE, 0, 285);
  pwm.setPWM(PWM_SERVOLINK1, 0, 450);
  pwm.setPWM(PWM_SERVOLINK2, 0, 200);
  pwm.setPWM(PWM_SERVOLINK3, 0, 260);
  pwm.setPWM(PWM_SERVOHEAD, 0, 400);
}

int receive_Command(){
  if (BlueArm.available()){ 
    int received_command = BlueArm.read();
    //Serial.print("Received Command:");
    //Serial.println(received_command);
    return received_command;
  } 
}
