#include "netcommand.h"
#include "ClockTime.h"
#include "display.h"
#include <string.h>

extern volatile bool DEBUG;
NetCommand Commander;

extern "C" {
void handleCommand(char *cmd) {
	Commander.handleCommand(cmd);
}
}

bool NetCommand::handleCommand(char *cmd) {
	switch (*cmd){
		//set time
		case 't' : 
			return clock.setTime(cmd+1);
		// set show end time	
//		case 's' :
//			return shows.add(cmd+1);
		// twitter text
		case '#' :
			strcpy(cmd, "result");
			return true;
		case 'X' : 
	  		while(switchBuffersFlag);
  			for(uint8_t i=0;i < XRES * 2;i++)
    				drawBuffer[i]=0xff;
			switchBuffersFlag = 1;
			DEBUG=true;
			return true;
	}

	return false;
	
};

