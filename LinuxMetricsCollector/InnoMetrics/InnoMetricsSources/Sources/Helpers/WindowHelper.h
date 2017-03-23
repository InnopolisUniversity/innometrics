/*
 * WindowHelper.h
 * Class represents the most influence functions for working with X-11
 *
 *  Created on: Oct 13, 2016
 *      Author: alon
 */

#ifndef WINDOWHELPER_H
#define WINDOWHELPER_H

#include <stdio.h>
#include <unistd.h>
#include <cstring>

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xmu/WinUtil.h>
#include <sys/types.h>


class WindowHelper {
public:
	static int xerror;
	static int HandleX11Error(Display *, XErrorEvent *error_event);
	static Window GetTopWindow(Display* display, Window startWindow);
	static Window GetClientWindow(Display* d, Window start);
	static XClassHint* GetWindowClass(Display* d, Window w);
	static int GetPid(Display *display, Window window);
	static void GetWindowHeightWidht(Display *display, Window window, int &width, int &height);
	static char* GetWindowName(Display* display, Window window);
	static char* GetWindowNameMultiByte(Display* display, Window window);
	static char* GetPathByPid(int pid);
	static Window* GetAllWindows(Display* display, Window rootWindow, unsigned long* len);
private:
	WindowHelper(){}

private:
	static const int PATH_SIZE = 128;
};
#endif /* WINDOWHELPER_H */
