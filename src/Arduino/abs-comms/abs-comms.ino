#include "abs_comms.h"
#include "abs_hdlc.h"
#include <adk.h>
#include <usbhub.h>
#include <TimerOne.h>
#include <SoftwareSerial.h>
#include <SD.h>
#include <SPI.h>
#include <Servo.h>
#include <avr/wdt.h>
#include "abs.h"

Comms comms;

/*Define pre-complier variables*/
#define FINAL 0
#define COMMS_TEST 1
#define ABS_TEST 2
#ifndef test_version
#define test_version COMMS_TEST
#endif

/* initializations */
Servo myServo[MAX_SERVO];
SoftwareSerial mySerial[MAX_SERIAL] = {SoftwareSerial(10, 11), SoftwareSerial(7, 8)};

File buffer;
USB Usb;
Event event_list[MAX_EVENTS];

/*initialize Variables*/
#if test_version == COMMS_TEST
int timer = 0, eventCount = 0, packetCount = 0;
char str[80];
int config_done, config_time;
int changex_done, changex_time;
int transmit_done, transmit_time;
#else 
int time = 0, eventCount = 0, packetCount = 0;
#endif

ADK adk(&Usb, "UPC, BarcelonaTech",
        "Android Beyond the Stratoshpere",
        "Android Beyond the Stratoshpere",
        "1.0",
        "http://www.upc.edu",
        "000000000000000001");

void reboot(void)
{
  wdt_enable(WDTO_15MS);
  while (1);
}

void setup(void)
{
#if test_version == COMMS_TEST
  Serial.begin(SERIAL_BITRATE);
  Serial.println("\r\nArduino firmware start");

  Timer1.initialize(TIMER_INTERVAL);
  Timer1.pwm(9, 512);
  Timer1.attachInterrupt(events_routine);

  pinMode(SD_CS, OUTPUT);

  Serial.println("Setup Ok\n");

#elif test_version == ABS_TEST
  Serial.begin(SERIAL_BITRATE);
  Serial.println("\r\nArduino firmware start");

  if (Usb.Init() == -1) {
    Serial.println("OSCOKIRQ failed to assert");
    while (Usb.Init() == -1); /* retry */
  }

  Timer1.initialize(TIMER_INTERVAL);
  Timer1.pwm(9, 512);
  Timer1.attachInterrupt(events_routine);

  pinMode(SD_CS, OUTPUT);

  if (!SD.begin(SD_CS)) {
    Serial.println("Error initializing SDcard");
  }

  Serial.println("Setup Ok\n");
  #else
  /*place final integrated code here*/
#endif
}

void loop(void) {

#if test_version == COMMS_TEST

USBPacket *packet;
    USBPacket *pkt_res;
    char str[80];
    uint8_t ret;
    uint16_t len;

    packet = (USBPacket *)malloc(sizeof(*packet));

    packet->packet_id = 1;
    packet->command = COMMS;
    packet->parameters = CONFIGURE;

    sprintf(str, "Received %d %d:%d %d:%d:%d", packet->packet_id,
                packet->command, packet->parameters, packet->cmd_arg1,
                packet->cmd_arg2, packet->data_size);
    Serial.println(str);

    pkt_res = (USBPacket *)malloc(sizeof(*pkt_res));
    pkt_res = execute_packet(packet);
    free(packet);

    sprintf(str, "Result %d %d:%d %d:%d:%d", pkt_res->packet_id,
                pkt_res->command, pkt_res->parameters, pkt_res->cmd_arg1,
                pkt_res->cmd_arg2, pkt_res->data_size);
    Serial.println(str);

    free(pkt_res);
    config_done = 1;

    packet->packet_id = 2;
    packet->command = COMMS;
    packet->parameters = CHANGE_X;
    packet->cmd_arg1 = BITRATE;
    packet->cmd_arg2 = 50000;


    sprintf(str, "Received %d %d:%d %d:%d:%d", packet->packet_id,
                packet->command, packet->parameters, packet->cmd_arg1,
                packet->cmd_arg2, packet->data_size);
    Serial.println(str);

    pkt_res = execute_packet(packet);
    free(packet);

    sprintf(str, "Result %d %d:%d %d:%d:%d", pkt_res->packet_id,
                pkt_res->command, pkt_res->parameters, pkt_res->cmd_arg1,
                pkt_res->cmd_arg2, pkt_res->data_size);
    Serial.println(str);

    free(pkt_res);
    changex_done = 1;

#elif test_version == ABS_TEST
  USBPacket *packet;
  USBPacket *pkt_res;
  uint8_t *response;
  uint8_t msg[MAX_PACKET_SIZE];
  int i = 0;
  char str[80];
  uint8_t ret;
  uint16_t len;

  Usb.Task();

  if (!adk.isReady()) {
    Serial.println("ADK not ready\n");
    return;
  }

  len = sizeof(msg);
  adk.RcvData(&len, msg);
  if (len > 0) {
    packet = process_packet(msg);

    sprintf(str, "Received %d %d:%d %d:%d:%d", packet->packet_id,
            packet->command, packet->parameters, packet->cmd_arg1,
            packet->cmd_arg2, packet->data_size);
    Serial.println(str);

    pkt_res = execute_packet(packet);
    free(packet);

    sprintf(str, "Result %d %d:%d %d:%d:%d", pkt_res->packet_id,
            pkt_res->command, pkt_res->parameters, pkt_res->cmd_arg1,
            pkt_res->cmd_arg2, pkt_res->data_size);
    Serial.println(str);

    response = to_raw(pkt_res, &len);
    free(pkt_res);

    adk.SndData(len, response);
    free(response);
  }

  for (i = 0; i < eventCount; i++) {
    if (event_list[i].execute == 1) {
      pkt_res = execute_packet(event_list[i].action);
      save_event_data(i, pkt_res);
      free(pkt_res);
      event_list[i].execute = 0;
    }
  }

  #else
  /*place final integrated code here*/
#endif
}

void events_routine(void)
{
  #if test_version == COMMS_TEST
  /*TIMER_INTERVAL has us units*/
    if(config_done) {
          config_time = timer * TIMER_INTERVAL / 1000;
          config_done = 0;
          sprintf(str, "The execution time of configuration was %d ms", config_time);
          Serial.println(str);
          timer = 0;
      
    } else if(changex_done) {
          changex_time = timer * TIMER_INTERVAL / 1000;
          changex_done = 0;
          sprintf(str, "The execution time of change_X was %d ms", changex_time);
          Serial.println(str);
          timer = 0;
      
    } else if(transmit_done) {
          transmit_time = timer * TIMER_INTERVAL / 1000;
          transmit_done = 0;
          sprintf(str,"The execution time of transmit was %d ms", transmit_time);
          Serial.println(str);
          timer = 0;
    }
    timer++;

  #elif test_version == ABS_TEST
  int i = 0;
  for (i = 0; i < eventCount; i++) {
    if (time % event_list[i].interval == 0) {
      Serial.println("Event!!!");
      event_list[i].execute = 1;
    }
  }
  time++;

  #else
  /*place final integrated code here*/
  #endif
}
