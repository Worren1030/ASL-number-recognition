#include <NumberSpeaker.h>

// ---------------- Pins ----------------
const int thumbPin  = A0;
const int indexPin  = A1;
const int middlePin = A2;
const int pinkyPin  = A3;
const int forcePin  = A4;

NumberSpeaker numberSpeaker;

// ---------------- Thresholds ----------------
const int thumbThreshold  = 700;
const int indexThreshold  = 700;
const int middleThreshold = 300;
const int pinkyThreshold  = 700;
const int forceThreshold  = 200;

// ---------------- Stability ----------------
const int stableNeeded = 3;
int recentResults[3] = {0,0,0};
int resultIndex = 0;
bool bufferFilled = false;

// ---------------- Functions ----------------
bool isBent(int v, int t) {
  return v >= t;
}

bool isForceActive(int v, int t) {
  return v >= t;
}

void addResult(int val) {
  recentResults[resultIndex++] = val;
  if (resultIndex >= stableNeeded) {
    resultIndex = 0;
    bufferFilled = true;
  }
}

bool getStableResult(int &val) {
  if (!bufferFilled) return false;

  int first = recentResults[0];
  for (int i=1;i<stableNeeded;i++){
    if (recentResults[i]!=first) return false;
  }
  val = first;
  return true;
}

void speakNumber(int num){
  if(num>=1 && num<=9){
    numberSpeaker.speak_int(num);
  }
}

// ---------------- Setup ----------------
void setup(){
  Serial.begin(9600);
  numberSpeaker.begin();
}

// ---------------- Loop ----------------
void loop(){

  int thumbValue  = analogRead(thumbPin);
  int indexValue  = analogRead(indexPin);
  int middleValue = analogRead(middlePin);
  int pinkyValue  = analogRead(pinkyPin);
  int forceValue  = analogRead(forcePin);

  bool b1 = isBent(thumbValue, thumbThreshold);
  bool b2 = isBent(indexValue, indexThreshold);
  bool b3 = isBent(middleValue, middleThreshold);
  bool b5 = isBent(pinkyValue, pinkyThreshold);
  bool f4 = isForceActive(forceValue, forceThreshold);

  int gesture = 0;

  if(f4){
    if ( b1 && !b2 &&  b3 &&  b5) gesture = 1;
    else if ( b1 && !b2 && !b3 &&  b5) gesture = 2;
    else if (!b1 && !b2 && !b3 &&  b5) gesture = 3;
    else if ( b1 && !b2 && !b3 && !b5) gesture = 7;
  }
  else{
    if ( b1 && !b2 && !b3 && !b5) gesture = 4;
    else if (!b1 && !b2 && !b3 && !b5) gesture = 5;
    else if ( b1 && !b2 && !b3 &&  b5) gesture = 6;
    else if ( b1 && !b2 &&  b3 && !b5) gesture = 8;
    else if ( b1 &&  b2 && !b3 && !b5) gesture = 9;
  }

  Serial.print("Gesture: ");
  Serial.println(gesture);

  addResult(gesture);

  int stable;
  static int last = 0;

  if(getStableResult(stable)){
    if(stable!=last && stable!=0){
      speakNumber(stable);
      last = stable;
    }
    if(stable==0) last=0;
  }

  delay(100);
}
