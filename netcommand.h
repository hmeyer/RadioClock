#ifndef NETCOMMAND_H
#define NETCOMMAND_H

class NetCommand {
	public:
		bool handleCommand(char *cmd);
};

extern NetCommand Commander;

#endif
