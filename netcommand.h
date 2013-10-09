#ifndef NETCOMMAND_H
#define NETCOMMAND_H

#include <stdlib.h>
#include <stdio.h>

class NetCommand{
	public:
		void handleCommand(char *cmd);
		uint32_t parse_number(char *line);
		void inline clear_line(char *line);
		void setEvent(char *line);
		void getEvent(char *line);
		void getSettings(char *line);
		inline void setMessage(char *line);
		inline void getMessage(char *line);
		inline void setUnixTime(char *line);
		inline void getUnixTime(char *line);
		inline void setScrollSpeed(char *line);
		inline void getScrollSpeed(char *line);
		inline void drawImage(char *line, uint8_t brightness);
		void setVideoPermission(char *line);
		inline void isVideoEnabled(char *line);
		inline void setVideoRequest(char *line);
		inline void getVideo(char *line);
		inline void setBrightness(char *line);
		inline void getBrightness(char *line);
};

extern NetCommand Commander;

#endif
