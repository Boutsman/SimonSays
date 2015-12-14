/*
 * Simon says
 *
 * @author Stijn Boutsen
 * @ author Stef Wynants
 * @author Robin Fripon
 * @author Jeroen Timmermans
 * @version 1.1
 * @link Boutsman.be
 *
 * This program was written for a demo-module for the Institute of material research.
 * The machine shows the usefullness of OLEDs, printed circuits and capacitive sensing.
 *
 * The module consists of:
 * - Capacitive humidity sensor
 * - Capacitive intensitysensor (slider)
 * - Capacitive sensing OLEDs
 *
 */

#include <CapacitiveSensor.h>
#define SENDPIN 13
#define READPIN0 2  //Pin for btn1
#define READPIN1 3  //Pin for btn2
#define READPIN2 4  //Pin for btn3
#define READPIN3 5  //Pin for btn4

#define INTPIN0 10  //Pin for slider btn1
#define INTPIN1 11  //Pin for slider btn2
#define INTPIN2 12  //Pin for slider btn3
#define INTPIN3 13  //Pin for slider btn4
#define INTPIN4 14  //Pin for slider btn5

#define LEDPIN0 6  //Pin for LED1
#define LEDPIN1 7  //Pin for LED2
#define LEDPIN2 8  //Pin for LED3
#define LEDPIN3 9  //Pin for LED4

//Capacitive pushbuttons
CapacitiveSensor   cs_2_0 = CapacitiveSensor(SENDPIN, READPIN0);       // 800K resistor between pins 13 & 2, pin 2 is sensor pin, add a wire and or foil if desired
CapacitiveSensor   cs_2_1 = CapacitiveSensor(SENDPIN, READPIN1);
CapacitiveSensor   cs_2_2 = CapacitiveSensor(SENDPIN, READPIN2);
CapacitiveSensor   cs_2_3 = CapacitiveSensor(SENDPIN, READPIN3);

//Capacitive intensity sensor/capacitive slider
CapacitiveSensor   cs_4_0 = CapacitiveSensor(SENDPIN, INTPIN0);
CapacitiveSensor   cs_4_1 = CapacitiveSensor(SENDPIN, INTPIN1);
CapacitiveSensor   cs_4_2 = CapacitiveSensor(SENDPIN, INTPIN2);
CapacitiveSensor   cs_4_3 = CapacitiveSensor(SENDPIN, INTPIN3);
CapacitiveSensor   cs_4_4 = CapacitiveSensor(SENDPIN, INTPIN4);

int sensitivity = 5;

//Treshold for the capacitive intensity sensor
const int intTreshold = 100;
//Treshold for the capacitive pushbutons
const int Treshold = 200;

//Array to store random numbers between 1 and 4
int storedArray[100];
//Define the lowest number to be generated (inclusive)
int mini = 1;
//Define which number is the highest (exclusive)
int maxi = 5;
//Integer used to cycle through the stored array
int adresInputVal = 0;
//Integer used to keep track of the round
int turn = 0;
//Used for debouncing
int buttonState[4] = {0, 0, 0, 0};
int lastButtonState[4] = {0, 0, 0, 0};
//Next state for the FSM
int newState = 0;
//Interval to blink the leds
int period = 500;

void setup() {
  //Define digital pins for capacitive buttons as inputs
  pinMode(2, INPUT);
  pinMode(3, INPUT);
  pinMode(4, INPUT);
  pinMode(5, INPUT);
  //Define digital pins for LEDs as outputs
  pinMode(LEDPIN0, OUTPUT);
  pinMode(LEDPIN1, OUTPUT);
  pinMode(LEDPIN2, OUTPUT);
  pinMode(LEDPIN3, OUTPUT);

  //Use autocallibration on capacitive sensors
  cs_2_0.set_CS_AutocaL_Millis(0xFFFFFFFF);
  cs_2_1.set_CS_AutocaL_Millis(0xFFFFFFFF);
  cs_2_2.set_CS_AutocaL_Millis(0xFFFFFFFF);
  cs_2_3.set_CS_AutocaL_Millis(0xFFFFFFFF);

  //Use randomSeed to be sure that you generate random numbers
  randomSeed(analogRead(0));

  //Initiate the serial port
  Serial.begin(115200);
}

//Generate a random array
void genData() {
  for (int i = 0; i <= 99; i++) {
    storedArray[i] = random(mini, maxi);
  }
}

//Display the stored array
void showArray() {
  for (int i = 0; i <= 99; i++)  {
    Serial.print(storedArray[i]);
    Serial.print(" ");
  }
  Serial.println();
}

/*
 * Start the finite state machine
 * @param Integer the state to where the program has to go
 */
void loop() {
  //Set the intensity of the LEDs
  //***setIntensity();
  //State the FSM
  //Define the New State
  fsm(newState);
}

//Ready the device to start playing a new game
void setupGame() {
  //Generate a random array
  genData();
  //Display the generated array
  showArray();
  //A new game always starts on the first turn
  turn = 1;
  //This is an extra integer used as an index to cycle through the stored array
  adresInputVal = 0;
  //Run the startUp LED sequence
  startUp();
}

/*
 * This function turns the capacitive values into an integer
 * @ param integer The raw value of the capacitive sensor
 * @ return an integer value corresponding to a certain state
 */
int filter(int val) {
  //Return 1 if the value is higher the the treshold.
  if (val > Treshold + 10) {
    return 1;
  }
  //Return 0 if the value is too low
  else if (val < Treshold - 10) {
    return 0;
  }
  //Return 2 if the value is in the "don't care" range
  /*else {
    return 2;
  }*/
}

/*
 * This function acts as a state machine
 */
void fsm(int state) {
  switch (state) {
    case 0:
      //State to start/restart the game.
      Serial.println("Start game");
      setupGame();
      newState = 1;
      delay(1000);
      break;
    case 1:
      //State to show the stored sequence
      Serial.println("Case 1");
      showSequence(turn);
      newState = 2;
      break;
    case 2:
      //State to check userinput with the stored array
      Serial.println("Case 2");
      readUserIpunt();
      break;
    default:
      // if nothing else matches, do the default
      // default is optional
      newState = 0;
      break;
  }
}

void showSequence(int n) {
  for (int i = 0; i < n; i++) {
    Serial.println();
    delay(1000);
    Serial.print( "turn = ");
    Serial.println(turn);
    Serial.print( "val = ");
    Serial.println(storedArray[i]);
    blinkLed(storedArray[i] + 5);
  }
}

void readUserIpunt() {
  debounce(2, filter(cs_2_0.capacitiveSensor(30)));
  debounce(3, filter(cs_2_1.capacitiveSensor(30)));
  debounce(4, filter(cs_2_2.capacitiveSensor(30)));
  debounce(5, filter(cs_2_3.capacitiveSensor(30)));
}

void debounce(int buttonPin, int val) {
  // read the pushbutton input pin:
  //buttonState[buttonPin - 2] = digitalRead(buttonPin);
  buttonState[buttonPin - 2] = val;

  // compare the buttonState to its previous state
  if (buttonState[buttonPin - 2] != lastButtonState[buttonPin - 2]) {
    // if the state has changed, do something
    if (buttonState[buttonPin - 2] == HIGH) {
      // if the current state is HIGH then the button
      // went from off to on:
      if (storedArray[adresInputVal] == buttonPin - 1 && adresInputVal == turn - 1) {
        adresInputVal = 0;
        turn++;
        newState = 1;
        blinkLed(buttonPin+4);
        Serial.print(storedArray[adresInputVal]);
        Serial.print(" ");
        Serial.print(buttonPin - 1);
        Serial.print(" ");
        Serial.println("SUCCES!!!");
        succes();
      }
      else if (storedArray[adresInputVal] == buttonPin - 1 && adresInputVal != turn - 1) {
        adresInputVal++;
        newState = 2;
        blinkLed(buttonPin+4);
        Serial.print(storedArray[adresInputVal]);
        Serial.print(" ");
        Serial.print(buttonPin - 1);
        Serial.print(" ");
        Serial.println("GOED");
      }
      else {
        newState = 0;
        blinkLed(buttonPin+4);
        Serial.print(storedArray[adresInputVal]);
        Serial.print(" ");
        Serial.print(buttonPin - 1);
        Serial.print(" ");
        Serial.println("FOUT");
        fout();
      }
    } else {
      // if the current state is LOW then the button
      // wend from on to off:
      //Serial.println("off");
    }
    // Delay a little bit to avoid bouncing
    delay(30);
  }
  // save the current state as the last state,
  //for next time through the loop
  lastButtonState[buttonPin - 2] = buttonState[buttonPin - 2];
}

/*
 * INTENSITY SENSOR/CAPACITIVE SLIDER
 */

void setIntensity()
{
  long sensor1 =  cs_4_0.capacitiveSensor(30);
  long sensor2 =  cs_4_1.capacitiveSensor(30);
  long sensor3 =  cs_4_2.capacitiveSensor(30);
  long sensor4 =  cs_4_3.capacitiveSensor(30);
  long sensor5 =  cs_4_4.capacitiveSensor(30);

  if (sensor1 > (sensor2 + intTreshold))
  {
    sensitivity = 1;
  }
  else if (sensor1 > intTreshold && sensor2 > intTreshold)
  {
    sensitivity = 2;
  }
  else if (sensor2 > (sensor1 + intTreshold) && sensor2 > (sensor3 + intTreshold))
  {
    sensitivity = 3;
  }
  else if (sensor2 > 100 && sensor3 > 100)
  {
    sensitivity = 4;
  }
  else if (sensor3 > (sensor2 + intTreshold) && sensor3 > (sensor4 + intTreshold))
  {
    sensitivity = 5;
  }
  else if (sensor3 > intTreshold && sensor4 > intTreshold)
  {
    sensitivity = 6;
  }
  else if (sensor4 > (sensor3 + intTreshold) && sensor4 > (sensor5 + intTreshold))
  {
    sensitivity = 7;
  }
  else if (sensor4 > intTreshold && sensor5 > intTreshold)
  {
    sensitivity = 8;
  }
  else if (sensor5 > (sensor4 + intTreshold))
  {
    sensitivity = 9;
  }
}

/*
 * LED-SEQUENCES AND ANIMATIONS
 */

//Little function to blink a LED
void blinkLed(int ledPin) {
  analogWrite(ledPin, map(sensitivity, 1, 9, 50, 255));
  delay(period);
  digitalWrite(ledPin, LOW);
  delay(period);
}

//LED sequence played when a new game is set
void startUp() {
  analogWrite(LEDPIN0, map(sensitivity, 1, 9, 50, 255));
  delay(period);
  analogWrite(LEDPIN1, map(sensitivity, 1, 9, 50, 255));
  delay(period);
  analogWrite(LEDPIN2, map(sensitivity, 1, 9, 50, 255));
  delay(period);
  analogWrite(LEDPIN3, map(sensitivity, 1, 9, 50, 255));
  delay(period);
  digitalWrite(6, LOW);
  delay(period);
  digitalWrite(7, LOW);
  delay(period);
  digitalWrite(8, LOW);
  delay(period);
  digitalWrite(9, LOW);
  delay(period);
}

//LED sequence played when a full round has been finished
void succes() {
  analogWrite(LEDPIN0, map(sensitivity, 1, 9, 50, 255));
  delay(period);
  analogWrite(LEDPIN1, map(sensitivity, 1, 9, 50, 255));
  delay(period);
  analogWrite(LEDPIN2, map(sensitivity, 1, 9, 50, 255));
  delay(period);
  analogWrite(LEDPIN3, map(sensitivity, 1, 9, 50, 255));
  delay(period * 4);
  digitalWrite(6, LOW);
  digitalWrite(7, LOW);
  digitalWrite(8, LOW);
  digitalWrite(9, LOW);
}

//LED sequence played when a wrong button is touched
void fout() {
  analogWrite(LEDPIN0, map(sensitivity, 1, 9, 50, 255));
  analogWrite(LEDPIN1, map(sensitivity, 1, 9, 50, 255));
  analogWrite(LEDPIN2, map(sensitivity, 1, 9, 50, 255));
  analogWrite(LEDPIN3, map(sensitivity, 1, 9, 50, 255));
  delay(period);
  digitalWrite(6, LOW);
  digitalWrite(7, LOW);
  digitalWrite(8, LOW);
  digitalWrite(9, LOW);
  delay(period);
  analogWrite(LEDPIN0, map(sensitivity, 1, 9, 50, 255));
  analogWrite(LEDPIN1, map(sensitivity, 1, 9, 50, 255));
  analogWrite(LEDPIN2, map(sensitivity, 1, 9, 50, 255));
  analogWrite(LEDPIN3, map(sensitivity, 1, 9, 50, 255));
  delay(period);
  digitalWrite(6, LOW);
  digitalWrite(7, LOW);
  digitalWrite(8, LOW);
  digitalWrite(9, LOW);
  delay(period*2);
}
