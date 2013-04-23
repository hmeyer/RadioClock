#include "netcommand.h"
#include "ClockTime.h"
#include "display.h"
#include <string.h>
#include "wiring.h"
#include <stdlib.h>
#include <stdio.h>

extern volatile bool DEBUG;
NetCommand Commander;

extern "C" {
uint8_t handleCommand(char *cmd) {
	return Commander.handleCommand(cmd);
}
}

void rowStat(char *buf) {
    sprintf(buf,"ST:%d OE:%d CP:%d DA:%d", 
		    digitalRead(ROW_ST),
		    digitalRead(ROW_OE),
		    digitalRead(ROW_CP),
		    digitalRead(ROW_D)
		    );
}

bool NetCommand::handleCommand(char *cmd) {
	//set time
	if (*cmd == 't') return clock.setTime(cmd+1);

	// set show end time	
//	if (*cmd == 'e') return shows.add(cmd+1);
	char *c = cmd;
	while (*c) {
    		DEBUG=true;
		switch (*c){
		case 's' :
    			digitalWrite(ROW_ST, false); break;
		case 'S' :
    			digitalWrite(ROW_ST, true); break;
		case 'c' :
    			digitalWrite(ROW_CP, false); break;
		case 'C' :
    			digitalWrite(ROW_CP, true); break;
		case 'd' :
    			digitalWrite(ROW_D, false); break;
		case 'D' :
    			digitalWrite(ROW_D, true); break;
		case 'o' :
    			digitalWrite(ROW_OE, false); break;
		case 'O' :
    			digitalWrite(ROW_OE, true); break;
		}
		c++;
	}
	rowStat(cmd); return true;
};

