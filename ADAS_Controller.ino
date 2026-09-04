#include <LiquidCrystal_I2C.h>
#include <Servo.h>

int TRIG_FRONT=2;
int ECHO_FRONT=3;

int TRIG_LEFT=4;
int ECHO_LEFT=13;

int TRIG_RIGHT=A2;
int ECHO_RIGHT=A3;

int servoPin1=5;
int servoPin2=6;

int buzzer=7;

int green=8;
int yellow=9;
int red=10;

int driverTakeover=11;
int laneDeviation=12;
int systemLimitation=A1;

int speedPotentiometer=A0;

LiquidCrystal_I2C lcd(0x27,16,2);

Servo steering;
Servo brake;

int direction=0;

float getDistance(int trigPin,int echoPin)
{
  long duration;
  float distance;

  digitalWrite(trigPin,LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin,HIGH);
  delayMicroseconds(10);

  digitalWrite(trigPin,LOW);

  duration=pulseIn(echoPin,HIGH,30000);

  if(duration==0)
  {
    distance=500;
  }
  else
  {
    distance=duration*0.0343/2;
  }
  return distance;
}

void setup()
{
  pinMode(TRIG_FRONT,OUTPUT);
  pinMode(ECHO_FRONT,INPUT);
  
  pinMode(TRIG_LEFT,OUTPUT);
  pinMode(ECHO_LEFT,INPUT);

  pinMode(TRIG_RIGHT,OUTPUT);
  pinMode(ECHO_RIGHT,INPUT);

  pinMode(buzzer,OUTPUT);

  pinMode(green,OUTPUT);
  pinMode(yellow,OUTPUT);
  pinMode(red,OUTPUT);

  pinMode(driverTakeover,INPUT);
  pinMode(laneDeviation,INPUT);
  pinMode(systemLimitation,INPUT);

  pinMode(speedPotentiometer,INPUT);

  lcd.init();
  lcd.backlight();

  steering.attach(servoPin1);
  brake.attach(servoPin2);

  steering.write(90);
  brake.write(0);
}
void loop()
{
  int driverTakeoverValue=digitalRead(driverTakeover);
  int laneDeviationValue=digitalRead(laneDeviation);
  int systemLimitationValue=digitalRead(systemLimitation);

  int speedValue=analogRead(speedPotentiometer);
  int speed=map(speedValue,0,1023,0,120);
  
  int distanceFrontValue=getDistance(TRIG_FRONT,ECHO_FRONT);
  int distanceLeftValue=getDistance(TRIG_LEFT,ECHO_LEFT);
  int distanceRightValue=getDistance(TRIG_RIGHT,ECHO_RIGHT);

  if(driverTakeoverValue==HIGH)
  {
    digitalWrite(red,HIGH);
    digitalWrite(yellow,LOW);
    digitalWrite(green,LOW);

    tone(buzzer,1000);

    lcd.clear();
    lcd.print("DRIVER CONTROL");
    lcd.setCursor(0,1);
    lcd.print("MANUAL MODE");

    steering.write(90);
    brake.write(0);
  }
    else if ((distanceFrontValue <= 30 && speed > 100) ||
           (laneDeviationValue == HIGH && speed > 100) ||
           systemLimitationValue == HIGH)
  {
    digitalWrite(green, LOW);
    digitalWrite(yellow, LOW);
    digitalWrite(red, HIGH);

    tone(buzzer, 1000);

    brake.write(90);
    
    if(distanceLeftValue > distanceRightValue)
{
    steering.write(60);
    direction = -1;
}
else if(distanceRightValue > distanceLeftValue)
{
    steering.write(120);
    direction = 1;
}
else
{
    steering.write(90);
    direction = 0;
}

    lcd.clear();
    lcd.print("CRITICAL RISK");
    lcd.setCursor(0, 1);
    lcd.print("AUTO INTERVENTION");
  }
   else if ((distanceFrontValue <= 50 && speed > 80) ||
         (distanceLeftValue <= 50 && speed > 80) ||
         (distanceRightValue <= 50 && speed > 80) ||
         (laneDeviationValue == HIGH && speed > 80))
  {
    digitalWrite(green, LOW);
    digitalWrite(yellow, HIGH);
    digitalWrite(red, HIGH);

    tone(buzzer, 800);

    brake.write(45);
    steering.write(90);

    lcd.clear();
    lcd.print("HIGH RISK");
    lcd.setCursor(0, 1);
    lcd.print("PREPARE TAKEOVER");
  }
  else if ((distanceFrontValue <= 80 && speed > 50) ||
         (distanceLeftValue <= 80 && speed > 50) ||
         (distanceRightValue <= 80 && speed > 50) ||
         laneDeviationValue == HIGH)
  {
    digitalWrite(green, LOW);
    digitalWrite(yellow, HIGH);
    digitalWrite(red, LOW);

    tone(buzzer, 500);

    brake.write(20);
    steering.write(90);

    lcd.clear();
    lcd.print("WARNING");
    lcd.setCursor(0, 1);
    lcd.print("DRIVE CAREFULLY");
  }
   else
  {
    digitalWrite(green, HIGH);
    digitalWrite(yellow, LOW);
    digitalWrite(red, LOW);

    noTone(buzzer);

    steering.write(90);
    brake.write(0);

    lcd.clear();
    lcd.print("ADAS ACTIVE");
    lcd.setCursor(0, 1);
    lcd.print("SYSTEM NORMAL");
  }

  delay(100);
}
