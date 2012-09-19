#include "WiShield/WiShield.h"

#define WIRELESS_MODE_INFRA	1
#define WIRELESS_MODE_ADHOC	2

// Wireless configuration parameters ----------------------------------------
unsigned char local_ip[] = {192,168,179,101};	// IP address of WiShield
unsigned char gateway_ip[] = {192,168,179,1};	// router or gateway IP address
unsigned char subnet_mask[] = {255,255,255,0};	// subnet mask for the local network
const prog_char ssid[] PROGMEM = {"bact_wlan_tim"};		// max 32 bytes

unsigned char security_type = 0;	// 0 - open; 1 - WEP; 2 - WPA; 3 - WPA2

// WPA/WPA2 passphrase
const prog_char security_passphrase[] PROGMEM = {"Hosemann"};	// max 64 characters

// setup the wireless mode
// infrastructure - connect to AP
// adhoc - connect to another WiFi device
unsigned char wireless_mode = WIRELESS_MODE_INFRA;

unsigned char ssid_len;
unsigned char security_passphrase_len;
char buffer[80];
/*
unsigned long int c=0;
//---------------------------------------------------------------------------

void setup()
{
  
        Serial.begin(9600); 
        Serial.println("init before"); 
  
	WiFi.init();

        Serial.println("init after"); 


}

void loop()
{
	WiFi.run();
        if (c%10000==0){
           Serial.print('.');
        }
        if (c%100000==0){
           Serial.print('\n');
           Serial.println(buffer) ; 
           c=0;
        }
        //if (buffer[0]!=0){
        //  Serial.println(buffer) ; 
        //  buffer[0]=0;
        //}
        c++;
//
}
*/
