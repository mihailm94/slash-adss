#include <Arduino.h>
#include <WiFi.h>
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
int data_array[13] = { 0 };

int mode = 0;

/* Values coming from GPIOs */
int s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13;

void setup() {
  Serial.begin(115200);

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
  if( !client )
  {
    return;
  }
  while( !client.available() )
  {
  }
  
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

  /* Serve data array at REST GET */
  rest.handle(client);

}