/*
 * WindowInfo.h
 *
 *  Created on: Oct 17, 2016
 *      Author: alon
 */

#ifndef WINDOWINFO_H
#define WINDOWINFO_H


#include <X11/Xlib.h>
#include <string>
#include "ApplicationType.h"

class WindowInfo {
public:
	WindowInfo();
	~WindowInfo();

public:
	std::string GetInfo();

public:
    ApplicationType Type;
	Window FocusedWindow;
	Window ClientWindow;
	std::string WindowTitle;
	std::string ResourceName;
	std::string ResourceClass;
	std::string Path;
	std::string URL;
	int Pid;
	long ConnectTime;
	long DisconnectTime;
};

#endif /* WINDOWINFO_H */
