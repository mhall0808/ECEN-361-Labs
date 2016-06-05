/**
   Mark Hall
   ECEN 361
   Lab 6: FreeRTOS - Semaphore

   Part of the code was copied from the example file located here:

    https://www.hackster.io/feilipu/using-freertos-semaphores-in-arduino-ide-b3cd6c

   My own comments have been made in order for me to understand each function.

   Furthermore, some code was also copied from Brother Grimmett's example file that
   he provided for the class.

   This lab is an illustration of a simple Semaphore, allowing two tasks to share one
   resource.  In this case, that resource will be an LED that will be toggled by each
   task.
*/
#include <Arduino_FreeRTOS.h>
#include <semphr.h>  // add the FreeRTOS functions for Semaphores (or Flags).


// This will create a global variable semaphore.  Right now in our learning, it is a
// glorified bool.
SemaphoreHandle_t xSerialSemaphore;

// creating variables for the pin 13 output, and the timer.  This way I can change the output
// if necessary.
int timer1_counter;
uint8_t ledOut = 13;

// define two Tasks, one to turn on the LED, one to turn it off.
void TaskLEDOff( void *pvParameters );
void TaskLEDOn ( void *pvParameters );

/**
   SETUP

    Setup will do the following:
      - Initialize the Serial port
      - Initialize the semaphore as a Mutex
      - Create a timed interrupt
*/
void setup() {

  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  Serial.println("Serial Setup Complete");

  pinMode(ledOut, OUTPUT);

  if ( xSerialSemaphore == NULL )  // Check to confirm that the Serial Semaphore has not already been created.
  {
    xSerialSemaphore = xSemaphoreCreateMutex();  // Create a mutex semaphore we will use to manage the Serial Port
    if ( ( xSerialSemaphore ) != NULL )
      xSemaphoreGive( ( xSerialSemaphore ) );  // Make the Serial Port available for use, by "Giving" the Semaphore.
  }

  // Now set up two Tasks to run independently.
  xTaskCreate(
    TaskLEDOff
    ,  (const portCHAR *)"LEDOff" 
    ,  128  // Stack size
    ,  NULL
    ,  1  // Priority, with 1 being the highest, and 4 being the lowest.
    ,  NULL );

  xTaskCreate(
    TaskLEDOn
    ,  (const portCHAR *) "LEDOn"
    ,  128  // Stack size
    ,  NULL
    ,  1  // Priority
    ,  NULL );

  // Now the Task scheduler, which takes over control of scheduling individual Tasks, is automatically started.
}

void loop()
{
  // Empty. Things are done in Tasks.
}

/**
   TASK LED OFF

    These functions are quite simple, because we did most of the work elsewhere.
    It will take the semaphore if possible. If not possible, it will wait.  Forever.

    Then it will turn the LED off.

*/
void TaskLEDOff( void *pvParameters __attribute__((unused)) )  // This is a Task.
{
  for (;;) // A Task shall never return or exit.
  {
    if ( xSemaphoreTake( xSerialSemaphore, portMAX_DELAY ) == pdTRUE)
    {
      digitalWrite(ledOut, !digitalRead(ledOut));
      Serial.println("Task 1 Triggered!");
      delay(1000);
      xSemaphoreGive(xSerialSemaphore);
    }

    vTaskDelay(1);  // one tick delay (15ms) in between reads for stability
  }
}

/**
   TASK LED ON

    These functions are quite simple, because we did most of the work elsewhere.
    It will take the semaphore if possible. If not possible, it will wait.  Forever.

    Then it will turn the LED on.

*/
void TaskLEDOn( void *pvParameters __attribute__((unused)) )  // This is a Task.
{
  unsigned long value;
  for (;;)
  {
    if ( xSemaphoreTake( xSerialSemaphore, portMAX_DELAY ) == pdTRUE)
    {
      digitalWrite(ledOut, !digitalRead(ledOut));
      Serial.println("Task 2 Triggered!");
      delay(2000);
      xSemaphoreGive(xSerialSemaphore);
    }
    vTaskDelay(1);  // one tick delay (15ms) in between reads for stability
  }
}
