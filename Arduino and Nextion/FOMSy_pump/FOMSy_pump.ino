#include <Nextion.h>          // Nextion Library by Bentley Born https://github.com/bborncr/nextion
#include <SoftwareSerial.h>   // Software Serial Port (included in Arduino IDE "no download")
SoftwareSerial nextion(50, 51);// Nextion TX connected to Arduino pin 50 and RX to pin 51

Nextion myNextion(nextion, 9600); // Setup of Nextion library with name NextionLCD at 9600bps

// X Motor on shield, define pins / motor syringe A
#define stepa A0
#define dira A1
#define enablea 38
#define stopa 2   //X+ on  Shield

// Y Motor on  shield, define pins / motor syringe B
#define stepb A6
#define dirb A7
#define enableb A2
#define stopb 3

// signal in from pressure sensor
#define presstooa A11
#define presstoob A12

long delaya = 240384;
long delayb = 240384;
long flowa = 5;
long flowb = 5;
byte flowdeza = 0;
byte flowdezb = 0;
unsigned long flowfina = 0;
unsigned long flowfinb = 0;
byte diaa = 0;
byte diab = 0;
byte diadeza = 0;
byte diadezb = 0;
unsigned long radfina = 0; //final radius * 1000
unsigned long radfinb = 0; //final radius * 1000
unsigned long syringefactora = 12732;  //syringefactor in µm/mL
unsigned long syringefactorb = 12732;  //syringefactor in µm/mL
unsigned long gearratio = 27;
unsigned long microsteps = 4;
unsigned long factor = 450000000; // factor from mm/mL --> µL/min for 1:1 ratio and microsteps = 1 (no microsteps)  (60*1000*1000 [µs] / (200 [steps per round  / 1,5 [mm per round, thread pitch] /1000)) =  450000000
unsigned long previousMicrosa = 0;
unsigned long previousMicrosb = 0;
unsigned long preva = 0;
unsigned long prevb = 0;
unsigned long addeda = 0;
unsigned long addedb = 0;
unsigned long pressuretimea = 0;
unsigned long pressuretimeb = 0;
unsigned long addedfina = 0;
unsigned long addedfinb = 0;

byte statea = 0;
byte stateb = 0;
byte timestatea = 0;
byte timestateb = 0;
byte toohigha = 0;
byte toohighb = 0;
byte pulla = 0;
byte pullb = 0;
byte pusha = 0;
byte pushb = 0;
unsigned long stepsa = 0;
unsigned long stepsb = 0;
unsigned long i = 0;
unsigned long j = 0;
unsigned long k = 0;
unsigned long l = 0;

String message;   // received message for Nextion LCD

void setup()
{
  // define pins
  pinMode(stepa, OUTPUT);
  pinMode(dira, OUTPUT);
  pinMode(enablea, OUTPUT);
  digitalWrite(enablea, HIGH);     //LOW = enable
  pinMode(stopa, INPUT);
  digitalWrite(stopa, HIGH); // turn on pullup resistors

  pinMode(stepb, OUTPUT);
  pinMode(dirb, OUTPUT);
  pinMode(enableb, OUTPUT);
  digitalWrite(enableb, HIGH);
  pinMode(stopb, INPUT);
  digitalWrite(stopb, HIGH); // turn on pullup resistors

  pinMode(presstooa, INPUT);
  digitalWrite(presstooa, HIGH); // turn on pullup resistors
  pinMode(presstoob, INPUT);
  digitalWrite(presstoob, HIGH); // turn on pullup resistors
  myNextion.init();

  factor = 450000000 / microsteps / gearratio; // calculate the factor with microsteps and gearratio

}
void loop() {

  //------------------------------------------------------------------------------------------
  //  buttons action
  //------------------------------------------------------------------------------------------

  message = myNextion.listen(); //check for message
  if (message != "") { // if a message is received...

    //------------------------------------------------------------------------------------------
    //  buttons action Pump A
    //------------------------------------------------------------------------------------------

    //--------------      start A      -------------------------------------------------------

    if ((message == "65 0 5 1 ffff ffff ffff") && (digitalRead(stopa) == 1)) {
      if ((delaya > 0) && (delaya < 30240 / microsteps / gearratio)) {
        statea = 0;
        timestatea = 0;
        digitalWrite(enablea, HIGH);     //HIGH = disable
        myNextion.sendCommand("vis flowtooa,1");
        myNextion.setComponentValue("main.tooa", 1);
        myNextion.setComponentValue("main.runninga", 0);
      }
      else {
        statea = 1;
        timestatea = 1;
        myNextion.sendCommand("vis flowtooa,0");
        myNextion.sendCommand("vis stopa,1");  //show stopa button
        myNextion.setComponentValue("main.runninga", 1);
      }
    }

    //--------------       stop A      -------------------------------------------------------
    if (message == "65 0 a 1 ffff ffff ffff") {
      statea = 0;
      timestatea = 0;
      digitalWrite(enablea, HIGH);     //HIGH = disable
      myNextion.sendCommand("vis stopa,0");  //hide stopa button
      myNextion.setComponentValue("main.runninga", 0);
      myNextion.setComponentValue("main.n8", addedfina);
    }

    //--------------       reset volume A     -------------------------------------------------------
    if (message == "65 0 18 1 ffff ffff ffff") {
      addeda = 0;
      addedfina = 0;
      myNextion.setComponentValue("main.n8", addedfina);
    }
    
    //--------------       update volume A     -------------------------------------------------------
    if (message == "65 0 1 1 ffff ffff ffff") {
      myNextion.setComponentValue("main.n8", addedfina);
    }

    //--------------       flow A save      -------------------------------------------------------
    if (message == "65 1 2 1 ffff ffff ffff") {

      delay(600);
      flowa = myNextion.getComponentValue("flowa.ratea");
      flowdeza = myNextion.getComponentValue("flowa.ratedeza");
      diaa = myNextion.getComponentValue("syA.diaa");
      diadeza = myNextion.getComponentValue("syA.diadeza");

      radfina = (diaa * 1000 + diadeza * 10) / 2;  //radius *1000
      syringefactora = 1000000000000 / ((radfina) * (radfina) * PI); //syringefactor in µm/mL

      flowfina = flowa * 10 + flowdeza;
      delaya = ((factor / (syringefactora * flowfina / 10000)));

      myNextion.setComponentValue("main.n0", flowa);
      myNextion.setComponentValue("main.n2", flowdeza);

      if ((delaya > 0) && (delaya < 30240 / microsteps / gearratio)) {
        statea = 0;
        timestatea = 0;
        digitalWrite(enablea, HIGH);     //HIGH = disable
        myNextion.sendCommand("vis flowtooa,1");
        myNextion.setComponentValue("main.tooa", 1);
        myNextion.setComponentValue("main.runninga", 0);
      }
      else {
        myNextion.sendCommand("vis flowtooa,0");
      }

    }
    //--------------       flowadez save      -------------------------------------------------------
    if (message == "65 2 2 1 ffff ffff ffff") {
      delay (600);
      flowa = myNextion.getComponentValue("flowa.ratea");
      flowdeza = myNextion.getComponentValue("flowa.ratedeza");
      diaa = myNextion.getComponentValue("syA.diaa");
      diadeza = myNextion.getComponentValue("syA.diadeza");

      radfina = (diaa * 1000 + diadeza * 10) / 2;  //radius *1000
      syringefactora = 1000000000000 / ((radfina) * (radfina) * PI); //syringefactor in µm/mL

      flowfina = flowa * 10 + flowdeza;
      delaya = ((factor / (syringefactora * flowfina / 10000)));

      myNextion.setComponentValue("main.n0", flowa);
      myNextion.setComponentValue("main.n2", flowdeza);

      if ((delaya > 0) && (delaya < 30240 / microsteps / gearratio)) {
        statea = 0;
        timestatea = 0;
        digitalWrite(enablea, HIGH);     //HIGH = disable
        myNextion.sendCommand("vis flowtooa,1");
        myNextion.setComponentValue("main.tooa", 1);
        myNextion.setComponentValue("main.runninga", 0);
      }
      else {
        myNextion.sendCommand("vis flowtooa,0");
      }
    }
    //--------------       syA save      -------------------------------------------------------
    if (message == "65 5 2 1 ffff ffff ffff") {
      delay (600);
      flowa = myNextion.getComponentValue("flowa.ratea");
      flowdeza = myNextion.getComponentValue("flowa.ratedeza");
      diaa = myNextion.getComponentValue("syA.diaa");
      diadeza = myNextion.getComponentValue("syA.diadeza");

      radfina = (diaa * 1000 + diadeza * 10) / 2;  //radius *1000
      syringefactora = 1000000000000 / ((radfina) * (radfina) * PI); //syringefactor in µm/mL

      flowfina = flowa * 10 + flowdeza;
      delaya = ((factor / (syringefactora * flowfina / 10000)));
      
      myNextion.setComponentValue("main.n4", diaa);
      myNextion.sendCommand("main.t6.txt=syA.t13.txt");


      if ((delaya > 0) && (delaya < 30240 / microsteps / gearratio)) {
        statea = 0;
        timestatea = 0;
        digitalWrite(enablea, HIGH);     //HIGH = disable
        myNextion.sendCommand("vis flowtooa,1");
        myNextion.setComponentValue("main.tooa", 1);
        myNextion.setComponentValue("main.runninga", 0);
      }
      else {
        myNextion.sendCommand("vis flowtooa,0");
      }
    }

    //--------------       pressure A restart     -------------------------------------------------------

    if (message == "65 0 23 1 ffff ffff ffff") {

      toohigha = 0;
      myNextion.sendCommand("vis pressurea,0");
      myNextion.setComponentValue("main.presstooa", 0);
    }

    //--------------       manual stop A      -------------------------------------------------------
    if (message == "65 b 1 1 ffff ffff ffff") {
      statea = 0;
      timestatea = 0;
      digitalWrite(enablea, HIGH);     //HIGH = disable
      pulla = 0;
      pusha = 0;
      i = 0; j = 0;
    }

    //--------------      manual A pull touch    -------------------------------------------------------
    if (message == "65 b 3 1 ffff ffff ffff") {
      statea = 0;
      timestatea = 0;
      pusha = 0;
      pulla = 1;
      stepsa = 8000 * microsteps * gearratio; //maximal pull ditance = 60 mm  // steps per mm: 133 (without gear and microsteps)
    }
    //--------------       manual A pull release      -------------------------------------------------------
    if (message == "65 b 3 0 ffff ffff ffff") {
      pulla = 0;
    }

    //--------------       // manual A push touch      -------------------------------------------------------
    if (message == "65 b 4 1 ffff ffff ffff") {
      statea = 0;
      timestatea = 0;
      pulla = 0;
      pusha = 1;
      stepsa = 8000 * microsteps * gearratio; //maximal pull ditance = 60 mm = 216000
    }

    //--------------       manual A push release     -------------------------------------------------------
    if (message == "65 b 4 0 ffff ffff ffff") {
      pusha = 0;
    }
    //--------------       manual A pull 10 mm     -------------------------------------------------------
    if (message == "65 b 8 1 ffff ffff ffff") {
      pusha = 0;
      statea = 0;
      timestatea = 0;
      pulla = 1;
      stepsa = 1333 * microsteps * gearratio; // steps per mm: 133,3 (without gear and microsteps)
    }
    //--------------       manual A push 10 mm     -------------------------------------------------------
    if (message == "65 b a 1 ffff ffff ffff") {
      pulla = 0;
      statea = 0;
      timestatea = 0;
      pusha = 1;
      stepsa = 1333 * microsteps * gearratio; // steps per mm: 133,3 (without gear and microsteps)
    }
    //--------------       manual A pull 20 mm     -------------------------------------------------------
    if (message == "65 b 9 1 ffff ffff ffff") {
      pusha = 0;
      statea = 0;
      timestatea = 0;
      pulla = 1;
      stepsa = 2666 * microsteps * gearratio; // steps per mm: 133,3 (without gear and microsteps)
    }
    //--------------       manual A push 20 mm     -------------------------------------------------------
    if (message == "65 b b 1 ffff ffff ffff") {
      pulla = 0;
      statea = 0;
      timestatea = 0;
      pusha = 1;
      stepsa = 2666 * microsteps * gearratio;
    }
    //--------------       manual A pull 40 mm     -------------------------------------------------------
    if (message == "65 b c 1 ffff ffff ffff") {
      pusha = 0;
      statea = 0;
      timestatea = 0;
      pulla = 1;
      stepsa = 5333 * microsteps * gearratio;
    }
    //--------------       manual A push 40 mm     -------------------------------------------------------
    if (message == "65 b e 1 ffff ffff ffff") {
      pulla = 0;
      statea = 0;
      timestatea = 0;
      pusha = 1;
      stepsa = 5333 * microsteps * gearratio;
    }
    //--------------       manual A pull 60 mm     -------------------------------------------------------
    if (message == "65 b d 1 ffff ffff ffff") {
      pusha = 0;
      statea = 0;
      timestatea = 0;
      pulla = 1;
      stepsa = 8000 * microsteps * gearratio;
    }
    //--------------       manual A push 60 mm     -------------------------------------------------------
    if (message == "65 b f 1 ffff ffff ffff") {
      pulla = 0;
      statea = 0;
      timestatea = 0;
      pusha = 1;
      stepsa = 8000 * microsteps * gearratio;
    }

    //------------------------------------------------------------------------------------------
    //  buttons action Pump B
    //------------------------------------------------------------------------------------------

    //--------------       start B      -------------------------------------------------------
    if ((message == "65 0 6 1 ffff ffff ffff") && (digitalRead(stopb) == 1)) {

      if ((delayb > 0) && (delayb < 30240 / microsteps / gearratio)) {
        stateb = 0;
        timestateb = 0;
        digitalWrite(enableb, HIGH);     //HIGH = disable
        myNextion.sendCommand("vis flowtoob,1");
        myNextion.setComponentValue("main.toob", 1);
        myNextion.setComponentValue("main.runningb", 0);

      }
      else {
        stateb = 1;
        timestateb = 1;
        myNextion.sendCommand("vis flowtoob,0");
        myNextion.sendCommand("vis stopb,1");  //show stopa button
        myNextion.setComponentValue("main.runningb", 1);
      }
    }
    //--------------       stop B      -------------------------------------------------------
    if (message == "65 0 b 1 ffff ffff ffff") {
      stateb = 0;
      timestateb = 0;
      digitalWrite(enableb, HIGH);     //HIGH = disable
      myNextion.sendCommand("vis stopb,0");  //hide stop button
      myNextion.setComponentValue("main.runningb", 0);
      myNextion.setComponentValue("main.n9", addedfinb);

    }

    //--------------       reset volume B     -------------------------------------------------------
    if (message == "65 0 1a 1 ffff ffff ffff") {
      addedb = 0;
      addedfinb = 0;
      myNextion.setComponentValue("main.n9", addedfinb);

    }
    //--------------       update volume B     -------------------------------------------------------
    if (message == "65 0 2 1 ffff ffff ffff") {
      myNextion.setComponentValue("main.n9", addedfinb);
    }


    //--------------       flow B save      -------------------------------------------------------
    if (message == "65 3 2 1 ffff ffff ffff") {
      delay (600);
      flowb = myNextion.getComponentValue("flowb.rateb");
      flowdezb = myNextion.getComponentValue("flowb.ratedezb");
      diab = myNextion.getComponentValue("syB.diab");
      diadezb = myNextion.getComponentValue("syB.diadezb");

      radfinb = (diab * 1000 + diadezb * 10) / 2;  //radius *1000
      syringefactorb = 1000000000000 / ((radfinb) * (radfinb) * PI); //syringefactor in µm/mL

      flowfinb = flowb * 10 + flowdezb;
      delayb = ((factor / (syringefactorb * flowfinb / 10000)));

      myNextion.setComponentValue("main.n1", flowb);
      myNextion.setComponentValue("main.n3", flowdezb);

      if ((delayb > 0) && (delayb < 30240 / microsteps / gearratio)) {
        stateb = 0;
        timestateb = 0;
        digitalWrite(enableb, HIGH);     //HIGH = disable
        myNextion.sendCommand("vis flowtoob,1");
        myNextion.setComponentValue("main.toob", 1);
        myNextion.setComponentValue("main.runningb", 0);
      }
      else {
        myNextion.sendCommand("vis flowtoob,0");
      }
    }
    //--------------       flowbdez save      -------------------------------------------------------
    if (message == "65 4 2 1 ffff ffff ffff") {
      delay (600);
      flowb = myNextion.getComponentValue("flowb.rateb");
      flowdezb = myNextion.getComponentValue("flowb.ratedezb");
      diab = myNextion.getComponentValue("syB.diab");
      diadezb = myNextion.getComponentValue("syB.diadezb");

      radfinb = (diab * 1000 + diadezb * 10) / 2;  //radius *1000
      syringefactorb = 1000000000000 / ((radfinb) * (radfinb) * PI); //syringefactor in µm/mL

      flowfinb = flowb * 10 + flowdezb;
      delayb = ((factor / (syringefactorb * flowfinb / 10000)));  //

      myNextion.setComponentValue("main.n1", flowb);
      myNextion.setComponentValue("main.n3", flowdezb);

      if ((delayb > 0) && (delayb < 30240 / microsteps / gearratio)) {
        stateb = 0;
        timestateb = 0;
        digitalWrite(enableb, HIGH);     //HIGH = disable
        myNextion.sendCommand("vis flowtoob,1");
        myNextion.setComponentValue("main.toob", 1);
        myNextion.setComponentValue("main.runningb", 0);
      }
      else {
        myNextion.sendCommand("vis flowtoob,0");
      }
    }
    //--------------       syB save      -------------------------------------------------------
    if (message == "65 8 2 1 ffff ffff ffff") {
      delay (600);
      flowb = myNextion.getComponentValue("flowb.rateb");
      flowdezb = myNextion.getComponentValue("flowb.ratedezb");
      diab = myNextion.getComponentValue("syB.diab");
      diadezb = myNextion.getComponentValue("syB.diadezb");

      radfinb = (diab * 1000 + diadezb * 10) / 2;  //radius *1000
      syringefactorb = 1000000000000 / ((radfinb) * (radfinb) * PI); //syringefactor in µm/mL

      flowfinb = flowb * 10 + flowdezb;
      delayb = ((factor / (syringefactorb * flowfinb / 10000)));

      myNextion.setComponentValue("main.n6", diab);
      myNextion.sendCommand("main.t10.txt=syB.t13.txt");


      if ((delayb > 0) && (delayb < 30240 / microsteps / gearratio)) {
        stateb = 0;
        timestateb = 0;
        digitalWrite(enableb, HIGH);     //HIGH = disable
        myNextion.sendCommand("vis flowtoob,1");
        myNextion.setComponentValue("main.toob", 1);
        myNextion.setComponentValue("main.runningb", 0);
      }
      else {
        myNextion.sendCommand("vis flowtoob,0");
      }
    }
    //--------------       manual stop B      -------------------------------------------------------
    if (message == "65 b 2 1 ffff ffff ffff") {
      stateb = 0;
      timestateb = 0;
      digitalWrite(enableb, HIGH);     //HIGH = disable
      pullb = 0;
      pushb = 0;
      k = 0; l = 0;
    }

    //--------------      manual b pull touch    -------------------------------------------------------
    if (message == "65 b 5 1 ffff ffff ffff") {
      stateb = 0;
      timestateb = 0;
      pushb = 0;
      pullb = 1;
      stepsb = 8000 * microsteps * gearratio; //maximal pull ditance = 60 mm  // steps per mm: 133 (without gear and microsteps)
    }
    //--------------       manual b pull release      -------------------------------------------------------
    if (message == "65 b 5 0 ffff ffff ffff") {
      pullb = 0;
    }

    //--------------       // manual B push touch      -------------------------------------------------------
    if (message == "65 b 6 1 ffff ffff ffff") {
      stateb = 0;
      timestateb = 0;
      pullb = 0;
      pushb = 1;
      stepsb = 8000 * microsteps * gearratio; //maximal pull ditance = 60 mm = 216000
    }

    //--------------       manual B push release     -------------------------------------------------------
    if (message == "65 b 6 0 ffff ffff ffff") {
      pushb = 0;
    }
    //--------------       manual B pull 10 mm     -------------------------------------------------------
    if (message == "65 b 10 1 ffff ffff ffff") {
      pushb = 0;
      stateb = 0;
      timestateb = 0;
      pullb = 1;
      stepsb = 1333 * microsteps * gearratio; // steps per mm: 133,3 (without gear and microsteps)
    }
    //--------------       manual B push 10 mm     -------------------------------------------------------
    if (message == "65 b 12 1 ffff ffff ffff") {
      pullb = 0;
      stateb = 0;
      timestateb = 0;
      pushb = 1;
      stepsb = 1333 * microsteps * gearratio;
    }
    //--------------       manual B pull 20 mm     -------------------------------------------------------
    if (message == "65 b 11 1 ffff ffff ffff") {
      pushb = 0;
      stateb = 0;
      timestateb = 0;
      pullb = 1;
      stepsb = 2666 * microsteps * gearratio;
    }
    //--------------       manual B push 20 mm     -------------------------------------------------------
    if (message == "65 b 13 1 ffff ffff ffff") {
      pullb = 0;
      stateb = 0;
      timestateb = 0;
      pushb = 1;
      stepsb = 2666 * microsteps * gearratio;
    }
    //--------------       manual B pull 40 mm     -------------------------------------------------------
    if (message == "65 b 14 1 ffff ffff ffff") {
      pushb = 0;
      stateb = 0;
      timestateb = 0;
      pullb = 1;
      stepsb = 5333 * microsteps * gearratio;
    }
    //--------------       manual B push 40 mm     -------------------------------------------------------
    if (message == "65 b 16 1 ffff ffff ffff") {
      pullb = 0;
      stateb = 0;
      timestateb = 0;
      pushb = 1;
      stepsb = 5333 * microsteps * gearratio;
    }
    //--------------       manual B pull 60 mm     -------------------------------------------------------
    if (message == "65 b 15 1 ffff ffff ffff") {
      pushb = 0;
      stateb = 0;
      timestateb = 0;
      pullb = 1;
      stepsb = 8000 * microsteps * gearratio;
    }
    //--------------       manual B push 60 mm     -------------------------------------------------------
    if (message == "65 b 17 1 ffff ffff ffff") {
      pullb = 0;
      stateb = 0;
      timestateb = 0;
      pushb = 1;
      stepsb = 8000 * microsteps * gearratio;
    }
    
    //--------------       pressure B restart     -------------------------------------------------------
    if (message == "65 0 24 1 ffff ffff ffff") {

      toohighb = 0;
      myNextion.sendCommand("vis pressureb,0");
      myNextion.setComponentValue("main.presstoob", 0);

    }

  }  //message recieved

  //------------------------------------------------------------------------------------------
  //  pressure too high A
  //------------------------------------------------------------------------------------------

  if (millis() - pressuretimea >= 1000) {   //every second
    pressuretimea = millis();

    if (digitalRead(presstooa) == 0) {
      toohigha = 1;
      myNextion.sendCommand("vis pressurea,1");
      myNextion.sendCommand("vis stopa,0");
      myNextion.setComponentValue("main.presstooa", 1);
    }

    if (toohigha == 1) {    // if pressure too high on sensor A --> both pumps are stopped
      statea = 0;
      timestatea = 0;
      stateb = 0;
      timestateb = 0;
      digitalWrite(enablea, HIGH);     //HIGH = disable
      digitalWrite(enableb, HIGH);     //HIGH = disable
    }
  }

  //------------------------------------------------------------------------------------------
  //  pressure too high B + Endstops
  //------------------------------------------------------------------------------------------

  if (millis() - pressuretimeb >= 1000) {   //every second
    pressuretimeb = millis();

    if (digitalRead(presstoob) == 0) {
      toohighb = 1;
      myNextion.sendCommand("vis pressureb,1");
      myNextion.sendCommand("vis stopb,0");
      myNextion.setComponentValue("main.presstoob", 1);
    }

    if (toohighb == 1) {    // if pressure too high on sensor B --> both pumps are stopped
      statea = 0;
      timestatea = 0;
      stateb = 0;
      timestateb = 0;
      digitalWrite(enablea, HIGH);     //HIGH = disable
      digitalWrite(enableb, HIGH);     //HIGH = disable
    }

    //  Endstops__________when 1 end stop is pressed, both pumps are stopped

    if (digitalRead(stopa) == 0) {
      statea = 0;
      timestatea = 0;
      stateb = 0;
      timestateb = 0;
      pusha = 0;
      digitalWrite(enablea, HIGH);     //HIGH = disable
      digitalWrite(enableb, HIGH);     //HIGH = disable
      myNextion.sendCommand("vis stopa,0");  //hide stopa button
      myNextion.sendCommand("vis stopb,0");  //hide stopa button
      myNextion.setComponentValue("main.runninga", 0);
      myNextion.setComponentValue("main.runningb", 0);
      myNextion.setComponentValue("main.n8", addedfina);
      myNextion.setComponentValue("main.n9", addedfinb);

    }

    if (digitalRead(stopb) == 0) {
      statea = 0;
      timestatea = 0;
      stateb = 0;
      timestateb = 0;
      pushb = 0;
      digitalWrite(enablea, HIGH);     //HIGH = disable
      digitalWrite(enableb, HIGH);     //HIGH = disable
      myNextion.sendCommand("vis stopa,0");  //hide stopa button
      myNextion.sendCommand("vis stopb,0");  //hide stopa button
      myNextion.setComponentValue("main.runninga", 0);
      myNextion.setComponentValue("main.runningb", 0);
      myNextion.setComponentValue("main.n8", addedfina);
      myNextion.setComponentValue("main.n9", addedfinb);
    }
  }

  //------------------------------------------------------------------------------------------
  //  pump A run
  //------------------------------------------------------------------------------------------

  if (statea == 1) {
    digitalWrite(enablea, LOW);     //LOW = enable
    if (micros() - previousMicrosa >= delaya) {
      previousMicrosa = micros();

      digitalWrite(dira, HIGH);
      digitalWrite(stepa, HIGH);
      digitalWrite(stepa, LOW);
    }
  }

  //------------------------------------------------------------------------------------------
  //  pump B run
  //------------------------------------------------------------------------------------------

  if (stateb == 1) {
    digitalWrite(enableb, LOW);     //LOW = enable
    if (micros() - previousMicrosb >= delayb) {
      previousMicrosb = micros();

      digitalWrite(dirb, HIGH);
      digitalWrite(stepb, HIGH);
      digitalWrite(stepb, LOW);
    }
  }

  //------------------------------------------------------------------------------------------
  //  pump A pull
  //------------------------------------------------------------------------------------------

  if (pulla == 1) {
    digitalWrite(enablea, LOW);     //LOW = enable
    if (micros() - previousMicrosa >= 30240 / microsteps / gearratio) { //30240 µs is the minimum dalay for maximum speed, with microsteps 4 and gearratio 27 = 280 µs
      previousMicrosa = micros();
      digitalWrite(dira, LOW);  //pull
      digitalWrite(stepa, HIGH);
      digitalWrite(stepa, LOW);
      i++;
      if (i > stepsa) {
        pulla = 0;
        i = 0;
      }
    }
  }

  //------------------------------------------------------------------------------------------
  //  pump A push
  //------------------------------------------------------------------------------------------
  if (pusha == 1) {
    digitalWrite(enablea, LOW);     //LOW = enable
    if (micros() - previousMicrosa >= 30240 / microsteps / gearratio) {
      previousMicrosa = micros();
      digitalWrite(dira, HIGH);  //pull
      digitalWrite(stepa, HIGH);
      digitalWrite(stepa, LOW);
      j++;
      if (j > stepsa) {
        pusha = 0;
        j = 0;
      }
    }
  }

  //------------------------------------------------------------------------------------------
  //  pump B pull
  //------------------------------------------------------------------------------------------

  if (pullb == 1) {
    digitalWrite(enableb, LOW);     //LOW = enable
    if (micros() - previousMicrosb >= 30240 / microsteps / gearratio) {
      previousMicrosb = micros();
      digitalWrite(dirb, LOW);  //pull
      digitalWrite(stepb, HIGH);
      digitalWrite(stepb, LOW);
      k++;
      if (k > stepsb) {
        pullb = 0;
        k = 0;
      }
    }
  }

  //------------------------------------------------------------------------------------------
  //  pump B push
  //------------------------------------------------------------------------------------------
  if (pushb == 1) {
    digitalWrite(enableb, LOW);     //LOW = enable
    if (micros() - previousMicrosb >= 30240 / microsteps / gearratio) {
      previousMicrosb = micros();
      digitalWrite(dirb, HIGH);  //push
      digitalWrite(stepb, HIGH);
      digitalWrite(stepb, LOW);
      l++;
      if (l > stepsb) {
        pushb = 0;
        l = 0;
      }
    }
  }


  //------------------------------------------------------------------------------------------
  //  added vol A calculation and sending
  //------------------------------------------------------------------------------------------

  if (timestatea == 1) {
    if (millis() - preva >= 100) {
      preva = millis();
      addeda = flowfina + addeda;
      addedfina = addeda * 0.0001666; //1/6000 due to 100 ms frequency, 1/10 due to decimal = 0,0001666
    }
  }

  //------------------------------------------------------------------------------------------
  //  added vol B calculation and sending
  //------------------------------------------------------------------------------------------

  if (timestateb == 1) {
    if (millis() - prevb >= 100) {
      prevb = millis();
      addedb = flowfinb + addedb;
      addedfinb = addedb * 0.0001666; //1/6000 due to 100 ms frequency, 1/10 due to decimal = 0,0001666
    }
  }

} //-------------- loop --------------
