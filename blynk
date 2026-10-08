#define BLYNK_TEMPLATE_ID "TMPLlbGUS5X-"
#define BLYNK_TEMPLATE_NAME "Quickstart Template"
#define BLYNK_AUTH_TOKEN "d6FyTAf8rXVJcxvk9Ilc0IcbAo5KNdgJ"



//customer = udaibhan



#define BLYNK_HEARTBEAT 60

#define BLYNK_PRINT Serial
#define TINY_GSM_MODEM_SIM800
//#define TINY_GSM_MODEM_SIM7600   //SIMA7608 Compatible with SIM7600 AT instructions

#include <TinyGsmClient.h>
#include <BlynkSimpleTinyGSM.h>
//#include <EEPROM.h>
#define relayPin 13 //D13
#define switchPin 10  //D7
int d1 __attribute__ ((section(".noinit")));
int d2 __attribute__ ((section(".noinit")));




int switchState = HIGH;
int relayState = 1; 
void(* resetFunc) (void) = 0;

BlynkTimer timer;

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "d6FyTAf8rXVJcxvk9Ilc0IcbAo5KNdgJ";
// Your GPRS credentials
// Leave empty, if missing user or pass
char apn[]  = "internet";
char user[] = "";
char pass[] = "";
int v2 = 3;
int v3 = 3;
int v4 = 0;
int v6 = 0;
int v7 = 0;
int v9 = 0;
int ok = 8;
int fault = 7;
int status = 9;
int of = 12;//motor off pin
int v5 = 3;//motor off event
// Hardware Serial on Mega, Leonardo, Micro
//#define SerialAT Serial1

// or Software Serial on Uno, Nano
#include <SoftwareSerial.h>
#include <avr/wdt.h>
SoftwareSerial SerialAT(2, 3); // RX, TX

TinyGsm modem(SerialAT);
void scanSwitch()
{
 
int readSwitch=digitalRead(switchPin); 


if ((readSwitch == LOW) && (switchState != LOW)){
  relayState = !relayState;
  digitalWrite(relayPin, relayState);
  Blynk.virtualWrite(V1, relayState);  // Update V1 Button Widget
   }

 
}


BLYNK_WRITE(V1) {
  relayState = param.asInt();
  digitalWrite(relayPin, relayState);
  }



BLYNK_CONNECTED() {
  // Request the latest state from the server
  Serial.println("connected");
  Blynk.syncVirtual(V1);
  
 
}


#define       MAX_WAIT_TIME       150

uint16_t waitTime = 0;

void setup()
{

cli();//stop interrupts

//wdt_enable(WDTO_8S);


  //set timer1 interrupt at 1Hz
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for 1hz increments
  OCR1A = 15624;// = (16*10^6) / (1*1024) - 1 (must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS12 and CS10 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);  
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);


sei();//allow interrupts




 
  Serial.begin(9600);

  

  delay(10);

  // Set GSM module baud rate
 SerialAT.begin(9600);
  delay(3000);

  Serial.println("Reset");
  // Restart takes quite some time
  // To skip it, call init() instead of restart()
  Serial.println("Initializing modem...");
  modem.restart();

  // Unlock your SIM card with a PIN
  //modem.simUnlock("1234");
  Serial.println("binit");
  Blynk.begin(auth, modem, apn, user, pass, "blynk.cloud", 8080);
  Serial.begin(9600);

   
  pinMode(relayPin, OUTPUT);

  pinMode(switchPin,INPUT_PULLUP);

 //On power ON all Relays in OFF state
  digitalWrite(relayPin, relayState);
  
 
  timer.setInterval(500L,scanSwitch);
 int val=digitalRead(ok);
int vol=digitalRead(fault);
int on=digitalRead(status);
   int off=digitalRead(of);


   //wdt_enable(WDTO_8S);
}


int initCounter = 0;
void loop()

{

 //wdt_reset();

  
int off=digitalRead(of);
int val=digitalRead(ok);
int vol=digitalRead(fault);
int on=digitalRead(status);


if(d2!=14)
{
 d2=14;
 v2=0;
 v3=0;
}
else
{
}


if(val==LOW && v2<1)
{
  if(v6>10)
 {  
v2++;
Blynk.logEvent("all_ok");
}
 v6++;
}
else 

{
  v6 = 0;
}
if(val==LOW)
{
  Blynk.virtualWrite(V2, 1000000);
}
else
{
  Blynk.virtualWrite(V2, 0);
}
if(vol==LOW && v3<1)
{

  if(v4>10)
  {
    v3++;

    Blynk.logEvent("fault_detect");
  }
  v4++;
}
else
{ 
  v4 = 0;
  
}


if(vol==LOW)
{
  Blynk.virtualWrite(V3, 1000000); //LED 10 "HIGH"
}
else 
{
Blynk.virtualWrite(V3, 0);
}
if(on==LOW)
{
Blynk.virtualWrite(V4, 1000000); //LED 10 "HIGH"
}
else 
{
  Blynk.virtualWrite(V4, 0);
}
if(vol==HIGH)
{
 // v3=0;
  
}
else
{
}
if(val==HIGH)
{
//v2=0;
}
else
{
}
if(off==LOW)
{
v2=0;
v3=0;
}
else
{
}

if(off==LOW && v5<1)
{
if(v7>10)

{
  v5++;
Blynk.logEvent("motor_off");
v9=0;
}
v7++;
}
else
{
  v7 = 0;

}
if(off==HIGH)
{
  v9++;
}
if(v9>20)
{
v5=0;
}
else
{

}
if(d1!=34)
{ 
 initCounter++;
 if(initCounter>5)
 {
  Serial.println("Oneline event");
   d1=34;
   Blynk.logEvent("online_");
 }
}
else
{
}

//Serial.println(Blynk.connected());


if(Blynk.connected() == 0)
{

}
else
{
  waitTime = 0;
}
Blynk.run();
timer.run();
}



ISR(TIMER1_COMPA_vect){
  waitTime++;
  if(waitTime>MAX_WAIT_TIME)
  {
    resetFunc();
  }
  
 //  wdt_reset();

}
