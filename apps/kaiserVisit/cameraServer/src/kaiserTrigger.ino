int ledPin = 13;   // select the pin for the LED
int val = 0;
// variable to store the data from the serial port

const int buttonPin = 2;     // the number of the pushbutton pin
int buttonState = LOW;

void setup() {
  pinMode(ledPin,OUTPUT);   // declare the LED's pin as output
  pinMode(buttonPin, INPUT);
  Serial.begin(9600);        // connect to the serial port
  
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
 
}

void loop () {
 
  if (Serial.available() > 0) {
    val = Serial.read();
    if (val != -1) {
      if (val == 'l') {
        digitalWrite(ledPin, HIGH);
        delay(200);
        digitalWrite(ledPin, LOW);
      }
      
      if (val == 'r') {
        buttonState = LOW; // reset for next trigger
      }
    }
  }
  
  if (digitalRead(buttonPin) == HIGH && buttonState == LOW) {
    buttonState = HIGH;
    Serial.write(buttonState);
  }

}