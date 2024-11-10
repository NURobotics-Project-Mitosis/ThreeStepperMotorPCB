// Shift register control pins
const int latchPin = 8;  // Shift register latch pin
const int clockPin = 12; // Shift register clock pin
const int dataPin = 11;  // Shift register data pin

class StepperMotor 
{
  private:
    double angle;
    const double anglePerStep;
    const int stepsPerRevolution;
    bool direction;  // Motor direction (clockwise or counterclockwise)

    int dirPin;
    int stepPin;
    byte motors;
  
  public: 
    // Constructor with steps per revolution
    StepperMotor(int stepsPerRevolution = 200) : 
      stepsPerRevolution(stepsPerRevolution), 
      anglePerStep(360.0 / stepsPerRevolution), 
      angle(0), 
      direction(true) 
    {  // Clockwise by default
      pinMode(stepPin, OUTPUT);
      pinMode(dirPin, OUTPUT);
      this->angle = 0;
    }

    // negative 
    void spinClockwise() 
    {
      digitalWrite(dirPin, HIGH);
    }
    
    // positive
    void spinCounterclockwise() 
    {
      digitalWrite(dirPin, LOW);
    }

    // Set motor to a specific angle
    void setAngle(double newAngle) 
    {
      if (newAngle < 0 || newAngle >= 360) 
      {
        Serial.println("Degree value out of bounds");
        return;
      }

      // Set direction based on angle difference
      if (newAngle > this->angle) 
      {
        this->spinClockwise();
      } 
      else 
      {
        this->spinCounterclockwise();
      }
  
      // Move motor to the new angle
      int steps = angleToSteps(newAngle - this->angle);
      for (int i = 0; i < steps; i++) 
      {
        pulseStep();
      }
  
      this->angle = newAngle;
  }

  // set motor enables
  void setMotors() 
  {
    //right now this just sets bit 0 but change it to do more.
    motors = 0;
    updateShiftRegister();
    bitSet(motors, 0);
    updateShiftRegister();
  }
  
  private:
    // Convert angle to number of steps
    int angleToSteps(double angleDiff) {
      return static_cast<int>(angleDiff / anglePerStep);
    }
  
    // Pulse step pin via shift register
    void pulseStep() {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(3000);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(3000);
    }

    /*
    * updateShiftRegister() - This function sets the latchPin to low, then calls the Arduino function 'shiftOut' to shift out contents of variable 'leds' in the shift register before putting the 'latchPin' high again.
    */
    void updateShiftRegister()
    {
      digitalWrite(latchPin, LOW);
      shiftOut(dataPin, clockPin, MSBFIRST, leds);
      digitalWrite(latchPin, HIGH);
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

  motorX->setMotors();
  
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
