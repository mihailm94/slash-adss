#include <Arduino.h>
/* Emulate serial comms. i2c not working */
#include <SoftwareSerial.h>

/* data_array[0] - MODE_TOP or MODE_FRONT
 * data_array[1..3] - front sensors value
 * data_array[4..12] - up sensors value
 */
int data_array[13] = { 0 };

/* SW emulated UART - pins 19, 18 */
#define ORIGIN_TXD 19
#define ORIGIN_RXD 18

#define DEST_TXD 18
#define DEST_RXD 19

/* RX, TX */
SoftwareSerial serial(ORIGIN_RXD, ORIGIN_TXD);

int mode = 0;

/* Values coming from GPIOs */
int s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13;

void setup()
{
  Serial.begin(115200);

  serial.begin(115200);
}


void loop()
{
  if(serial.available())
  {
    Serial.println(serial.read());
  }

  /* Gather data from Pins
  data_array[4] = 4095 - analogRead(36);
  data_array[5] = 4095 - analogRead(39);
  data_array[6] = 4095 - analogRead(34);
  data_array[7] = 4095 - analogRead(35);
  data_array[8] = 4095 - analogRead(32);
  data_array[9] = 4095 - analogRead(33);
  data_array[10] = 4095 - analogRead(25);
  data_array[11] = 4095 - analogRead(26);
  data_array[12] = 4095 - analogRead(27);
  data_array[0] = mode;
  data_array[1] = 4095 - analogRead(14);
  data_array[2] = 4095 - analogRead(12);
  data_array[3] = 4095 - analogRead(4);

 */
}