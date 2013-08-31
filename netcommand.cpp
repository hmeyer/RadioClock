#include "netcommand.h"
#include "display.h"
#include <string.h>
#include "wiring.h"
#include <stdlib.h>
#include <stdio.h>
#include "rtc/RTClib.h"

extern volatile bool DEBUG;
NetCommand Commander;

extern "C" {
uint8_t handleCommand(char *cmd) {
	return Commander.handleCommand(cmd);
}
}

bool NetCommand::handleCommand(char *cmd) {
	//set time
//	if (*cmd == 't') return clock.setTime(cmd+1, cmd);
	//if (*cmd == 'e') return shows.add(cmd+1);
  DateTime d = RTC.now();
 sprintf(cmd, "%lu", d.second());
	return true;
};

