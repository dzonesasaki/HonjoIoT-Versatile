#include <WiFi.h>
#include <PubSubClient.h> // https://github.com/knolleary/pubsubclient

const char gcc_SSID[] = "SSID";
const char gcc_PASSWORD[] = "PASSWORD";
WiFiClient myWifiClient;
const int NUM_PORT_MQTT = 1883;
PubSubClient mqttClient(myWifiClient);
const char * gcc_MQTT_BROKER_IP = "192.168.1.1";
const char * gcc_MQTT_TOPIC = "IoT2019fy";
const String gcst_MQTT_CLIENT_ID = "myMqttPub";


#define PIN_READ 33
#define FACT_IIR_ALPHA ((float)0.9)
#define FACT_IIR_BETA ((float)1-FACT_IIR_ALPHA)

float gfValPrev;

hw_timer_t * handleTimer = NULL; 
volatile int gviCountWatchDog=0;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED; //rer to https://techtutorialsx.com/2017/10/07/esp32-arduino-timer-interrupts/

#define NUM_MAX_WATCHDOG (10*120) // 120sec 
#define NUM_1SEC_MICROSEC (1000000)
#define NUM_1MIRISEC_MICROSE (1000)
//#define NUM_SEC_TIMER (30)
#define NUM_MIRISEC_TIMER (100)
#define NUM_TIMER_MICROSEC (NUM_MIRISEC_TIMER*NUM_1MIRISEC_MICROSE)

#define NUM_STEP_WHILE_MQTT_PUB (10)

unsigned int guiCountAveItter=0;


void init_WifiClient(void){
  Serial.print("Connecting to ");
  Serial.println(gcc_SSID);
  uint16_t tmpCount =0;
  
  WiFi.begin( gcc_SSID, gcc_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    tmpCount++;
    if(tmpCount>128)
    {
      Serial.println("failed  ");
      return;
    }
  }
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}//init_WifiClient()

void init_connectMqttBroker()
{
  mqttClient.setServer(gcc_MQTT_BROKER_IP, NUM_PORT_MQTT);
  while( ! mqttClient.connected() )
  {
    Serial.println("Connecting to MQTT Broker...");
    
    if ( mqttClient.connect(gcst_MQTT_CLIENT_ID.c_str()) )
    {
      Serial.println("connected to MQTT broker"); 
      mqttClient.loop();
    }
  }
  // connected
}//init_connectMqttBroker()


void reconnectMqttBroker()
{
  if(!mqttClient.connected())
  {  
    if (mqttClient.connect(gcst_MQTT_CLIENT_ID.c_str()) )
    {
      Serial.println("reconnected to MQTT broker"); 
      mqttClient.loop();
    }// done connect
    else
    {
      Serial.print("failure to reconnect to MQTT broker. rc= ");
      Serial.println(mqttClient.state());
      Serial.print("Wifi State= ");
      Serial.println(WiFi.status());
      delay(1000);// 1 sec wait
      return;
    }// else = failure connect
  }// if not connected
  // connected
}//reconnectMqttBroker()


void doPubMqttJson()
{
  char *cValue;
  String sValue = "{\"intensity\":"+(String)gfValPrev+"}";
  cValue = (char *)malloc(sizeof(char)*(sValue.length()+1));
  sValue.toCharArray(cValue,(sValue.length()+1)); 
  reconnectMqttBroker();
  boolean bFlagSucceed=mqttClient.publish(gcc_MQTT_TOPIC,cValue);
  free(cValue);

  if(bFlagSucceed)
  {
    Serial.print("done: ");
    Serial.println(sValue);
  }
  else
  {
    Serial.print("failure publish, rc= ");
    Serial.println(mqttClient.state());
    Serial.print("Wifi State= ");
    Serial.println(WiFi.status());
  }
  // run to check: mosquitto_sub -t myTopic
}//doPubMqttJson()


void IRAM_ATTR irq_Timer01()
{
  portENTER_CRITICAL_ISR(&timerMux);
  gviCountWatchDog++;
  if (gviCountWatchDog > NUM_MAX_WATCHDOG)
  {
    ESP.restart(); // reboot
  }
  portEXIT_CRITICAL_ISR(&timerMux);
}//irq_Timer01()


void init_TimerInterrupt()
{
  gviCountWatchDog =0;

  handleTimer = timerBegin(0, 80, true); // Num_timer ,  counter per clock 
  timerAttachInterrupt(handleTimer, &irq_Timer01, true);
  timerAlarmWrite(handleTimer, NUM_TIMER_MICROSEC, true); //[us] per 80 clock @ 80MHz
  timerAlarmEnable(handleTimer);
}//init_TimerInterrupt()


void setup() {
  Serial.begin(115200);
  gfValPrev=0;
  guiCountAveItter =0;
  init_WifiClient();
  init_connectMqttBroker();
  init_TimerInterrupt();
}

void loop() {
  if (gviCountWatchDog >0)
  {
    gviCountWatchDog=0;
    float fValCurr = analogRead(PIN_READ);
    gfValPrev = fValCurr * FACT_IIR_ALPHA + gfValPrev * FACT_IIR_BETA ;
    //Serial.println(gfValPrev);
    guiCountAveItter++;
    if(guiCountAveItter>4)
    {
      doPubMqttJson();
      guiCountAveItter=0;
    }
  }
