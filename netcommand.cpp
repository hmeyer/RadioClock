#include "netcommand.h"


NetCommand Commander;

extern "C" {
void handleCommand(char *cmd) {
	Commander.handleCommand(cmd);
}
}

void NetCommand::handleCommand(char *cmd) {
};

