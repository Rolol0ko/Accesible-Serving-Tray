#include <Arduino.h>
#include <AccelStepper.h>

// PIN DEFINITIONS
const int stepPin = 9;
const int dirPin = 8;
const int enablePin = 5;

const int bigButtonPin = 2;
const int closeLimitPin = 3;
const int farLimitPin = 4;

// ACCELSTEPPER CONFIGURATION
AccelStepper stepper(AccelStepper::DRIVER, stepPin, dirPin);

const float motorSpeed = 1000.0;
const float motorAcceleration = 10.0;

const long travelTarget = 1000000L;

// TRAY POSITION
enum TrayPosition {
  POSITION_UNKNOWN,
  POSITION_CLOSE,
  POSITION_FAR
};
TrayPosition currentPosition = POSITION_UNKNOWN;
bool isMoving = false;
bool movingToFar = true;

// DEBOUNCING
const unsigned long buttonDebounceTime = 50;
const unsigned long limitDebounceTime = 10;

struct DebouncedInput {
  int pin;
  bool stableState;
  bool lastReading;
  unsigned long lastChange;
};
DebouncedInput buttonInput;
DebouncedInput closeInput;
DebouncedInput farInput;

bool lastButtonPressed = false;

// INPUT INITIALIZATION
void initializeInput(DebouncedInput &input, int pin) {
  input.pin = pin;
  bool initialReading = digitalRead(pin);
  input.stableState = initialReading;
  input.lastReading = initialReading;
  input.lastChange = millis();
}
// DEBOUNCED INPUT READING
bool readDebounced(
  DebouncedInput &input,
  unsigned long debounceTime) {

  bool reading = digitalRead(input.pin);

  if (reading != input.lastReading) {
    input.lastReading = reading;
    input.lastChange = millis();
  }
  if (millis() - input.lastChange >= debounceTime) {
    input.stableState = input.lastReading;
  }
  return input.stableState == LOW;
}
// MOTOR POWER CONTROL
void enableMotor() {
  digitalWrite(enablePin, LOW);
  delayMicroseconds(10);
  Serial.println("Motor energized.");
}
void disableMotor() {
  digitalWrite(enablePin, HIGH);
  Serial.println("Motor de-energized.");
}
// MANUAL HOMING / ZEROING
void checkHoming(bool closePressed, bool farPressed) {
  if (currentPosition != POSITION_UNKNOWN) {
    return;
  }
  if (closePressed && !farPressed) {
    currentPosition = POSITION_CLOSE;
    stepper.setCurrentPosition(0);
    Serial.println("Tray zeroed at CLOSE.");
  } else if (farPressed && !closePressed) {
    currentPosition = POSITION_FAR;
    stepper.setCurrentPosition(0);
    Serial.println("Tray zeroed at FAR.");
  } else if (closePressed && farPressed) {
    Serial.println("WARNING: Both limit switches activated.");
  }
}
// START MOVEMENT
void startMovement(bool closePressed, bool farPressed) {
  if (isMoving || currentPosition == POSITION_UNKNOWN) {
    return;
  }
  if (currentPosition == POSITION_CLOSE) {
    if (farPressed) {
      Serial.println("ERROR: Far switch already activated.");
      return;
    }
    movingToFar = true;
  } else if (currentPosition == POSITION_FAR) {
    if (closePressed) {
      Serial.println("ERROR: Close switch already activated.");
      return;
    }
    movingToFar = false;
  }
  enableMotor();
  stepper.setCurrentPosition(0);

  if (movingToFar) {
    stepper.moveTo(travelTarget);
    Serial.println("Moving towards FAR...");
  } else {
    stepper.moveTo(-travelTarget);
    Serial.println("Moving towards CLOSE...");
  }
  isMoving = true;
}

// STOP AT LIMIT
void stopAtLimit(TrayPosition newPosition) {

  stepper.setCurrentPosition(0);

  isMoving = false;
  currentPosition = newPosition;

  if (newPosition == POSITION_FAR) {
    Serial.println("FAR limit reached!");
  } else {
    Serial.println("CLOSE limit reached!");
  }
  disableMotor();
}
// UNEXPECTED LIMIT SWITCH
void stopOnFault() {
  stepper.setCurrentPosition(0);

  isMoving = false;
  currentPosition = POSITION_UNKNOWN;

  disableMotor();

  Serial.println("ERROR: Position unknown. Manual re-zero required.");
}
// BUTTON HANDLING
void checkButton(bool closePressed, bool farPressed) {
  bool buttonPressed = readDebounced(
    buttonInput,
    buttonDebounceTime);

  if (buttonPressed && !lastButtonPressed) {
    if (isMoving) {
      Serial.println("Motor moving. Button ignored.");
    } else if (currentPosition == POSITION_UNKNOWN) {
      Serial.println("Please manually slide tray to a limit switch first.");
    } else {
      startMovement(closePressed, farPressed);
    }
  }
  lastButtonPressed = buttonPressed;
}

// LIMIT SWITCH HANDLING
void checkLimitSwitches(bool closePressed, bool farPressed) {
  if (!isMoving) {
    return;
  }
  if (movingToFar) {
    if (farPressed) {
      stopAtLimit(POSITION_FAR);
    } else if (closePressed) {
      stopOnFault();
    }
  } else {
    if (closePressed) {
      stopAtLimit(POSITION_CLOSE);
    } else if (farPressed) {
      stopOnFault();
    }
  }
}

// SETUP
void setup() {

  Serial.begin(9600);

  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(enablePin, OUTPUT);

  pinMode(bigButtonPin, INPUT_PULLUP);
  pinMode(closeLimitPin, INPUT_PULLUP);
  pinMode(farLimitPin, INPUT_PULLUP);

  // Motor starts de-energized
  digitalWrite(enablePin, HIGH);

  stepper.setMaxSpeed(motorSpeed);
  stepper.setAcceleration(motorAcceleration);
  stepper.setMinPulseWidth(5);

  initializeInput(buttonInput, bigButtonPin);
  initializeInput(closeInput, closeLimitPin);
  initializeInput(farInput, farLimitPin);

  lastButtonPressed = buttonInput.stableState == LOW;

  currentPosition = POSITION_UNKNOWN;

  Serial.println("TRAY CONTROLLER READY");
  Serial.println("Motor is de-energized.");
  Serial.println("Manually slide tray to CLOSE or FAR.");
}

// MAIN LOOP
void loop() {
  bool closePressed = readDebounced(closeInput, limitDebounceTime);
  bool farPressed = readDebounced(farInput, limitDebounceTime);

  checkHoming(closePressed, farPressed);
  checkButton(closePressed, farPressed);
  checkLimitSwitches(closePressed, farPressed);

  // 4. Run motor
  if (isMoving) {
    // stepper.run();
    stepper.setSpeed(900);
    stepper.runSpeed();

    if (stepper.distanceToGo() == 0) {
      isMoving = false;
      currentPosition = POSITION_UNKNOWN;

      disableMotor();

      Serial.println("WARNING: Target reached without limit switch.");
      Serial.println("Please manually re-zero the tray.");
    }
  }
}
