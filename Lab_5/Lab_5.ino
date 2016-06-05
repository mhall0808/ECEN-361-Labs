/**
   Mark Hall
   ECEN 361
   Lab 5: FreeRTOS

   Part of the code was copied from the example file located here:

    https://www.hackster.io/feilipu/using-freertos-semaphores-in-arduino-ide-b3cd6c

   My own comments have been made in order for me to understand each function.

   Furthermore, some code was also copied from Brother Grimmett's example file that
   he provided for the class.

   This lab has an LED that will toggle on and off.  The time that it will toggle will
   vary from trigger to trigger.
*/
#include <Arduino_FreeRTOS.h>
#include <semphr.h>  // add the FreeRTOS functions for Semaphores (or Flags).

// Semaphore creation.
SemaphoreHandle_t xTimingSemaphore;

// The queue will handle the queue, obviously, but also it will pass values from
// one function to the next.
QueueHandle_t xIntegerQueue;

// My wait function is extremely simple; multiply it by a factor of 100 for the 
// wait time.
unsigned int toWait = 1;

/* The service routine for the interrupt. This is the interrupt that the task
will be synchronized with. */
static void vExampleInterruptHandler( void );

void TaskLedOn( void *pvParameters );
void TaskDisplay( void *pvParameters );
const uint8_t ledPin = 13;

/**
 * SETUP
 * 
 *  Setup will do the following:
 *    - Initialize the serial port
 *    - initialize both the semaphore, and the queue
 *    - initialize the two tasks and assign them equal priorities
 *    - begin the task scheduler.
 */
void setup() {

// initialize serial communication at 9600 bits per second:
Serial.begin(9600);
Serial.println("Setup ready");
vSemaphoreCreateBinary( xTimingSemaphore );
xIntegerQueue = xQueueCreate( 1, sizeof( unsigned long ) );
pinMode(ledPin, OUTPUT);

// Now set up the Task to handle the analog read.

xTaskCreate(
TaskAnalogRead
, (const portCHAR *) "AnalogRead"
, 128 // Stack size
, NULL
, 1 // Priority
, NULL );
xTaskCreate(
TaskDisplay
, (const portCHAR *) "Display"
, 128 // Stack size
, NULL
, 1 // Priority
, NULL );

// Now the Task scheduler, which takes over control of scheduling individual Tasks, is automatically started.
vTaskStartScheduler();
}

void loop()
{
// Empty. Things are done in Tasks.
}

/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/

/**
 * TASK LED ON
 *  This task is handled by the task manager.  it will flip the LED pin,
 *  then take the semaphore. Since there is no trigger, we will remove the 
 *  second Semaphore trigger from Brother Grimmetts code.
 *  
 *  The delay function is a simple one.  In lab 6, we used a timed interrupt, 
 *  and I could have just as easily used that.  However, I didn't think it was
 *  necessary.
 *  
 *  Then, we simply increment the amount of time to wait by 100, and carry on.
 */
void TaskLedOn( void *pvParameters __attribute__((unused)) ) // This is a Task.
{
Serial.println("Starting analog task");
for (;;)
{
  digitalWrite(ledPin, !digitalRead(ledPin));
  xSemaphoreTake( xTimingSemaphore, portMAX_DELAY );
  unsigned long ulValueToSend = toWait * 100;
  xQueueSendToBack( xIntegerQueue, &ulValueToSend, 0 );
  xSemaphoreGive( xTimingSemaphore);
  delay(toWait * 100);
  toWait++;
  vTaskDelay(1); // one tick delay (15ms) in between reads for stability
  }
}

/**
 * TASK DISPLAY
 *  This task will say "Hello World" and output the parameter given by the queue.
 */
void TaskDisplay( void *pvParameters __attribute__((unused)) ) // This is a Task.
{
  unsigned long value;
  
  Serial.println("Starting display task");
  for (;;)
    {
      xQueueReceive( xIntegerQueue, &value, portMAX_DELAY );
      Serial.println("Hello World!");
      Serial.println(value);
      vTaskDelay(1); // one tick delay (15ms) in between reads for stability
    }
}
