/**
 * Mark Hall
 * ECEN 361
 * Lab 4: A Simple Scheduler
 * 
 * This program is designed to teach us how to create a simple scheduler; one that
 * does not prioritize, nor can it interrupt another task.
 */



///Up to this many tasks can be run, in addition to the idle task
#define MAXTASKS  8
uint8_t pulse1_pin = 3;
uint8_t pulse2_pin = 4;
uint8_t idle_pin = 7;
int pin1_status = LOW;
int pin2_status = HIGH;
 
///A task callback function
typedef void (*task_cb)();
 
/**
 * Initialise the scheduler.  This should be called once in the setup routine.
 */
void Scheduler_Init();
void Scheduler_StartTask(int16_t delay, int16_t period, task_cb task);
 
/**
 * Go through the task list and run any tasks that need to be run.  The main function should simply be this
 * function called as often as possible, plus any low-priority code that you want to run sporadically.
 */
uint32_t Scheduler_Dispatch();

typedef struct
{
  int32_t period;
  int32_t remaining_time;
  uint8_t is_running;
  task_cb callback;
} task_t;
 
task_t tasks[MAXTASKS];
 
uint32_t last_runtime;
 
void Scheduler_Init()
{
  last_runtime = millis();
}
 
void Scheduler_StartTask(int16_t delay, int16_t period, task_cb task)
{
  static uint8_t id = 0;
  if (id < MAXTASKS)
  {
    tasks[id].remaining_time = delay;
    tasks[id].period = period;
    tasks[id].is_running = 1;
    tasks[id].callback = task;
    id++;
  }
}


 /**
  * Okay, because this function is officially MY function, I will
  * go through each variable here.
  * 
  * currentMillis:  Collects the most recent time as given by the Arduino
  * previousMillis: Collects the previous runtime that this function was used.
  * curTask: Current task being ran.  If we find nothing, it will simply return null.
  * idle_time:  The most confusing variable at first!  Idle time is a wait time,
  *             determining whether we will go forward with a task or not.
  * 
  * This function will determine whether or not a function is callable or not yet.  
  *   First, we check and see if a function's countdown timer has hit zero yet.  If
  *   it did, then reset the timer and run the function.  If it didn't, then put the
  *   idle time at the lowest possible point.  
  */
uint32_t Scheduler_Dispatch()
{
  unsigned int currentMillis  =                     millis();
  unsigned int previousMillis = currentMillis - last_runtime;
  last_runtime                =                currentMillis;   // update last runtime.
  task_cb curTask             =                         NULL;   
  uint32_t idle_time          =                    999999999;   // Some ridiculous number.  It doesn't matter.
 
  // update each task's remaining time, and identify the first ready task (if there is one).
  for (int i = 0; i < MAXTASKS; i++){
    // If the task isn't running, no point checking it, right?
    if (tasks[i].is_running){
      tasks[i].remaining_time -= previousMillis;

      // Now we get to the fun part of things.
      if (tasks[i].remaining_time <= 0){
        if (curTask == NULL){
          // If a task hasn't been taken yet, select this one.
          curTask = tasks[i].callback;
          // reset the remaining time.  This is important!
          tasks[i].remaining_time += tasks[i].period;
          // it's not idling anymore.
          idle_time = 0;
          break;
        }
      }

      // This brings down the idle time to the lowest possible levels.
      else{
        if (idle_time > tasks[i].remaining_time)
          idle_time = tasks[i].remaining_time;
      }
    }
  }
  if (curTask != NULL){
    // If this works, it will call a function!
    curTask();
  }
  return idle_time;
}



// task function for PulsePin task
void pulse_pin1_task()
{
  Serial.println("Pin 1 Task");
  digitalWrite(pulse1_pin, !digitalRead(pulse1_pin));
}
 
// task function for PulsePin task
void pulse_pin2_task()
{
  Serial.println("Pin 2 Task");
  digitalWrite(pulse2_pin, !digitalRead(pulse2_pin));
}
 
// idle task
void idle(uint32_t idle_period)
{
  // this function can perform some low-priority task while the scheduler has nothing to run.
  // It should return before the idle period (measured in ms) has expired.  For example, it
  // could sleep or respond to I/O.
 
  // example idle function that just pulses a pin.
  Serial.println("idle process");
  digitalWrite(idle_pin, HIGH);
  delay(idle_period);
  digitalWrite(idle_pin, LOW);
}
 
void setup()
{
  pinMode(pulse1_pin, OUTPUT);
  pinMode(pulse2_pin, OUTPUT);
  pinMode(idle_pin,   OUTPUT);
  Serial.begin(9600);
 
  Scheduler_Init();
  Serial.println("Scheduler started");
 
  // Start task arguments are:
  //    start offset in ms, period in ms, function callback
 
  Scheduler_StartTask(0, 1000, pulse_pin1_task);
  Scheduler_StartTask(0, 1500, pulse_pin2_task);
}
 
void loop()
{
  uint32_t idle_period = Scheduler_Dispatch();
  if (idle_period)
  {
    idle(idle_period);
  }
}

