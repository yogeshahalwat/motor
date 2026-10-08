#include <Arduino.h>
#include <avr/wdt.h>
#include <avr/io.h>
#include <EEPROM.h>
#include <TM1637Display.h>



/*-------------------------- Pin Config ------------------------------*/
#define CLK             8   // TM1637 CLK pin
#define DIO             9   // TM1637 DIO pin
#define Relay_Signal    12  // Relay used in Project
#define encoderPinA     3   // encoder  CLK right
#define encoderPinB     2   // encoder  DT  left
#define AC_Detect       4   // Detect AC Supply
#define electriPhasePin 10  // Feedback from 3-phase motor
#define BAT_Backup      6   // Battery used as backup supply
/*---------------------------- End ------------------------------*/



/*-------------------------- Macros ------------------------------*/
#define addressMin        0
#define addressHour       4
#define addressContActive 8
/*---------------------------- End ------------------------------*/



/*-------------------------- Prototypes ------------------------------*/
void updateDisplay();
void doEncoderA();
void doEncoderB();
/*---------------------------- End ------------------------------*/



/*-------------------------- Variables ------------------------------*/
TM1637Display display(CLK, DIO);

volatile unsigned long lastInterruptTime = 0;
volatile unsigned long lastInterruptTime_B = 0;
const unsigned long debounceDelay = 2;

int ledState __attribute__((section(".noinit")));

int hours = 0, minutes = 0;
bool colonState = true;
bool countdownActive = false;
unsigned long lastInteractionTime = 0;
unsigned long lastBlinkTime = 0;
// unsigned long lastSaveTime = 0;
const uint8_t SEG_DONE[] = {
    SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,    // O
    SEG_A | SEG_E | SEG_F | SEG_G,                    // F
    SEG_A | SEG_E | SEG_F | SEG_G,                    // F
    0b00000000
};

volatile int encoderPos = 0;  
int lastReportedPos = 0; 
static bool rotating = false; 
volatile bool A_set = false;
volatile bool B_set = false;

volatile bool touchEncoder = false;
bool pauseColon = true;
bool blinkColon = false;
// bool oneTimeWrite = false;
// bool phaseModeWrite = false;
bool readEEPROM = false;
bool timResumed = false;
bool readyToGo = false;
bool conditionMet = false;
// bool trigger = true;
// bool phaseModeEncoderActive = true;

unsigned long elapseCounter = 0;
unsigned long currentMillis = 0;
// unsigned long previousMillis = 0;
unsigned long writeEEPROM = 0;

bool AC_Detect_Flag = false;
bool batBackupCount_Flag = false;
/*---------------------------- End ------------------------------*/



void setup() {
  Serial.begin(9600);
  pinMode(Relay_Signal, OUTPUT);
  pinMode(BAT_Backup, OUTPUT);
  pinMode(electriPhasePin, INPUT);
  pinMode(AC_Detect, INPUT_PULLUP);
  pinMode(encoderPinA, INPUT_PULLUP);
  pinMode(encoderPinB, INPUT_PULLUP);
  attachInterrupt(1, doEncoderA, CHANGE); //pin 3
  attachInterrupt(0, doEncoderB, CHANGE); //pin 2
  // wdt_enable(WDTO_8S);
  if (MCUSR & (1 << WDRF)) {  
      Serial.println("WDTR");
  }
  else {
      ledState = LOW;
  }
  MCUSR = 0;
  display.setBrightness(5);

  if(EEPROM.read(addressMin) != 255 && EEPROM.read(addressContActive) == true)
  {
    hours = EEPROM.read(addressHour);
    minutes = EEPROM.read(addressMin);
    countdownActive = EEPROM.read(addressContActive);
    pauseColon = false;
    readEEPROM = true;
    EEPROM.put(addressHour, 255);
    EEPROM.put(addressMin, 255);
    EEPROM.put(addressContActive, 255);
    timResumed = true;
    Serial.print("R&W!!");
  }
  updateDisplay();
  digitalWrite(Relay_Signal, ledState);
  if(countdownActive == true)
  {
    digitalWrite(Relay_Signal, 1);
  }
  Serial.println("Sys");
}



void loop() {
  rotating = true; 
  encoderPos = constrain(encoderPos, -1, 1);

  //--------------- AC Detect Pin ---------------------------
  if(digitalRead(AC_Detect) == LOW)
  {
    if(!batBackupCount_Flag)
    {
      digitalWrite(BAT_Backup, HIGH);
      batBackupCount_Flag = true;
    }
    if (!AC_Detect_Flag) 
    {
      writeEEPROM = millis(); 
      colonState = true;
      updateDisplay();
      AC_Detect_Flag = true;
    }

    if (AC_Detect_Flag && (millis() - writeEEPROM >= 55000)) 
    {
      if(hours>0 || minutes>0)
      {
        countdownActive = true;
        if(EEPROM.read(addressHour) != hours || EEPROM.read(addressMin) != minutes)
        {
          EEPROM.put(addressHour, hours);
          EEPROM.put(addressMin, minutes);
          EEPROM.put(addressContActive, countdownActive);
          Serial.println("tS!!!");
        }
      }
      delay(10);
      digitalWrite(BAT_Backup, LOW);
      AC_Detect_Flag = false;
    }
  }
  else
  {
    digitalWrite(BAT_Backup, HIGH);
    batBackupCount_Flag = true;
    AC_Detect_Flag = false;
  }

  //----------------- ElectriPhase Pin -----------------------
  if(digitalRead(electriPhasePin) == LOW)
  {
    if (!conditionMet) 
    {
      currentMillis = millis();
      conditionMet = true;
    }
    if(conditionMet && (millis() - currentMillis > 1000))
    {
      readyToGo = true;
    }
    
    if(readyToGo == true)
    {
      if (lastReportedPos != encoderPos) 
      {
        if(hours && encoderPos < 0 && minutes <= 0)
        {
          minutes = 60;
          minutes = minutes + (10*encoderPos);
          minutes = constrain(minutes, 0, 60);
          hours = hours - 1;
        }
        else
        {
          minutes = minutes + (10*encoderPos);
          minutes = constrain(minutes, 0, 60);  
          if(minutes >= 60)
          {
            hours = hours + 1;
            minutes = 0;
          }
        }
        countdownActive = false;  
        updateDisplay();
        encoderPos = 0;
        lastReportedPos = encoderPos;
        lastInteractionTime = millis();
      }
    }
  }
  else
  {
    conditionMet = false;
    if (lastReportedPos != encoderPos) 
    {
      if(hours && encoderPos < 0 && minutes <= 0)
      {
        minutes = 60;
        minutes = minutes + (10*encoderPos);
        minutes = constrain(minutes, 0, 60);
        hours = hours - 1;
      }
      else
      {
        minutes = minutes + (10*encoderPos);
        minutes = constrain(minutes, 0, 60);  
        if(minutes >= 60)
        {
          hours = hours + 1;
          minutes = 0;
        }
      }
      countdownActive = false;   
      if(blinkColon == true)
      {
        pauseColon = false;
      }
      else
      {
        pauseColon = true;
      }
      updateDisplay();
      encoderPos = 0;
      lastReportedPos = encoderPos;
      lastInteractionTime = millis();
    }

    if (countdownActive && millis() - elapseCounter >= 60000) 
    {
      elapseCounter = millis();
      if (minutes > 0) 
      {
        minutes--;
      } 
      else 
      {
        if (hours > 0) 
        {
          hours--;
          minutes = 59;
        } 
      }
    }

    if (!pauseColon && millis() - lastBlinkTime >= 1000)
    {
      lastBlinkTime = millis();
      colonState = !colonState;
      updateDisplay();
      // wdt_reset();
    }
  }

  if (!countdownActive && touchEncoder && (millis() - lastInteractionTime >= 3000) && (hours>0 || minutes>0) && (digitalRead(AC_Detect) == HIGH)) 
  {
    digitalWrite(Relay_Signal, 1);
    countdownActive = true;
    touchEncoder = false;
    pauseColon = false;
    blinkColon = true;
    elapseCounter = millis();
    if(timResumed == true)
    {
      timResumed = false;
    }
  }
}

/*--------------------------- Update Display ----------------------*/
void updateDisplay() 
{
  if (hours <= 0 && minutes <= 0 && countdownActive) 
  {
    countdownActive = false;
    pauseColon = true;
    blinkColon = false;
    display.setSegments(SEG_DONE);
    digitalWrite(Relay_Signal, 0);
  } 
  else 
  {
    if(hours <= 0 && minutes <= 0 && encoderPos < 0)
    {
      pauseColon = true;
      blinkColon = false;
      display.showNumberDecEx(0000, 0b01000000, true);
      digitalWrite(Relay_Signal, 0);
    }
    else
    {
      int timeValue = (hours * 100) + minutes;
      display.showNumberDecEx(timeValue, colonState ? 0b01000000 : 0, true);
    }
  }
}
/*---------------------------- End ------------------------------*/



/*------------------ Interrupt on A changing state ---------------*/
void doEncoderA() {
  unsigned long interruptTime = millis(); 

  if (interruptTime - lastInterruptTime > debounceDelay) {
    touchEncoder = true;
    bool currentA = digitalRead(encoderPinA);

    if (currentA != A_set) {
        A_set = currentA;
        if (A_set && !B_set) {
            encoderPos = 1;
        }
    }
    
    lastInterruptTime = interruptTime;
  }
}
/*---------------------------- End ------------------------------*/



/*------------------ Interrupt on V changing state ---------------*/
void doEncoderB() {
  unsigned long interruptTime = millis(); 

  if (interruptTime - lastInterruptTime_B > debounceDelay) {
    touchEncoder = true;
    bool currentB = digitalRead(encoderPinB);
    
    if (currentB != B_set) {
        B_set = currentB;
        if (B_set && !A_set) {
            encoderPos = -1; 
        }
    }
    
    lastInterruptTime_B = interruptTime; 
  }
}
/*---------------------------- End ------------------------------*/
