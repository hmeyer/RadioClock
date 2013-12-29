#ifndef NETCOMMAND_H
#define NETCOMMAND_H

#include <stdlib.h>
#include <stdio.h>

class NetCommand{
	public:
		void handleCommand(const char *cmd, char *out);

	private:
		const char* cmd;
		char* output;

		uint32_t parse_number(uint8_t &pos);
		void printOk();
		void setEvent();
		void getEvent();
		void getSettings();
		inline void setMessage();

		inline void setUnixTime();
		inline void getUnixTime();
		inline void setScrollSpeed();
		inline void drawImage(uint8_t brightness);
		inline void setVideoRequest();
		inline void isVideoEnabled();
		inline void enableVideo();
		inline void disableVideo();
		inline void setBrightness();
		inline void getSystemStart();
};

extern NetCommand Commander;

#endif
