// sound level meter
// standard output sound rms
// processor : esp32
// IDE : arduino
// device : ADMP441 http://akizukidenshi.com/catalog/g/gK-06864/
// ref to https://github.com/MhageGH/esp32_SoundRecorder

#include "driver/i2s.h"

// pin config
#define PIN_I2S_BCLK 26
#define PIN_I2S_WS 25
#define PIN_I2S_DIN 34
//#define PIN_I2S_DOUT 25

#define STATE_N_L_R_PULL_DOWN 1 //PullUp = 0, PullDown=1
#define N_BYTE_STEP_LR (STATE_N_L_R_PULL_DOWN*4)

// sampling info
#define HZ_SAMPLE_RATE 44100
#define N_SEC_REC  1  // second
// hardware and software parameter
#define N_SAMPLE_IN_MILLI_SEC 44
#define N_ITTER_READ_BUF  (N_SEC_REC * N_SAMPLE_IN_MILLI_SEC ) 
#define N_LEN_BUF_2BYTE 1000 
#define N_BYTES_STRACT_PAR_SAMPLE 8
#define N_LEN_BUF_ALL_BYTES  (N_LEN_BUF_2BYTE*N_BYTES_STRACT_PAR_SAMPLE)
#define N_SKIP_SAMPLES (11*2)

char i8BufAll[N_LEN_BUF_ALL_BYTES];
int16_t strmData[N_ITTER_READ_BUF*N_LEN_BUF_2BYTE];
float fLoudnessLevelCurr;
float fLoudnessLevelPrev;

void I2S_Init(void) {
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = HZ_SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
    .intr_alloc_flags = 0,
    .dma_buf_count = 16,
    .dma_buf_len = 60
  };
  i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);

  i2s_pin_config_t pin_config;
  pin_config.bck_io_num = PIN_I2S_BCLK;
  pin_config.ws_io_num = PIN_I2S_WS;
  pin_config.data_out_num = I2S_PIN_NO_CHANGE;
  pin_config.data_in_num = PIN_I2S_DIN;
  i2s_set_pin(I2S_NUM_0, &pin_config);

  i2s_set_clk(I2S_NUM_0, HZ_SAMPLE_RATE, I2S_BITS_PER_SAMPLE_32BIT, I2S_CHANNEL_STEREO);
}

void skipNoisySound(){
  for (int j = 0; j < N_SKIP_SAMPLES ; j++) 
  {
    i2s_read_bytes(I2S_NUM_0, (char *)i8BufAll, N_LEN_BUF_ALL_BYTES, portMAX_DELAY);
  }
}

void getData(){
  int32_t tmp32=0;
  for (int j = 0; j < N_ITTER_READ_BUF ; j++) 
  {
    i2s_read_bytes(I2S_NUM_0, (char *)i8BufAll, N_LEN_BUF_ALL_BYTES, portMAX_DELAY);

    for (int i = 0; i < N_LEN_BUF_2BYTE ; i++) {
      tmp32 = (i8BufAll[N_BYTES_STRACT_PAR_SAMPLE*i + 3 +N_BYTE_STEP_LR] <<8) | i8BufAll[N_BYTES_STRACT_PAR_SAMPLE*i + 2 +N_BYTE_STEP_LR];
      strmData[i + N_LEN_BUF_2BYTE*j] = ( (tmp32>(1<<15)) ? tmp32-(1<<16) : tmp32 );
    } // i
  } // j
}

void stdoutStrm(){
  for (int j = 0; j < N_ITTER_READ_BUF*N_LEN_BUF_2BYTE ; j++) 
  {
    Serial.println(strmData[j]);
  }
}

float measureLevel()
{
  int32_t tmp32;
  i2s_read_bytes(I2S_NUM_0, (char *)i8BufAll, N_LEN_BUF_ALL_BYTES, portMAX_DELAY);
  for (int i = 0; i < N_LEN_BUF_2BYTE ; i++) {
      tmp32 = (i8BufAll[N_BYTES_STRACT_PAR_SAMPLE*i + 3] <<8) | i8BufAll[N_BYTES_STRACT_PAR_SAMPLE*i + 2];
      strmData[i] = ( (tmp32>(1<<15)) ? tmp32-(1<<16) : tmp32 );
  } // i
  //sum
  long ltmpsum=0;
  for (int i = 0; i < N_LEN_BUF_2BYTE ; i++) {
    ltmpsum += strmData[i];
  }
  float fTmpMeanSq = (ltmpsum /(float)N_LEN_BUF_2BYTE);
  fTmpMeanSq *=fTmpMeanSq;
  float ftmpsum =0;
  //sum diff powered
  for (int i = 0; i < N_LEN_BUF_2BYTE ; i++) {
     ftmpsum += (float)strmData[i]*(float)strmData[i] - fTmpMeanSq;
  }
  
  float results=0;
  
  //sq.root mean
  float fRms = sqrt(ftmpsum/(float)N_LEN_BUF_2BYTE);
  results=fRms;
  
  // float y = 1.4374*fRms - 44.5015;
  // float z = 20*log10(y);
  // results=z;

  return( results );
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("---start---");
  I2S_Init();
  skipNoisySound();
  Serial.println("---done---");
  fLoudnessLevelCurr = 0;
  fLoudnessLevelPrev = 0;
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println(measureLevel());
}
