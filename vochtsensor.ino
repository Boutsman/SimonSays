/*
*@ author Henning Lakiere
*@ author Willem Geerts
*/

//Capacitive humidity
const int HUM_OUT_PIN = A3;
const int HUM_IN_PIN = A1;
const float IN_STRAY_CAP_TO_GND = 24.48; 
const float IN_EXTRA_CAP_TO_GND = 0.0;
const float IN_CAP_TO_GND  = IN_STRAY_CAP_TO_GND + IN_EXTRA_CAP_TO_GND;
const int MAX_ADC_VALUE = 1023;
boolean start= false;
int VALUE = 0;
int teller = 0;


void setup() {
  
  pinMode(HUM_OUT_PIN, OUTPUT);
  pinMode(HUM_IN_PIN, OUTPUT);
  
  
  Serial.begin(115200);
  VALUE = initialize();
  
}

void loop() {
  //delay(200);
  //Serial.print("Waarde= ");
  boolean ON_OFF = capacitanceHumidity(VALUE);
  

  if(ON_OFF == true) {
    teller = teller + 1;
  }
  else {
    teller = 0;
  }
  
  delay(100);
  if(teller = 600){ // 1 minuut
    ON_OFF = false;
    // end_program!
  }

  
  
    
}

//Opmerking Stijn, nut van deze functie?
int initialize(){
  
  int samples = 50;
  int data_init[samples];

  pinMode(HUM_IN_PIN, INPUT);
  digitalWrite(HUM_OUT_PIN, HIGH);
  int val = analogRead(HUM_IN_PIN);
  int sum = 0;
  int gem = 0;

  for(int i = 0; i<samples; i++){
    data_init[i] = val;
    sum = sum + data_init[i];
  }
  
  gem = sum/samples;
  
  Serial.println(gem);
  return gem;    
}

boolean capacitanceHumidity(int init_value){
  pinMode(HUM_IN_PIN, INPUT);
  digitalWrite(HUM_OUT_PIN, HIGH);
  int val = analogRead(HUM_IN_PIN);
  digitalWrite(HUM_OUT_PIN, LOW);
  pinMode(HUM_IN_PIN, OUTPUT);
  Serial.println(val);

  if(val >= init_value+15){    
    start = true;
    //float capacitance = (float)val * IN_CAP_TO_GND / (float)(MAX_ADC_VALUE - val);
  }
  else{
    start = false;
  }
 return start;  
}
