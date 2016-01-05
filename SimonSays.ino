/*
 * Simon says
 *
 * @author Stijn Boutsen
 * @ author Stef Wynants
 * @author Robin Fripon
 * @author Jeroen Timmermans
 * @author Thomas Aerts
 * @author Willem Geerts
 * @author Henning Lakiere
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
#define READPIN1 4  //Pin for btn2
#define READPIN2 7  //Pin for btn3
#define READPIN3 8  //Pin for btn4

#define INTPIN0 10  //Pin for slider btn1
#define INTPIN1 11  //Pin for slider btn2
#define INTPIN2 12  //Pin for slider btn3
//#define INTPIN3 1  //Pin for slider btn4
//#define INTPIN4 14  //Pin for slider btn5

#define LEDPIN0 3  //Pin for LED1
#define LEDPIN1 5  //Pin for LED2
#define LEDPIN2 6  //Pin for LED3
#define LEDPIN3 9  //Pin for LED4

#define OUT_PIN  A3
#define IN_PIN  A1

int ledArray[4] = {3, 5, 6, 9};

//Capacitive pushbuttons
CapacitiveSensor   cs_2_0 = CapacitiveSensor(SENDPIN, READPIN0);       // 800K resistor between pins 13 & 2, pin 2 is sensor pin, add a wire and or foil if desired
CapacitiveSensor   cs_2_1 = CapacitiveSensor(SENDPIN, READPIN1);
CapacitiveSensor   cs_2_2 = CapacitiveSensor(SENDPIN, READPIN2);
CapacitiveSensor   cs_2_3 = CapacitiveSensor(SENDPIN, READPIN3);

////Capacitive intensity sensor/capacitive slider
CapacitiveSensor   cs_4_0 = CapacitiveSensor(SENDPIN, INTPIN0);
CapacitiveSensor   cs_4_1 = CapacitiveSensor(SENDPIN, INTPIN1);
CapacitiveSensor   cs_4_2 = CapacitiveSensor(SENDPIN, INTPIN2);
//CapacitiveSensor   cs_4_3 = CapacitiveSensor(SENDPIN, INTPIN3);
//CapacitiveSensor   cs_4_4 = CapacitiveSensor(SENDPIN, INTPIN4);

int intensity = 7;
int power = 0;

//Treshold for the capacitive intensity sensor
const int intTreshold = 100;

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

//Capacitance between IN_PIN and Ground
//Stray capacitance is always present. Extra capacitance can be added to
//allow higher capacitance to be measured.
const float IN_STRAY_CAP_TO_GND = 24.48; //initially this was 30.00
const float IN_EXTRA_CAP_TO_GND = 0.0;
const float IN_CAP_TO_GND  = IN_STRAY_CAP_TO_GND + IN_EXTRA_CAP_TO_GND;
const int MAX_ADC_VALUE = 1023;

void setup() {
  //Define digital pins for capacitive buttons as inputs
  pinMode(READPIN0, INPUT);
  pinMode(READPIN1, INPUT);
  pinMode(READPIN2, INPUT);
  pinMode(READPIN3, INPUT);
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
  getCapacity();
}

//Generate a random array
void genData() {
  for (int i = 0; i <= 99; i++) {
    storedArray[i] = random(mini, maxi);
  }
}

//Display the stored array
//void showArray() {
//  for (int i = 0; i <= 99; i++)  {
//    Serial.print(storedArray[i]);
//    Serial.print(" ");
//  }
//  Serial.println();
//}

/*
 * Start the finite state machine
 * @param Integer the state to where the program has to go
 */
void loop() {
  getCapacity();
  //Set the intensity of the LEDs
  setIntensity();
  //State the FSM
  //Define the New State
  fsm(newState);
}

//Ready the device to start playing a new game
void setupGame() {
  //Generate a random array
  genData();
  //Display the generated array
  //showArray();
  getCapacity();
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
int filter(int val, int Treshold) {
  //Serial.print(val);
  //Serial.print("   ");
  //Return 1 if the value is higher the the treshold.
  if (val > Treshold + 5) {
    return 1;
  }
  //Return 0 if the value is too low
  else if (val < Treshold - 5) {
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
//      Serial.println("Start game");
      setupGame();
      newState = 1;
      delay(1000);
      break;
    case 1:
      //State to show the stored sequence
      //Serial.println("Case 1");
      showSequence(turn);
      newState = 2;
      break;
    case 2:
      //State to check userinput with the stored array
      //Serial.println("Case 2");
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
//    Serial.println();
    delay(1000);
//    Serial.print( "turn = ");
//    Serial.println(turn);
//    Serial.print( "val = ");
//    Serial.println(storedArray[i]);
    blinkLed(ledArray[storedArray[i]-1]);
  }
}

void readUserIpunt() {
  //Avatar theme ^^
  //Air
  debounce(LEDPIN0, filter(cs_2_0.capacitiveSensor(30),28));
  //Earth
  debounce(LEDPIN1, filter(cs_2_1.capacitiveSensor(30),28));
  //Water
  debounce(LEDPIN2, filter(cs_2_2.capacitiveSensor(30),45));
  //Fire
  debounce(LEDPIN3, filter(cs_2_3.capacitiveSensor(30),30));
  //Serial.println();
}

void debounce(int buttonPin, int val) {
  // read the pushbutton input pin:
  //buttonState[(buttonPin/2)-1] = digitalRead(buttonPin);
  buttonState[(buttonPin / 2) - 1] = val;

  // compare the buttonState to its previous state
  if (buttonState[(buttonPin / 2) - 1] != lastButtonState[(buttonPin / 2) - 1]) {
    // if the state has changed, do something
    if (buttonState[(buttonPin / 2) - 1] == HIGH) {
      // if the current state is HIGH then the button
      // went from off to on:
      if (storedArray[adresInputVal] == ((buttonPin / 2)) && adresInputVal == turn - 1) {
        adresInputVal = 0;
        turn++;
        newState = 1;
        blinkLed(ledArray[(buttonPin / 2) - 1]);
//        Serial.print(storedArray[adresInputVal]);
//        Serial.print(" ");
//        Serial.print((buttonPin / 2));
//        Serial.print(" ");
//        Serial.println("SUCCES!!!");
        succes();
      }
      else if (storedArray[adresInputVal] == (buttonPin / 2) && adresInputVal != turn - 1) {
        adresInputVal++;
        newState = 2;
        blinkLed(ledArray[(buttonPin / 2) - 1]);
//        Serial.print(storedArray[adresInputVal]);
//        Serial.print(" ");
//        Serial.print((buttonPin / 2));
//        Serial.print(" ");
//        Serial.println("GOED");
      }
      else {
        newState = 0;
        blinkLed(ledArray[(buttonPin / 2) - 1]);
//        Serial.print(storedArray[adresInputVal]);
//        Serial.print(" ");
//        Serial.print((buttonPin / 2));
//        Serial.print(" ");
//        Serial.println("FOUT");
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
  lastButtonState[(buttonPin / 2) - 1] = buttonState[(buttonPin / 2) - 1];
}

/*
 * INTENSITY SENSOR/CAPACITIVE SLIDER
 */

void setIntensity()
{
  long start = millis();
  long sensor1 =  cs_4_0.capacitiveSensor(30);
  long sensor2 =  cs_4_1.capacitiveSensor(30);
  long sensor3 =  cs_4_2.capacitiveSensor(30);
  long sensor4 =  0; //cs_4_3.capacitiveSensor(30);
  long sensor5 = 0;

//  Serial.print(sensor1);
//  Serial.print("  ");
//  Serial.print(sensor2);
//  Serial.print("  ");
//  Serial.print(sensor3);
//  Serial.print("  ");
//  Serial.println(intensity);

  if (sensor1 > 110 && sensor2 < 60)
  {
    intensity = 1;
    LEDON();
  }
  else if (sensor1 > 60  && sensor2 > 60)
  {
    intensity = 2;
    LEDON();
  }
  else if (sensor1 < 60 && sensor2 > 100 && sensor3 < 60)
  {
    intensity = 3;
    LEDON();
  }
  else if (sensor2 > 60 && sensor3 > 60)
  {
    intensity = 4;
    LEDON();
  }
  else if (sensor2 < 60 && sensor3 > 110 && sensor4 < 60)
  {
    intensity = 5;
    LEDON();
  }
  else if (sensor3 > 60 && sensor4 > 60)
  {
    intensity = 6;
    LEDON();
  }
  else if (sensor3 < 60 && sensor4 > 100 && sensor5 < 60)
  {
    intensity = 7;
    LEDON();
  }
  else if (sensor4 > 500 && sensor5 > 500)                                  
  {
    intensity = 8;
    LEDON();
  }
  else if (sensor4 < 300 && sensor5 > 500)
  {
    intensity = 9;
    LEDON();
  }
  else if (sensor1 < 40 && sensor2 < 40 && sensor3 < 40 && sensor4 < 40 && sensor5 < 40)
  {
    LEDOFF();
  }
  else{
    LEDON();  
  }
}

void getCapacity()
{
  //Capacitor under test between OUT_PIN and IN_PIN
  //Rising high edge on OUT_PIN
  pinMode(IN_PIN, INPUT);
  digitalWrite(OUT_PIN, HIGH);
  int val = analogRead(IN_PIN);

  //Clear everything for next measurement
  digitalWrite(OUT_PIN, LOW);
  pinMode(IN_PIN, OUTPUT);

  //Calculate and print result

  float capacitance = (float)val * IN_CAP_TO_GND / (float)(MAX_ADC_VALUE - val);

  if (capacitance > 2.8){
    power = 1;
  }
  else if (capacitance < 2.5){
    power = 0;    
  }
  Serial.print(capacitance);
  Serial.print("   ");
  Serial.println(power);
}

/*
 * LED-SEQUENCES AND ANIMATIONS
 */

//Turn on LEDs
void LEDON(){
  analogWrite(LEDPIN0, map(intensity, 1, 7, 5, 200));
  analogWrite(LEDPIN1, map(intensity, 1, 7, 5, 200));
  analogWrite(LEDPIN2, map(intensity, 1, 7, 5, 200));
  analogWrite(LEDPIN3, map(intensity, 1, 7, 5, 200));  
}

//Turn all LEDs OFF
void LEDOFF(){
  digitalWrite(LEDPIN0, LOW);
  digitalWrite(LEDPIN1, LOW);
  digitalWrite(LEDPIN2, LOW);
  digitalWrite(LEDPIN3, LOW);  
}

//Little function to blink a LED
void blinkLed(int ledPin) {
  analogWrite(ledPin, map(intensity, 1, 7, 5, 200));
  delay(period);
  digitalWrite(ledPin, LOW);
  delay(period/2);
}

//LED sequence played when a new game is set
void startUp() {
  analogWrite(LEDPIN0, map(intensity, 1, 7, 5, 200));
  delay(period);
  analogWrite(LEDPIN1, map(intensity, 1, 7, 5, 200));
  delay(period);
  analogWrite(LEDPIN2, map(intensity, 1, 7, 5, 200));
  delay(period);
  analogWrite(LEDPIN3, map(intensity, 1, 7, 5, 200));
  delay(period);
  digitalWrite(LEDPIN0, LOW);
  delay(period);
  digitalWrite(LEDPIN1, LOW);
  delay(period);
  digitalWrite(LEDPIN2, LOW);
  delay(period);
  digitalWrite(LEDPIN3, LOW);
  delay(period);
}

//LED sequence played when a full round has been finished
void succes() {
  analogWrite(LEDPIN0, map(intensity, 1, 7, 5, 200));
  delay(period);
  analogWrite(LEDPIN1, map(intensity, 1, 7, 5, 200));
  delay(period);
  analogWrite(LEDPIN2, map(intensity, 1, 7, 5, 200));
  delay(period);
  analogWrite(LEDPIN3, map(intensity, 1, 7, 5, 200));
  delay(period);
  digitalWrite(LEDPIN0, LOW);
  digitalWrite(LEDPIN1, LOW);
  digitalWrite(LEDPIN2, LOW);
  digitalWrite(LEDPIN3, LOW);
}

//LED sequence played when a wrong button is touched
void fout() {
  analogWrite(LEDPIN0, map(intensity, 1, 7, 5, 200));
  analogWrite(LEDPIN1, map(intensity, 1, 7, 5, 200));
  analogWrite(LEDPIN2, map(intensity, 1, 7, 5, 200));
  analogWrite(LEDPIN3, map(intensity, 1, 7, 5, 200));
  delay(period);
  digitalWrite(LEDPIN0, LOW);
  digitalWrite(LEDPIN1, LOW);
  digitalWrite(LEDPIN2, LOW);
  digitalWrite(LEDPIN3, LOW);
  delay(period);
  analogWrite(LEDPIN0, map(intensity, 1, 7, 5, 200));
  analogWrite(LEDPIN1, map(intensity, 1, 7, 5, 200));
  analogWrite(LEDPIN2, map(intensity, 1, 7, 5, 200));
  analogWrite(LEDPIN3, map(intensity, 1, 7, 5, 200));
  delay(period);
  digitalWrite(LEDPIN0, LOW);
  digitalWrite(LEDPIN1, LOW);
  digitalWrite(LEDPIN2, LOW);
  digitalWrite(LEDPIN3, LOW);
  delay(period);
}
