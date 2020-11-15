

/////////////////////////////////////////////////////////////////
//                                                             //
// Software is written for use with the                        //
// add-on module for EC1515 digital led                        //
// clock.                                                      //
//                                                             //
// Work instruction for the EC1515:                            //
//                                                             //
// Mount only: Leds, 7-segment, R1-R8, LS                      //
// (Re)place: U1 for male header                               //
// Wire: R9(processor side) to LS+                             //
//                                                             //
// The software can be uploaded through                        //
// ISP with the board module from Mightycore.                  //
//                                                             //
// The used processor is the Atmega16.                         //
// The reason for using this "big" IC                          //
// is that the EC1515 is using for the leds                    //
// an 8x8 matrix and for the 7 segments a                      //
// 4x8 matrix.                                                 //
//                                                             //
// To display something you need to load                       //
// a number through the serial port.                           //
//                                                             //
//     0-   359 = steady led degrees           (  0/ 59)       //
//   512-   871 = steady 7-segment degrees     (  0/359)       //
//  1024-  3383 = steady 7-segment time                        //
//  4096-  4455 = flash  led degrees                           //
//  8192-  8551 = flash  7-segment degrees                     //
// 16384- 16585 = steady 7-segment temperature (-50/150)       //
// 24576- 24589 = steady 7-segment windforce   (  0/ 12)       //
//        32768 = beep                                         //
//        32769 = deactivate steady led degrees                //
//        32770 = deactivate steady 7-segment degrees          //
//        32771 = deactivate steady 7-segment time             //
//        32772 = deactivate flash  led degrees                //
//        32773 = deactivate flash  7-segment                  //
//        32774 = deactivate steady 7-segment temperature      //
//        32775 = deactivate steady 7-segment windforce        //
//                                                             //
//        To send values use the next routine:                 //
//                                                             //
//        K = any number                                       //
//        L = K >> 8;                                          //
//        M = K & B11111111;                                   //
//        Serial.write(L);                                     //
//        Serial.write(M);                                     //
//                                                             //
/////////////////////////////////////////////////////////////////


const byte pixel[124]    = {
  B00010000, B01111111,
  B00010000, B10111111,
  B00010000, B11011111,
  B00010000, B11101111,
  B00010000, B11110111,
  B00010000, B11111011,
  B00010000, B11111110,
  B00010000, B11111101,

  B00100000, B01111111,
  B00100000, B10111111,
  B00100000, B11011111,
  B00100000, B11101111,
  B00100000, B11110111,
  B00100000, B11111011,
  B00100000, B11111110,
  B00100000, B11111101,

  B01000000, B01111111,
  B01000000, B10111111,
  B01000000, B11011111,
  B01000000, B11101111,
  B01000000, B11110111,
  B01000000, B11111011,
  B01000000, B11111110,
  B01000000, B11111101,

  B10000000, B01111111,
  B10000000, B10111111,
  B10000000, B11011111,
  B10000000, B11101111,
  B10000000, B11110111,
  B10000000, B11111011,
  B10000000, B11111110,
  B10000000, B11111101,

  B00001000, B01111111,
  B00001000, B10111111,
  B00001000, B11011111,
  B00001000, B11101111,
  B00001000, B11110111,
  B00001000, B11111011,
  B00001000, B11111110,
  B00001000, B11111101,

  B00000100, B01111111,
  B00000100, B10111111,
  B00000100, B11011111,
  B00000100, B11101111,
  B00000100, B11110111,
  B00000100, B11111011,
  B00000100, B11111110,
  B00000100, B11111101,

  B00000010, B01111111,
  B00000010, B10111111,
  B00000010, B11011111,
  B00000010, B11101111,
  B00000010, B11110111,
  B00000010, B11111011,
  B00000010, B11111110,
  B00000010, B11111101,

  B00000001, B01111111,
  B00000001, B10111111,
  B00000001, B11011111,
  B00000001, B11101111,
  B00000001, B11110111,
  B00000000, B11111111
};
const byte number[26]    = {
  B00000011, B10011111,
  B00100110, B00001110,
  B10011010, B01001010,
  B01000010, B00011111,
  B00000010, B00001010,
  B11111111,

  B00000001, B10011101,
  B00100100, B00001100,
  B10011000, B01001000,
  B01000000, B00011101,
  B00000000, B00001000,
  B11111111,

  B11111110, B11100110,
  B11000010, B11111101
};

const byte digit[4]      = {
  B00100000, B00010000,
  B01000000, B10000000
};

unsigned int serialValue          = 0;
unsigned int newserialValue       = 0;

unsigned int LedserialValue       = 0;
unsigned int newLedserialValue    = 0;

unsigned int DigitserialValue     = 0;
unsigned int newDigitserialValue  = 0;

unsigned int TimeserialValue      = 0;

int       TemperatureserialValue  = 0;
boolean negativeTemperatureActive = 0;

unsigned int WindforceserialValue = 0;

unsigned int serialFlag           = 0;

unsigned int ledValue             = 1;
unsigned int rowValue             = 0;

unsigned int newledValue          = 1;
unsigned int newrowValue          = 0;

unsigned int firstDigit           = 8;
unsigned int secondDigit          = 8;
unsigned int thirdDigit           = 8;
unsigned int fourthDigit          = 8;

unsigned int newfirstDigit       = 8;
unsigned int newsecondDigit      = 8;
unsigned int newthirdDigit       = 8;
unsigned int newfourthDigit      = 8;

volatile boolean toggle1         = 1;
volatile boolean toggle2         = 1;
boolean flashing                 = 1;
boolean toggleLed                = 1;
boolean toggleDigit              = 1;

boolean beep                     = 0;
boolean toggleCountactive        = 0;
unsigned int toggleCount         = 0;



void setup() {

     MCUCSR|=(1<<JTD);                                             // Disable JTAG for activate PORTC 5..3

     MCUCSR|=(1<<JTD);                                             // for output usage. Instruction needs twice

     cli();                                                        // Disable interrupts

     DDRA  = B11111111;                                            // Set PORT A inputs/outputs

     DDRB  = B11111111;                                            // Set PORT B inputs/outputs

     DDRC  = B11111000;                                            // Set PORT C inputs/outputs

     DDRD  = B10000000;                                            // Set PORT D inputs/outputs

     PORTA = B11111111;                                            // Set PORT A pullups

     PORTB = B11111111;                                            // Set PORT B pullups

     PORTC = B11110000;                                            // Set PORT C pullups

     PORTD = B10000000;                                            // Set PORT D pullups

     setTimer1();
     
     setTimer2(); 

     sei();                                                        // Enable global interrupts
     
     Serial.begin(9600);                                           // Start serial communication

     delay(50);                                                    // Wait a while

}

void setTimer1() {

     TCCR1A  = 0;                                                  // Set the registers;

     TCCR1B  = (1 << WGM12);                                       // Set mode

     TCCR1B |= (1 << CS12) |(0 << CS11) | (1 << CS10);             // Set prescale values

     TIMSK  |= (1 << OCIE1A);                                      // TIMSK for Timer 1

     OCR1A   = 2048;                                               // Set delay
}


void setTimer2() {
  
     TCCR2 |= (1 << WGM21)|(0 << WGM20);

     TCCR2 |= (1 << CS22)| (0 << CS21) | (0 << CS20);              // Set prescale value

     TCNT2 = 0;

     TIMSK |= (1 << TOIE2);
 
     OCR2   = 192;                                   // Set duty cycle

}



     ISR (TIMER1_COMPA_vect) {
         toggle1 = !toggle1;
     }

     ISR (TIMER2_OVF_vect) {
         toggle2 = !toggle2;

         if (toggle2 == 1 && toggleCountactive == 1) { PORTC |= B00001000;
         }
         
         if (toggle2 == 0 && toggleCountactive ==1) { PORTC &= ~B00001000;
         }

         }

     


void loop() {

     if (Serial.available() == 2) {                                // Check if serial is received

        serialValue  = Serial.read() << 8;                         // Read value and bitshift
        serialValue += Serial.read();                              // Read value

        serialFlag = 1;                                            // Set flag if serial is compleet
      }

     if (serialFlag == 1) {                                        // When flag is set

        if (serialValue < 360) {                                   // Calculate present led value

           toggleLed = 0;                                          // No flashing

           LedserialValue = serialValue;                           // Move variable

           rowValue = round (LedserialValue / 6) * 2;                    // Calculate
           ledValue = round ((LedserialValue / 6) * 2) + 1;              // Calculate
        }

        if (serialValue > 511 && serialValue < 872) {              // Calculate present 7-segment value

           toggleDigit = 0;                                        // No flashing

           DigitserialValue = serialValue - 512;                   // Move variable

           firstDigit  = 10;                                       // Calculate 
           secondDigit = DigitserialValue / 100;
           thirdDigit  = (DigitserialValue - (secondDigit * 100)) / 10;
           fourthDigit = DigitserialValue - (secondDigit * 100) - (thirdDigit * 10);

           if (DigitserialValue < 100) {                           // Check for "0"

              secondDigit = 10;
           }

           if (DigitserialValue < 10) {                            // Check for "0"

              thirdDigit = 10;
           }

        }

        if (serialValue > 1023 && serialValue < 3384) {            // Calculate 7-segment time

           toggleDigit = 0;                                        // No flashing

           TimeserialValue = serialValue - 1024;                   // Move variable

           firstDigit  = TimeserialValue / 1000;                   // Calculate
           secondDigit = (TimeserialValue - (firstDigit * 1000)) / 100;
           thirdDigit  = (TimeserialValue - (firstDigit * 1000) - (secondDigit * 100)) / 10;
           fourthDigit = TimeserialValue - (firstDigit * 1000) - (secondDigit * 100) - (thirdDigit * 10);

           secondDigit = secondDigit + 11;                         // Adding ":"

           if (TimeserialValue < 1000) {                           // Check for "0"

              firstDigit = 10;
           }
        }

        if (serialValue > 4095 && serialValue < 4456) {            // Calculate destination led value

           toggleLed = 1;                                          // Flashing

           newLedserialValue = serialValue - 4096;                 // Move variable

           newrowValue = round (newLedserialValue / 6) * 2;              // Calculate
           newledValue = round ((newLedserialValue / 6) * 2) + 1;        // Calculate

        }

        if (serialValue > 8191 && serialValue < 8552) {            // Calculate destination 7-segment value

           toggleDigit = 1;                                        // Flashing

           newDigitserialValue = serialValue - 8192;               // Move variable

           newfirstDigit  = 10;                                    // Calculate
           newsecondDigit = newDigitserialValue / 100;
           newthirdDigit  = (newDigitserialValue - (newsecondDigit * 100)) / 10;
           newfourthDigit = newDigitserialValue - (newsecondDigit * 100) - (newthirdDigit * 10);

           if (newDigitserialValue < 100) {                        // Check for "0"

              newsecondDigit = 10;
           }

           if (newDigitserialValue < 10) {                         // Check for "0"

              newthirdDigit = 10;
           }
        }

        if (serialValue > 16383 && serialValue < 16585) {          // Calculate 7-segment temperature

           toggleDigit = 0;                                        // No flashing

           TemperatureserialValue = serialValue - 16434;           // Move variable

        if (TemperatureserialValue < 0) {                          // Check for negative
           TemperatureserialValue = abs(TemperatureserialValue);
           negativeTemperatureActive = 1;
        }

           firstDigit  = TemperatureserialValue / 100;             // Calculate
           secondDigit = (TemperatureserialValue - (firstDigit * 100)) / 10;
           thirdDigit  = TemperatureserialValue - (firstDigit * 100) - (secondDigit * 10);
           fourthDigit = 23;                                       // Adding "c"

           if (TemperatureserialValue < 100) {                     // Check for "0"

              firstDigit = 10;
           }

           if (TemperatureserialValue < 10) {                      // Check for "0"

              secondDigit = 10;
           }

           if (negativeTemperatureActive == 1) {                   // Adding "-"

              firstDigit = 22;
              negativeTemperatureActive = 0;
           }
        }

        if (serialValue > 24575 && serialValue < 24590) {          // Calculate 7-segment windforce

           toggleDigit = 0;                                        // No flashing

           WindforceserialValue = serialValue - 24576;             // Move variable

           firstDigit  = WindforceserialValue / 10;                // Calculate
           secondDigit = WindforceserialValue  - (firstDigit * 10);
           thirdDigit =  10;
           fourthDigit = 24;                                       // Adding "b"

           if (WindforceserialValue < 10) {                        // Check for "0"

              firstDigit = 10;
           }
        }

        if (serialValue == 32768) {                                // Calculate beep
           beep = 1;                                               // Beep active
        }

        if (serialValue == 32769) {                                // Deactivate present led value

           toggleLed = 0;                                          // No flashing

           rowValue = 122;                                         // Adding "blank" pixel
           ledValue = 123;                                         // Adding "blank" pixel
        }

        if (serialValue == 32770) {                                // Deactivate present 7-segment value

           toggleDigit = 0;                                        // No flashing

           firstDigit  = 10;                                       // Adding "blank" digit
           secondDigit = 10;                                       // Adding "blank" digit
           thirdDigit  = 10;                                       // Adding "blank" digit
           fourthDigit = 10;                                       // Adding "blank" digit
        }

        if (serialValue == 32771) {                                // Deactivate 7-segment time

           toggleDigit = 0;                                        // No flash

           firstDigit  = 10;                                       // Adding "blank" digit
           secondDigit = 25;                                       // Adding ":"
           thirdDigit  = 10;                                       // Adding "blank" digit
           fourthDigit = 10;                                       // Adding "blank" digit
         }

        if (serialValue == 32772) {                                // Deactivate destination led value

           toggleLed = 1;                                          // Flashing

           newrowValue = 122;                                      // Adding "blank" pixel
           newledValue = 123;                                      // Adding "blank" pixel
        }

        if (serialValue == 32773) {                                // Deactivate destination 7-segment value

           toggleDigit = 1;                                        // Flashing

           newfirstDigit  = 10;                                    // Adding "blank" digit
           newsecondDigit = 10;                                    // Adding "blank" digit
           newthirdDigit  = 10;                                    // Adding "blank" digit
           newfourthDigit = 10;                                    // Adding "blank" digit
        }

        if (serialValue == 32774) {                                // Deactivate 7-segment temperature

           toggleDigit = 0;                                        // No flash

           firstDigit  = 10;                                       // Adding "blank" digit
           secondDigit = 10;                                       // Adding "blank" digit
           thirdDigit  = 10;                                       // Adding "blank" digit
           fourthDigit = 23;                                       // Adding "c"
        }

        if (serialValue == 32775) {                                // Deactivate 7-segment windforce

           toggleDigit = 0;                                        // No flash

           firstDigit  = 10;                                       // Adding "blank" digit
           secondDigit = 10;                                       // Adding "blank" digit
           thirdDigit =  10;                                       // Adding "blank" digit
           fourthDigit = 24;                                       // Adding "b"
        }

        serialFlag = 0;                                            // Reset flag
     }

     if ((rowValue == newrowValue) && (ledValue == newledValue)) { // If both leds are same stop blinking

        toggleLed = 0;                                             // No flashing
        flashing = 1;                                              // Keep interrupt high
     }

     else {

          toggleLed = 1;                                           // Flashing
     }
     
     if (beep == 1) {                                              // When beep is active

        toggleCountactive = 1;                                     // Activate counter

        beep = 0;                                                  // Reset beep
     }

     if (toggleCountactive == 1) {                                 // When counter active

        toggleCount = toggleCount + 1;                             // Calculate
     }

     if (toggleCount > 15) {                                       // When counting reach value
        
        toggleCountactive = 0;                                     // Deactivate counter
        toggleCount = 0;                                           // Reset counting
     }
  
     flashing = toggle1;                                            // Interrupt to variable

     showLeds();                                                   // Show leds
     showDigits();                                                 // Show digits
}


void showLeds() {

     PORTC = B00000000;

     PORTB = pixel[rowValue];
     PORTA = pixel[ledValue];
     delay(1);

     if (toggleLed == 1) {            // Flash active

        if (flashing == 1) {          // Flash led on

           PORTB = pixel[newrowValue];
           PORTA = pixel[newledValue];
           delay(1);
        }
   
        else {                           // Flash led off
     
             PORTA = B11111111;
             delay(1);
        }
     }
}


void showDigits() {
  
     PORTB = B00000000;

     if (toggleDigit == 0) {
     PORTC = digit[0];
     PORTA = number[firstDigit];
     delay(1);

     PORTC = digit[1];
     PORTA = number[secondDigit];
     delay(1);

     PORTC = digit[2];
     PORTA = number[thirdDigit];
     delay(1);

     PORTC = digit[3];
     PORTA = number[fourthDigit];
     delay(1);
     }

     if (toggleDigit == 1) {

        if (flashing == 1) {

           PORTC = digit[0];
           PORTA = number[newfirstDigit];
           delay(1);

           PORTC = digit[1];
           PORTA = number[newsecondDigit];
           delay(1);

           PORTC = digit[2];
           PORTA = number[newthirdDigit];
           delay(1);

           PORTC = digit[3];
           PORTA = number[newfourthDigit];
           delay(1);
        }

        else {

             PORTC = digit[0];
             PORTA = B11111111;
             delay(1);

             PORTC = digit[1];
             PORTA = B11111111;
             delay(1);

             PORTC = digit[2];
             PORTA = B11111111;
             delay(1);

             PORTC = digit[3];
             PORTA = B11111111;
             delay(1);
        }
     }
 }





