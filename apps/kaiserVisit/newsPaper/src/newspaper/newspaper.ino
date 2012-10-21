
void setup() {
  
  for (int i=2;i<=12;i++) {
     pinMode(i,INPUT);
  }
   
  Serial.begin(9600);
  while (!Serial) {
    ;
  }
}

void loop() {
  
   for (int i=2;i<=12;i++) {
     Serial.print(digitalRead(i));
   }

   Serial.print('e');
   Serial.flush();
   delay(100);
   
}
  
