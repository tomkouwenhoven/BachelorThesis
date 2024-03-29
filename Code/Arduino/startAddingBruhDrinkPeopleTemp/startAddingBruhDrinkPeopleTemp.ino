#include "Timer.h"
#include <dht.h>;

#define trigPin 2
#define echoPin 3

#define trigPin2 6
#define echoPin2 5

#define PIEZO A0 

#define ldrPin A2

#define DHT11_PIN 8

boolean profPresent = false;

const int wall = 100;
const int door = 100;

int counter = 0;
int lightCounter = 1;
int tempCounter = 1;
int humCounter = 1;

float averageLight;
float lightTotal = 0.0;
float averageTemp;
float tempTotal = 0.0;
float averageHum;
float humTotal = 0.0;

int hydrationCounter = 0;
int sensorReading;
const int threshold = 25;

unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 500;
unsigned long debounceDelayKnock = 500;

Timer t;                                              //instantiate the timer object
dht DHT;                                              //instantiate the sensor object
const unsigned long period1 = 10000;                  // 10 seconds

int totalNumberOfPeople = 0;
int realPeople = 0;
int totalNumberOfDrinks = 0;
int realDrinks = 0;

int readDistance(int pin1, int pin2){
  long duration, distance;
  digitalWrite(pin1, LOW);  
  delayMicroseconds(2); 
  digitalWrite(pin1, HIGH);
  
  delayMicroseconds(10); 
  digitalWrite(pin1, LOW);
  duration = pulseIn(pin2, HIGH);
  distance = (duration / 2) / 29.1;
  //Serial.println(distance);
  if (distance < wall){
      return true;
  }
  else if(distance >= wall){
    return false;
  }
}

int personCounter(){
  profPresent = readDistance(trigPin, echoPin);

  while(profPresent){
    //Serial.println("in while");
    boolean personIn = readDistance(trigPin2, echoPin2);
    if(personIn) {
      Serial.println("YES");
      long pastTimeProf = millis();
      if (pastTimeProf > (lastDebounceTime + debounceDelay)){
        Serial.println("NO");  
        counter ++ ;
      }
      lastDebounceTime = pastTimeProf;
    }
    break;
  }
}

int getPeople(){
  realPeople = counter; // 2;
  Serial.print("The number of people now is: ");
  Serial.println(realPeople);
  return realPeople;
}

int countHydration(){
  sensorReading = analogRead(PIEZO);
  //Serial.println(sensorReading);
  
  if (sensorReading >= threshold) {
    long pastTime = millis();
  
    if (pastTime > (lastDebounceTime + debounceDelayKnock)){
      hydrationCounter ++;
    }
    lastDebounceTime = pastTime;
  }
  return hydrationCounter;
}

int getDrinks(){
  realDrinks = (hydrationCounter);
  //hydrationCounter = 0;
  Serial.print("Number of drinks is: ");
  Serial.println(realDrinks);
  return realDrinks;
}

float getTemp(){
  int chk = DHT.read11(DHT11_PIN);
  float temp = DHT.temperature;
  
  Serial.print("The temperature now is: ");
  Serial.println(temp);
  return temp;
}
float getHum(){
  float hum = DHT.humidity;
  int chk = DHT.read11(DHT11_PIN);
  Serial.print("The humidity now is: ");
  Serial.println(hum);
  return hum;
}

float getLight(){
  float total = 0;
  for (int i=0; i<101; i++){
    int currentRead = analogRead(ldrPin); 
    total = total + currentRead;
  }
  float averageRead = (total/100);
  Serial.print("The average light of now is: ");
  Serial.println(averageRead);
  return averageRead;
}

char getValues(){
  int numberOfPeople = getPeople();
  totalNumberOfPeople += numberOfPeople;
  numberOfPeople = 0;
  realPeople = 0;
  counter = 0;
  Serial.print("The total people you've seen today is: ");
  Serial.println(totalNumberOfPeople);

  int numberOfDrinks = getDrinks();
  totalNumberOfDrinks += numberOfDrinks;
  numberOfDrinks = 0;
  realDrinks = 0;
  hydrationCounter = 0;
  Serial.print("The total drinks you've had today is: ");
  Serial.println(totalNumberOfDrinks);

  float light = getLight();
  lightTotal += light;
  averageLight = lightTotal / lightCounter;
  lightCounter += 1;
  Serial.print("The light you've had today is: ");
  Serial.println(averageLight);

  float temp = getTemp();
  tempTotal += temp;
  averageTemp = tempTotal / tempCounter;
  tempCounter += 1;
  Serial.print("The temperature you've had today is: ");
  Serial.println(averageTemp);
  
  float hum = getHum();
  humTotal += hum;
  averageHum = humTotal / humCounter;
  humCounter += 1;
  Serial.print("The humididty you've had today is: ");
  Serial.println(averageHum);

  Serial.println("-------------------------------------------");
}

void setup() {
  // put your setup code here, to run once: 
  Serial.begin (9600);
  
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  pinMode(PIEZO, INPUT);

  pinMode(ldrPin, INPUT);

  t.every(period1, getValues);
//  t.every(period1, getPeople);
//  t.every(period1, getDrinks);
//  t.every(period1, getTemp);
//  t.every(period1, getHum);
//  t.every(period1, getLight);
}

void loop() {
  // put your main code here, to run repeatedly:
  personCounter();
  countHydration();
  t.update();
  delay(50);
}
