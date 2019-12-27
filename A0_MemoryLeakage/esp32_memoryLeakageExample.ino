// example of memory Leakage
// for ESP32 using Arduino-IDE 1.8.9

#define N_MEM 3
int iCnt=0;

int * funcBugBug(){
  int * a;
  a=(int *)malloc(N_MEM*2);
  return (a);
}

void setup() {
  Serial.begin(115200);
  Serial.println(" ^_^ ^-^ ^.^ ^o^ ^O^ ^o^ ^.^ ^-^ ^_^ ");
  Serial.println("---starting program---");
  Serial.println(" ^O^ ^o^ ^.^ ^-^ ^_^ ^-^ ^.^ ^o^ ^O^ ");
}

void loop() {
  int * a;
  a = funcBugBug();
  // //a=(int *)malloc(N_MEM*2);

  for(int k=0;k<N_MEM;k++)a[k]=k;

  Serial.print("itteration = ");
  Serial.println(iCnt);
  iCnt++;
  
  if(iCnt>12100)delay(500);//will be crash at 12104
  if(iCnt<5)delay(500);// start slowly

  // //free(a);
}
