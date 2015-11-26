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

#define SEND_PIN 12
#define RECEIVE_PIN_1 2
#define RECEIVE_PIN_2 3
#define RECEIVE_PIN_3 4
#define RECEIVE_PIN_4 5

int storedArray[100];
int adresInputwaarde = 0;
int val[4] = {0,0,0,0};
int lastVal[4] = {0,0,0,0};
int turn = 0;
int mini = 1;
int maxi = 5;
int lastButtonState = 0;

void setup() {
  pinMode(2,INPUT);
  pinMode(3,INPUT);
  pinMode(4,INPUT);
  pinMode(5,INPUT);
  randomSeed(analogRead(0));
  Serial.begin(9600);

  setupGame();

  //storedArray[0] = random(mini, maxi);
  delay(1000);
}

void setupGame(){
    //Generate a random array
    genData();
    //Display the generated array
    showArray();
}

void loop() {
  //showArray();
  //extendArray();
  //delay(1000);

  readBtn(2, 0);
  readBtn(3, 1);
  readBtn(4, 2);
  readBtn(5, 3);
}

//Add an element to the array
void extendArray() {
  turn++;
  storedArray[turn] = random(mini, maxi);
}

//Display the array
void showArray() {
  for (int i = 0; i <= 19; i++)  {
    Serial.print(storedArray[i]);
    Serial.print(" ");
  }
  Serial.println();
  Serial.print( "turn = ");
  Serial.println(turn);
}

void readBtn(int pin, int n)
{
  //Digital Values 1
  val[n] = digitalRead(pin);

  if (val[n] == 1) {
    if (lastVal[n] == 0) {
      if (storedArray[turn] == pin - 1)
      {
        Serial.print(turn);
        Serial.print( " ");
        Serial.println(pin);
      }
      else
      {
        Serial.println("fout");
      }
    }
  }
  else if (val[n] == 0) {
    if (lastVal[n] == 1) {
      turn = turn+1;
    }
  }
  lastVal[n] = val[n];
}

//Generate a random array
void genData() {
  storedArray[0] = random(mini, maxi);
  storedArray[1] = random(mini, maxi);
  storedArray[2] = random(mini, maxi);
  storedArray[3] = random(mini, maxi);
  storedArray[4] = random(mini, maxi);
  storedArray[5] = random(mini, maxi);
  storedArray[6] = random(mini, maxi);
  storedArray[7] = random(mini, maxi);
  storedArray[8] = random(mini, maxi);
  storedArray[9] = random(mini, maxi);
  storedArray[10] = random(mini, maxi);
  storedArray[11] = random(mini, maxi);
  storedArray[12] = random(mini, maxi);
  storedArray[13] = random(mini, maxi);
  storedArray[14] = random(mini, maxi);
  storedArray[15] = random(mini, maxi);
  storedArray[16] = random(mini, maxi);
  storedArray[17] = random(mini, maxi);
  storedArray[18] = random(mini, maxi);
  storedArray[19] = random(mini, maxi);
}

//Use debounce if needed
void debounce(int buttonPin) {
  // read the state of the switch into a local variable:
  int reading = digitalRead(buttonPin);

  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH),  and you've waited
  // long enough since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;

      // only toggle the LED if the new button state is HIGH
      if (buttonState == HIGH) {
        Serial.println(buttonPin-1)
      }
    }
  }

  // set the LED:
  digitalWrite(ledPin, ledState);

  // save the reading.  Next time through the loop,
  // it'll be the lastButtonState:
  lastButtonState = reading;
}
