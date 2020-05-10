/* Pomodoro Timer
 *  by Thomas Juma <http://www.campfossa.org>
 *  
 *  The pomodoro technique is a time management method that uses a timer to break down
 *  work into intervals, traditionally 25 minutes in length, seperated by short breaks.
 *  Each interval is known as a pomodoro.
 *  
 *  This timer should allow you to do the following:
 *  1. Start the pomodoro timer
 *  2. Stop the pomodoro timer
 *  3. Pause the pomodoro timer
 *  5. Get notified when a work interval has ended
 *  6. Get notified when a short or long break has ended
 *  7. See the number of pomodoros you've achieved at any time
 *  
 */
#include <Servo.h>

// Here we have a servo that should turns a circular top kind of like a clock or timer.
// The servo helps us indicate how far along a pomodoro you are. It should count from 25
// minutes to 0 and then reset back to 0. If we are on a short break it should count from 5 to 
// 0 and for the long break it should count from 15 to 0.
Servo pomoServo;

// The pin that the servo signal is connected to
int pomoServoPin = 9;

// The length of one pomodoro work interval (25 minutes by tradition)
int pomodoroLength = 10;

// The length of one short break interval (5 minutes by tradition)
int shortBreakLength = 2;

// The number of pomodoros before taking a long break
int longBreakAfter = 4;

// The length of one long break interval (15 minutes by tradition)
int longBreakLength = 5;

// A counter to keep track of the number of pomodoros achieved and short or long breaks taken
int pomodoros = 0;
int shortBreaks = 0;
int longBreaks = 0;

// How many seconds in a minute. (During testing I would adjust this to make it shorter)
int minuteDuration = 5;

// The timer
int timer = 0;
int minutes = 0;
int breaks = 0;

// Number of servo steps to make per minute
int minuteSteps = 6; // 360 degrees divide by 60

// Position of where the interval timers begin on the circular rotating disc
int pomodoroStart = 0;
int shortBreakStart = 0;
int longBreakStart = 0;

// Pomodoro states (Allows us to pause the timer, stop the timer or reset everything)
int statusStopped = 0;
int statusRunning = 1;
int statusPaused = 11;

int pomodoroStatus = 0;

// Start button
int startButton = 6;
int startButtonLed = 7;

// Pause button
int pauseButton = 2;
int pauseButtonLed = 3;

// Stop button
int stopButton = 4;
int stopButtonLed = 5;

// Buzzer
int buzzer = 10;

void setup() {
  // Attach our servo object to the output pin
  pomoServo.attach(pomoServoPin);

  // Always reset to position 0 when the script starts
  pomoServo.write(0);

  // Initialize the buttons and their leds
  pinMode(startButton, INPUT);
  pinMode(startButtonLed, OUTPUT);
  
  pinMode(pauseButton, INPUT);
  pinMode(pauseButtonLed, OUTPUT);
  
  pinMode(stopButton, INPUT);
  pinMode(stopButtonLed, OUTPUT);

  // Initialize the buzzer
  pinMode(buzzer, OUTPUT);

  // Start serial for logging purposes
  Serial.begin(9600);

  // Set the default status of the pomodoro when the device is booted
  pomodoroStatus = statusStopped;
  digitalWrite(stopButtonLed, HIGH);
}

void loop() {
  // Wait for the user to press and hold the start button inorder to start the timer
  // Otherwise display status stopped

  // Start the pomodoro timer in the next loop
  if (digitalRead(startButton) == HIGH) {
    Serial.print("Start button pressed");
    Serial.println();
    setPomodoroStatus(statusRunning);
  }

  // Reset the pomodoro fully
  if (digitalRead(stopButton) == HIGH) {
    Serial.print("Stop button pressed");
    Serial.println();
    resetPomodoro();
  }
  
  if (pomodoroStatus == statusRunning)
  {
    Serial.print("Pomodoro status: RUNNING");
    Serial.println();
    
    // Start a pomodoro
    pomodoroTimer();
  
    breaks++;
  
    // When a pomodoro is done determine whether to start a short break or long break depending on the number of pomodoros
    // that have been achieved.
    if (breaks <= longBreakAfter)
    {
      shortBreakTimer();
    }
    else
    {
      longBreakTimer();
  
      // Reset the number of breaks
      breaks = 0;
    }
  }
  else
  {
    Serial.print("Pomodoro status: STOPPED");
    Serial.println();
    delay(1000);
  }
}

// Times a pomodoro interval (Traditionally 25 minutes)
void pomodoroTimer() {
  playBuzzer(1);
  
  Serial.print("Starting pomodoro timer....");
  Serial.println();
  
  // Reset the servo to pomodoro timer starting position
  pomoServo.write(pomodoroStart);

  // Reset the timer
  resetTimer();

  do
  {
    // Check if pause button is pressed
    if (digitalRead(pauseButton) == HIGH) {
      Serial.print("Pause button pressed");
      Serial.println();
      setPomodoroStatus(statusPaused);
    }
    
    // Check if stop button is pressed
    if (digitalRead(stopButton) == HIGH) {
      Serial.print("Stop button pressed");
      Serial.println();
      setPomodoroStatus(statusStopped);
      break;
    }
    
    // Count every second
    delay(1000);
    timer++;

    // Every 60 seconds update the minutes
    if (timer == minuteDuration) {
      minutes = minutes + 1;
      
      Serial.print(minutes);
      Serial.print(" minutes passed");
      Serial.println();
  
      pomoServo.write(minutes * minuteSteps);
      timer = 0;
    }
  }
  while(minutes < pomodoroLength);

  // Increment the number of pomodoroes
  pomodoros++;
}

// Times a short break interval (Traditionally 5 minutes)
void shortBreakTimer() {
  playBuzzer(2);
  
  Serial.print("Starting short break timer....");
  Serial.println();

  // Reset the timer
  resetTimer();
  
  // Reset the servo to pomodoro timer starting position
  pomoServo.write(shortBreakStart);

  do
  {
    // Check if pause button is pressed
    if (digitalRead(pauseButton) == HIGH) {
      Serial.print("Pause button pressed");
      Serial.println();
      setPomodoroStatus(statusPaused);
    }
    
    // Check if stop button is pressed
    if (digitalRead(stopButton) == HIGH) {
      Serial.print("Stop button pressed");
      Serial.println();
      setPomodoroStatus(statusStopped);
      break;
    }

    // Count every second
    delay(1000);
    timer++;

    // Every 60 seconds update the minutes
    if (timer == minuteDuration) {
      minutes = minutes + 1;
      
      Serial.print(minutes);
      Serial.print(" short break minutes passed");
      Serial.println();
  
      pomoServo.write(minutes * minuteSteps);
      timer = 0;
    }
  }
  while(minutes < shortBreakLength);

  // Increment the number of short breaks
  shortBreaks++;
}

// Times a long break interval (Traditionally 15 minutes)
void longBreakTimer() {
  playBuzzer(3);
  
  Serial.print("Starting long break timer....");
  Serial.println();

  // Reset the timer
  resetTimer();
  
  // Reset the servo to pomodoro timer starting position
  pomoServo.write(longBreakStart);

  do
  {
    // Check if pause button is pressed
    if (digitalRead(pauseButton) == HIGH) {
      Serial.print("Pause button pressed");
      Serial.println();
      setPomodoroStatus(statusPaused);
    }
    
    // Check if stop button is pressed
    if (digitalRead(stopButton) == HIGH) {
      Serial.print("Stop button pressed");
      Serial.println();
      setPomodoroStatus(statusStopped);
      break;
    }

    // Count every second
    delay(1000);
    timer++;

    // Every 60 seconds update the minutes
    if (timer == minuteDuration) {
      minutes = minutes + 1;
      
      Serial.print(minutes);
      Serial.print(" long break minutes passed");
      Serial.println();
  
      pomoServo.write(minutes * minuteSteps);
      timer = 0;
    }
  }
  while(minutes < longBreakLength);

  // Increment the number of long breaks
  longBreaks++;
}

void setPomodoroStatus(int pomStatus)
{
  if (pomStatus == statusRunning) {
    pomodoroStatus = statusRunning;
    digitalWrite(startButtonLed, HIGH);
    digitalWrite(stopButtonLed, LOW);
    digitalWrite(pauseButtonLed, LOW);
  }
  else if (pomStatus == statusPaused) {
    pomodoroStatus = statusPaused;
    digitalWrite(startButtonLed, LOW);
    digitalWrite(stopButtonLed, LOW);
    digitalWrite(pauseButtonLed, HIGH);
    pauseTimer();
  }
  else {
    pomodoroStatus = statusStopped;
    digitalWrite(startButtonLed, LOW);
    digitalWrite(stopButtonLed, HIGH);
    digitalWrite(pauseButtonLed, LOW);
  }
}

// Pauses the timer (e.g. When someone needs to take a bathroom break or there is some other interferance)
void pauseTimer()
{
  pomodoroStatus = statusPaused;
  do
  {
    if (digitalRead(startButton) == HIGH) {
      Serial.print("Start button pressed");
      Serial.println();
      setPomodoroStatus(statusRunning);
    }
  }
  while(pomodoroStatus == statusPaused);
}

void playBuzzer(int times)
{
  if (times == 1)
  {
    tone(buzzer, 1000); // Send 1KHz sound signal
    delay(1000);
    noTone(buzzer); // Stop sound
  }
  else {
    for (int i = 1; i <= times; i++)
    {
      tone(buzzer, 1000); // Send 1KHz sound signal
      delay(200);
      noTone(buzzer); // Stop sound
    }
  }
}

// Reset the number of timer and minutes to zero (e.g. Jumping between breaks and the pomodoro)
void resetTimer()
{
  timer = 0;
  minutes = 0;
}

// Full reset of the pomodoro back to 0
void resetPomodoro()
{
  timer = 0;
  minutes = 0;
  breaks = 0;
  pomodoros = 0;
  shortBreaks = 0;
  longBreaks = 0;
  pomoServo.write(0);
}
