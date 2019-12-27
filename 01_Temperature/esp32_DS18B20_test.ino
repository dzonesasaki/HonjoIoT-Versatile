#include <OneWire.h> 
#include <DallasTemperature.h> // https://github.com/milesburton/Arduino-Temperature-Control-Library
//ref to example/DallasTemperature/single.ino

#define PIN_NUM_TEMPE 26
#define NUM_BIT_WIDTH_TEMPE 12

OneWire myOneWire(PIN_NUM_TEMPE); 
DallasTemperature myTempSenses(&myOneWire);
DeviceAddress myAryAddressTemp;

void setup(void) 
{ 
  Serial.begin(115200); 
  
  myTempSenses.begin(); 

  Serial.print("Num of Devices = ");
  Serial.println(myTempSenses.getDeviceCount(), DEC); // it must be 1

  myTempSenses.getAddress(myAryAddressTemp, 0);
  myTempSenses.setResolution(myAryAddressTemp,NUM_BIT_WIDTH_TEMPE);
 
  Serial.print("Bit Width for resolution = ");
  Serial.println(myTempSenses.getResolution(myAryAddressTemp), DEC);  // it must be 12 (=NUM_BIT_WIDTH_TEMPE)
} 
void loop(void) 
{ 
 Serial.print(" Requesting temperatures..."); 

 myTempSenses.requestTemperatures();
 float fTempe = myTempSenses.getTempC(myAryAddressTemp);
 
 Serial.println("DONE"); 
 Serial.print("Temperature is: "); 
 Serial.print(fTempe);
 delay(1000); 
} 
