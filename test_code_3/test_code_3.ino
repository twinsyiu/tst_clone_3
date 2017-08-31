// This program sourced from
// https://www.arduino.cc/en/Reference/AttachInterrupt
// to test the interrupt pin 
// NOTES: The gain of the audio sensor is tuned CCW to very low until the RED LED on the module is just OFF

const byte ledPin = 5;
const byte interruptPin = 2;
int blinkCnt = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("setup() - begin");
  pinMode(ledPin, OUTPUT);
//  attachInterrupt(digitalPinToInterrupt(interruptPin), blink, CHANGE);
  attachInterrupt(digitalPinToInterrupt(interruptPin), blink, RISING);
  digitalWrite(ledPin, LOW);
  Serial.println("LED_OFF");
  Serial.println("setup() - end");
}

void loop() {
//  state = !state;
  if (blinkCnt)
  {
     blinkCnt = 0;
     digitalWrite(ledPin, HIGH);
     Serial.println("LED_ON");
     delay(1000);
  }
  digitalWrite(ledPin, LOW);

/*
  if (digitalRead(interruptPin) == HIGH)
  {
    //digitalWrite(ledPin, HIGH);
    analogWrite(ledPin, 50);
  }
  else
  {
    //digitalWrite(ledPin, LOW);
    analogWrite(ledPin, 0);
  }
  delay(50);
*/  
}

void blink() {
  blinkCnt++;
  Serial.println(millis());
  //state = !state;
}
