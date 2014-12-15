const int dataPin = 2;
const int dataClock = 3;
const int latchClock = 4;

void setup() {
  Serial.begin(9600);
  
  pinMode(dataClock, OUTPUT);
  pinMode(latchClock, OUTPUT);
  pinMode(dataPin, INPUT);
  
  digitalWrite(dataPin, LOW);
  digitalWrite(dataClock, HIGH);
  digitalWrite(latchClock, LOW);
}

const char *buttonName[16] = {
  "B",
  "Y",
  "Select",
  "Start",
  "Up",
  "Down",
  "Left",
  "Right",
  "A",
  "X",
  "L",
  "R"
};

void loop() {
  digitalWrite(latchClock, HIGH);
  delayMicroseconds(12);
  digitalWrite(latchClock, LOW);
  
  bool button[16] = { 0 };
  for (int i = 0; i < 16; ++i)
  {
    delayMicroseconds(6);
    digitalWrite(dataClock, LOW);
    button[i] = (digitalRead(dataPin) == LOW);
    delayMicroseconds(6);
    digitalWrite(dataClock, HIGH);
  }
  
  for (int i = 0; i < 16; ++i)
  {
    if (button[i])
      Serial.print(buttonName[i]);
  }
  Serial.println("");
  
  delay(16);
}

