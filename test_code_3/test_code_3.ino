// This program generates random values for Voltage and Current calculates 
// and calculates the current in a circuit

float Current = 4.7;    //Dummy initial value in Amperes
float Voltage = 12;   // Dummy initial value in Volts
float Resistance = 56;  // Dummy initial value in Ohms

void setup() {
  Serial.begin(9600);
  Serial.setTimeout(10000);
  randomSeed(analogRead(1));
}

void loop() {
  Voltage = random(10,300)/10;
  Resistance = random(2,400);
  Serial.print("V = ");
  Serial.println(Voltage);
  Serial.print("R = ");
  Serial.println(Resistance);
  Serial.print("Current (I) = ");
  Serial.println(Voltage/Resistance, 3);
  Serial.println();
  delay(2000); 
}

