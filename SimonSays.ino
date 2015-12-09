/*
 * Simon says
 *
 * @author Stijn Boutsen
 * @ author Stef Wynants
 * @author Robin Fripon
 * @author Jeroen Timmermans
 * @version 1.0
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
#define INTPIN0 10  //Pin for btn1
#define INTPIN1 11  //Pin for btn2
#define INTPIN2 12  //Pin for btn3
#define INTPIN3 13  //Pin for btn4
#define INTPIN4 14  //Pin for btn4

//Capacitive pushbuttons
CapacitiveSensor   cs_2_0 = CapacitiveSensor(SENDPIN,READPIN0);        // 800K resistor between pins 4 & 2, pin 2 is sensor pin, add a wire and or foil if desired
CapacitiveSensor   cs_2_1 = CapacitiveSensor(SENDPIN,READPIN1);
CapacitiveSensor   cs_2_2 = CapacitiveSensor(SENDPIN,READPIN2);
CapacitiveSensor   cs_2_3 = CapacitiveSensor(SENDPIN,READPIN3);


//Capacitive intensity sensor/capacitive slider
CapacitiveSensor   cs_4_0 = CapacitiveSensor(4,2);
CapacitiveSensor   cs_4_1 = CapacitiveSensor(4,3);
CapacitiveSensor   cs_4_2 = CapacitiveSensor(4,5);
CapacitiveSensor   cs_4_3 = CapacitiveSensor(4,7);
CapacitiveSensor   cs_4_4 = CapacitiveSensor(4,8);

int sensitivity = 5;

const int Treshold = 40;

int storedArray[100];
int mini = 1;
int maxi = 5;
int adresInputwaarde = 0;
int turn = 0;
//Used for debouncing
int buttonState[4] = {0, 0, 0, 0};
int lastButtonState[4] = {0, 0, 0, 0};
//Next state for the FSM
int newState = 0;
//Interval to blink the leds
int period = 500;

void setup() {
  //Define digital pins as inputs
  pinMode(2, INPUT);
  pinMode(3, INPUT);
  pinMode(4, INPUT);
  pinMode(5, INPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);

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

//Display the array
void showArray() {
  for (int i = 0; i <= 99; i++)  {
    Serial.print(storedArray[i]);
    Serial.print(" ");
  }
  Serial.println();
}

void loop() {
  fsm(newState);
}

void setupGame() {
  //Generate a random array
  genData();
  //Display the generated array
  showArray();
  turn = 1;
  adresInputwaarde = 0;
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
 * This function act as a state machine with 3 states.
 */
void fsm(int state) {
  switch (state) {
    case 0:
      //Run the funtion to start the game.
      Serial.println("Start game");
      setupGame();
      newState = 1;
      delay(1000);
      break;
    case 1:
      Serial.println("Case 1");
      //Toon n aantal posities
      showSequence(turn);
      newState = 2;
      break;
    case 2:
      //vergelijk userinput met randomSequence
      readUserIpunt();
      break;
    case 3:
      Serial.println("Case 3");
      //vergelijk userinput met randomSequence
      newState = 3;
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
    blinkLed(storedArray[i]+5);
  }
}

void readUserIpunt() {
  debounce(2,filter(cs_2_0.capacitiveSensor(30)));
  debounce(3,filter(cs_2_1.capacitiveSensor(30)));
  debounce(4,filter(cs_2_2.capacitiveSensor(30)));
  debounce(5,filter(cs_2_3.capacitiveSensor(30)));
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
      if (storedArray[adresInputwaarde] == buttonPin - 1 && adresInputwaarde == turn - 1) {
        adresInputwaarde = 0;
        turn++;
        newState = 1;
        Serial.print(storedArray[adresInputwaarde]);
        Serial.print(" ");
        Serial.print(buttonPin - 1);
        Serial.print(" ");
        Serial.println("SUCCES!!!");
        succes();
      }
      else if (storedArray[adresInputwaarde] == buttonPin - 1 && adresInputwaarde != turn - 1) {
        adresInputwaarde++;
        newState = 2;
        Serial.print(storedArray[adresInputwaarde]);
        Serial.print(" ");
        Serial.print(buttonPin - 1);
        Serial.print(" ");
        Serial.println("GOED");
      }
      else {
        newState = 0;
        Serial.print(storedArray[adresInputwaarde]);
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
    delay(20);
  }
  // save the current state as the last state,
  //for next time through the loop
  lastButtonState[buttonPin - 2] = buttonState[buttonPin - 2];
}

void setIntensity()                    
{
    long start = millis();
    long sensor1 =  cs_4_0.capacitiveSensor(30);
    long sensor2 =  cs_4_1.capacitiveSensor(30);
    long sensor3 =  cs_4_2.capacitiveSensor(30);
    long sensor4 =  cs_4_3.capacitiveSensor(30);
    long sensor5 =  cs_4_4.capacitiveSensor(30);
    
    if (sensor1 > (sensor2 + 100)) 
    {
      sensitivity = 1;
    } 
    else if (sensor1 > 100 && sensor2 > 100)
    {
      sensitivity = 2;
    }
    else if (sensor2 > (sensor1 + 100) && sensor2 > (sensor3 + 100))
    {
      sensitivity = 3;
    }
    else if (sensor2 > 100 && sensor3 > 100)
    {
      sensitivity = 4;
    }
    else if (sensor3 > (sensor2 + 100) && sensor3 > (sensor4 + 100))
    {
      sensitivity = 5;
    }
    else if (sensor3 > 100 && sensor4 > 100)
    {
      sensitivity = 6;
    }
    else if (sensor4 > (sensor3 + 100) && sensor4 > (sensor5 + 100))
    {
      sensitivity = 7;
    }
    else if (sensor4 > 100 && sensor5 > 100)
    {
      sensitivity = 8;
    }
    else if (sensor5 > (sensor4 + 100))
    {
      sensitivity = 9;
    }
}

/*
 * LED-SEQUENCES AND ANIMATIONS
 */

void blinkLed(int ledPin){
  digitalWrite(ledPin, HIGH);
  delay(period);
  digitalWrite(ledPin, LOW);
  delay(period);
}

void startUp(){
  digitalWrite(6, HIGH);
  delay(period);
  digitalWrite(7, HIGH);
  delay(period);
  digitalWrite(8, HIGH);
  delay(period);
  digitalWrite(9, HIGH);
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

void succes(){
  digitalWrite(6, HIGH);
  delay(period);
  digitalWrite(7, HIGH);
  delay(period);
  digitalWrite(8, HIGH);
  delay(period);
  digitalWrite(9, HIGH);
  delay(period*4);
  digitalWrite(6, LOW);
  digitalWrite(7, LOW);
  digitalWrite(8, LOW);
  digitalWrite(9, LOW);
}

void fout(){
  digitalWrite(6, HIGH);
  digitalWrite(7, HIGH);
  digitalWrite(8, HIGH);
  digitalWrite(9, HIGH);
  delay(period);
  digitalWrite(6, LOW);
  digitalWrite(7, LOW);
  digitalWrite(8, LOW);
  digitalWrite(9, LOW);
  delay(period);
  digitalWrite(6, HIGH);
  digitalWrite(7, HIGH);
  digitalWrite(8, HIGH);
  digitalWrite(9, HIGH);
  delay(period);
  digitalWrite(6, LOW);
  digitalWrite(7, LOW);
  digitalWrite(8, LOW);
  digitalWrite(9, LOW);
}
