/*
 * Simon says
 *
 * @author Stijn Boutsen
 * @version 0.1
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

const int Treshold = 40;
int storedArray[100];
int adresInputwaarde = 0;
int turn = 0;
int mini = 1;
int maxi = 5;
int buttonState[4] = {0, 0, 0, 0};
int lastButtonState[4] = {0, 0, 0, 0};
int newState = 0;

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
  //  cs_2_0.set_CS_AutocaL_Millis(0xFFFFFFFF);
  //  cs_2_1.set_CS_AutocaL_Millis(0xFFFFFFFF);
  //  cs_2_2.set_CS_AutocaL_Millis(0xFFFFFFFF);
  //  cs_2_3.set_CS_AutocaL_Millis(0xFFFFFFFF);

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
  debounce(2);
  debounce(3);
  debounce(4);
  debounce(5);
  fsm(newState);
}

void setupGame() {
  //Generate a random array
  genData();
  //Display the generated array
  showArray();
  turn = 1;
  adresInputwaarde = 0;
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
  else {
    return 2;
  }
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

  }
}

void readUserIpunt() {
  debounce(2);
  debounce(3);
  debounce(4);
  debounce(5);
}

void debounce(int buttonPin) {
  // read the pushbutton input pin:
  buttonState[buttonPin - 2] = digitalRead(buttonPin);

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
