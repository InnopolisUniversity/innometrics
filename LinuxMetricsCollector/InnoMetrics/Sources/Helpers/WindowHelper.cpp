/*
 * WindowHelper.cpp
 *
 *  Created on: Oct 13, 2016
 *      Author: alon
 */
#include "WindowHelper.h"

int WindowHelper::xerror = 0;

int WindowHelper::HandleX11Error(Display *, XErrorEvent *error_event) {
	//printf("ERRROR");
	xerror = 1;
	return 0;
}

Window WindowHelper::GetTopWindow(Display* display, Window startWindow) {
	Window window = startWindow;
	Window parent = startWindow;
	Window root = None;
	Window *children;
	unsigned int nChildren;
	Status s;

	while (parent != root) {
		window = parent;
		s = XQueryTree(display, window, &root, &parent, &children, &nChildren);

		if (s)
			XFree(children);

		if (xerror) {
			//ERROR: GetTopWindow
			xerror = 0;
			return startWindow;
		}
		// Intermediate window can be written
	}
	return window;
}

//Window which has a name
Window WindowHelper::GetClientWindow(Display* display, Window startWindow) {
	Window window;
	window = XmuClientWindow(display, startWindow);
	if (window == startWindow) {
	} //printf("fail\n");
	xerror = 0;
	return window;
}

XClassHint* WindowHelper::GetWindowClass(Display* d, Window w) {
	Status s;
	XClassHint* pXClass;

	pXClass = XAllocClassHint();
	if (xerror) {
		//ERROR: XAllocClassHint
		xerror = 0;
		return NULL;
	}

	s = XGetClassHint(d, w, pXClass);
	if (s) {
		return pXClass;
	} else {
		//ERROR: XGetClassHint
		return NULL;
	}
}

int WindowHelper::GetPid(Display *display, Window window) {
	int pid = 0;

	XTextProperty text_data;
	Atom atom = XInternAtom(display, "_NET_WM_PID", True);

	if (XGetTextProperty(display, window, &text_data, atom)) {
		pid = text_data.value[1] * 256;
		pid += text_data.value[0];
	}
	xerror = 0;
	return pid;
}

void WindowHelper::GetWindowHeightWidht(Display *display, Window window, int &width, int &height) {
	XWindowAttributes windowAttributes;
	if (xerror != 0	|| XGetWindowAttributes(display, window, &windowAttributes)) {
		width = windowAttributes.width;
		height = windowAttributes.height;
	}
	xerror = 0;
}

char* WindowHelper::GetWindowName(Display* display, Window window) {
	char *pName;
	if (XFetchName(display, window, &pName) <= 0) {
		pName = (char*) "";
		return pName;
	}
	xerror = 0;
	return pName;
}

char* WindowHelper::GetWindowNameMultiByte(Display* display, Window window) {
	XTextProperty prop;
	Status s;
	char* ret;
	s = XGetWMName(display, window, &prop);
	if (!xerror && s) {
		int count = 0, result;
		char **list = NULL;
		result = XmbTextPropertyToTextList(display, &prop, &list, &count);
		//printf("%d", count);
		if (result == Success && count > 0 && list[0]) {
			xerror = 0;
			ret = new char[strlen(list[0])+1];
			std::strcpy(ret, list[0]);
		} else {
			//ERROR: XmbTextPropertyToTextList
			ret = new char[strlen((char*)prop.value)+1];
			std::strcpy(ret, (char*)prop.value);
		}
		XFreeStringList(list);
	} else {
		ret = (char *) "";
	}
	XFree((void *)prop.value);
	xerror = 0;
	return ret;
}


char* WindowHelper::GetPathByPid(int pid) {
	char path[64];
	char *pBuf= new char[WindowHelper::PATH_SIZE];
    bzero(pBuf, sizeof(char)* WindowHelper::PATH_SIZE);
	sprintf(path, "/proc/%d/exe", pid);
	readlink(path, pBuf, WindowHelper::PATH_SIZE);
	return pBuf;
}

Window* WindowHelper::GetAllWindows(Display* display, Window rootWindow, unsigned long* len){
    Atom prop = XInternAtom(display, "_NET_CLIENT_LIST", False), type;
    int form;
    unsigned long remain;
    unsigned char *list;

    XGetWindowProperty(display, rootWindow, prop, 0, 1024, False, XA_WINDOW,
                            &type, &form, len, &remain, &list);

    return (Window *)list;
}
