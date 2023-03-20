float maxVal = 0;
long knockTime = 0;
long knockThreshold = 1800;
int knocksNeeded = 3;
int knockCode[] = {2, 3, 2};
int knockCodeLength = 3;  //length of knock code array
long inactivityTimeout = 3000; //number of ms to wait before resetting state
long knockReceivedDelay = 100; //number of ms to wait after getting a knock so that we don't accidentally count it multiple times
long knockCodeGroupDelay = 1000; //number of ms to wait before starting a new knock code group;

// state constants
/*int stateIdle = 0;
int stateKnockReceived = 1;
int stateUnlocked = 2;

int state = stateIdle;*/
int knocksReceived = 0;
int knockCodeProgress = 0;

void setup() {
  analogReadResolution(12); //Set analog input resolution to max, 12-bits
 
  Serial.begin(9600);

  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  if (knocksReceived > 0 && timeSinceKnock() > knockCodeGroupDelay) { //wait for next knock code group, and progress
    if (knocksReceived == knockCode[knockCodeProgress]) {
      knockCodeProgress++;
      knocksReceived = 0;
      //Serial.print(knockCodeProgress);
      //Serial.println(" knock code progress");
      if (knockCodeProgress >= knockCodeLength) {
        unlock();
      }
    } else {  // if the knock code group delay has elapsed without enough knocks, reset
      reset();
    }
  }
  if (checkForKnock() && timeSinceKnock() > knockReceivedDelay) {
    knockTime = millis();
    knocksReceived++;
    flash(1);
    //Serial.print(knocksReceived);
    //Serial.println(" knocks received");
  }
  if (knocksReceived > knockCode[knockCodeProgress]) {  // if the number of knocks exceeds the number expected, instantly reset
    reset();
  }
  if ((knocksReceived > 0 || knockCodeProgress > 0) && timeSinceKnock() > inactivityTimeout) {
    reset();
  }

  delay(1);
}

long timeSinceKnock() {
  return millis()-knockTime;
}

boolean checkForKnock() {
  float knockVal = analogRead(A2);
  return knockVal > knockThreshold;
}

void unlock() {
  Serial.println("Unlock!");
  digitalWrite(LED_BUILTIN, LOW);
}

void reset() {
  flash(2);
  knocksReceived = 0;
  knockCodeProgress = 0;
  digitalWrite(LED_BUILTIN, HIGH);
}

void flash(int count) {
  for (int i = 0; i < count; i++) {
    if (i > 0) {
      delay(40);
    }
    digitalWrite(LED_BUILTIN, LOW);
    delay(20);
    digitalWrite(LED_BUILTIN, HIGH);
  }
}
