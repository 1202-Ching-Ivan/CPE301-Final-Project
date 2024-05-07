//CPE Final Project
//Ivan Ching, Angela Wu, and Divisha Naharas

//Libraries
#include <Wire.h>
#include <LiquidCrystal.h>
#include <Stepper.h>
#include <DHT.h>
#include <DS3231-RTC.h>


//Analog Pins: Senors
#define WATER_LEVEL_PIN A0
#define TEMPERATURE_SENSOR A1
#define HUMIDITY_SENSOR A2

//Digital Pins: Stepper Fan Motor
#define FAN_MOTOR 6
#define STEPPER_MOTOR1 8
#define STEPPER_MOTOR2 9
#define STEPPER_MOTOR3 10
#define STEPPER_MOTOR4 11

//Digital Pins: Start/Stop Button
#define START_BUTTON 12
#define STOP_BUTTON 13

//Digital Pins: LEDs Pins
#define YELLOW_LED 28
#define RED_LED 29
#define BLUE_LED 30
#define GREEN_LED 31

//Senors Thresholds
#define WATER_LEVEL_THRESHOLD 500
#define TEMPERATURE_THRESHOLD 30
#define HUMIDITY_THRESHOLD 60

#define RDA 0x80
#define TBE 0x20


//Object Initializaions
DS3231 rtc;
LiquidCrystal lcd(22, 23, 24, 25, 26, 27);
DHT dht(TEMPERATURE_SENSOR, DHT11);
Stepper stepper(2048, STEPPER_MOTOR1, STEPPER_MOTOR2, STEPPER_MOTOR3, STEPPER_MOTOR4);

volatile unsigned char *myUCSR0A = (unsigned char *)0x00C0;
volatile unsigned char *myUCSR0B = (unsigned char *)0x00C1;
volatile unsigned char *myUCSR0C = (unsigned char *)0x00C2;
volatile unsigned int *myUBRR0 = (unsigned int *)0x00C4;
volatile unsigned char *myUDR0 = (unsigned char *)0x00C6;

//Flags
volatile bool startButtonPressed = false;
volatile bool stopButtonPressed = false;
volatile bool isRunning = false;

//State Flags
enum States {
  DISABLED,
  IDLE,
  RUNNING,
  ERROR,
  START
};

States currentState = DISABLED;
States prevState = START;

//Function Names
void idleState();
void errorState();
void runningState();
void disabledState();
void updateDateTime();
void displayTemperatureAndHumidity();
void checkWaterLevel();
void startButtonISR();
void stopButtonISR();

//Set Up
void setup() {
  Serial.begin(9600);

  // set start and stop as input
  DDRB &= ~((1 << START_BUTTON) | (1 << STOP_BUTTON));
  // pull up resistors
  PORTB |= ((1 << START_BUTTON) | (1 << STOP_BUTTON));
  // Set fan motor and LEDs as outputs
  DDRD |= (1 << FAN_MOTOR) | (1 << YELLOW_LED) | (1 << GREEN_LED) | (1 << RED_LED) | (1 << BLUE_LED);
  // Turn off LEDs initially
  PORTD |= (1 << YELLOW_LED) | (1 << GREEN_LED) | (1 << RED_LED) | (1 << BLUE_LED);

  dht.begin();
  Wire.begin();
  lcd.begin(16, 2);

  lcd.print("Disabled");
}

//Loop
void loop() {

  static float temperature, humidity;

  if (currentState != prevState) {
    switch (currentState) {
      case IDLE:
        idleState();
        break;
      case ERROR:
        errorState();
        break;
      case RUNNING:
        runningState();
        break;
      case DISABLED:
        disabledState();
        break;
    }
  }
}


///////////////////////
// States Functions
///////////////////////

//Device State: Idle
void idleState() {
    //update time and record temp and humid
    updateDateTime();
    displayTemperatureAndHumidity();
    checkWaterLevel();

    //change flags for the states and button and turn on cooresponding LED
    if (startButtonPressed) {
      currentState = RUNNING;
      startButtonPressed = false;
      PORTD &= ~(1 << GREEN_LED);
    }

    //change flags for the states and button and turn on cooresponding LED
    //Turn off fan
    if (stopButtonPressed) {
      stopButtonPressed = false;
      currentState = DISABLED;
      PORTD &= ~((1 << GREEN_LED) | (1 << BLUE_LED));
      PORTD &= ~(1 << FAN_MOTOR);
    }
  }

  //Device State: Error
  void errorState() {
    updateDateTime();
    lcd.clear();
    lcd.print("ERROR: Low Water");
    //Red for error
    PORTD |= (1 << RED_LED);

    //change flags for the states and button and turn on cooresponding LED
    if (startButtonPressed) {
      startButtonPressed = false;
      currentState = IDLE;
      PORTD &= ~(1 << RED_LED);
    }
  }

  //Device State: Running
  void runningState() {
    updateDateTime();
    displayTemperatureAndHumidity();
    checkWaterLevel();

    //change flags for the states and button and turn on cooresponding LED
    if (!isRunning) {
      PORTD |= (1 << FAN_MOTOR);
      isRunning = true;
      PORTD &= ~(1 << BLUE_LED);
    }

    //change flags for the states and button and turn on cooresponding LED
    if (stopButtonPressed) {
      stopButtonPressed = false;
      currentState = DISABLED;
      isRunning = false;
      PORTD &= ~((1 << FAN_MOTOR) | (1 << BLUE_LED));
    }


    //Read humidity and temp and adjust state
    if (dht.readHumidity() < HUMIDITY_THRESHOLD && dht.readTemperature() > TEMPERATURE_THRESHOLD) {
      currentState = ERROR;
      PORTD &= ~(1 << FAN_MOTOR);
      isRunning = false;
      PORTD &= ~(1 << BLUE_LED);
    }
  }

  //Device State: Disabled
  void disabledState() {
    updateDateTime();
    lcd.clear();
    lcd.print("System Disabled");
    //Yellow for disabled
    PORTD |= (1 << YELLOW_LED);

    //change flags for the states and button and turn on cooresponding LED
    if (startButtonPressed) {
      startButtonPressed = false;
      currentState = IDLE;
      PORTD &= ~(1 << YELLOW_LED);
    }
  }

///////////////////////
// Utiliity Functions
///////////////////////

  //Get current time
  void updateDateTime() {
    DateTime now = RTClib::now();
    lcd.setCursor(0, 0);
    lcd.print(now.getHour(), DEC);
    lcd.print(':');
    lcd.print(now.getMinute(), DEC);
    lcd.print(':');
    lcd.print(now.getSecond(), DEC);
  }

  //Get Temp and Humid and display them
  void displayTemperatureAndHumidity() {
    lcd.setCursor(0, 1);
    lcd.print("Temp: ");
    lcd.print(dht.readTemperature());
    lcd.print("C  Humidity: ");
    lcd.print(dht.readHumidity());
    lcd.print("%");
  }

  //water level checker
  void checkWaterLevel() {
    int waterLevel = analogRead(WATER_LEVEL_PIN);
    if (waterLevel < WATER_LEVEL_THRESHOLD) {
      currentState = ERROR;
      PORTD &= ~(1 << FAN_MOTOR);
      isRunning = false;
      PORTD &= ~(1 << BLUE_LED);
    }
  }


  //Interrupts
  void startButtonISR() {
    delay(50);
    if (!(PINB & (1 << START_BUTTON))) {
      startButtonPressed = true;
    }
  }

  void stopButtonISR() {
    delay(50);
    if (!(PINB & (1 << STOP_BUTTON))) {
      stopButtonPressed = true;
    }
  }
