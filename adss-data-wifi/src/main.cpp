#include <Arduino.h>
#include <WiFi.h>
#include <SoftwareSerial.h>
/* Defines the "buffer" size */
#define AREST_NUMBER_VARIABLES 13
#include <aREST.h>

const char* ssid = "adssWifi";
const char* password = "adssWifiPassword";

/* aREST instance */
aREST rest = aREST();

/* The port to listen for incoming TCP connections */
#define LISTEN_PORT 80

IPAddress staticIP(192, 168, 4, 20);
IPAddress gateway(192, 168, 4, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress dns(192, 168, 4, 1);

/* Create an instance of the server */
WiFiServer server(LISTEN_PORT);

/* data_array[0] - MODE_TOP or MODE_FRONT
 * data_array[1..3] - front sensors value
 * data_array[4..12] - up sensors value
 */
uint8_t data_array[13] = { 0 };
uint8_t tx_val = 0;
uint8_t mode = 0;

/* Values coming from GPIOs */
uint8_t s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13;


/* Passed to swSerial constructor */
SoftwareSerialConfig swSerialConfig = SWSERIAL_8N1;
/* SW emulated UART - pins 19, 18 */
#define WORKER_TX   17
#define WORKER_RX   16

/* WORKER_RX - 16, WORKER_TX - 17 */
HardwareSerial hwSerial(2);

void setup() {
  Serial.begin(115200);
  hwSerial.begin(115200);

  /* Expose variables to REST API */
  rest.variable("mode",         &data_array[0]);
  rest.variable("val_front_0",  &data_array[1]);
  rest.variable("val_front_1",  &data_array[2]);
  rest.variable("val_front_2",  &data_array[3]);
  rest.variable("val_top_0",    &data_array[4]);
  rest.variable("val_top_1",    &data_array[5]);
  rest.variable("val_top_2",    &data_array[6]);
  rest.variable("val_top_3",    &data_array[7]);
  rest.variable("val_top_4",    &data_array[8]);
  rest.variable("val_top_5",    &data_array[9]);
  rest.variable("val_top_6",    &data_array[10]);
  rest.variable("val_top_7",    &data_array[11]);
  rest.variable("val_top_8",    &data_array[12]);

  /* Set device name and ID */
  rest.set_id("1");
  rest.set_name("esp_data_ldr");

  /* Avoid any existing connections */
  WiFi.disconnect();

  /* If configuring with static IP did not work, use a DHCP assigned IP */
  if( false == WiFi.config(staticIP, gateway, subnet, dns, dns) )
  {
    Serial.println("Static IP config failed.");
  }

  /* Initiate wifi connection to specified ssid and pass */
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Connected to the WiFi network");

  /* Begin server */
  server.begin();
  Serial.println("Server started");
 
  /* Print connection information */
  Serial.print("Local IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("Subnet Mask: ");
  Serial.println(WiFi.subnetMask());
  Serial.print("Gateway IP: ");
  Serial.println(WiFi.gatewayIP());
  Serial.print("DNS 1: ");
  Serial.println(WiFi.dnsIP(0));
  Serial.print("DNS 2: ");
  Serial.println(WiFi.dnsIP(1));
 
}

void loop() {

  WiFiClient client = server.available();

  /* No need to ask if available, the read() function does it */
  if(hwSerial.available())
  {
    tx_val = uint8_t(hwSerial.read());
    /* Received value is an array index */
    if( 0 <= tx_val && 12 >= tx_val )
    {
      /* Next read is value from sensor */
      data_array[tx_val] = uint8_t(hwSerial.read());
    }
    else
    {
      /* We assume the value is garbage.
       * Do not update what is in array already.
       * EMPTY ELSE
       */
    }
  }

  /* Serve data array at REST GET */
  if( client.available() )
  {
    rest.handle(client);
  }

}