onst int interruptPin = 0; //GPIO 0 (Flash Button) 
const int LED=2; //On board blue LED 


ICACHE_RAM_ATTR void detectsMovement() {
  Serial.println("MOTION DETECTED!!!");
  digitalWrite(led, HIGH);
  startTimer = true;
  lastTrigger = millis();
}

void setup() { 
  pinMode(motionSensor, INPUT_PULLUP);
  Serial.begin(115200); 
  digitalWrite(led, LOW);
  attachInterrupt(digitalPinToInterrupt(motionSensor), detectsMovement, RISING);

} 

void loop() 
{ 
  now = millis();
  if(startTimer && (now - lastTrigger > (timeSeconds*1000))) {
  Serial.println("Motion stopped…");
  digitalWrite(led, LOW);
  startTimer = false;
}
} 
