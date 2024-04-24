#include <Nextion.h>          // Nextion Library by Bentley Born https://github.com/bborncr/nextion
#include <SoftwareSerial.h>   // Software Serial Port (included in Arduino IDE "no download")
SoftwareSerial nextion(10, 11);// Nextion (TX, RX)
Nextion myNextion(nextion, 9600); // Setup of Nextion library with name NextionLCD at 9600bps

#define pina A0
#define pinb A1
#define tooa 7
#define toob 8
int buttonred = 2;
int buttongreen = 3;
int buttongreenstate = 0;
int buttonredstate = 0;
int rawa = 0;
int rawb = 0;
float bara = 0;
float barb = 0;
float maxa = 10;
float maxb = 10;
float cala = 0;
float calb = 0;
int statecal = 0;
unsigned long senda = 0;
unsigned long sendgrapha = 0;
unsigned long sendb = 0;
unsigned long sendgraphb = 0;
byte statea = 0;
byte stateb = 0;
byte maxmenua = 0;
byte maxmenub = 0;
//motor pins
int enA = 4;
int motorin1 = 5;
int motorin2 = 6;
String message;   // received message for Nextion LCD

void setup()
{
  digitalWrite(pina, HIGH); // turn on pullup resistors
  digitalWrite(pinb, HIGH); // turn on pullup resistors
  pinMode(tooa, OUTPUT);
  pinMode(toob, OUTPUT);
  pinMode(buttonred, INPUT_PULLUP);
  pinMode(buttongreen, INPUT_PULLUP);

  pinMode(enA, OUTPUT);
  pinMode(motorin1, OUTPUT);
  pinMode(motorin2, OUTPUT);

  myNextion.init();
  nextion.begin(9600); // open the serial port at 115200 bps:
}

void loop() {
  //------------------------------------------------------------------------------------------
  //  buttons action
  //------------------------------------------------------------------------------------------

  //--------------       button green pressed      -------------------------------------------------------

  if (digitalRead(buttongreen) == LOW) { //LOW = button pressed
    buttongreenstate = 1;
  } else {
    buttongreenstate = 0;
  }

  //--------------       button red pressed      -------------------------------------------------------

  if (digitalRead(buttonred) == LOW) { //LOW = button pressed
    buttonredstate = 1;
  } else {
    buttonredstate = 0;
  }

  //--------------------------- motor spins clockwise = increase pressure on BPR-----------------------------

  if (buttongreenstate == 1) {

    digitalWrite(motorin1, HIGH);
    digitalWrite(motorin2, LOW);
    digitalWrite(enA, 255);
  }

  //--------------------------- motor spins counterclockwise = decrease pressure on BPR-----------------------------

  if (buttonredstate == 1) {

    digitalWrite(motorin1, LOW);
    digitalWrite(motorin2, HIGH);
    digitalWrite(enA, 255);
  }

  //--------------------------- motor stops-----------------------------

  if (buttonredstate == 0 && buttongreenstate == 0) {

    digitalWrite(motorin1, LOW);
    digitalWrite(motorin2, LOW);
  }
  if (buttonredstate == 1 && buttongreenstate == 1) {

    digitalWrite(motorin1, LOW);
    digitalWrite(motorin2, LOW);
  }

  //--------------       nextion actions      -------------------------------------------------------


  message = myNextion.listen(); //check for message
  if (message != "") { // if a message is received...

    //--------------       start A      -------------------------------------------------------

    if (message == "65 0 4 1 ffff ffff ffff") {
      statea = 1;
      myNextion.sendCommand("vis stopa,1");  //show stopa button
      myNextion.sendCommand("va0.val=1");
    }
    //--------------       stop A      -------------------------------------------------------

    if (message == "65 0 12 1 ffff ffff ffff") {
      statea = 0;
      myNextion.sendCommand("vis stopa,0");  //hide stopa button
      myNextion.sendCommand("va0.val=0");
    }

    //--------------       reset A      -------------------------------------------------------

    if (message == "65 0 5 1 ffff ffff ffff") {
      myNextion.sendCommand("cle 1,0");
      myNextion.sendCommand("cle 1,2");
    }

    //--------------       start B      -------------------------------------------------------

    if (message == "65 0 6 1 ffff ffff ffff") {
      stateb = 1;
      myNextion.sendCommand("vis stopb,1");  //show stopa button
      myNextion.sendCommand("va1.val=1");
    }

    //--------------       stop B      -------------------------------------------------------

    if (message == "65 0 13 1 ffff ffff ffff") {
      stateb = 0;
      myNextion.sendCommand("vis stopb,0");  //hide stopa button
      myNextion.sendCommand("va1.val=0");
    }

    //--------------       reset B      -------------------------------------------------------

    if (message == "65 0 7 1 ffff ffff ffff") {
      myNextion.sendCommand("cle 1,1");
      myNextion.sendCommand("cle 1,3");
    }

    //--------------       save max B      -------------------------------------------------------

    if (message == "65 2 2 1 ffff ffff ffff") {
      delay(600);
      maxb = myNextion.getComponentValue("maxpressb.maxb");
      myNextion.setComponentValue("main.maxb", maxb);
    }

    //------------ Max Pressure menu A----------------

    if (message == "65 0 10 1 ffff ffff ffff") {
      maxmenua = 1;
    }

    do {
      message = myNextion.listen(); //check for message

      if (message == "65 1 2 1 ffff ffff ffff") {
        maxmenua = 0;
        senda = 0;      //sending pressure starts from beginning, otherwise it could be a conflict between saving max pressure and sending pressure
        sendb = 0;
        sendgrapha = 0;
        sendgraphb = 0;
        delay(200);
        maxa = myNextion.getComponentValue("maxpressa.maxa");
        myNextion.setComponentValue("main.maxa", maxa);
      }
    } while ( maxmenua == 1);

    //------------ Max Pressure menu B----------------

    if (message == "65 0 11 1 ffff ffff ffff") {
      maxmenub = 1;
    }

    do {

      message = myNextion.listen(); //check for message

      if (message == "65 2 2 1 ffff ffff ffff") {
        maxmenub = 0;
        senda = 0;  //sending pressure starts from beginning, otherwise it could be a conflict between saving max pressure and sending pressure
        sendb = 0;
        sendgrapha = 0;
        sendgraphb = 0;
        delay(200);
        maxb = myNextion.getComponentValue("maxpressb.maxb");
        myNextion.setComponentValue("main.maxb", maxb);
      }
    } while ( maxmenub == 1);


    //--------------       Calibration    -------------------------------------------------------

    //--------------       calibration menu      -------------------------------------------------------

    if (message == "65 0 1a 1 ffff ffff ffff") {
      delay(600);
      statecal = 1;
    }
    do {

      message = myNextion.listen(); //check for message

      if (message == "65 3 2 1 ffff ffff ffff") {
        delay(600);
        statecal = 0;
      }

      if (millis() - senda >= 1000) {
        senda = millis();
        cala = analogRead(A0);
        nextion.print("calibration.cala.txt=\"");
        nextion.print(cala, 0);
        nextion.print("\"");
        nextion.write(0xff);
        nextion.write(0xff);
        nextion.write(0xff);

        calb = analogRead(A1);
        nextion.print("calibration.calb.txt=\"");
        nextion.print(calb, 0);
        nextion.print("\"");
        nextion.write(0xff);
        nextion.write(0xff);
        nextion.write(0xff);
      }
    } while ( statecal == 1);

    //-------------- calibration end -------------------------------------------------------

  } //message

  //------------------------------------------------------------------------------------------
  //  pressure sensor A sending value  pressure too high
  //------------------------------------------------------------------------------------------

  if (millis() - senda >= 1000) {
    senda = millis();
    bara = conva(analogRead(A0));

    nextion.print("main.bara.txt=\"");
    nextion.print(bara, 2);
    nextion.print("\"");
    nextion.write(0xff);
    nextion.write(0xff);
    nextion.write(0xff);

    //--------------------------- pressure A too high-----------------------------

    if (bara > maxa) {
      myNextion.setComponentValue("main.tooa", 1);
      digitalWrite(tooa, LOW);
      myNextion.sendCommand("vis toopica,1");  //show too high
    }
    else {
      digitalWrite(tooa, HIGH);
      myNextion.sendCommand("vis toopica,0");  //hide too high
    }
  }

  //------------------------------------------------------------------------------------------
  //  pressure sensor A sending graph
  //------------------------------------------------------------------------------------------

  if (statea == 1) {
    if (millis() - sendgrapha >= 1000) {
      sendgrapha = millis();
      bara = conva(analogRead(A0));
      myNextion.setComponentValue("main.barvala", bara * 15.4);  //height 185 pixels /12 bar (maximum scale) = 15.4

      nextion.print("add 1,0,");       //add value to grapha (add ID, channel)
      nextion.print("barvala.val");
      nextion.write(0xff);
      nextion.write(0xff);
      nextion.write(0xff);

      myNextion.setComponentValue("main.barvala", (bara * 15.4) + 1); //height 185 pixels /12 bar (maximum scale) = 15.4
      nextion.print("add 1,2,");       //add value to graphb (add ID, channel)
      nextion.print("barvala.val");
      nextion.write(0xff);
      nextion.write(0xff);
      nextion.write(0xff);
    }
  }

  //------------------------------------------------------------------------------------------
  //  pressure sensor B sending value  pressure too high
  //------------------------------------------------------------------------------------------

  if (millis() - sendb >= 1000) {
    sendb = millis();
    barb = convb(analogRead(A1));

    nextion.print("main.barb.txt=\"");
    nextion.print(barb, 2);
    nextion.print("\"");
    nextion.write(0xff);
    nextion.write(0xff);
    nextion.write(0xff);

    //--------------------------- pressure A too high-----------------------------

    if (barb > maxb) {
      myNextion.setComponentValue("main.toob", 1);
      digitalWrite(toob, LOW);
      myNextion.sendCommand("vis toopicb,1");  //show too high
    }
    else {
      digitalWrite(toob, HIGH);
      myNextion.sendCommand("vis toopicb,0");  //hide too high
    }
  }

  //------------------------------------------------------------------------------------------
  //  pressure sensor B sending graph
  //------------------------------------------------------------------------------------------

  if (stateb == 1) {
    if (millis() - sendgraphb >= 1000) {
      sendgraphb = millis();
      barb = convb(analogRead(A1));

      myNextion.setComponentValue("main.barvalb", barb * 15.4);  //height 185 pixels /12 bar (maximum scale) = 15.4

      nextion.print("add 1,1,");       //add value to grapha (add ID, channel)
      nextion.print("barvalb.val");
      nextion.write(0xff);
      nextion.write(0xff);
      nextion.write(0xff);

      myNextion.setComponentValue("main.barvalb", (barb * 15.4) + 1); //for thicker line width

      nextion.print("add 1,3,");       //add value to graphb (add ID, channel)
      nextion.print("barvalb.val");
      nextion.write(0xff);
      nextion.write(0xff);
      nextion.write(0xff);
    }
  }

} // loop

float conva(int raw) {
  float bar;

  switch (raw) {    // 1024 cases
    case 1: bar = 316; break; case 2: bar = 185.86; break; case 3: bar = 135.56; break; case 4: bar = 108.16; break; case 5: bar = 90.7; break; case 6: bar = 78.5; break; case 7: bar = 69.44; break; case 8: bar = 62.42; break; case 9: bar = 56.81; break; case 10: bar = 52.21; break; case 11: bar = 48.36; break; case 12: bar = 45.08; break; case 13: bar = 42.26; break; case 14: bar = 39.8; break; case 15: bar = 37.64; break; case 16: bar = 35.71; break; case 17: bar = 34; break; case 18: bar = 32.45; break; case 19: bar = 31.05; break; case 20: bar = 29.77; break; case 21: bar = 28.61; break; case 22: bar = 27.53; break; case 23: bar = 26.55; break; case 24: bar = 25.63; break; case 25: bar = 24.78; break; case 26: bar = 23.99; break; case 27: bar = 23.26; break; case 28: bar = 22.57; break; case 29: bar = 21.92; break; case 30: bar = 21.31; break; case 31: bar = 20.74; break; case 32: bar = 20.2; break; case 33: bar = 19.68; break; case 34: bar = 19.2; break; case 35: bar = 18.74; break; case 36: bar = 18.3; break; case 37: bar = 17.89; break; case 38: bar = 17.49; break; case 39: bar = 17.11; break; case 40: bar = 16.75; break; case 41: bar = 16.4; break; case 42: bar = 16.07; break; case 43: bar = 15.76; break; case 44: bar = 15.45; break; case 45: bar = 15.16; break; case 46: bar = 14.88; break; case 47: bar = 14.61; break; case 48: bar = 14.35; break; case 49: bar = 14.1; break; case 50: bar = 13.86; break; case 51: bar = 13.63; break; case 52: bar = 13.4; break; case 53: bar = 13.18; break; case 54: bar = 12.98; break; case 55: bar = 12.77; break; case 56: bar = 12.58; break; case 57: bar = 12.39; break; case 58: bar = 12.2; break; case 59: bar = 12.02; break; case 60: bar = 11.85; break; case 61: bar = 11.68; break; case 62: bar = 11.52; break; case 63: bar = 11.36; break; case 64: bar = 11.2; break; case 65: bar = 11.05; break; case 66: bar = 10.91; break; case 67: bar = 10.76; break; case 68: bar = 10.63; break; case 69: bar = 10.49; break; case 70: bar = 10.36; break; case 71: bar = 10.23; break; case 72: bar = 10.11; break; case 73: bar = 9.98; break; case 74: bar = 9.87; break; case 75: bar = 9.75; break; case 76: bar = 9.64; break; case 77: bar = 9.53; break; case 78: bar = 9.42; break; case 79: bar = 9.31; break; case 80: bar = 9.21; break; case 81: bar = 9.11; break; case 82: bar = 9.01; break; case 83: bar = 8.91; break; case 84: bar = 8.82; break; case 85: bar = 8.72; break; case 86: bar = 8.63; break; case 87: bar = 8.55; break; case 88: bar = 8.46; break; case 89: bar = 8.37; break; case 90: bar = 8.29; break; case 91: bar = 8.21; break; case 92: bar = 8.13; break; case 93: bar = 8.05; break; case 94: bar = 7.97; break; case 95: bar = 7.9; break; case 96: bar = 7.82; break; case 97: bar = 7.75; break; case 98: bar = 7.68; break; case 99: bar = 7.61; break; case 100: bar = 7.54; break; case 101: bar = 7.47; break; case 102: bar = 7.4; break; case 103: bar = 7.34; break; case 104: bar = 7.27; break; case 105: bar = 7.21; break; case 106: bar = 7.15; break; case 107: bar = 7.09; break; case 108: bar = 7.02; break; case 109: bar = 6.97; break; case 110: bar = 6.91; break; case 111: bar = 6.85; break; case 112: bar = 6.79; break; case 113: bar = 6.74; break; case 114: bar = 6.68; break; case 115: bar = 6.63; break; case 116: bar = 6.58; break; case 117: bar = 6.52; break; case 118: bar = 6.47; break; case 119: bar = 6.42; break; case 120: bar = 6.37; break; case 121: bar = 6.32; break; case 122: bar = 6.28; break; case 123: bar = 6.23; break; case 124: bar = 6.18; break; case 125: bar = 6.13; break; case 126: bar = 6.09; break; case 127: bar = 6.04; break; case 128: bar = 6; break; case 129: bar = 5.96; break; case 130: bar = 5.91; break; case 131: bar = 5.87; break; case 132: bar = 5.83; break; case 133: bar = 5.79; break; case 134: bar = 5.75; break; case 135: bar = 5.71; break; case 136: bar = 5.67; break; case 137: bar = 5.63; break; case 138: bar = 5.59; break; case 139: bar = 5.55; break; case 140: bar = 5.51; break; case 141: bar = 5.48; break; case 142: bar = 5.44; break; case 143: bar = 5.4; break; case 144: bar = 5.37; break; case 145: bar = 5.33; break; case 146: bar = 5.3; break; case 147: bar = 5.26; break; case 148: bar = 5.23; break; case 149: bar = 5.19; break; case 150: bar = 5.16; break; case 151: bar = 5.13; break; case 152: bar = 5.09; break; case 153: bar = 5.06; break; case 154: bar = 5.03; break; case 155: bar = 5; break; case 156: bar = 4.97; break; case 157: bar = 4.94; break; case 158: bar = 4.91; break; case 159: bar = 4.88; break; case 160: bar = 4.85; break; case 161: bar = 4.82; break; case 162: bar = 4.79; break; case 163: bar = 4.76; break; case 164: bar = 4.73; break; case 165: bar = 4.7; break; case 166: bar = 4.68; break; case 167: bar = 4.65; break; case 168: bar = 4.62; break; case 169: bar = 4.59; break; case 170: bar = 4.57; break; case 171: bar = 4.54; break; case 172: bar = 4.52; break; case 173: bar = 4.49; break; case 174: bar = 4.46; break; case 175: bar = 4.44; break; case 176: bar = 4.41; break; case 177: bar = 4.39; break; case 178: bar = 4.36; break; case 179: bar = 4.34; break; case 180: bar = 4.32; break; case 181: bar = 4.29; break; case 182: bar = 4.27; break; case 183: bar = 4.25; break; case 184: bar = 4.22; break; case 185: bar = 4.2; break; case 186: bar = 4.18; break; case 187: bar = 4.15; break; case 188: bar = 4.13; break; case 189: bar = 4.11; break; case 190: bar = 4.09; break; case 191: bar = 4.07; break; case 192: bar = 4.05; break; case 193: bar = 4.02; break; case 194: bar = 4; break; case 195: bar = 3.98; break; case 196: bar = 3.96; break; case 197: bar = 3.94; break; case 198: bar = 3.92; break; case 199: bar = 3.9; break; case 200: bar = 3.88; break; case 201: bar = 3.86; break; case 202: bar = 3.84; break; case 203: bar = 3.82; break; case 204: bar = 3.8; break; case 205: bar = 3.78; break; case 206: bar = 3.77; break; case 207: bar = 3.75; break; case 208: bar = 3.73; break; case 209: bar = 3.71; break; case 210: bar = 3.69; break; case 211: bar = 3.67; break; case 212: bar = 3.66; break; case 213: bar = 3.64; break; case 214: bar = 3.62; break; case 215: bar = 3.6; break; case 216: bar = 3.59; break; case 217: bar = 3.57; break; case 218: bar = 3.55; break; case 219: bar = 3.53; break; case 220: bar = 3.52; break; case 221: bar = 3.5; break; case 222: bar = 3.48; break; case 223: bar = 3.47; break; case 224: bar = 3.45; break; case 225: bar = 3.44; break; case 226: bar = 3.42; break; case 227: bar = 3.4; break; case 228: bar = 3.39; break; case 229: bar = 3.37; break; case 230: bar = 3.36; break; case 231: bar = 3.34; break; case 232: bar = 3.33; break; case 233: bar = 3.31; break; case 234: bar = 3.3; break; case 235: bar = 3.28; break; case 236: bar = 3.27; break; case 237: bar = 3.25; break; case 238: bar = 3.24; break; case 239: bar = 3.22; break; case 240: bar = 3.21; break; case 241: bar = 3.19; break; case 242: bar = 3.18; break; case 243: bar = 3.17; break; case 244: bar = 3.15; break; case 245: bar = 3.14; break; case 246: bar = 3.12; break; case 247: bar = 3.11; break; case 248: bar = 3.1; break; case 249: bar = 3.08; break; case 250: bar = 3.07; break; case 251: bar = 3.06; break; case 252: bar = 3.04; break; case 253: bar = 3.03; break; case 254: bar = 3.02; break; case 255: bar = 3.01; break; case 256: bar = 2.99; break; case 257: bar = 2.98; break; case 258: bar = 2.97; break; case 259: bar = 2.96; break; case 260: bar = 2.94; break; case 261: bar = 2.93; break; case 262: bar = 2.92; break; case 263: bar = 2.91; break; case 264: bar = 2.89; break; case 265: bar = 2.88; break; case 266: bar = 2.87; break; case 267: bar = 2.86; break; case 268: bar = 2.85; break; case 269: bar = 2.83; break; case 270: bar = 2.82; break; case 271: bar = 2.81; break; case 272: bar = 2.8; break; case 273: bar = 2.79; break; case 274: bar = 2.78; break; case 275: bar = 2.77; break; case 276: bar = 2.76; break; case 277: bar = 2.74; break; case 278: bar = 2.73; break; case 279: bar = 2.72; break; case 280: bar = 2.71; break; case 281: bar = 2.7; break; case 282: bar = 2.69; break; case 283: bar = 2.68; break; case 284: bar = 2.67; break; case 285: bar = 2.66; break; case 286: bar = 2.65; break; case 287: bar = 2.64; break; case 288: bar = 2.63; break; case 289: bar = 2.62; break; case 290: bar = 2.61; break; case 291: bar = 2.6; break; case 292: bar = 2.59; break; case 293: bar = 2.58; break; case 294: bar = 2.57; break; case 295: bar = 2.56; break; case 296: bar = 2.55; break; case 297: bar = 2.54; break; case 298: bar = 2.53; break; case 299: bar = 2.52; break; case 300: bar = 2.51; break; case 301: bar = 2.5; break; case 302: bar = 2.49; break; case 303: bar = 2.48; break; case 304: bar = 2.47; break; case 305: bar = 2.46; break; case 306: bar = 2.45; break; case 307: bar = 2.44; break; case 308: bar = 2.43; break; case 309: bar = 2.42; break; case 310: bar = 2.41; break; case 311: bar = 2.41; break; case 312: bar = 2.4; break; case 313: bar = 2.39; break; case 314: bar = 2.38; break; case 315: bar = 2.37; break; case 316: bar = 2.36; break; case 317: bar = 2.35; break; case 318: bar = 2.34; break; case 319: bar = 2.34; break; case 320: bar = 2.33; break; case 321: bar = 2.32; break; case 322: bar = 2.31; break; case 323: bar = 2.3; break; case 324: bar = 2.29; break; case 325: bar = 2.28; break; case 326: bar = 2.28; break; case 327: bar = 2.27; break; case 328: bar = 2.26; break; case 329: bar = 2.25; break; case 330: bar = 2.24; break; case 331: bar = 2.24; break; case 332: bar = 2.23; break; case 333: bar = 2.22; break; case 334: bar = 2.21; break; case 335: bar = 2.2; break; case 336: bar = 2.2; break; case 337: bar = 2.19; break; case 338: bar = 2.18; break; case 339: bar = 2.17; break; case 340: bar = 2.17; break; case 341: bar = 2.16; break; case 342: bar = 2.15; break; case 343: bar = 2.14; break; case 344: bar = 2.14; break; case 345: bar = 2.13; break; case 346: bar = 2.12; break; case 347: bar = 2.11; break; case 348: bar = 2.11; break; case 349: bar = 2.1; break; case 350: bar = 2.09; break; case 351: bar = 2.08; break; case 352: bar = 2.08; break; case 353: bar = 2.07; break; case 354: bar = 2.06; break; case 355: bar = 2.05; break; case 356: bar = 2.05; break; case 357: bar = 2.04; break; case 358: bar = 2.03; break; case 359: bar = 2.03; break; case 360: bar = 2.02; break; case 361: bar = 2.01; break; case 362: bar = 2.01; break; case 363: bar = 2; break; case 364: bar = 1.99; break; case 365: bar = 1.99; break; case 366: bar = 1.98; break; case 367: bar = 1.97; break; case 368: bar = 1.97; break; case 369: bar = 1.96; break; case 370: bar = 1.95; break; case 371: bar = 1.95; break; case 372: bar = 1.94; break; case 373: bar = 1.93; break; case 374: bar = 1.93; break; case 375: bar = 1.92; break; case 376: bar = 1.91; break; case 377: bar = 1.91; break; case 378: bar = 1.9; break; case 379: bar = 1.89; break; case 380: bar = 1.89; break; case 381: bar = 1.88; break; case 382: bar = 1.88; break; case 383: bar = 1.87; break; case 384: bar = 1.86; break; case 385: bar = 1.86; break; case 386: bar = 1.85; break; case 387: bar = 1.85; break; case 388: bar = 1.84; break; case 389: bar = 1.83; break; case 390: bar = 1.83; break; case 391: bar = 1.82; break; case 392: bar = 1.82; break; case 393: bar = 1.81; break; case 394: bar = 1.8; break; case 395: bar = 1.8; break; case 396: bar = 1.79; break; case 397: bar = 1.79; break; case 398: bar = 1.78; break; case 399: bar = 1.77; break; case 400: bar = 1.77; break; case 401: bar = 1.76; break; case 402: bar = 1.76; break; case 403: bar = 1.75; break; case 404: bar = 1.75; break; case 405: bar = 1.74; break; case 406: bar = 1.73; break; case 407: bar = 1.73; break; case 408: bar = 1.72; break; case 409: bar = 1.72; break; case 410: bar = 1.71; break; case 411: bar = 1.71; break; case 412: bar = 1.7; break; case 413: bar = 1.7; break; case 414: bar = 1.69; break; case 415: bar = 1.69; break; case 416: bar = 1.68; break; case 417: bar = 1.68; break; case 418: bar = 1.67; break; case 419: bar = 1.66; break; case 420: bar = 1.66; break; case 421: bar = 1.65; break; case 422: bar = 1.65; break; case 423: bar = 1.64; break; case 424: bar = 1.64; break; case 425: bar = 1.63; break; case 426: bar = 1.63; break; case 427: bar = 1.62; break; case 428: bar = 1.62; break; case 429: bar = 1.61; break; case 430: bar = 1.61; break; case 431: bar = 1.6; break; case 432: bar = 1.6; break; case 433: bar = 1.59; break; case 434: bar = 1.59; break; case 435: bar = 1.58; break; case 436: bar = 1.58; break; case 437: bar = 1.57; break; case 438: bar = 1.57; break; case 439: bar = 1.56; break; case 440: bar = 1.56; break; case 441: bar = 1.55; break; case 442: bar = 1.55; break; case 443: bar = 1.54; break; case 444: bar = 1.54; break; case 445: bar = 1.53; break; case 446: bar = 1.53; break; case 447: bar = 1.53; break; case 448: bar = 1.52; break; case 449: bar = 1.52; break; case 450: bar = 1.51; break; case 451: bar = 1.51; break; case 452: bar = 1.5; break; case 453: bar = 1.5; break; case 454: bar = 1.49; break; case 455: bar = 1.49; break; case 456: bar = 1.48; break; case 457: bar = 1.48; break; case 458: bar = 1.48; break; case 459: bar = 1.47; break; case 460: bar = 1.47; break; case 461: bar = 1.46; break; case 462: bar = 1.46; break; case 463: bar = 1.45; break; case 464: bar = 1.45; break; case 465: bar = 1.44; break; case 466: bar = 1.44; break; case 467: bar = 1.44; break; case 468: bar = 1.43; break; case 469: bar = 1.43; break; case 470: bar = 1.42; break; case 471: bar = 1.42; break; case 472: bar = 1.41; break; case 473: bar = 1.41; break; case 474: bar = 1.41; break; case 475: bar = 1.4; break; case 476: bar = 1.4; break; case 477: bar = 1.39; break; case 478: bar = 1.39; break; case 479: bar = 1.38; break; case 480: bar = 1.38; break; case 481: bar = 1.38; break; case 482: bar = 1.37; break; case 483: bar = 1.37; break; case 484: bar = 1.36; break; case 485: bar = 1.36; break; case 486: bar = 1.36; break; case 487: bar = 1.35; break; case 488: bar = 1.35; break; case 489: bar = 1.34; break; case 490: bar = 1.34; break; case 491: bar = 1.34; break; case 492: bar = 1.33; break; case 493: bar = 1.33; break; case 494: bar = 1.32; break; case 495: bar = 1.32; break; case 496: bar = 1.32; break; case 497: bar = 1.31; break; case 498: bar = 1.31; break; case 499: bar = 1.3; break; case 500: bar = 1.3; break; case 501: bar = 1.3; break; case 502: bar = 1.29; break; case 503: bar = 1.29; break; case 504: bar = 1.29; break; case 505: bar = 1.28; break; case 506: bar = 1.28; break; case 507: bar = 1.27; break; case 508: bar = 1.27; break; case 509: bar = 1.27; break; case 510: bar = 1.26; break; case 511: bar = 1.26; break; case 512: bar = 1.26; break; case 513: bar = 1.25; break; case 514: bar = 1.25; break; case 515: bar = 1.24; break; case 516: bar = 1.24; break; case 517: bar = 1.24; break; case 518: bar = 1.23; break; case 519: bar = 1.23; break; case 520: bar = 1.23; break; case 521: bar = 1.22; break; case 522: bar = 1.22; break; case 523: bar = 1.22; break; case 524: bar = 1.21; break; case 525: bar = 1.21; break; case 526: bar = 1.21; break; case 527: bar = 1.2; break; case 528: bar = 1.2; break; case 529: bar = 1.2; break; case 530: bar = 1.19; break; case 531: bar = 1.19; break; case 532: bar = 1.18; break; case 533: bar = 1.18; break; case 534: bar = 1.18; break; case 535: bar = 1.17; break; case 536: bar = 1.17; break; case 537: bar = 1.17; break; case 538: bar = 1.16; break; case 539: bar = 1.16; break; case 540: bar = 1.16; break; case 541: bar = 1.15; break; case 542: bar = 1.15; break; case 543: bar = 1.15; break; case 544: bar = 1.14; break; case 545: bar = 1.14; break; case 546: bar = 1.14; break; case 547: bar = 1.13; break; case 548: bar = 1.13; break; case 549: bar = 1.13; break; case 550: bar = 1.13; break; case 551: bar = 1.12; break; case 552: bar = 1.12; break; case 553: bar = 1.12; break; case 554: bar = 1.11; break; case 555: bar = 1.11; break; case 556: bar = 1.11; break; case 557: bar = 1.1; break; case 558: bar = 1.1; break; case 559: bar = 1.1; break; case 560: bar = 1.09; break; case 561: bar = 1.09; break; case 562: bar = 1.09; break; case 563: bar = 1.08; break; case 564: bar = 1.08; break; case 565: bar = 1.08; break; case 566: bar = 1.07; break; case 567: bar = 1.07; break; case 568: bar = 1.07; break; case 569: bar = 1.07; break; case 570: bar = 1.06; break; case 571: bar = 1.06; break; case 572: bar = 1.06; break; case 573: bar = 1.05; break; case 574: bar = 1.05; break; case 575: bar = 1.05; break; case 576: bar = 1.04; break; case 577: bar = 1.04; break; case 578: bar = 1.04; break; case 579: bar = 1.04; break; case 580: bar = 1.03; break; case 581: bar = 1.03; break; case 582: bar = 1.03; break; case 583: bar = 1.02; break; case 584: bar = 1.02; break; case 585: bar = 1.02; break; case 586: bar = 1.02; break; case 587: bar = 1.01; break; case 588: bar = 1.01; break; case 589: bar = 1.01; break; case 590: bar = 1; break; case 591: bar = 1; break; case 592: bar = 1; break; case 593: bar = 1; break; case 594: bar = 0.99; break; case 595: bar = 0.99; break; case 596: bar = 0.99; break; case 597: bar = 0.98; break; case 598: bar = 0.98; break; case 599: bar = 0.98; break; case 600: bar = 0.98; break; case 601: bar = 0.97; break; case 602: bar = 0.97; break; case 603: bar = 0.97; break; case 604: bar = 0.96; break; case 605: bar = 0.96; break; case 606: bar = 0.96; break; case 607: bar = 0.96; break; case 608: bar = 0.95; break; case 609: bar = 0.95; break; case 610: bar = 0.95; break; case 611: bar = 0.95; break; case 612: bar = 0.94; break; case 613: bar = 0.94; break; case 614: bar = 0.94; break; case 615: bar = 0.94; break; case 616: bar = 0.93; break; case 617: bar = 0.93; break; case 618: bar = 0.93; break; case 619: bar = 0.92; break; case 620: bar = 0.92; break; case 621: bar = 0.92; break; case 622: bar = 0.92; break; case 623: bar = 0.91; break; case 624: bar = 0.91; break; case 625: bar = 0.91; break; case 626: bar = 0.91; break; case 627: bar = 0.9; break; case 628: bar = 0.9; break; case 629: bar = 0.9; break; case 630: bar = 0.9; break; case 631: bar = 0.89; break; case 632: bar = 0.89; break; case 633: bar = 0.89; break; case 634: bar = 0.89; break; case 635: bar = 0.88; break; case 636: bar = 0.88; break; case 637: bar = 0.88; break; case 638: bar = 0.88; break; case 639: bar = 0.87; break; case 640: bar = 0.87; break; case 641: bar = 0.87; break; case 642: bar = 0.87; break; case 643: bar = 0.86; break; case 644: bar = 0.86; break; case 645: bar = 0.86; break; case 646: bar = 0.86; break; case 647: bar = 0.85; break; case 648: bar = 0.85; break; case 649: bar = 0.85; break; case 650: bar = 0.85; break; case 651: bar = 0.84; break; case 652: bar = 0.84; break; case 653: bar = 0.84; break; case 654: bar = 0.84; break; case 655: bar = 0.84; break; case 656: bar = 0.83; break; case 657: bar = 0.83; break; case 658: bar = 0.83; break; case 659: bar = 0.83; break; case 660: bar = 0.82; break; case 661: bar = 0.82; break; case 662: bar = 0.82; break; case 663: bar = 0.82; break; case 664: bar = 0.81; break; case 665: bar = 0.81; break; case 666: bar = 0.81; break; case 667: bar = 0.81; break; case 668: bar = 0.8; break; case 669: bar = 0.8; break; case 670: bar = 0.8; break; case 671: bar = 0.8; break; case 672: bar = 0.8; break; case 673: bar = 0.79; break; case 674: bar = 0.79; break; case 675: bar = 0.79; break; case 676: bar = 0.79; break; case 677: bar = 0.78; break; case 678: bar = 0.78; break; case 679: bar = 0.78; break; case 680: bar = 0.78; break; case 681: bar = 0.78; break; case 682: bar = 0.77; break; case 683: bar = 0.77; break; case 684: bar = 0.77; break; case 685: bar = 0.77; break; case 686: bar = 0.76; break; case 687: bar = 0.76; break; case 688: bar = 0.76; break; case 689: bar = 0.76; break; case 690: bar = 0.76; break; case 691: bar = 0.75; break; case 692: bar = 0.75; break; case 693: bar = 0.75; break; case 694: bar = 0.75; break; case 695: bar = 0.75; break; case 696: bar = 0.74; break; case 697: bar = 0.74; break; case 698: bar = 0.74; break; case 699: bar = 0.74; break; case 700: bar = 0.73; break; case 701: bar = 0.73; break; case 702: bar = 0.73; break; case 703: bar = 0.73; break; case 704: bar = 0.73; break; case 705: bar = 0.72; break; case 706: bar = 0.72; break; case 707: bar = 0.72; break; case 708: bar = 0.72; break; case 709: bar = 0.72; break; case 710: bar = 0.71; break; case 711: bar = 0.71; break; case 712: bar = 0.71; break; case 713: bar = 0.71; break; case 714: bar = 0.71; break; case 715: bar = 0.7; break; case 716: bar = 0.7; break; case 717: bar = 0.7; break; case 718: bar = 0.7; break; case 719: bar = 0.7; break; case 720: bar = 0.69; break; case 721: bar = 0.69; break; case 722: bar = 0.69; break; case 723: bar = 0.69; break; case 724: bar = 0.69; break; case 725: bar = 0.68; break; case 726: bar = 0.68; break; case 727: bar = 0.68; break; case 728: bar = 0.68; break; case 729: bar = 0.68; break; case 730: bar = 0.67; break; case 731: bar = 0.67; break; case 732: bar = 0.67; break; case 733: bar = 0.67; break; case 734: bar = 0.67; break; case 735: bar = 0.66; break; case 736: bar = 0.66; break; case 737: bar = 0.66; break; case 738: bar = 0.66; break; case 739: bar = 0.66; break; case 740: bar = 0.66; break; case 741: bar = 0.65; break; case 742: bar = 0.65; break; case 743: bar = 0.65; break; case 744: bar = 0.65; break; case 745: bar = 0.65; break; case 746: bar = 0.64; break; case 747: bar = 0.64; break; case 748: bar = 0.64; break; case 749: bar = 0.64; break; case 750: bar = 0.64; break; case 751: bar = 0.63; break; case 752: bar = 0.63; break; case 753: bar = 0.63; break; case 754: bar = 0.63; break; case 755: bar = 0.63; break; case 756: bar = 0.63; break; case 757: bar = 0.62; break; case 758: bar = 0.62; break; case 759: bar = 0.62; break; case 760: bar = 0.62; break; case 761: bar = 0.62; break; case 762: bar = 0.61; break; case 763: bar = 0.61; break; case 764: bar = 0.61; break; case 765: bar = 0.61; break; case 766: bar = 0.61; break; case 767: bar = 0.61; break; case 768: bar = 0.6; break; case 769: bar = 0.6; break; case 770: bar = 0.6; break; case 771: bar = 0.6; break; case 772: bar = 0.6; break; case 773: bar = 0.59; break; case 774: bar = 0.59; break; case 775: bar = 0.59; break; case 776: bar = 0.59; break; case 777: bar = 0.59; break; case 778: bar = 0.59; break; case 779: bar = 0.58; break; case 780: bar = 0.58; break; case 781: bar = 0.58; break; case 782: bar = 0.58; break; case 783: bar = 0.58; break; case 784: bar = 0.58; break; case 785: bar = 0.57; break; case 786: bar = 0.57; break; case 787: bar = 0.57; break; case 788: bar = 0.57; break; case 789: bar = 0.57; break; case 790: bar = 0.57; break; case 791: bar = 0.56; break; case 792: bar = 0.56; break; case 793: bar = 0.56; break; case 794: bar = 0.56; break; case 795: bar = 0.56; break; case 796: bar = 0.56; break; case 797: bar = 0.55; break; case 798: bar = 0.55; break; case 799: bar = 0.55; break; case 800: bar = 0.55; break; case 801: bar = 0.55; break; case 802: bar = 0.55; break; case 803: bar = 0.54; break; case 804: bar = 0.54; break; case 805: bar = 0.54; break; case 806: bar = 0.54; break; case 807: bar = 0.54; break; case 808: bar = 0.54; break; case 809: bar = 0.53; break; case 810: bar = 0.53; break; case 811: bar = 0.53; break; case 812: bar = 0.53; break; case 813: bar = 0.53; break; case 814: bar = 0.53; break; case 815: bar = 0.52; break; case 816: bar = 0.52; break; case 817: bar = 0.52; break; case 818: bar = 0.52; break; case 819: bar = 0.52; break; case 820: bar = 0.52; break; case 821: bar = 0.52; break; case 822: bar = 0.51; break; case 823: bar = 0.51; break; case 824: bar = 0.51; break; case 825: bar = 0.51; break; case 826: bar = 0.51; break; case 827: bar = 0.51; break; case 828: bar = 0.5; break; case 829: bar = 0.5; break; case 830: bar = 0.5; break; case 831: bar = 0.5; break; case 832: bar = 0.5; break; case 833: bar = 0.5; break; case 834: bar = 0.49; break; case 835: bar = 0.49; break; case 836: bar = 0.49; break; case 837: bar = 0.49; break; case 838: bar = 0.49; break; case 839: bar = 0.49; break; case 840: bar = 0.49; break; case 841: bar = 0.48; break; case 842: bar = 0.48; break; case 843: bar = 0.48; break; case 844: bar = 0.48; break; case 845: bar = 0.48; break; case 846: bar = 0.48; break; case 847: bar = 0.48; break; case 848: bar = 0.47; break; case 849: bar = 0.47; break; case 850: bar = 0.47; break; case 851: bar = 0.47; break; case 852: bar = 0.47; break; case 853: bar = 0.47; break; case 854: bar = 0.46; break; case 855: bar = 0.46; break; case 856: bar = 0.46; break; case 857: bar = 0.46; break; case 858: bar = 0.46; break; case 859: bar = 0.46; break; case 860: bar = 0.46; break; case 861: bar = 0.45; break; case 862: bar = 0.45; break; case 863: bar = 0.45; break; case 864: bar = 0.45; break; case 865: bar = 0.45; break; case 866: bar = 0.45; break; case 867: bar = 0.45; break; case 868: bar = 0.44; break; case 869: bar = 0.44; break; case 870: bar = 0.44; break; case 871: bar = 0.44; break; case 872: bar = 0.44; break; case 873: bar = 0.44; break; case 874: bar = 0.44; break; case 875: bar = 0.43; break; case 876: bar = 0.43; break; case 877: bar = 0.43; break; case 878: bar = 0.43; break; case 879: bar = 0.43; break; case 880: bar = 0.43; break; case 881: bar = 0.43; break; case 882: bar = 0.42; break; case 883: bar = 0.42; break; case 884: bar = 0.42; break; case 885: bar = 0.42; break; case 886: bar = 0.42; break; case 887: bar = 0.42; break; case 888: bar = 0.42; break; case 889: bar = 0.42; break; case 890: bar = 0.41; break; case 891: bar = 0.41; break; case 892: bar = 0.41; break; case 893: bar = 0.41; break; case 894: bar = 0.41; break; case 895: bar = 0.41; break; case 896: bar = 0.41; break; case 897: bar = 0.4; break; case 898: bar = 0.4; break; case 899: bar = 0.4; break; case 900: bar = 0.4; break; case 901: bar = 0.4; break; case 902: bar = 0.4; break; case 903: bar = 0.4; break; case 904: bar = 0.4; break; case 905: bar = 0.39; break; case 906: bar = 0.39; break; case 907: bar = 0.39; break; case 908: bar = 0.39; break; case 909: bar = 0.39; break; case 910: bar = 0.39; break; case 911: bar = 0.39; break; case 912: bar = 0.38; break; case 913: bar = 0.38; break; case 914: bar = 0.38; break; case 915: bar = 0.38; break; case 916: bar = 0.38; break; case 917: bar = 0.38; break; case 918: bar = 0.38; break; case 919: bar = 0.38; break; case 920: bar = 0.37; break; case 921: bar = 0.37; break; case 922: bar = 0.37; break; case 923: bar = 0.37; break; case 924: bar = 0.37; break; case 925: bar = 0.37; break; case 926: bar = 0.37; break; case 927: bar = 0.37; break; case 928: bar = 0.36; break; case 929: bar = 0.36; break; case 930: bar = 0.36; break; case 931: bar = 0.36; break; case 932: bar = 0.36; break; case 933: bar = 0.36; break; case 934: bar = 0.36; break; case 935: bar = 0.36; break; case 936: bar = 0.35; break; case 937: bar = 0.35; break; case 938: bar = 0.35; break; case 939: bar = 0.35; break; case 940: bar = 0.35; break; case 941: bar = 0.35; break; case 942: bar = 0.35; break; case 943: bar = 0.35; break; case 944: bar = 0.34; break; case 945: bar = 0.34; break; case 946: bar = 0.34; break; case 947: bar = 0.34; break; case 948: bar = 0.34; break; case 949: bar = 0.34; break; case 950: bar = 0.34; break; case 951: bar = 0.34; break; case 952: bar = 0.33; break; case 953: bar = 0.33; break; case 954: bar = 0.33; break; case 955: bar = 0.33; break; case 956: bar = 0.33; break; case 957: bar = 0.33; break; case 958: bar = 0.33; break; case 959: bar = 0.33; break; case 960: bar = 0.32; break; case 961: bar = 0.32; break; case 962: bar = 0.32; break; case 963: bar = 0.32; break; case 964: bar = 0.32; break; case 965: bar = 0.32; break; case 966: bar = 0.32; break; case 967: bar = 0.32; break; case 968: bar = 0.32; break; case 969: bar = 0.31; break; case 970: bar = 0.31; break; case 971: bar = 0.31; break; case 972: bar = 0.31; break; case 973: bar = 0.31; break; case 974: bar = 0.31; break; case 975: bar = 0.31; break; case 976: bar = 0.31; break; case 977: bar = 0.3; break; case 978: bar = 0.3; break; case 979: bar = 0.3; break; case 980: bar = 0.3; break; case 981: bar = 0.3; break; case 982: bar = 0.3; break; case 983: bar = 0.3; break; case 984: bar = 0.3; break; case 985: bar = 0.3; break; case 986: bar = 0.29; break; case 987: bar = 0.29; break; case 988: bar = 0.29; break; case 989: bar = 0.29; break; case 990: bar = 0.29; break; case 991: bar = 0.29; break; case 992: bar = 0.29; break; case 993: bar = 0.29; break; case 994: bar = 0.29; break; case 995: bar = 0.28; break; case 996: bar = 0.28; break; case 997: bar = 0.28; break; case 998: bar = 0.28; break; case 999: bar = 0.28; break; case 1000: bar = 0.28; break; case 1001: bar = 0.28; break; case 1002: bar = 0.28; break; case 1003: bar = 0.28; break; case 1004: bar = 0.27; break; case 1005: bar = 0.27; break; case 1006: bar = 0.27; break; case 1007: bar = 0.27; break; case 1008: bar = 0.27; break; case 1009: bar = 0.27; break; case 1010: bar = 0.27; break; case 1011: bar = 0.27; break; case 1012: bar = 0.27; break; case 1013: bar = 0.26; break; case 1014: bar = 0.26; break; case 1015: bar = 0.26; break; case 1016: bar = 0.26; break; case 1017: bar = 0.26; break; case 1018: bar = 0.26; break; case 1019: bar = 0.26; break; case 1020: bar = 0.26; break; case 1021: bar = 0.26; break; case 1022: bar = 0.25; break; case 1023: bar = 0.25; break; case 1024: bar = 0.25; break;
  }
  return bar;
}
float convb(int raw) {
  float bar;

  switch (raw) {    // 1024 cases
    case 1: bar = 358.19; break; case 2: bar = 253.15; break; case 3: bar = 198.63; break; case 4: bar = 164.62; break; case 5: bar = 141.17; break; case 6: bar = 123.93; break; case 7: bar = 110.67; break; case 8: bar = 100.13; break; case 9: bar = 91.53; break; case 10: bar = 84.36; break; case 11: bar = 78.3; break; case 12: bar = 73.1; break; case 13: bar = 68.58; break; case 14: bar = 64.61; break; case 15: bar = 61.1; break; case 16: bar = 57.97; break; case 17: bar = 55.16; break; case 18: bar = 52.62; break; case 19: bar = 50.32; break; case 20: bar = 48.21; break; case 21: bar = 46.29; break; case 22: bar = 44.52; break; case 23: bar = 42.88; break; case 24: bar = 41.37; break; case 25: bar = 39.96; break; case 26: bar = 38.65; break; case 27: bar = 37.42; break; case 28: bar = 36.27; break; case 29: bar = 35.2; break; case 30: bar = 34.18; break; case 31: bar = 33.23; break; case 32: bar = 32.33; break; case 33: bar = 31.48; break; case 34: bar = 30.67; break; case 35: bar = 29.9; break; case 36: bar = 29.18; break; case 37: bar = 28.49; break; case 38: bar = 27.83; break; case 39: bar = 27.2; break; case 40: bar = 26.6; break; case 41: bar = 26.02; break; case 42: bar = 25.47; break; case 43: bar = 24.95; break; case 44: bar = 24.44; break; case 45: bar = 23.96; break; case 46: bar = 23.49; break; case 47: bar = 23.05; break; case 48: bar = 22.61; break; case 49: bar = 22.2; break; case 50: bar = 21.8; break; case 51: bar = 21.42; break; case 52: bar = 21.04; break; case 53: bar = 20.68; break; case 54: bar = 20.34; break; case 55: bar = 20; break; case 56: bar = 19.68; break; case 57: bar = 19.36; break; case 58: bar = 19.06; break; case 59: bar = 18.76; break; case 60: bar = 18.48; break; case 61: bar = 18.2; break; case 62: bar = 17.93; break; case 63: bar = 17.67; break; case 64: bar = 17.41; break; case 65: bar = 17.17; break; case 66: bar = 16.93; break; case 67: bar = 16.69; break; case 68: bar = 16.47; break; case 69: bar = 16.24; break; case 70: bar = 16.03; break; case 71: bar = 15.82; break; case 72: bar = 15.61; break; case 73: bar = 15.41; break; case 74: bar = 15.22; break; case 75: bar = 15.03; break; case 76: bar = 14.85; break; case 77: bar = 14.66; break; case 78: bar = 14.49; break; case 79: bar = 14.31; break; case 80: bar = 14.15; break; case 81: bar = 13.98; break; case 82: bar = 13.82; break; case 83: bar = 13.66; break; case 84: bar = 13.51; break; case 85: bar = 13.36; break; case 86: bar = 13.21; break; case 87: bar = 13.07; break; case 88: bar = 12.92; break; case 89: bar = 12.79; break; case 90: bar = 12.65; break; case 91: bar = 12.52; break; case 92: bar = 12.39; break; case 93: bar = 12.26; break; case 94: bar = 12.13; break; case 95: bar = 12.01; break; case 96: bar = 11.89; break; case 97: bar = 11.77; break; case 98: bar = 11.66; break; case 99: bar = 11.54; break; case 100: bar = 11.43; break; case 101: bar = 11.32; break; case 102: bar = 11.21; break; case 103: bar = 11.11; break; case 104: bar = 11; break; case 105: bar = 10.9; break; case 106: bar = 10.8; break; case 107: bar = 10.7; break; case 108: bar = 10.61; break; case 109: bar = 10.51; break; case 110: bar = 10.42; break; case 111: bar = 10.32; break; case 112: bar = 10.23; break; case 113: bar = 10.14; break; case 114: bar = 10.06; break; case 115: bar = 9.97; break; case 116: bar = 9.89; break; case 117: bar = 9.8; break; case 118: bar = 9.72; break; case 119: bar = 9.64; break; case 120: bar = 9.56; break; case 121: bar = 9.48; break; case 122: bar = 9.4; break; case 123: bar = 9.33; break; case 124: bar = 9.25; break; case 125: bar = 9.18; break; case 126: bar = 9.1; break; case 127: bar = 9.03; break; case 128: bar = 8.96; break; case 129: bar = 8.89; break; case 130: bar = 8.82; break; case 131: bar = 8.76; break; case 132: bar = 8.69; break; case 133: bar = 8.62; break; case 134: bar = 8.56; break; case 135: bar = 8.49; break; case 136: bar = 8.43; break; case 137: bar = 8.37; break; case 138: bar = 8.31; break; case 139: bar = 8.24; break; case 140: bar = 8.18; break; case 141: bar = 8.13; break; case 142: bar = 8.07; break; case 143: bar = 8.01; break; case 144: bar = 7.95; break; case 145: bar = 7.9; break; case 146: bar = 7.84; break; case 147: bar = 7.79; break; case 148: bar = 7.73; break; case 149: bar = 7.68; break; case 150: bar = 7.63; break; case 151: bar = 7.57; break; case 152: bar = 7.52; break; case 153: bar = 7.47; break; case 154: bar = 7.42; break; case 155: bar = 7.37; break; case 156: bar = 7.32; break; case 157: bar = 7.27; break; case 158: bar = 7.23; break; case 159: bar = 7.18; break; case 160: bar = 7.13; break; case 161: bar = 7.08; break; case 162: bar = 7.04; break; case 163: bar = 6.99; break; case 164: bar = 6.95; break; case 165: bar = 6.9; break; case 166: bar = 6.86; break; case 167: bar = 6.82; break; case 168: bar = 6.77; break; case 169: bar = 6.73; break; case 170: bar = 6.69; break; case 171: bar = 6.65; break; case 172: bar = 6.61; break; case 173: bar = 6.57; break; case 174: bar = 6.53; break; case 175: bar = 6.49; break; case 176: bar = 6.45; break; case 177: bar = 6.41; break; case 178: bar = 6.37; break; case 179: bar = 6.33; break; case 180: bar = 6.3; break; case 181: bar = 6.26; break; case 182: bar = 6.22; break; case 183: bar = 6.18; break; case 184: bar = 6.15; break; case 185: bar = 6.11; break; case 186: bar = 6.08; break; case 187: bar = 6.04; break; case 188: bar = 6.01; break; case 189: bar = 5.97; break; case 190: bar = 5.94; break; case 191: bar = 5.91; break; case 192: bar = 5.87; break; case 193: bar = 5.84; break; case 194: bar = 5.81; break; case 195: bar = 5.77; break; case 196: bar = 5.74; break; case 197: bar = 5.71; break; case 198: bar = 5.68; break; case 199: bar = 5.65; break; case 200: bar = 5.62; break; case 201: bar = 5.58; break; case 202: bar = 5.55; break; case 203: bar = 5.52; break; case 204: bar = 5.49; break; case 205: bar = 5.46; break; case 206: bar = 5.44; break; case 207: bar = 5.41; break; case 208: bar = 5.38; break; case 209: bar = 5.35; break; case 210: bar = 5.32; break; case 211: bar = 5.29; break; case 212: bar = 5.26; break; case 213: bar = 5.24; break; case 214: bar = 5.21; break; case 215: bar = 5.18; break; case 216: bar = 5.16; break; case 217: bar = 5.13; break; case 218: bar = 5.1; break; case 219: bar = 5.08; break; case 220: bar = 5.05; break; case 221: bar = 5.03; break; case 222: bar = 5; break; case 223: bar = 4.97; break; case 224: bar = 4.95; break; case 225: bar = 4.92; break; case 226: bar = 4.9; break; case 227: bar = 4.88; break; case 228: bar = 4.85; break; case 229: bar = 4.83; break; case 230: bar = 4.8; break; case 231: bar = 4.78; break; case 232: bar = 4.76; break; case 233: bar = 4.73; break; case 234: bar = 4.71; break; case 235: bar = 4.69; break; case 236: bar = 4.66; break; case 237: bar = 4.64; break; case 238: bar = 4.62; break; case 239: bar = 4.6; break; case 240: bar = 4.57; break; case 241: bar = 4.55; break; case 242: bar = 4.53; break; case 243: bar = 4.51; break; case 244: bar = 4.49; break; case 245: bar = 4.47; break; case 246: bar = 4.44; break; case 247: bar = 4.42; break; case 248: bar = 4.4; break; case 249: bar = 4.38; break; case 250: bar = 4.36; break; case 251: bar = 4.34; break; case 252: bar = 4.32; break; case 253: bar = 4.3; break; case 254: bar = 4.28; break; case 255: bar = 4.26; break; case 256: bar = 4.24; break; case 257: bar = 4.22; break; case 258: bar = 4.2; break; case 259: bar = 4.18; break; case 260: bar = 4.17; break; case 261: bar = 4.15; break; case 262: bar = 4.13; break; case 263: bar = 4.11; break; case 264: bar = 4.09; break; case 265: bar = 4.07; break; case 266: bar = 4.05; break; case 267: bar = 4.04; break; case 268: bar = 4.02; break; case 269: bar = 4; break; case 270: bar = 3.98; break; case 271: bar = 3.96; break; case 272: bar = 3.95; break; case 273: bar = 3.93; break; case 274: bar = 3.91; break; case 275: bar = 3.9; break; case 276: bar = 3.88; break; case 277: bar = 3.86; break; case 278: bar = 3.84; break; case 279: bar = 3.83; break; case 280: bar = 3.81; break; case 281: bar = 3.79; break; case 282: bar = 3.78; break; case 283: bar = 3.76; break; case 284: bar = 3.75; break; case 285: bar = 3.73; break; case 286: bar = 3.71; break; case 287: bar = 3.7; break; case 288: bar = 3.68; break; case 289: bar = 3.67; break; case 290: bar = 3.65; break; case 291: bar = 3.64; break; case 292: bar = 3.62; break; case 293: bar = 3.6; break; case 294: bar = 3.59; break; case 295: bar = 3.57; break; case 296: bar = 3.56; break; case 297: bar = 3.54; break; case 298: bar = 3.53; break; case 299: bar = 3.52; break; case 300: bar = 3.5; break; case 301: bar = 3.49; break; case 302: bar = 3.47; break; case 303: bar = 3.46; break; case 304: bar = 3.44; break; case 305: bar = 3.43; break; case 306: bar = 3.42; break; case 307: bar = 3.4; break; case 308: bar = 3.39; break; case 309: bar = 3.37; break; case 310: bar = 3.36; break; case 311: bar = 3.35; break; case 312: bar = 3.33; break; case 313: bar = 3.32; break; case 314: bar = 3.31; break; case 315: bar = 3.29; break; case 316: bar = 3.28; break; case 317: bar = 3.27; break; case 318: bar = 3.25; break; case 319: bar = 3.24; break; case 320: bar = 3.23; break; case 321: bar = 3.21; break; case 322: bar = 3.2; break; case 323: bar = 3.19; break; case 324: bar = 3.18; break; case 325: bar = 3.16; break; case 326: bar = 3.15; break; case 327: bar = 3.14; break; case 328: bar = 3.13; break; case 329: bar = 3.11; break; case 330: bar = 3.1; break; case 331: bar = 3.09; break; case 332: bar = 3.08; break; case 333: bar = 3.07; break; case 334: bar = 3.05; break; case 335: bar = 3.04; break; case 336: bar = 3.03; break; case 337: bar = 3.02; break; case 338: bar = 3.01; break; case 339: bar = 2.99; break; case 340: bar = 2.98; break; case 341: bar = 2.97; break; case 342: bar = 2.96; break; case 343: bar = 2.95; break; case 344: bar = 2.94; break; case 345: bar = 2.93; break; case 346: bar = 2.91; break; case 347: bar = 2.9; break; case 348: bar = 2.89; break; case 349: bar = 2.88; break; case 350: bar = 2.87; break; case 351: bar = 2.86; break; case 352: bar = 2.85; break; case 353: bar = 2.84; break; case 354: bar = 2.83; break; case 355: bar = 2.82; break; case 356: bar = 2.81; break; case 357: bar = 2.8; break; case 358: bar = 2.78; break; case 359: bar = 2.77; break; case 360: bar = 2.76; break; case 361: bar = 2.75; break; case 362: bar = 2.74; break; case 363: bar = 2.73; break; case 364: bar = 2.72; break; case 365: bar = 2.71; break; case 366: bar = 2.7; break; case 367: bar = 2.69; break; case 368: bar = 2.68; break; case 369: bar = 2.67; break; case 370: bar = 2.66; break; case 371: bar = 2.65; break; case 372: bar = 2.64; break; case 373: bar = 2.63; break; case 374: bar = 2.62; break; case 375: bar = 2.61; break; case 376: bar = 2.6; break; case 377: bar = 2.6; break; case 378: bar = 2.59; break; case 379: bar = 2.58; break; case 380: bar = 2.57; break; case 381: bar = 2.56; break; case 382: bar = 2.55; break; case 383: bar = 2.54; break; case 384: bar = 2.53; break; case 385: bar = 2.52; break; case 386: bar = 2.51; break; case 387: bar = 2.5; break; case 388: bar = 2.49; break; case 389: bar = 2.48; break; case 390: bar = 2.48; break; case 391: bar = 2.47; break; case 392: bar = 2.46; break; case 393: bar = 2.45; break; case 394: bar = 2.44; break; case 395: bar = 2.43; break; case 396: bar = 2.42; break; case 397: bar = 2.41; break; case 398: bar = 2.41; break; case 399: bar = 2.4; break; case 400: bar = 2.39; break; case 401: bar = 2.38; break; case 402: bar = 2.37; break; case 403: bar = 2.36; break; case 404: bar = 2.35; break; case 405: bar = 2.35; break; case 406: bar = 2.34; break; case 407: bar = 2.33; break; case 408: bar = 2.32; break; case 409: bar = 2.31; break; case 410: bar = 2.3; break; case 411: bar = 2.3; break; case 412: bar = 2.29; break; case 413: bar = 2.28; break; case 414: bar = 2.27; break; case 415: bar = 2.26; break; case 416: bar = 2.26; break; case 417: bar = 2.25; break; case 418: bar = 2.24; break; case 419: bar = 2.23; break; case 420: bar = 2.22; break; case 421: bar = 2.22; break; case 422: bar = 2.21; break; case 423: bar = 2.2; break; case 424: bar = 2.19; break; case 425: bar = 2.19; break; case 426: bar = 2.18; break; case 427: bar = 2.17; break; case 428: bar = 2.16; break; case 429: bar = 2.16; break; case 430: bar = 2.15; break; case 431: bar = 2.14; break; case 432: bar = 2.13; break; case 433: bar = 2.13; break; case 434: bar = 2.12; break; case 435: bar = 2.11; break; case 436: bar = 2.1; break; case 437: bar = 2.1; break; case 438: bar = 2.09; break; case 439: bar = 2.08; break; case 440: bar = 2.08; break; case 441: bar = 2.07; break; case 442: bar = 2.06; break; case 443: bar = 2.05; break; case 444: bar = 2.05; break; case 445: bar = 2.04; break; case 446: bar = 2.03; break; case 447: bar = 2.03; break; case 448: bar = 2.02; break; case 449: bar = 2.01; break; case 450: bar = 2.01; break; case 451: bar = 2; break; case 452: bar = 1.99; break; case 453: bar = 1.99; break; case 454: bar = 1.98; break; case 455: bar = 1.97; break; case 456: bar = 1.97; break; case 457: bar = 1.96; break; case 458: bar = 1.95; break; case 459: bar = 1.95; break; case 460: bar = 1.94; break; case 461: bar = 1.93; break; case 462: bar = 1.93; break; case 463: bar = 1.92; break; case 464: bar = 1.91; break; case 465: bar = 1.91; break; case 466: bar = 1.9; break; case 467: bar = 1.89; break; case 468: bar = 1.89; break; case 469: bar = 1.88; break; case 470: bar = 1.87; break; case 471: bar = 1.87; break; case 472: bar = 1.86; break; case 473: bar = 1.86; break; case 474: bar = 1.85; break; case 475: bar = 1.84; break; case 476: bar = 1.84; break; case 477: bar = 1.83; break; case 478: bar = 1.82; break; case 479: bar = 1.82; break; case 480: bar = 1.81; break; case 481: bar = 1.81; break; case 482: bar = 1.8; break; case 483: bar = 1.79; break; case 484: bar = 1.79; break; case 485: bar = 1.78; break; case 486: bar = 1.78; break; case 487: bar = 1.77; break; case 488: bar = 1.76; break; case 489: bar = 1.76; break; case 490: bar = 1.75; break; case 491: bar = 1.75; break; case 492: bar = 1.74; break; case 493: bar = 1.74; break; case 494: bar = 1.73; break; case 495: bar = 1.72; break; case 496: bar = 1.72; break; case 497: bar = 1.71; break; case 498: bar = 1.71; break; case 499: bar = 1.7; break; case 500: bar = 1.7; break; case 501: bar = 1.69; break; case 502: bar = 1.68; break; case 503: bar = 1.68; break; case 504: bar = 1.67; break; case 505: bar = 1.67; break; case 506: bar = 1.66; break; case 507: bar = 1.66; break; case 508: bar = 1.65; break; case 509: bar = 1.65; break; case 510: bar = 1.64; break; case 511: bar = 1.63; break; case 512: bar = 1.63; break; case 513: bar = 1.62; break; case 514: bar = 1.62; break; case 515: bar = 1.61; break; case 516: bar = 1.61; break; case 517: bar = 1.6; break; case 518: bar = 1.6; break; case 519: bar = 1.59; break; case 520: bar = 1.59; break; case 521: bar = 1.58; break; case 522: bar = 1.58; break; case 523: bar = 1.57; break; case 524: bar = 1.57; break; case 525: bar = 1.56; break; case 526: bar = 1.56; break; case 527: bar = 1.55; break; case 528: bar = 1.55; break; case 529: bar = 1.54; break; case 530: bar = 1.54; break; case 531: bar = 1.53; break; case 532: bar = 1.53; break; case 533: bar = 1.52; break; case 534: bar = 1.52; break; case 535: bar = 1.51; break; case 536: bar = 1.51; break; case 537: bar = 1.5; break; case 538: bar = 1.5; break; case 539: bar = 1.49; break; case 540: bar = 1.49; break; case 541: bar = 1.48; break; case 542: bar = 1.48; break; case 543: bar = 1.47; break; case 544: bar = 1.47; break; case 545: bar = 1.46; break; case 546: bar = 1.46; break; case 547: bar = 1.45; break; case 548: bar = 1.45; break; case 549: bar = 1.44; break; case 550: bar = 1.44; break; case 551: bar = 1.43; break; case 552: bar = 1.43; break; case 553: bar = 1.42; break; case 554: bar = 1.42; break; case 555: bar = 1.41; break; case 556: bar = 1.41; break; case 557: bar = 1.4; break; case 558: bar = 1.4; break; case 559: bar = 1.4; break; case 560: bar = 1.39; break; case 561: bar = 1.39; break; case 562: bar = 1.38; break; case 563: bar = 1.38; break; case 564: bar = 1.37; break; case 565: bar = 1.37; break; case 566: bar = 1.36; break; case 567: bar = 1.36; break; case 568: bar = 1.36; break; case 569: bar = 1.35; break; case 570: bar = 1.35; break; case 571: bar = 1.34; break; case 572: bar = 1.34; break; case 573: bar = 1.33; break; case 574: bar = 1.33; break; case 575: bar = 1.32; break; case 576: bar = 1.32; break; case 577: bar = 1.32; break; case 578: bar = 1.31; break; case 579: bar = 1.31; break; case 580: bar = 1.3; break; case 581: bar = 1.3; break; case 582: bar = 1.29; break; case 583: bar = 1.29; break; case 584: bar = 1.29; break; case 585: bar = 1.28; break; case 586: bar = 1.28; break; case 587: bar = 1.27; break; case 588: bar = 1.27; break; case 589: bar = 1.26; break; case 590: bar = 1.26; break; case 591: bar = 1.26; break; case 592: bar = 1.25; break; case 593: bar = 1.25; break; case 594: bar = 1.24; break; case 595: bar = 1.24; break; case 596: bar = 1.24; break; case 597: bar = 1.23; break; case 598: bar = 1.23; break; case 599: bar = 1.22; break; case 600: bar = 1.22; break; case 601: bar = 1.22; break; case 602: bar = 1.21; break; case 603: bar = 1.21; break; case 604: bar = 1.2; break; case 605: bar = 1.2; break; case 606: bar = 1.2; break; case 607: bar = 1.19; break; case 608: bar = 1.19; break; case 609: bar = 1.18; break; case 610: bar = 1.18; break; case 611: bar = 1.18; break; case 612: bar = 1.17; break; case 613: bar = 1.17; break; case 614: bar = 1.16; break; case 615: bar = 1.16; break; case 616: bar = 1.16; break; case 617: bar = 1.15; break; case 618: bar = 1.15; break; case 619: bar = 1.15; break; case 620: bar = 1.14; break; case 621: bar = 1.14; break; case 622: bar = 1.13; break; case 623: bar = 1.13; break; case 624: bar = 1.13; break; case 625: bar = 1.12; break; case 626: bar = 1.12; break; case 627: bar = 1.12; break; case 628: bar = 1.11; break; case 629: bar = 1.11; break; case 630: bar = 1.1; break; case 631: bar = 1.1; break; case 632: bar = 1.1; break; case 633: bar = 1.09; break; case 634: bar = 1.09; break; case 635: bar = 1.09; break; case 636: bar = 1.08; break; case 637: bar = 1.08; break; case 638: bar = 1.08; break; case 639: bar = 1.07; break; case 640: bar = 1.07; break; case 641: bar = 1.07; break; case 642: bar = 1.06; break; case 643: bar = 1.06; break; case 644: bar = 1.05; break; case 645: bar = 1.05; break; case 646: bar = 1.05; break; case 647: bar = 1.04; break; case 648: bar = 1.04; break; case 649: bar = 1.04; break; case 650: bar = 1.03; break; case 651: bar = 1.03; break; case 652: bar = 1.03; break; case 653: bar = 1.02; break; case 654: bar = 1.02; break; case 655: bar = 1.02; break; case 656: bar = 1.01; break; case 657: bar = 1.01; break; case 658: bar = 1.01; break; case 659: bar = 1; break; case 660: bar = 1; break; case 661: bar = 1; break; case 662: bar = 0.99; break; case 663: bar = 0.99; break; case 664: bar = 0.99; break; case 665: bar = 0.98; break; case 666: bar = 0.98; break; case 667: bar = 0.98; break; case 668: bar = 0.97; break; case 669: bar = 0.97; break; case 670: bar = 0.97; break; case 671: bar = 0.96; break; case 672: bar = 0.96; break; case 673: bar = 0.96; break; case 674: bar = 0.95; break; case 675: bar = 0.95; break; case 676: bar = 0.95; break; case 677: bar = 0.94; break; case 678: bar = 0.94; break; case 679: bar = 0.94; break; case 680: bar = 0.93; break; case 681: bar = 0.93; break; case 682: bar = 0.93; break; case 683: bar = 0.92; break; case 684: bar = 0.92; break; case 685: bar = 0.92; break; case 686: bar = 0.92; break; case 687: bar = 0.91; break; case 688: bar = 0.91; break; case 689: bar = 0.91; break; case 690: bar = 0.9; break; case 691: bar = 0.9; break; case 692: bar = 0.9; break; case 693: bar = 0.89; break; case 694: bar = 0.89; break; case 695: bar = 0.89; break; case 696: bar = 0.88; break; case 697: bar = 0.88; break; case 698: bar = 0.88; break; case 699: bar = 0.88; break; case 700: bar = 0.87; break; case 701: bar = 0.87; break; case 702: bar = 0.87; break; case 703: bar = 0.86; break; case 704: bar = 0.86; break; case 705: bar = 0.86; break; case 706: bar = 0.85; break; case 707: bar = 0.85; break; case 708: bar = 0.85; break; case 709: bar = 0.85; break; case 710: bar = 0.84; break; case 711: bar = 0.84; break; case 712: bar = 0.84; break; case 713: bar = 0.83; break; case 714: bar = 0.83; break; case 715: bar = 0.83; break; case 716: bar = 0.83; break; case 717: bar = 0.82; break; case 718: bar = 0.82; break; case 719: bar = 0.82; break; case 720: bar = 0.81; break; case 721: bar = 0.81; break; case 722: bar = 0.81; break; case 723: bar = 0.81; break; case 724: bar = 0.8; break; case 725: bar = 0.8; break; case 726: bar = 0.8; break; case 727: bar = 0.79; break; case 728: bar = 0.79; break; case 729: bar = 0.79; break; case 730: bar = 0.79; break; case 731: bar = 0.78; break; case 732: bar = 0.78; break; case 733: bar = 0.78; break; case 734: bar = 0.77; break; case 735: bar = 0.77; break; case 736: bar = 0.77; break; case 737: bar = 0.77; break; case 738: bar = 0.76; break; case 739: bar = 0.76; break; case 740: bar = 0.76; break; case 741: bar = 0.76; break; case 742: bar = 0.75; break; case 743: bar = 0.75; break; case 744: bar = 0.75; break; case 745: bar = 0.74; break; case 746: bar = 0.74; break; case 747: bar = 0.74; break; case 748: bar = 0.74; break; case 749: bar = 0.73; break; case 750: bar = 0.73; break; case 751: bar = 0.73; break; case 752: bar = 0.73; break; case 753: bar = 0.72; break; case 754: bar = 0.72; break; case 755: bar = 0.72; break; case 756: bar = 0.72; break; case 757: bar = 0.71; break; case 758: bar = 0.71; break; case 759: bar = 0.71; break; case 760: bar = 0.71; break; case 761: bar = 0.7; break; case 762: bar = 0.7; break; case 763: bar = 0.7; break; case 764: bar = 0.69; break; case 765: bar = 0.69; break; case 766: bar = 0.69; break; case 767: bar = 0.69; break; case 768: bar = 0.68; break; case 769: bar = 0.68; break; case 770: bar = 0.68; break; case 771: bar = 0.68; break; case 772: bar = 0.67; break; case 773: bar = 0.67; break; case 774: bar = 0.67; break; case 775: bar = 0.67; break; case 776: bar = 0.66; break; case 777: bar = 0.66; break; case 778: bar = 0.66; break; case 779: bar = 0.66; break; case 780: bar = 0.65; break; case 781: bar = 0.65; break; case 782: bar = 0.65; break; case 783: bar = 0.65; break; case 784: bar = 0.64; break; case 785: bar = 0.64; break; case 786: bar = 0.64; break; case 787: bar = 0.64; break; case 788: bar = 0.64; break; case 789: bar = 0.63; break; case 790: bar = 0.63; break; case 791: bar = 0.63; break; case 792: bar = 0.63; break; case 793: bar = 0.62; break; case 794: bar = 0.62; break; case 795: bar = 0.62; break; case 796: bar = 0.62; break; case 797: bar = 0.61; break; case 798: bar = 0.61; break; case 799: bar = 0.61; break; case 800: bar = 0.61; break; case 801: bar = 0.6; break; case 802: bar = 0.6; break; case 803: bar = 0.6; break; case 804: bar = 0.6; break; case 805: bar = 0.59; break; case 806: bar = 0.59; break; case 807: bar = 0.59; break; case 808: bar = 0.59; break; case 809: bar = 0.59; break; case 810: bar = 0.58; break; case 811: bar = 0.58; break; case 812: bar = 0.58; break; case 813: bar = 0.58; break; case 814: bar = 0.57; break; case 815: bar = 0.57; break; case 816: bar = 0.57; break; case 817: bar = 0.57; break; case 818: bar = 0.57; break; case 819: bar = 0.56; break; case 820: bar = 0.56; break; case 821: bar = 0.56; break; case 822: bar = 0.56; break; case 823: bar = 0.55; break; case 824: bar = 0.55; break; case 825: bar = 0.55; break; case 826: bar = 0.55; break; case 827: bar = 0.55; break; case 828: bar = 0.54; break; case 829: bar = 0.54; break; case 830: bar = 0.54; break; case 831: bar = 0.54; break; case 832: bar = 0.53; break; case 833: bar = 0.53; break; case 834: bar = 0.53; break; case 835: bar = 0.53; break; case 836: bar = 0.53; break; case 837: bar = 0.52; break; case 838: bar = 0.52; break; case 839: bar = 0.52; break; case 840: bar = 0.52; break; case 841: bar = 0.51; break; case 842: bar = 0.51; break; case 843: bar = 0.51; break; case 844: bar = 0.51; break; case 845: bar = 0.51; break; case 846: bar = 0.5; break; case 847: bar = 0.5; break; case 848: bar = 0.5; break; case 849: bar = 0.5; break; case 850: bar = 0.5; break; case 851: bar = 0.49; break; case 852: bar = 0.49; break; case 853: bar = 0.49; break; case 854: bar = 0.49; break; case 855: bar = 0.49; break; case 856: bar = 0.48; break; case 857: bar = 0.48; break; case 858: bar = 0.48; break; case 859: bar = 0.48; break; case 860: bar = 0.47; break; case 861: bar = 0.47; break; case 862: bar = 0.47; break; case 863: bar = 0.47; break; case 864: bar = 0.47; break; case 865: bar = 0.46; break; case 866: bar = 0.46; break; case 867: bar = 0.46; break; case 868: bar = 0.46; break; case 869: bar = 0.46; break; case 870: bar = 0.45; break; case 871: bar = 0.45; break; case 872: bar = 0.45; break; case 873: bar = 0.45; break; case 874: bar = 0.45; break; case 875: bar = 0.44; break; case 876: bar = 0.44; break; case 877: bar = 0.44; break; case 878: bar = 0.44; break; case 879: bar = 0.44; break; case 880: bar = 0.43; break; case 881: bar = 0.43; break; case 882: bar = 0.43; break; case 883: bar = 0.43; break; case 884: bar = 0.43; break; case 885: bar = 0.42; break; case 886: bar = 0.42; break; case 887: bar = 0.42; break; case 888: bar = 0.42; break; case 889: bar = 0.42; break; case 890: bar = 0.42; break; case 891: bar = 0.41; break; case 892: bar = 0.41; break; case 893: bar = 0.41; break; case 894: bar = 0.41; break; case 895: bar = 0.41; break; case 896: bar = 0.4; break; case 897: bar = 0.4; break; case 898: bar = 0.4; break; case 899: bar = 0.4; break; case 900: bar = 0.4; break; case 901: bar = 0.39; break; case 902: bar = 0.39; break; case 903: bar = 0.39; break; case 904: bar = 0.39; break; case 905: bar = 0.39; break; case 906: bar = 0.38; break; case 907: bar = 0.38; break; case 908: bar = 0.38; break; case 909: bar = 0.38; break; case 910: bar = 0.38; break; case 911: bar = 0.38; break; case 912: bar = 0.37; break; case 913: bar = 0.37; break; case 914: bar = 0.37; break; case 915: bar = 0.37; break; case 916: bar = 0.37; break; case 917: bar = 0.36; break; case 918: bar = 0.36; break; case 919: bar = 0.36; break; case 920: bar = 0.36; break; case 921: bar = 0.36; break; case 922: bar = 0.36; break; case 923: bar = 0.35; break; case 924: bar = 0.35; break; case 925: bar = 0.35; break; case 926: bar = 0.35; break; case 927: bar = 0.35; break; case 928: bar = 0.34; break; case 929: bar = 0.34; break; case 930: bar = 0.34; break; case 931: bar = 0.34; break; case 932: bar = 0.34; break; case 933: bar = 0.34; break; case 934: bar = 0.33; break; case 935: bar = 0.33; break; case 936: bar = 0.33; break; case 937: bar = 0.33; break; case 938: bar = 0.33; break; case 939: bar = 0.33; break; case 940: bar = 0.32; break; case 941: bar = 0.32; break; case 942: bar = 0.32; break; case 943: bar = 0.32; break; case 944: bar = 0.32; break; case 945: bar = 0.32; break; case 946: bar = 0.31; break; case 947: bar = 0.31; break; case 948: bar = 0.31; break; case 949: bar = 0.31; break; case 950: bar = 0.31; break; case 951: bar = 0.3; break; case 952: bar = 0.3; break; case 953: bar = 0.3; break; case 954: bar = 0.3; break; case 955: bar = 0.3; break; case 956: bar = 0.3; break; case 957: bar = 0.29; break; case 958: bar = 0.29; break; case 959: bar = 0.29; break; case 960: bar = 0.29; break; case 961: bar = 0.29; break; case 962: bar = 0.29; break; case 963: bar = 0.28; break; case 964: bar = 0.28; break; case 965: bar = 0.28; break; case 966: bar = 0.28; break; case 967: bar = 0.28; break; case 968: bar = 0.28; break; case 969: bar = 0.27; break; case 970: bar = 0.27; break; case 971: bar = 0.27; break; case 972: bar = 0.27; break; case 973: bar = 0.27; break; case 974: bar = 0.27; break; case 975: bar = 0.26; break; case 976: bar = 0.26; break; case 977: bar = 0.26; break; case 978: bar = 0.26; break; case 979: bar = 0.26; break; case 980: bar = 0.26; break; case 981: bar = 0.26; break; case 982: bar = 0.25; break; case 983: bar = 0.25; break; case 984: bar = 0.25; break; case 985: bar = 0.25; break; case 986: bar = 0.25; break; case 987: bar = 0.25; break; case 988: bar = 0.24; break; case 989: bar = 0.24; break; case 990: bar = 0.24; break; case 991: bar = 0.24; break; case 992: bar = 0.24; break; case 993: bar = 0.24; break; case 994: bar = 0.23; break; case 995: bar = 0.23; break; case 996: bar = 0.23; break; case 997: bar = 0.23; break; case 998: bar = 0.23; break; case 999: bar = 0.23; break; case 1000: bar = 0.23; break; case 1001: bar = 0.22; break; case 1002: bar = 0.22; break; case 1003: bar = 0.22; break; case 1004: bar = 0.22; break; case 1005: bar = 0.22; break; case 1006: bar = 0.22; break; case 1007: bar = 0.21; break; case 1008: bar = 0.21; break; case 1009: bar = 0.21; break; case 1010: bar = 0.21; break; case 1011: bar = 0.21; break; case 1012: bar = 0.21; break; case 1013: bar = 0.21; break; case 1014: bar = 0.2; break; case 1015: bar = 0.2; break; case 1016: bar = 0.2; break; case 1017: bar = 0.2; break; case 1018: bar = 0.2; break; case 1019: bar = 0.2; break; case 1020: bar = 0.19; break; case 1021: bar = 0.19; break; case 1022: bar = 0.19; break; case 1023: bar = 0.19; break; case 1024: bar = 0.19; break;
  }
  return bar;
}
