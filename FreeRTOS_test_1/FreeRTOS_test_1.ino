#include <Arduino_FreeRTOS.h>   // use FreeRTOS, install lib required
#include <semphr.h>  // add the FreeRTOS functions for Semaphores (or Flags).

#include <SPI.h>    // use SPI library for MicroMag
//MY //#include <MicroMag.h> // use MicroMag, install lib required

#include <Wire.h>   // use I2C library
//MY #include <LiquidCrystal_I2C.h> // // use LiquidCrystal_I2C FC-113, install lib required

#define SERIAL_DBG_ON   // enable serial monitor output debug message
/**********************************************************/
// initialize the library with the numbers of the interface pins
//MY LiquidCrystal_I2C lcd(0x27,16,2); // set the LCD address to 0x27 for a 16 chars and 2 line display

// Declare a mutex Semaphore Handle which we will use to manage the Serial Port.
// It will be used to ensure only only one Task is accessing this resource at any time.
SemaphoreHandle_t xSerialSemaphore;

// MicroMag Pins definition
const uint8_t mmSsPin = 10; // This is the chip select pin on the MM3
const uint8_t mmDrdyPin = 9;
const uint8_t mmResetPin = 8;
// Change this to suit your MicroMag version.
const uint8_t model = 3;

int16_t data[model];

// UltraSound module
const int trigPin = 6; 
const int echoPin = 7;

float duration, distance; 


// define two Tasks for DigitalRead & AnalogRead
void TaskUltraSoundRead( void *pvParameters );
void TaskMagnetRead( void *pvParameters );
void TaskDispLCDDummy( void *pvParameters );

// Create a MicroMag object
//MY MicroMag MM = MicroMag::MicroMag3(mmSsPin, mmDrdyPin, mmResetPin);


// the setup function runs once when you press reset or power the board
void setup() {

  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);

  pinMode(trigPin, OUTPUT); 
  pinMode(echoPin, INPUT); 

  if (MM.begin() != 0) {
    Serial.println("Could not initialise MicroMag");
    while (1)
      ; // no point in continuing
  }
  lcd.init(); //initialize the lcd
  lcd.backlight(); //open the backlight 

  // Semaphores are useful to stop a Task proceeding, where it should be paused to wait,
  // because it is sharing a resource, such as the Serial port.
  // Semaphores should only be used whilst the scheduler is running, but we can set it up here.
  if ( xSerialSemaphore == NULL )  // Check to confirm that the Serial Semaphore has not already been created.
  {
    xSerialSemaphore = xSemaphoreCreateMutex();  // Create a mutex semaphore we will use to manage the Serial Port
    if ( ( xSerialSemaphore ) != NULL )
      xSemaphoreGive( ( xSerialSemaphore ) );  // Make the Serial Port available for use, by "Giving" the Semaphore.
  }

  // Now set up two Tasks to run independently.
  xTaskCreate(
    TaskUltraSoundRead
    ,  (const portCHAR *)"DigitalRead"  // A name just for humans
    ,  128  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  2  // Priority, with 1 being the highest, and 4 being the lowest.
    ,  NULL );

  xTaskCreate(
    TaskMagnetRead
    ,  (const portCHAR *) "TaskMagnetRead"
    ,  128  // Stack size
    ,  NULL
    ,  1  // Priority
    ,  NULL );

  xTaskCreate(
    TaskDispLCDDummy
    ,  (const portCHAR *) "DispLCD"
    ,  128  // Stack size
    ,  NULL
    ,  3  // Priority
    ,  NULL );

  // Now the Task scheduler, which takes over control of scheduling individual Tasks, is automatically started.
}

void loop()
{
  // Empty. Things are done in Tasks.
}

/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/

void TaskUltraSoundRead( void *pvParameters __attribute__((unused)) )  // This is a Task.
{
  for (;;) // A Task shall never return or exit.
  {
    // put your main code here, to run repeatedly:
    digitalWrite(trigPin, LOW); 
    delayMicroseconds(2); 
    digitalWrite(trigPin, HIGH); 
    delayMicroseconds(10); 
    digitalWrite(trigPin, LOW); 
    
    duration = pulseIn(echoPin, HIGH); 
    
    distance = (duration*.0343)/2; 
/*    
    Serial.print("Distance: "); 
    Serial.print(distance); 
    Serial.println(" cm"); 
    delay(1000); 
*/
    //vTaskDelay(1);  // one tick delay (15ms) in between reads for stability
    vTaskDelay(200 / portTICK_PERIOD_MS);  // nof tick delay (200ms)
  }
}

void TaskMagnetRead( void *pvParameters __attribute__((unused)) )  // This is a Task.
{

  for (;;)
  {
    double total = 0.0; // total field value
    for (uint8_t axis = 0; axis < model; ++axis) 
    {
      int8_t r;
      if ((r = MM.read(axis, MM_PERIOD_4096, data[axis])) == 0) 
      {
        if (axis)
      #ifdef SERIAL_DBG_ON
        Serial.print("   ");
        Serial.print(char('X' + axis));
        Serial.print(" = ");
        Serial.print(data[axis]);
      #endif
        total += sqrt(double(data[axis]) * data[axis]);
      }
      else 
      {
      #ifdef SERIAL_DBG_ON
        Serial.print("Cannot read from ");
        Serial.print(char('X' + axis));
        Serial.print("axis: ");
        Serial.print(r, DEC);
      #endif
      }
    }
    #ifdef SERIAL_DBG_ON
    Serial.print("   Total = ");
    Serial.println(total);
    #endif
    
    //delay(2000);
    vTaskDelay(250 / portTICK_PERIOD_MS);  // nof tick delay (250ms)

    // See if we can obtain or "Take" the Serial Semaphore.
    // If the semaphore is not available, wait 5 ticks of the Scheduler to see if it becomes free.
    if ( xSemaphoreTake( xSerialSemaphore, ( TickType_t ) 5 ) == pdTRUE )
    {
      // We were able to obtain or "Take" the semaphore and can now access the shared resource.
      // We want to have the Serial Port for us alone, as it takes some time to print,
      // so we don't want it getting stolen during the middle of a conversion.
      // print out the value you read:
//      Serial.println(sensorValue);

      xSemaphoreGive( xSerialSemaphore ); // Now free or "Give" the Serial Port for others.
    }

    vTaskDelay(1);  // one tick delay (15ms) in between reads for stability
  }
}

void TaskDispLCDDummy( void *pvParameters __attribute__((unused)) )  // This is a Task. 
{
  int i = -100;
  for (;;)
  {
    for (int positionCounter1 = 0; positionCounter1 < 26; positionCounter1++)
    {
      //lcd.scrollDisplayLeft(); //Scrolls the contents of the display one space to the left.
      //lcd.print(array1[positionCounter1]); // Print a message to the LCD.
      for (i = -100; i <= 100; i++)
      {
        lcd.setCursor(0,0); // set the cursor to column 15, line 0
        lcd.print(distance); // Print a message to the LCD.
        
        lcd.setCursor(8,0); // set the cursor to column 15, line 0
        lcd.print("X="); // Print a message to the LCD.
        lcd.setCursor(10,0); // set the cursor to column 15, line 0
        lcd.print(data[0]); // Print a message to the LCD.
        
        lcd.setCursor(0,1); // set the cursor to column 15, line 0
        lcd.print("Y="); // Print a message to the LCD.
        lcd.setCursor(2,1); // set the cursor to column 15, line 0
        lcd.print(data[1]); // Print a message to the LCD.
        
        lcd.setCursor(8,1); // set the cursor to column 15, line 0
        lcd.print("Z="); // Print a message to the LCD.
        lcd.setCursor(10,1); // set the cursor to column 15, line 0
        lcd.print(data[2]); // Print a message to the LCD.
        
        vTaskDelay(150 / portTICK_PERIOD_MS);  // nof tick delay (150ms)
        lcd.clear(); //Clears the LCD screen and positions the cursor in the upper-left  corner.
      }
    }
/*
    lcd.setCursor(15,1); // set the cursor to column 15, line 1
    for (int positionCounter = 0; positionCounter < 26; positionCounter++)
    {
      lcd.scrollDisplayLeft(); //Scrolls the contents of the display one space to the left.
      lcd.print(array2[positionCounter]); // Print a message to the LCD.
      //delay(tim); //wait for 250 microseconds
      vTaskDelay(250 / portTICK_PERIOD_MS);  // nof tick delay (250ms)
    }
    lcd.clear(); //Clears the LCD screen and positions the cursor in the upper-left corner.
*/
  }
}
