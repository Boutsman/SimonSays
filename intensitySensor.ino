#include <CapacitiveSensor.h>

CapacitiveSensor   cs_4_2 = CapacitiveSensor(4,2);
CapacitiveSensor   cs_4_3 = CapacitiveSensor(4,3);
CapacitiveSensor   cs_4_5 = CapacitiveSensor(4,5);
CapacitiveSensor   cs_4_7 = CapacitiveSensor(4,7);
CapacitiveSensor   cs_4_8 = CapacitiveSensor(4,8);

// value: 1  3  5  7  9  +  overlappende waarden: 2  4  6  8  //
// dus value kan volgende waarden aannemen:                   // 9 stappen genoeg?
// 1  2  3  4  5  6  7  8  9                                  //
int sensitivity = 5;              // default value
String valueBar = "(*)(*)(*)(*)(*)";  //default value

void setup()                    
{

   //cs_4_2.set_CS_AutocaL_Millis(0xFFFFFFFF);     // turn off autocalibrate on channel 1 - just as an example
   Serial.begin(9600);

}

void loop()                    
{
    long start = millis();
    long sensor1 =  cs_4_2.capacitiveSensor(30);
    long sensor2 =  cs_4_3.capacitiveSensor(30);
    long sensor3 =  cs_4_5.capacitiveSensor(30);
    long sensor4 =  cs_4_7.capacitiveSensor(30);
    long sensor5 =  cs_4_8.capacitiveSensor(30);
    
    if (sensor1 > (sensor2 + 100)) 
    {
      sensitivity = 1;
      valueBar = "(*)";
    } 
    else if (sensor1 > 100 && sensor2 > 100)
    {
      sensitivity = 2;
      valueBar = "(*)(*)";
    }
    else if (sensor2 > (sensor1 + 100) && sensor2 > (sensor3 + 100))
    {
      sensitivity = 3;
      valueBar = "(*)(*)(*)";
    }
    else if (sensor2 > 100 && sensor3 > 100)
    {
      sensitivity = 4;
      valueBar = "(*)(*)(*)(*)";
    }
    else if (sensor3 > (sensor2 + 100) && sensor3 > (sensor4 + 100))
    {
      sensitivity = 5;
      valueBar = "(*)(*)(*)(*)(*)";
    }
    else if (sensor3 > 100 && sensor4 > 100)
    {
      sensitivity = 6;
      valueBar = "(*)(*)(*)(*)(*)(*)";
    }
    else if (sensor4 > (sensor3 + 100) && sensor4 > (sensor5 + 100))
    {
      sensitivity = 7;
      valueBar = "(*)(*)(*)(*)(*)(*)(*)";
    }
    else if (sensor4 > 100 && sensor5 > 100)
    {
      sensitivity = 8;
      valueBar = "(*)(*)(*)(*)(*)(*)(*)(*)";
    }
    else if (sensor5 > (sensor4 + 100))
    {
      sensitivity = 9;
      valueBar = "(*)(*)(*)(*)(*)(*)(*)(*)(*)";
    }

    Serial.print(millis() - start);        // check on performance in milliseconds
    Serial.print("\t");                    // tab character for debug window spacing

    Serial.print(sensor1);                  // print sensor output 1
    Serial.print("\t");
    Serial.print(sensor2);                  // print sensor output 2
    Serial.print("\t");
    Serial.print(sensor3);                  // print sensor output 3
    Serial.print("\t");
    Serial.print(sensor4);                  // print sensor output 4
    Serial.print("\t");
    Serial.print(sensor5);                  // print sensor output 5
    Serial.print("\t");
    Serial.print(sensitivity );                    // print value                  
    Serial.print("\t");
    Serial.print(valueBar);                 // print valueBar
    Serial.print("\n");

    delay(50);                             // arbitrary delay to limit data to serial port 
}
