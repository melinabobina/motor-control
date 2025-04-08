/*

      Motor Driver Program:
      Controls 3, 6, or 9 DM542T stepper motor drivers for the Nerual Kinetic Sculpture

      Modified: April 7, 2025
      By: Letzy Mota
      Note: this codde might not fully work but it compiled so ill test the logic at a later date :p
      What is left: starting height function and limit switch implementation

*/ 

#define PulPin0 2 // pin 2 will control motor 1s pulse 
#define DirPin0 3 // pin 3 will control motor 1s direction

#define PulPin1 4 // pin 4 will control motor 2s pulse
#define DirPin1 5 // pin 5 will control motor 2s direction

#define PulPin2 6 // pin 6 will control motor 3s pulse
#define DirPin2 7 // pin 7 will control motor 3s direction

#define PulPin3 8 // pin 8 will control motor 4s pulse
#define DirPin3 9 // pin 9 will control motor 4s direction

#define PulPin4 10 // pin 10 will control motor 5s pulse
#define DirPin4 11 // pin 11 will control motor 5s direction

#define PulPin5 12 // pin 12 will control motor 6s pulse
#define DirPin5 13 // pin 13 will control motor 6s direction

#define PulPin6 22 // pin 22 will control motor 7s pulse
#define DirPin6 23 // pin 23 will control motor 7s direction

#define PulPin7 24 // pin 24 will control motor 8s pulse
#define DirPin7 25 // pin 25 will control motor 8s direction

#define PulPin8 26 // pin 26 will control motor 9s pulse
#define DirPin8 27 // pin 27 will control motor 9s direction 

int PulPins[] = {PulPin0, PulPin1, PulPin2, PulPin3, PulPin4, PulPin5, PulPin6, PulPin7, PulPin8};
int DirPins[] = {DirPin0, DirPin1, DirPin2, DirPin3, DirPin4, DirPin5, DirPin6, DirPin7, DirPin8};

// configuration object containing all individual motor parameters the user can map
struct configuration{
  int PulPin;
  int DirPin;
};

void motorControl(int MotorNum, int Direction, int Delay);

/* TO DO : create function to get all panels to correct starting height */
void startingHeight(void);

// app input for all motors
int startHeight; 
int rows; 
int columns;

// app input for individual motors
int rpos; 
int cpos;
int speed; 
int direction; 

configuration** MotorArr;

void setup(){

// initialize UART communication (pin 19 -> RX1)
  Serial1.begin(9600); 

// wait unitl data is received
  while(!Serial1.available()){}

// read configuration data
  startHeight = Serial1.parseInt();
  rows = Serial1.parseInt();
  columns = Serial1.parseInt();

// allocate memory for rows
  MotorArr = new configuration*[rows];
// allocate memory for each column in each row
  for(int i = 0; i < rows; i++){
    MotorArr[i] = new configuration[columns];
  }

  int idx = 0;
  for(int i = 0; i<rows; i++){
    for(int j = 0; j<columns; j++){
      MotorArr[i][j].PulPin = PulPins[idx]; // asign pulse pin to matrix poition
      MotorArr[i][j].DirPin = DirPins[idx]; // asign direction pin to matrix position

      pinMode(MotorArr[i][j].PulPin, OUTPUT); // configure pulse pin to output
      pinMode(MotorArr[i][j].DirPin, OUTPUT); // configure direction pin to output

      idx++;
    }
  }

}

void loop(){

  if(Serial1.available() >= 8){
// read data
    rpos = Serial1.parseInt();
    cpos = Serial1.parseInt();
    speed = Serial1.parseInt();
    direction = Serial1.parseInt(); // 0 -> clockwise, 1-> counterclockwise 

// execute motor instructions (10 pulses per configuration)
      for(int i = 0; i < 10; i++){
        motorControl(rpos, cpos, direction, speed);
      }
  }

}

/* TO DO: implement limit switch operation in motor control function to keep panel in bounds */
void motorControl(int rpos, int cpos, int Direction, int Delay){

  digitalWrite(MotorArr[rpos][cpos].DirPin, Direction);

  digitalWrite(MotorArr[rpos][cpos].PulPin, HIGH);
  delayMicroseconds(Delay);
  digitalWrite(MotorArr[rpos][cpos].PulPin, LOW);
  delayMicroseconds(Delay);

}

/* TO DO: fill function */
void startingHeight(void){

}
