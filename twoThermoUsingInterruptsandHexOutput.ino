#include <SPI.h>

// Define CS pins for each SPI interface
#define MAXCS1 25
#define MAXCS2 26
#define LED_PIN 2

// Define shared SPI pins
// #define MAXSCLK 18
// #define MAXDO 19
// #define MAXMOSI 23

byte dummyData[] = {0x00, 0x00, 0x00, 0x00};
SPIClass *MAX1 = NULL;
// SPIClass *MAX2 = NULL;

int received_data1[4];
hw_timer_t * timer = NULL;
int LED_STATE=LOW;
// int received_data2[4];
int TemperatureFlag1 = 0, TemperatureFlag2 = 0, intr_cntr = 0;

void IRAM_ATTR onTimer() {   
  // a=1;   //Defining Inerrupt function with IRAM_ATTR for faster access
//  portENTER_CRITICAL_ISR(&timerMux);
//  TemperatureFlag = 1;
  intr_cntr = intr_cntr + 1;
  if(intr_cntr == 500){
    TemperatureFlag1 = 1;
    // intr_cntr = 0;
  }
  if(intr_cntr == 1000){
    TemperatureFlag2 = 1;
    intr_cntr = 0;
  }
  //  if(intr_cntr == 500 || intr_cntr == 1000{
  //   if(intr_cntr == 500){
  //   TemperatureFlag1 = 1;
  //   }
  //   if(intr_cntr == 1000){
  //    TemperatureFlag2 = 1;
  //    }
    // intr_cntr = 0;

  
}


void setup() {
    Serial.begin(115200); 

    MAX1 = new SPIClass(VSPI);
    // MAX2 = new SPIClass(HSPI);

  
    MAX1->begin();
    // MAX2->begin(MAXSCLK, MAXDO, MAXCS2);

    // CS pins as outputs
    pinMode(MAXCS1, OUTPUT);
    digitalWrite(MAXCS1, HIGH); // Deselect the VSPI device
    pinMode(MAXCS2, OUTPUT);
    digitalWrite(MAXCS2, HIGH);
    pinMode (LED_PIN, OUTPUT);
 
    timer = timerBegin(0, 80, true);             // timer 0, prescalar: 80, UP counting
    timerAttachInterrupt(timer, &onTimer, true);   // Attach interrupt
    timerAlarmWrite(timer, 1000, true);     // Match value= 1000000 for 1 sec. delay.
    timerAlarmEnable(timer);  // Deselect the HSPI device
}

void loop() {
    
    if (TemperatureFlag1 == 1) {
        Serial.print("Sensor 1 data:");
        readSensor(MAX1, MAXCS1);
        Serial.print("\n");
        
        TemperatureFlag1 = 0;
        LED_STATE= !LED_STATE;         
        digitalWrite(LED_PIN, LED_STATE);
        
         // Reset the flag
    }

    if (TemperatureFlag2 == 1) {
        Serial.print("Sensor 2 data:");
        readSensor(MAX1, MAXCS2);
        Serial.print("\n");
        TemperatureFlag2 = 0; // Reset the flag
    }
}

void readSensor(SPIClass *spi, int csPin) {
    spi->beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));
    digitalWrite(csPin, LOW); // Pull CS low to prepare for transfer
    for (int i = 0; i < 4; i++) {
        received_data1[i] = spi->transfer(dummyData[i]);
        
        Serial.print(received_data1[i],HEX); // Send dummy data
        Serial.print(" ");
    }
    digitalWrite(csPin, HIGH); // Pull CS high to signify end of data transfer
    spi->endTransaction();
  
}
