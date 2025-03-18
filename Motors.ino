/*

      Motor Driver Program:
      Controls 9 DM542T stepper motor drivers for the Nerual Kinetic Sculpture
  
      Modified: March 12, 2025
      By: Letzy Mota and Karina Montero

*/ 

#define PulPin1 2 // pin 2 will control motor 1s pulse 
#define DirPin1 3 // pin 3 will control motor 1s direction

#define PulPin2 4 // pin 4 will control motor 2s pulse
#define DirPin2 5 // pin 5 will control motor 2s direction

#define PulPin3 6 // pin 6 will control motor 3s pulse
#define DirPin3 7 // pin 7 will control motor 3s direction

#define PulPin4 8 // pin 8 will control motor 4s pulse
#define DirPin4 9 // pin 9 will control motor 4s direction

#define PulPin5 10 // pin 10 will control motor 5s pulse
#define DirPin5 11 // pin 11 will control motor 5s direction

#define PulPin6 12 // pin 12 will control motor 6s pulse
#define DirPin6 13 // pin 13 will control motor 6s direction

#define PulPin7 22 // pin 22 will control motor 7s pulse
#define DirPin7 23 // pin 23 will control motor 7s direction

#define PulPin8 24 // pin 24 will control motor 8s pulse
#define DirPin8 25 // pin 25 will control motor 8s direction

#define PulPin9 26 // pin 26 will control motor 9s pulse
#define DirPin9 27 // pin 27 will control motor 9s direction 

void Motor1Control(int Direction, int Delay);  
void Motor2Control(int Direction, int Delay);
void Motor3Control(int Direction, int Delay);
void Motor4Control(int Direction, int Delay);  
void Motor5Control(int Direction, int Delay);
void Motor6Control(int Direction, int Delay);
void Motor7Control(int Direction, int Delay);  
void Motor8Control(int Direction, int Delay);
void Motor9Control(int Direction, int Delay);

void setup(){

// configure input pins for app instructions??? (analog???)
// ... ???
// read configuration and save instructions (analog to map or switch???)
// ... ???

// configure pins for filtered EEG input (analog) [idk how many channels?]
// ...

// configure pins to output pulse and direction (digital)
int PulPins[] = {PulPin1, PulPin2, PulPin3, PulPin4, PulPin5, PulPin6, PulPin7, PulPin8, PulPin9};
int DirPins[] = {DirPin1, DirPin2, DirPin3, DirPin4, DirPin5, DirPin6, DirPin7, DirPin8, DirPin9};

  for(int i=0; i<9; i++){
    pinMode(PulPins[i], OUTPUT);
    pinMode(DirPins[i], OUTPUT);
  }

}

void loop(){

// read the input signal (analog)
// ...

// execute motor instructions 
  Motor1Control(1,500); //0 -> clockwise, 1-> counterclockwise 
  Motor2Control(1,50);
  Motor3Control(1,50);
  Motor4Control(1,50);
  Motor5Control(1,50);
  Motor6Control(1,50);
  Motor7Control(1,50);
  Motor8Control(1,50);
  Motor9Control(1,50);

}

// Functions to 
void Motor1Control(int Direction, int Delay){

  digitalWrite(DirPin1, Direction);

  digitalWrite(PulPin1, HIGH);
  delayMicroseconds(Delay);
  digitalWrite(PulPin1, LOW);
  delayMicroseconds(Delay);

}  

void Motor2Control(int Direction, int Delay){

  digitalWrite(DirPin2, Direction);

  digitalWrite(PulPin2, HIGH);
  delayMicroseconds(Delay);
  digitalWrite(PulPin2, LOW);
  delayMicroseconds(Delay);

}

void Motor3Control(int Direction, int Delay){

  digitalWrite(DirPin3, Direction);

  digitalWrite(PulPin3, HIGH);
  delayMicroseconds(Delay);
  digitalWrite(PulPin3, LOW);
  delayMicroseconds(Delay);

}

void Motor4Control(int Direction, int Delay){

  digitalWrite(DirPin4, Direction);

  digitalWrite(PulPin4, HIGH);
  delayMicroseconds(Delay);
  digitalWrite(PulPin4, LOW);
  delayMicroseconds(Delay);

}  

void Motor5Control(int Direction, int Delay){

  digitalWrite(DirPin5, Direction);

  digitalWrite(PulPin5, HIGH);
  delayMicroseconds(Delay);
  digitalWrite(PulPin5, LOW);
  delayMicroseconds(Delay);

}

void Motor6Control(int Direction, int Delay){

  digitalWrite(DirPin6, Direction);

  digitalWrite(PulPin6, HIGH);
  delayMicroseconds(Delay);
  digitalWrite(PulPin6, LOW);
  delayMicroseconds(Delay);

}

void Motor7Control(int Direction, int Delay){

  digitalWrite(DirPin7, Direction);

  digitalWrite(PulPin7, HIGH);
  delayMicroseconds(Delay);
  digitalWrite(PulPin7, LOW);
  delayMicroseconds(Delay);

}  

void Motor8Control(int Direction, int Delay){

  digitalWrite(DirPin8, Direction);

  digitalWrite(PulPin8, HIGH);
  delayMicroseconds(Delay);
  digitalWrite(PulPin8, LOW);
  delayMicroseconds(Delay);

}

void Motor9Control(int Direction, int Delay){

  digitalWrite(DirPin9, Direction);

  digitalWrite(PulPin9, HIGH);
  delayMicroseconds(Delay);
  digitalWrite(PulPin9, LOW);
  delayMicroseconds(Delay);

}