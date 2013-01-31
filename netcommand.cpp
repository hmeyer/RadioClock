#include "netcommand.h"


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
			return true;
	}

	return false;
	
};

