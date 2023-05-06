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
}

void loop() {
  if (knocksReceived > 0 && timeSinceKnock() > KNOCK_CODE_GROUP_DELAY) { //wait for next knock code group, and progress
    if (knocksReceived == KNOCK_CODE[knockCodeProgress]) {
      knockCodeProgress++;
      knocksReceived = 0;
      if (knockCodeProgress >= KNOCK_CODE_LENGTH) {
        unlock();
      }
    } else {  // if the knock code group delay has elapsed without enough knocks, reset
      fail();
    }
  }
  if (checkForKnock() && timeSinceKnock() > KNOCK_RECEIVED_DELAY) {
    knockTime = millis();
    knocksReceived++;
  }
  if (knocksReceived > KNOCK_CODE[knockCodeProgress]) {  // if the number of knocks exceeds the number expected, instantly reset
    fail();
  }
  if ((knocksReceived > 0 || knockCodeProgress > 0) && timeSinceKnock() > INACTIVITY_TIMEOUT) { // if knocks have been recorded but the inactivity timeout has elapsed, reset
    fail();
  }

  delay(1);
}

long timeSinceKnock() {
  return millis()-knockTime;
}

boolean checkForKnock() {
  float knockVal = analogRead(BUZZER_PIN);
  return knockVal > KNOCK_THRESHOLD;
}

// unlock and play success tone simultaneously
void unlock() {
  reset();
  digitalWrite(LOCK_PIN, HIGH);
  pinMode(BUZZER_PIN, OUTPUT);
  tone(BUZZER_PIN, 500);
  delay(200);
  tone(BUZZER_PIN, 1000);
  delay(200);
  digitalWrite(LOCK_PIN, LOW);
  tone(BUZZER_PIN, 1250);
  delay(200);
  tone(BUZZER_PIN, 1500);
  delay(200);
  noTone(BUZZER_PIN);
  delay(200);
  pinMode(BUZZER_PIN, INPUT);
}

void fail() {
  if (knocksReceived > 1 || knockCodeProgress > 0) {  // don't play the failure tone for a single knock
    failTone();
  }
  reset();
}

void reset() {
  knocksReceived = 0;
  knockCodeProgress = 0;
}

void failTone() {
  pinMode(BUZZER_PIN, OUTPUT);
  tone(BUZZER_PIN, 1000);
  delay(200);
  tone(BUZZER_PIN, 500);
  delay(200);
  noTone(BUZZER_PIN);
  delay(200);
  pinMode(BUZZER_PIN, INPUT);
}
