#define muxSIG 9
#define muxS0 3
#define muxS1 4
#define muxS2 5
#define muxS3 6


void setup() {
  // put your setup code here, to run once:
  pinMode(muxSIG, OUTPUT);
  pinMode(muxS0, OUTPUT);
  pinMode(muxS1, OUTPUT);
  pinMode(muxS2, OUTPUT);
  pinMode(muxS3, OUTPUT);
  Serial.begin(9600);
  delay(1000);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(muxS0, 1);
  digitalWrite(muxS1, 1);
  digitalWrite(muxS2, 0);
  digitalWrite(muxS3, 0);

  digitalWrite(muxSIG, HIGH);
  delay(1000);
  digitalWrite(muxSIG, LOW);
  delay(1000);
  
}
