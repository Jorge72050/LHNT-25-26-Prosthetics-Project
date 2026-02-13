#define EncoderPinA 18
#define EncoderPinB 21
#define ForwardPin 27
#define BackwardPin 26
#define EnablePin 25
volatile long Encodervalue=0;
void setup() {
pinMode(ForwardPin,OUTPUT);
pinMode(BackwardPin,OUTPUT);
pinMode(EnablePin,OUTPUT);
pinMode(EncoderPinA, INPUT);
pinMode(EncoderPinB, INPUT);
attachInterrupt(digitalPinToInterrupt(EncoderPinA), updateEncoder, RISING);
Serial.begin(9600);
}
void loop() {
digitalWrite(ForwardPin,HIGH);
digitalWrite(BackwardPin,LOW);
analogWrite(EnablePin,255);
Serial.println(Encodervalue);
delay(100);
}
void updateEncoder()
{
if (digitalRead(EncoderPinA)> digitalRead(EncoderPinB))
Encodervalue++;
else
Encodervalue--;
}