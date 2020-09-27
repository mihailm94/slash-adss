#include <Arduino.h>
#include <ESP32Servo.h>

/* 0 is reserved for the end of the array.
 * Used in UART.
 */
#define END_OF_ARRAY    0U
/* array index gets sent over uart. Array is 12 elements */
#define ARRAY_SIZE      13U
#define MODE_TOP        2U
#define MODE_FRONT      4U

/* data_array[0] - MODE_TOP or MODE_FRONT
 * data_array[1..3] - front sensors value
 * data_array[4..12] - up sensors value
 */
uint16_t data_array[ARRAY_SIZE] = { 0 };
/* Holds the value to transfer over UART */
uint8_t c_uart_val;

/* HIVE_TX - 17, HIVE_RX - 16 */
HardwareSerial hwSerial(2);

/* Take data from front sensors or from top sensors */
uint8_t mode = MODE_TOP;

/* Values coming from GPIOs */
int s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13;

int min_thres = 300;
int darkness_counter=0;
int init_mov=0; 

//GPIO 16-17 Reserved

int pwm=0;

int fwd_pin = 23;
int bck_pin = 22;
int pwm_pin = 5; 

int servo_pin = 19 ;

Servo myservo ;
int pos = 0;

// Setting PWM properties
const int freq = 30000;
const int pwmChannel = 0;
const int resolution = 8;

int actual_pos = -1 ;

#define s1    data_array[4]
#define s2    data_array[5]
#define s3    data_array[6]
#define s4    data_array[7]
#define s5    data_array[8]
#define s6    data_array[9]
#define s7    data_array[10]
#define s8    data_array[11]
#define s9    data_array[12]

#define MODE  data_array[0]

#define fs1   data_array[1]
#define fs2   data_array[2]
#define fs3   data_array[3]

unsigned long saved_time;

void setup()
{
  /* 8N1 */
  hwSerial.begin(115200);

  Serial.begin(115200);
  Serial.println("\n\n\n\n");
  pinMode(pwm_pin, OUTPUT);
  pinMode(fwd_pin, OUTPUT);
  pinMode(bck_pin, OUTPUT);

  ledcSetup(pwmChannel, freq, resolution);
  ledcAttachPin(pwm_pin,pwmChannel);

  ESP32PWM::allocateTimer(0);
	ESP32PWM::allocateTimer(1);
	ESP32PWM::allocateTimer(2);
	ESP32PWM::allocateTimer(3);
	
  myservo.setPeriodHertz(50);    // standard 50 hz servo
	myservo.attach(servo_pin, 500, 2400); // attaches the servo on pin 13 to the servo object
  
  saved_time = millis();
}

void loop()
{
  //Pins for Data Aquisition 
  // 0    -> DARKNESS
  // 4095 -> MAX LUMINOSITY

  /* Gather data from Pins */
  s1 = 4095 - analogRead(36);
  s2 = 4095 - analogRead(39);
  s3 = 4095 - analogRead(34);
  s4 = 4095 - analogRead(35);
  s5 = 4095 - analogRead(32);
  s6 = 4095 - analogRead(33);
  s7 = 4095 - analogRead(25);
  s8 = 4095 - analogRead(26);
  s9 = 4095 - analogRead(27);

  MODE = mode;
  
  fs1 = 4095 - analogRead(14);
  fs2 = 4095 - analogRead(12);
  fs3 = 4095 - analogRead(4);

  int pot = analogRead(15);


  for(int i = 0; i < sizeof(data_array)/sizeof(data_array[0]); i++)
  {
    /* Remap 2^10 to 2^8 because serial write() func takes a char (uint8_t) */
    c_uart_val = uint8_t(map(data_array[i], 0, 4095, ARRAY_SIZE, 255));
    /* range of nums: 0..12 is saved for the index, also sent over uart. */
    hwSerial.write(uint8_t(i));
    hwSerial.write(c_uart_val);
    /* DEBUG */
    delay(10);
  }

  //SPEED  -------------------------------------------
  printf( "POT: %d " , pot );
  if(pot>10)
  {
    //FRONT
    digitalWrite(fwd_pin,LOW);
    digitalWrite(bck_pin,LOW);
    pwm = map(pot,0,4095,0,255);
    //ledcWrite(pwmChannel,pwm);
    //printf(" PWM:%d " , pwm);
    
  }
  else
  {
    digitalWrite(fwd_pin,LOW);
    digitalWrite(bck_pin,LOW);
  }

  //STEERING ---------------------------------------------
  if(actual_pos == -1)
  {
    actual_pos = 90 ;
    myservo.write(actual_pos);
  }

  // pos = map(pot,0,4095,45,120);

  // if(pos > prev_pos + 1 || pos < prev_pos - 1 || prev_pos == -1)
  // {
  //   if( prev_pos!= -1 ) myservo.write(pos); 
  //   Serial.println(pos);
  //   prev_pos = pos;
  // }

  //General State of the Sensors
  //printf("%d  %d  %d  %d  %d  %d  %d  %d  %d " ,s1,s2,s3,s4,s5,s6,s7,s8,s9);
  printf("%d  %d  %d  %d  %d  %d  %d  %d  %d" ,s1,s2,s3,s4,s5,s6,s7,s8,s9);
  //Combinatorial Cases

  //TOP COVER DETECTED
  if( s1<min_thres && s2<min_thres && s3<min_thres && 
      s4<min_thres && s5<min_thres && s6<min_thres && 
      s7<min_thres && s8<min_thres && s9<min_thres    )
  {
    printf("    --- DARKNESS MODE --");

  
    //GO STRAIGHT
    if (actual_pos != 90)
    {
      if (actual_pos < 90)
      {
        if ( millis() > saved_time + 10 )
        {
          actual_pos++;
          myservo.write(actual_pos);
          saved_time = millis();
          //printf("    --- POS : %d --",actual_pos);
        } 
      }
      else
      {
        if ( millis() > saved_time + 10 )
        {
          actual_pos--;
          myservo.write(actual_pos);
          saved_time = millis();
          //printf("    --- POS : %d --",actual_pos);
        }
      }
    }
    
    if(darkness_counter > min_thres)
    {
      darkness_counter=0;
      mode = 1;
      
    }
    else{darkness_counter++;}
  }

  else if( s1 > s2 + min_thres && 
           s1 > s3 + min_thres &&
           s1 > s4 + min_thres &&
           s1 > s5 + min_thres &&
           s1 > s6 + min_thres && 
           s1 > s7 + min_thres &&
           s1 > s8 + min_thres && 
           s1 > s9 + min_thres  )
  {
    printf(" --- S1_MAX -> Move FRONT and LEFT         ");
    // if (init_mov == 0 )
    // {
    //   printf(" --- S1_MAX -> Move FRONT and LEFT         ");
    //   init_mov++;
    //   //moving
    // }
    // else if (s5 > s1 + min_thres)
    // {
    //   printf(" --- CENTER REACHED ---          ");
    //   init_mov=0;
    //   //stop
    // } 
  }
  //S2_MAX›
  else if( s2 > s1 + min_thres && 
           s2 > s3 + min_thres &&
           s2 > s4 + min_thres &&
           s2 > s5 + min_thres &&
           s2 > s6 + min_thres && 
           s2 > s7 + min_thres &&
           s2 > s8 + min_thres && 
           s2 > s9 + min_thres  )
  {
    printf(" --- S2_MAX -> Move FRONT                ");
    
    digitalWrite(fwd_pin,LOW);
    digitalWrite(bck_pin,HIGH);
    

    // if (init_mov == 0 )
    // {
    //   printf(" --- S2_MAX -> Move FRONT                ");
    //   init_mov++;
    //   //moving
    // }
    // else if (s5 > s2 + min_thres)
    // {
    //   init_mov=0;
    //   printf(" --- CENTER REACHED ---          ");
    //   //stop
    // }
  }

  //S3_MAX›
  else if( s3 > s1 + min_thres && 
           s3 > s2 + min_thres &&
           s3 > s4 + min_thres &&
           s3 > s5 + min_thres &&
           s3 > s6 + min_thres && 
           s3 > s7 + min_thres &&
           s3 > s8 + min_thres && 
           s3 > s9 + min_thres  )
  {
    printf(" --- S3_MAX -> Move FRONT AND RIGHT          ");

    // if (init_mov == 0 )
    // {
    //   printf(" --- S3_MAX -> Move FRONT AND RIGHT          ");

    //   //moving
    // }
    // else if (s5 > s3 + min_thres)
    // {
    //   init_mov=0;
    //   printf(" --- CENTER REACHED ---          ");
    //   //stop
    // }
  }

  //S4_MAX›
  else if( s4 > s1 + min_thres && 
           s4 > s2 + min_thres &&
           s4 > s3 + min_thres &&
           s4 > s5 + min_thres &&
           s4 > s6 + min_thres && 
           s4 > s7 + min_thres &&
           s4 > s8 + min_thres && 
           s4 > s9 + min_thres  )
  {
    if (init_mov == 0 )
    {
      printf(" --- S4_MAX -> Move to the LEFT          ");

      //moving

      //MOVE LEFT
      //GO STRAIGHT
      if (actual_pos != 45)
      {
        if (actual_pos < 45)
        {
          if ( millis() > saved_time + 10 )
          {
            actual_pos++;
            myservo.write(actual_pos);
            saved_time = millis();
            printf("    --- POS : %d --",actual_pos);
          } 
        }
        else
        {
          if ( millis() > saved_time + 10 )
          {
            actual_pos--;
            myservo.write(actual_pos);
            saved_time = millis();
            printf("    --- POS : %d --",actual_pos);
          }
        }
      }


    }
    else if (s5 > s4 + min_thres)
    {
      init_mov=0;
      printf(" --- CENTER REACHED ---          ");
      //stop
    }
  }
  //S5_MAX› CENTER
  else if( s5 > s1 + min_thres && 
           s5 > s2 + min_thres &&
           s5 > s3 + min_thres &&
           s5 > s4 + min_thres &&
           s5 > s6 + min_thres && 
           s5 > s7 + min_thres &&
           s5 > s8 + min_thres && 
           s5 > s9 + min_thres  )
  {
    //GO STRAIGHT
    if (actual_pos != 90)
    {
      if (actual_pos < 90)
      {
        if ( millis() > saved_time + 10 )
        {
          actual_pos++;
          myservo.write(actual_pos);
          saved_time = millis();
          printf("    --- POS : %d --",actual_pos);
        } 
      }
      else
      {
        if ( millis() > saved_time + 10 )
        {
          actual_pos--;
          myservo.write(actual_pos);
          saved_time = millis();
          printf("    --- POS : %d --",actual_pos);
        }
      }
    }

    else if (init_mov == 0 && actual_pos==90 )
    {
      printf(" --- CENTER REACHED ---          ");
      //moving
    }
  }
  //S6_MAX›
  else if( s6 > s1 + min_thres && 
           s6 > s2 + min_thres &&
           s6 > s3 + min_thres &&
           s6 > s4 + min_thres &&
           s6 > s5 + min_thres && 
           s6 > s7 + min_thres &&
           s6 > s8 + min_thres && 
           s6 > s9 + min_thres  )
  {
    printf(" --- S6_MAX -> Move to the RIGHT          ");
    
    //MOVE TO LEFT
    if (actual_pos != 125)
    {
      if (actual_pos < 125)
      {
        if ( millis() > saved_time + 10 )
        {
          actual_pos++;
          myservo.write(actual_pos);
          saved_time = millis();
          printf("    --- POS : %d --",actual_pos);
        } 
      }
      else
      {
        if ( millis() > saved_time + 10 )
        {
          actual_pos--;
          myservo.write(actual_pos);
          saved_time = millis();
          printf("    --- POS : %d --",actual_pos);
        }
      }
    }

    else if (s5 > s6 + min_thres)
    {
      init_mov=0;
      printf(" --- CENTER REACHED ---          ");
      //stop
    }
  }
  //S7_MAX›
  else if( s7 > s1 + min_thres && 
           s7 > s2 + min_thres &&
           s7 > s3 + min_thres &&
           s7 > s4 + min_thres &&
           s7 > s5 + min_thres && 
           s7 > s6 + min_thres &&
           s7 > s8 + min_thres && 
           s7 > s9 + min_thres  )
  {
    if (init_mov == 0 )
    {
      printf(" --- S7_MAX -> Move BACK and LEFT          ");

      //moving
    }
    else if (s5 > s7 + min_thres)
    {
      init_mov=0;
      printf(" --- CENTER REACHED ---          ");
      //stop
    }
  }
  //S8_MAX›
  else if( s8 > s1 + min_thres && 
           s8 > s2 + min_thres &&
           s8 > s3 + min_thres &&
           s8 > s4 + min_thres &&
           s8 > s5 + min_thres && 
           s8 > s6 + min_thres &&
           s8 > s7 + min_thres && 
           s8 > s9 + min_thres  )
  {
    if (init_mov == 0 )
    {
      printf(" --- S8_MAX -> Move BACK          ");

       digitalWrite(fwd_pin,HIGH);
        digitalWrite(bck_pin,LOW);
    }
    else if (s5 > s8 + min_thres)
    {
      init_mov=0;
      printf(" --- CENTER REACHED ---          ");
      //stop
    }
  }
  //S9_MAX›
  else if( s9 > s1 + min_thres && 
           s9 > s2 + min_thres &&
           s9 > s3 + min_thres &&
           s9 > s4 + min_thres &&
           s9 > s5 + min_thres && 
           s9 > s6 + min_thres &&
           s9 > s7 + min_thres && 
           s9 > s8 + min_thres  )
  {
    if (init_mov == 0 )
    {
      printf(" --- S9_MAX -> Move BACK and RIGHT          ");

      //moving
    }
    else if (s5 > s9 + min_thres)
    {
      init_mov=0;
      printf(" --- CENTER REACHED ---          ");
      //stop
    }
  }

  printf("           \r");
  fflush(stdout);

  //MOVE THE SERVO

  //BUZZER TO CHANGE MODE

}