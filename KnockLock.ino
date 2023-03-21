const int BUZZER_PIN = A2;
const int LOCK_PIN = 5;

const long KNOCK_THRESHOLD = 100; //Based on having 1M resistor connected across buzzer
const int KNOCK_CODE[] = {2, 3, 2};
const int KNOCK_CODE_LENGTH = 3;  //length of knock code array
const long INACTIVITY_TIMEOUT = 3000; //number of ms to wait before resetting state
const long KNOCK_RECEIVED_DELAY = 100; //number of ms to wait after getting a knock so that we don't accidentally count it multiple times
const long KNOCK_CODE_GROUP_DELAY = 500; //number of ms to wait before starting a new knock code group;

long knockTime = 0;
int knocksReceived = 0;
int knockCodeProgress = 0;

void setup() {
  analogReadResolution(12); //Set analog input resolution to max, 12-bits

  pinMode(LOCK_PIN, OUTPUT);
  //Serial.begin(9600);
}

void loop() {
  if (knocksReceived > 0 && timeSinceKnock() > KNOCK_CODE_GROUP_DELAY) { //wait for next knock code group, and progress
    if (knocksReceived == KNOCK_CODE[knockCodeProgress]) {
      knockCodeProgress++;
      knocksReceived = 0;
      //Serial.print(knockCodeProgress);
      //Serial.println(" knock code progress");
      if (knockCodeProgress >= KNOCK_CODE_LENGTH) {
        unlock();
        successTone();
      }
    } else {  // if the knock code group delay has elapsed without enough knocks, reset
      reset();
      failTone();
    }
  }
  if (checkForKnock() && timeSinceKnock() > KNOCK_RECEIVED_DELAY) {
    knockTime = millis();
    knocksReceived++;
    //Serial.print(knocksReceived);
    //Serial.println(" knocks received");
  }
  if (knocksReceived > KNOCK_CODE[knockCodeProgress]) {  // if the number of knocks exceeds the number expected, instantly reset
    reset();
    failTone();
  }
  if ((knocksReceived > 0 || knockCodeProgress > 0) && timeSinceKnock() > INACTIVITY_TIMEOUT) {
    reset();
    failTone();
  }

  delay(1);
}

long timeSinceKnock() {
  return millis()-knockTime;
}

boolean checkForKnock() {
  float knockVal = analogRead(A2);
  return knockVal > KNOCK_THRESHOLD;
}

void unlock() {
  //Serial.println("Unlock!");
  reset();
  digitalWrite(LOCK_PIN, HIGH);
  delay(500);
  digitalWrite(LOCK_PIN, LOW);
  delay(1000);  //delay for a second so that the noise of the deadbolts unlocking isn't interpreted as a knock
}

void reset() {
  knocksReceived = 0;
  knockCodeProgress = 0;
}

void successTone() {
  pinMode(A2, OUTPUT);
  tone(A2, 500);
  delay(200);
  tone(A2, 1000);
  delay(200);
  tone(A2, 1250);
  delay(200);
  tone(A2, 1500);
  delay(200);
  noTone(A2);
  delay(200);
  pinMode(A2, INPUT);
}

void failTone() {
  pinMode(A2, OUTPUT);
  tone(A2, 1000);
  delay(200);
  tone(A2, 500);
  delay(200);
  noTone(A2);
  delay(200);
  pinMode(A2, INPUT);
}
