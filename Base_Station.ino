// ===================================================
// ANALOG INPUTS
// ===================================================

int voltagePin = A1;

// Battery terminal check
int Terminal1 = A2;
int Terminal2 = A3;

// ===================================================
// PWM PINS
// ===================================================

int pwmPin_1 = 9;
int pwmPin_2 = 10;

// ===================================================
// H-BRIDGE
// ===================================================

int outPin = 8;
int outPin1 = 7;

// ===================================================
// INPUT PINS
// ===================================================

// Start trigger
bool inputState = HIGH;
char serialCommand;

// ===================================================
// STEPPER MOTOR PINS
// ===================================================

int IN1 = 3;
int IN2 = 4;
int IN3 = 5;
int IN4 = 6;

// ===================================================
// STEPPER SEQUENCE
// ===================================================

int stepSequence[4][4] = {
  {1, 0, 0, 1},
  {1, 0, 1, 0},
  {0, 1, 1, 0},
  {0, 1, 0, 1}
};

int stepDelay = 3;
int stepDelay_B = 3;

// Motor setting
int stepsPerRevolution = 200;
int rotations = 0;//109,108

// ===================================================
// EDGE DETECTION
// ===================================================

int lastState = HIGH;
unsigned long lastPrintTime = 0;

int state1 = 1;
int state2 = 1;
int Voltage_M = 1;

// ===================================================
// SETUP
// ===================================================

void setup() {

  Serial.begin(9600);

  // PWM
  pinMode(pwmPin_1, OUTPUT);
  pinMode(pwmPin_2, OUTPUT);

  // H-Bridge
  pinMode(outPin, OUTPUT);
  pinMode(outPin1, OUTPUT);

  // Trigger Input
  //pinMode(inputPin, INPUT);

  pinMode(Terminal1, INPUT_PULLUP);
  pinMode(Terminal2, INPUT_PULLUP);
  pinMode(voltagePin, INPUT_PULLUP);



  // Stepper Outputs
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  Serial.println("System Ready...");

}

// ===================================================
// MAIN LOOP
// ===================================================

void loop() {

  state1 = digitalRead(Terminal1);
  state2 = digitalRead(Terminal2);
  Voltage_M = digitalRead(voltagePin);
  //Serial.println(state1);
  //Serial.println(state2);

    // Read Serial Commands
  if (Serial.available())
  {
    serialCommand = Serial.read();

    if (serialCommand == 'L' || serialCommand == 'l')
    {
        inputState = LOW;     // Simulate trigger pressed
        Serial.println("START COMMAND RECEIVED");
    }

    if (serialCommand == 'T' || serialCommand == 't')
    {
        inputState = HIGH;    // Simulate trigger released
        Serial.println("STOP COMMAND RECEIVED");
    }
  }

  int currentState = inputState;

  // Detect pulse
  if (lastState == HIGH && currentState == LOW) {

    delay(20);

    if (inputState == LOW) {

      Serial.println("START SIGNAL RECEIVED");

      runStepperSequence();
    }
  }

  lastState = currentState;

  if (inputState == LOW) {

    if (millis() - lastPrintTime > 500) {
      Serial.println("LAND SIGNAL ACTIVE");
      lastPrintTime = millis();
    }
    monitorVoltageAndBridge();
  }
  else {

     turnOffBridge();


    //Serial.println("nth");
    delay(100);
  }
}

// ===================================================
// STEPPER CONTROL
// ===================================================

void runStepperSequence() {

  int totalSteps = rotations * stepsPerRevolution;

  // Forward
  for (int i = 0; i < totalSteps; i++) {

    stepMotor(i % 4);

    delay(stepDelay);
    //Serial.println(state1);

    if ( state2 == 0 || state1 == 0 )
    {
        break;
    }
  }

  delay(500);

  // Reverse
  for (int i = 0; i < totalSteps; i++) {

    stepMotor(3-(i % 4));

    delay(stepDelay_B);
  }

  releaseMotor();

  Serial.println("Home Position Reached.");
}

// ===================================================
// POWER + H-BRIDGE MONITORING
// ===================================================

void monitorVoltageAndBridge() {

  Serial.println(state2);
  Serial.println(state1);

  // ==========================================
  // H-BRIDGE DIRECTION
  // ==========================================

  delay(100);

  if((state2 == 0 || state1 == 0 ) && Voltage_M == 0 )
  {
    delay(100);
    turnOffBridge();
    if (millis() - lastPrintTime > 490) { // Sync with loop print
      Serial.println("CHARGE FULL");
    }
    return;
  }

  if (state1 == 0 && state2 == 1 && Voltage_M == 1 ) {

    digitalWrite(pwmPin_2, HIGH);
    digitalWrite(pwmPin_1, LOW);
    digitalWrite(outPin, HIGH);
    digitalWrite(outPin1, HIGH);
    Serial.println("CHARGE");

    Serial.println("state==high");

    delay(20);
  }

  else if (state2 == 0 && state1 == 1 &&  Voltage_M == 1) {

    digitalWrite(pwmPin_1, HIGH);
    digitalWrite(pwmPin_2, LOW);
    digitalWrite(outPin, HIGH);
    digitalWrite(outPin1, HIGH);
    Serial.println("CHARGE");

    Serial.println("state2==high");

    delay(20);
  }
  else
  {
    
    turnOffBridge();

    Serial.println("statex==high");

  }

  delay(20);
  
}



//======================================================
//TURN OFF BRIDGE
//=====================================================
void turnOffBridge() {

  digitalWrite(outPin, LOW);
  digitalWrite(outPin1, LOW);
  digitalWrite(pwmPin_1, LOW);
  digitalWrite(pwmPin_2, LOW);
   
  
}
// ===================================================
// STEP MOTOR
// ===================================================

void stepMotor(int step) {

  digitalWrite(IN1, stepSequence[step][0]);
  digitalWrite(IN2, stepSequence[step][1]);
  digitalWrite(IN3, stepSequence[step][2]);
  digitalWrite(IN4, stepSequence[step][3]);
}

// ===================================================
// RELEASE MOTOR
// ===================================================

void releaseMotor() {

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);

  Serial.println("Motor Released.");
}