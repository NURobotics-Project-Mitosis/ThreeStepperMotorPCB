class StepperMotor {
  private:
    double angle;
    const double anglePerStep;
    const int motorIndex;  // Index for this motor (0 for X, 1 for Y, 2 for Z)
    const int stepsPerRevolution;
    bool direction;  // Motor direction (clockwise or counterclockwise)

  public: 
    // Constructor with motor index and steps per revolution
    StepperMotor(int motorIndex, int stepsPerRevolution = 200) 
      : motorIndex(motorIndex), stepsPerRevolution(stepsPerRevolution), anglePerStep(360.0 / stepsPerRevolution) 
    {
      this->angle = 0;
      this->direction = true;  // Clockwise by default
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
      if (newAngle > 360 || newAngle < 0) {
        Serial.println("Degree value out of bounds");
        return;
      }

      // Set direction based on angle difference
      if (newAngle > this->angle) {
        this->spinCounterclockwise();
      } else {
        this->spinClockwise();
      }

      // Move motor to the new angle
      int steps = angleToSteps(newAngle - this->angle);
      for (int i = 0; i < abs(steps); i++) {
        pulseStep();
        delayMicroseconds(3000);  // Adjust for motor speed
      }

      this->angle = newAngle;
    }

  private:
    // Convert angle to number of steps
    int angleToSteps(double angle) {
      return angle / anglePerStep;
    }

    // Pulse step pin via shift register
    void pulseStep() {
      // Shift out the direction and step signals for this motor
      updateShiftRegister();
      
      // Toggle step pin for this motor
      shiftRegisterStepPulse();
    }

    // Update the shift register with the current direction for this motor
    void updateShiftRegister() {
      // Set the direction bit for this motor in the shift register byte
      bitWrite(shiftRegisterData, motorIndex * 2 + 1, direction);
    }
};

// Global shift register control data
byte shiftRegisterData = 0;

// Shift register control pins
const int latchPin = 8;  // Shift register latch pin
const int clockPin = 12; // Shift register clock pin
const int dataPin = 11;  // Shift register data pin

// Create three motor instances for X, Y, and Z axes
StepperMotor motorX(0);  // Motor X (shift register bits 0 and 1)
StepperMotor motorY(1);  // Motor Y (shift register bits 2 and 3)
StepperMotor motorZ(2);  // Motor Z (shift register bits 4 and 5)

void setup() {
  // Initialize shift register pins
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);

  Serial.begin(9600);
}

// Function to shift out the control signals to the shift register
void updateShiftRegister() {
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

void loop() {
  float degree;
  Serial.println("Enter a degree for X axis: ");
  
  if (Serial.available() > 0) {
    degree = Serial.parseFloat();
    Serial.println("Moving X axis to angle...");
    motorX.setAngle(degree);  // Move X axis motor to the entered degree
  }

  // Similarly, you can control motorY and motorZ by creating additional prompts
  // and calls to motorY.setAngle() and motorZ.setAngle() here.
}
