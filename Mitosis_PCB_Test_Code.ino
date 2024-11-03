// Shift register control pins
const int latchPin = 8;  // Shift register latch pin
const int clockPin = 12; // Shift register clock pin
const int dataPin = 11;  // Shift register data pin

class StepperMotor {
  private:
  double angle;
  const double anglePerStep;
  const int stepsPerRevolution;
  bool direction;  // Motor direction (clockwise or counterclockwise)
  int motorIndex;  
  // Global shift register control data
  byte shiftRegisterData = 0;
  
  public: 
  // Constructor with steps per revolution
  StepperMotor(int stepsPerRevolution = 200) 
    : stepsPerRevolution(stepsPerRevolution), anglePerStep(360.0 / stepsPerRevolution), 
    angle(0), direction(true) {  // Clockwise by default
  }
  
  // Spin motor in a clockwise direction
  void spinClockwise() {
    this->direction = true;
    updateShiftRegister();
  }
  
  // Spin motor in a counterclockwise direction
  void spinCounterclockwise() {
    this->direction = false;
    updateShiftRegister();
  }

  // Set motor to a specific angle
  void setAngle(double newAngle) {
    if (newAngle < 0 || newAngle >= 360) {
    Serial.println("Degree value out of bounds");
    return;
    }

    // Set direction based on angle difference
    if (newAngle > this->angle) {
    this->spinClockwise();
    } else {
    this->spinCounterclockwise();
    }
  
    // Move motor to the new angle
    int steps = angleToSteps(newAngle - this->angle);
    for (int i = 0; i < steps; i++) {
    pulseStep();
    delayMicroseconds(3000);  // Adjust for motor speed
    }
  
    this->angle = newAngle;
  }
  
  private:
  // Convert angle to number of steps
  int angleToSteps(double angleDiff) {
    return static_cast<int>(angleDiff / anglePerStep);
  }
  
  // Pulse step pin via shift register
  void pulseStep() {
    // Toggle step pin for this motor
    shiftRegisterStepPulse();
  }

  // Shift out the control signals to the shift register
  void updateShiftRegister() {
    bitWrite(shiftRegisterData, motorIndex * 2 + 1, direction);
    digitalWrite(latchPin, LOW);  // Prepare to shift data
    shiftOut(dataPin, clockPin, MSBFIRST, shiftRegisterData);  // Shift out the data
    digitalWrite(latchPin, HIGH);  // Latch the data
  }
  
  // Toggle the step signal for all motors in the shift register
  void shiftRegisterStepPulse() {
    // Set step bits (0, 2, 4) high temporarily for all motors
    shiftRegisterData |= 0b00010101;  // Set step pins high
    updateShiftRegister();
    
    delayMicroseconds(10);  // Short pulse
    
    // Clear step bits (0, 2, 4) low
    shiftRegisterData &= ~0b00010101;  // Set step pins low
    updateShiftRegister();
  }
};

// Static variable initialization
int StepperMotor::motorCount = 0;

// Create three motor instances for X, Y, and Z axes
StepperMotor motorX(200);  // Motor X
StepperMotor motorY(200);  // Motor Y
StepperMotor motorZ(200);  // Motor Z

void setup() {
  // Initialize shift register pins
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  
  Serial.begin(9600);
}

void loop() {
  float degree;
  Serial.println("Enter a degree for X axis: ");
  
  if (Serial.available() > 0) {
  degree = Serial.parseFloat();
  Serial.println("Moving X axis to angle...");
  motorX.setAngle(degree);  // Move X axis motor to the entered degree
  }
}
