#include <Arduino.h>
/* Emulate serial comms. i2c not working */
#include <SoftwareSerial.h>

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

void setup()
{
  /* 8N1 */
  hwSerial.begin(115200);
}

void loop()
{
  /* Gather data from Pins */
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

  for(int i = 0; i < sizeof(data_array)/sizeof(data_array[0]); i++)
  {
    /* Remap 2^10 to 2^8 because serial write() func takes a char (uint8_t) */
    c_uart_val = uint8_t(map(data_array[i], 0, 4095, ARRAY_SIZE, 255));
    /* range of nums: 0..12 is saved for the index, also sent over uart. */
    hwSerial.write(uint8_t(i));
    hwSerial.write(c_uart_val);
    /* DEBUG */
    delay(100);
  }

}