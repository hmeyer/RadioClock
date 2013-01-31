#ifndef NETCOMMAND_H
#define NETCOMMAND_H

class NetCommand {
	public:
		void handleCommand(char *cmd);
};

extern NetCommand Commander;

#endif
