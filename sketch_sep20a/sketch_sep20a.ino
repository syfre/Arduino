// Test ujet32plus 
// 

void setup() {
  // initialize digital pin 13 as an output.
  // 19 : RB3
  pinMode(19, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(19, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1);              // wait for a second
  digitalWrite(19, LOW);    // turn the LED off by making the voltage LOW
  delay(1);              // wait for a second
}
