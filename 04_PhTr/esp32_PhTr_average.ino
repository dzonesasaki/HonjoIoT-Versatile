#define PIN_READ 33
#define FACT_IIR_ALPHA ((float)0.9)
#define FACT_IIR_BETA ((float)1-FACT_IIR_ALPHA)

float gfValPrev;

void setup() {
  Serial.begin(115200);
  gfValPrev=0;
}

void loop() {
  float fValCurr = analogRead(PIN_READ);
  gfValPrev = fValCurr * FACT_IIR_ALPHA + gfValPrev * FACT_IIR_BETA ;
  Serial.println(gfValPrev);
  delay(10);
}
