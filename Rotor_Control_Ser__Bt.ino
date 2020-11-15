
#define LATCHSTATE  3

#define ROTORPOT   A0 //PC0
#define PUSHBUTTON A1 //PC1
#define KNOBLEFT   A3 //PC3
#define KNOBRIGHT  A2 //PC4

#define ROTORCW    8  //PB2
#define ROTORCCW   9  //PB3

int K;
int L;
int M;
int oldK;

int RotorPot;

int PushButton;
int PushButton1;
int PushButton2;
int PushButton3;
int PushButton4;
int PushButtonFirst;
int KnobLeft;
int KnobRight;
int KnobFirst;
int KnobDir;
int oldKnobState;
int newKnobState;
unsigned int Position;
unsigned int PositionExtern;

int newButton;
int newPosition;
int newPos;
int newRotor;

int newMode;

int RotorValue;
int newRotorValue;
int stepRotorValue;

int slow;
int fast;

int beep;

int msDelay = 30;

const int KNOBDIR[] = { 0, -1,  1,  0,
                        1,  0,  0, -1,
                       -1,  0,  0,  1,
                        0,  1, -1,  0
                      };



void setup() {

     Serial.begin(9600);
 
     pinMode(PUSHBUTTON, INPUT);
     pinMode(KNOBLEFT,   INPUT);
     pinMode(KNOBRIGHT,  INPUT);

     pinMode(ROTORCW,   OUTPUT);
     pinMode(ROTORCCW,  OUTPUT);

     setPCINTinterrupts();  
     resetPosition();
     getRotorValue();
     sendLedValues();
     sendRotorValueDigit();
     
     newRotorValue = RotorValue;
     oldKnobState = 3;
     newMode      = 0;

     slow = 1;
     fast = 5;
}

void resetPosition() {
  
     Position = 8;
     PositionExtern = 2;
}

void getRotorValue() {
     RotorValue = analogRead(RotorPot);
     RotorValue = map(RotorValue, 0, 1023, 0, 359);
//     Serial.println(RotorValue);
}

void readPushButton() {

     PushButton1 = digitalRead(PUSHBUTTON);
     PushButton2 = digitalRead(PUSHBUTTON);
     PushButton3 = digitalRead(PUSHBUTTON);
     PushButton4 = digitalRead(PUSHBUTTON);
     PushButton  = (PushButton1 + PushButton2+PushButton3 + PushButton4)/4;
//     Serial.println(PushButton);
}

void readKnob() {

     KnobLeft   = digitalRead(KNOBLEFT);
     KnobRight  = digitalRead(KNOBRIGHT);
     newKnobState = KnobLeft | (KnobRight << 1);
}

void calculateKnobState() {

     if (oldKnobState != newKnobState) {

        Position += KNOBDIR[newKnobState | (oldKnobState << 2)];

        if (newKnobState == LATCHSTATE) {

           PositionExtern = Position >> 2;
        }

        oldKnobState = newKnobState;
     }
}

void calculateCW() {

     if (PositionExtern > 2) { // Turning CW
        newRotorValue = newRotorValue+stepRotorValue;

        if (newRotorValue > 359) { // Out of value
           newRotorValue =   0;
        }
        resetPosition();
     }

}

void calculateCCW() {

     if (PositionExtern < 2) { // Turning CCW
        newRotorValue = newRotorValue-stepRotorValue;

        if (newRotorValue <   0) { // Out of value
           newRotorValue = 359;
        }
        resetPosition();
     }
}

void setMode() {
  
     if ((newKnobState != 3) && (PushButton == 1) && (newMode == 0)) { // Knob turned
 
        newMode = 1;
     }

     if ((PushButton == 0) && (newKnobState == 3) && (newMode == 1)) { // Pushbutton pressed fast mode

        newMode = 2;
     }

     if ((PushButton == 1) && (newKnobState == 3) && (newMode == 2)) { // Pushbutton released fast mode

        newMode = 3;
     }

     if ((PushButton == 0) && (newKnobState == 3) && (newMode == 3)) { // Stop immidiate fast mode

        newMode = 4;
     }

     if ((PushButton == 1) && (newKnobState == 3) && (newMode == 4)) { // Pushbutton released fast mode

        newMode = 5;
     }

     if ((PushButton == 0) && (newKnobState == 3) && (newMode == 0)) { // Pushbutton pressed slow mode

        newMode = 8;
     }

     if ((PushButton == 1) && (newKnobState == 3) && (newMode == 8)) {  // Pushbutton released slow mode

        newMode = 9;
     }
//Serial.println(newMode);
}


void enablePushButton() {

     PCMSK1 | (1<<PCINT9);
}

void enableKnob() {

     PCMSK1 | ((1<<PCINT10) | (1<<PCINT11));
}

void disablePushButton() {

     PCMSK1 &= ~(1<<PCINT9);
}

void disableKnob() {

     PCMSK1 &= ~((1<<PCINT10) | ~(PCINT11));
}

void turnRotor() {

     if (newRotorValue > RotorValue) {
         turnCW();
     }
        
     if (newRotorValue < RotorValue) {
        turnCCW(); 
     }

     if (newRotorValue == RotorValue) {
        stopRotor();
        K=32772;
        sendValuesBT();
     }
}

void turnCCW() {

     digitalWrite(ROTORCW,  LOW);
     digitalWrite(ROTORCCW,HIGH);
}

void turnCW() {

     digitalWrite(ROTORCW, HIGH);
     digitalWrite(ROTORCCW, LOW);
}

void stopRotor() {

     digitalWrite(ROTORCW, HIGH);
     digitalWrite(ROTORCCW,HIGH);
     beep = 1;
     newMode = 0;
     setPCINTinterrupts();
}

void directStop1() {

     digitalWrite(ROTORCW, HIGH);
     digitalWrite(ROTORCCW,HIGH);
}

void directStop2() {

     digitalWrite(ROTORCW, HIGH);
     digitalWrite(ROTORCCW,HIGH);
     newRotorValue = RotorValue;
     newMode = 0;
     setPCINTinterrupts();
}

void sendLedValues() {

     static int rotor = 0;
     static int pos = 0;

     newRotor = RotorValue;
     newPos = newRotorValue;

     if (rotor != newRotor) {

        sendRotorValueLed();
        rotor = newRotor;
     }
     
     if (pos != newPos) {

        sendnewRotorValueLed();
        pos = newPos; 
     }

     if (beep == 1) {

        sendBeep();
        beep = 0;
     }
}

void sendRotorValueLed() {

     K = RotorValue;
     sendValuesBT();
}

void sendRotorValueDigit() {

     K = RotorValue + 512;
     sendValuesBT();
}

void sendnewRotorValueLed() {

     K = newRotorValue + 4096;
     sendValuesBT();
}

void sendnewRotorValueDigit() {

     K = newRotorValue + 8192;
     sendValuesBT();
}
void sendBeep() {

     K = 32768;
     sendValuesBT();
}

void sendValuesBT() { // Bluetooth
  
     if (K != oldK){  // Only if change
        L = K >> 8;
        M = K & B11111111;

        Serial.write(L);
        Serial.write(M);

        delay(100);
        oldK=K;
     }
}

void sendValuesSHORA() { // Short range

}

void getEncoderValues() {

readPushButton();
readKnob();
calculateKnobState();
calculateCW();
calculateCCW();
setMode();
}

void setPCINTinterrupts() { // Interrupts on encoder

     PCICR  |= (1 << PCIE1);   
     PCMSK1 |= (1 << PCINT9);
     PCMSK1 |= (1 << PCINT10) | (1 << PCINT11);  
}

ISR(PCINT1_vect) {
getEncoderValues(); 
}

void msdelay() {

     for (int MSdelay = 0; MSdelay < msDelay; MSdelay++) {
    delay(1);
      }
  
}

void loop() {

     getRotorValue();
     sendLedValues();

     if (newMode == 0 ) { // Programm stable

        sendRotorValueDigit();
     }
     
     if (newMode == 1 ) { // Knob is first

        stepRotorValue = fast;
        sendnewRotorValueDigit();
     }

     if (newMode == 2) { // Pushbutton pressed

        disableKnob();
        turnRotor();
        sendLedValues();
        sendRotorValueDigit();
     }

     if (newMode == 3) { // Pushbutton released

        turnRotor();
        sendLedValues();
        sendRotorValueDigit();
     }
     
     if (newMode == 4) { // Pushbutton pressed

        directStop1();
        sendLedValues();
        sendRotorValueDigit();
     }

     if (newMode == 5) { // Pushbutton released

        disablePushButton();
        directStop2();
        sendLedValues();
        sendRotorValueDigit();
     }

     if (newMode == 8 ) { // Pushbutton is first

        stepRotorValue = slow;
        turnRotor();
        sendLedValues();
        sendnewRotorValueDigit();
     }

     if (newMode == 9) { // Pushbutton released

        sendRotorValueDigit();
        disablePushButton();
        disableKnob();
                
        while (RotorValue != newRotorValue) { // Keep moving to position
              getRotorValue();
              turnRotor();
              sendLedValues();
              sendRotorValueDigit();
        }
        
        newMode = 0;
        setPCINTinterrupts();
     }
}


