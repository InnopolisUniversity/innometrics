#include <locale.h>		// set locale for getting 2bytes encoding
#include <stdio.h>
#include <stdlib.h>
#include <iostream>		// cout, cin
#include <sys/time.h>	// timeval - and gettimeofday
#include <memory>		// Smart Pointers
#include <unordered_map>
#include <X11/Xlib.h>

#include "WindowInfo.h"
#include "Helpers/WindowHelper.h"
#include "Helpers/XInputListener.h"
#include "Helpers/SystemInfoHelper.h"
#include "BrowserFile.h"
#include "DbQueries.h"
#include <sqlite3.h>

#define CHROME "Google-chrome"
#define FIREFOX "Firefox"

//#define NOT_DEFINED 0
//#define BROWSER 1
//#define BROWSER_CHROME 2

#define OUTPUT_WAY 2 //0 Console, 1 File, 2 SQLite


using namespace std;

const long EVENT_MASK = FocusChangeMask;

//unordered_map<Window, int> windowMap;

Window lastWindow;
long lastSwitch;
unique_ptr<WindowInfo> windowInfo(new WindowInfo());

long getMilliseconds() {
    timeval tv;
    gettimeofday(&tv, 0);
    return (tv.tv_sec * 1000000LL + tv.tv_usec) / 1000LL;
}

#if OUTPUT_WAY == 0

void HandleSaveInitialization() {

}

void SubmitMetric() {
    cout << windowInfo->GetInfo();
}

#elif OUTPUT_WAY == 2

#define SQLITE_PATH "metrics.db"
sqlite3 *db;
//
//static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
//    int i;
//    for (i = 0; i < argc; i++) {
//        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
//    }
//    printf("\n");
//    return 0;
//}

void HandleSaveInitialization() {
    int rc = sqlite3_open(SQLITE_PATH, &db);
    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        exit(-1);
    } else {
        fprintf(stdout, "Opened database successfully\n");
    }

    sqlite3_stmt *stmt;

    rc = sqlite3_prepare(db, DbQueries::CreateMetricTable().c_str(), -1, &stmt, 0);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error:\n");
    } else {
        fprintf(stdout, "Table created successfully\n");
    }
}

void SubmitMetric() {
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare(db, DbQueries::InsertMetric().c_str(), -1, &stmt, 0);

    sqlite3_bind_int64(stmt, 1, windowInfo->FocusedWindow);
    sqlite3_bind_int64(stmt, 2, windowInfo->ClientWindow);
    sqlite3_bind_text(stmt, 3, windowInfo->WindowTitle.c_str(), -1, 0);
    sqlite3_bind_text(stmt, 4, windowInfo->ResourceName.c_str(), -1, 0);
    sqlite3_bind_text(stmt, 5, windowInfo->ResourceClass.c_str(), -1, 0);
    sqlite3_bind_text(stmt, 6, windowInfo->Path.c_str(), -1, 0);
    sqlite3_bind_int(stmt, 7, windowInfo->Pid);
    sqlite3_bind_int64(stmt, 8, windowInfo->ConnectTime);
    sqlite3_bind_int64(stmt, 9, windowInfo->DisconnectTime);

    sqlite3_step(stmt);
    sqlite3_finalize(stmt);


    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error:\n");
    } else {
        //fprintf(stdout, "Insert query complited successfully\n");
    }
}

//void ShowMetric() {
//    char *zErrMsg = 0;
//    int rc = sqlite3_exec(db, DbQueries::SelectAllMetrics().c_str(), callback, 0, &zErrMsg);
//    if (rc != SQLITE_OK) {
//        fprintf(stderr, "SQL error: %s\n", zErrMsg);
//        sqlite3_free(zErrMsg);
//    } else {
//        //fprintf(stdout, "Query completed successfully\n");
//    }
//}

#elif OUTPUT_WAY == 1
#include <sstream>		// For concatenate string
#include <fstream>		// For writing to file system

std::ofstream logFile;

void SubmitMetric() {
    logFile << windowInfo->GetInfo();
    logFile.flush();
}

void HandleSaveInitialization(){
    logFile.open("metrics.txt", ios_base::app);
}
#endif



void LogWindowInfo(Display *display, Window focusedWindow, Bool forced = false) {

    Window window = focusedWindow;
    window = WindowHelper::GetTopWindow(display, window);
    window = WindowHelper::GetClientWindow(display, window);

    if (lastWindow == window && !forced) {
        return;
    }
    lastSwitch = getMilliseconds();
    windowInfo->DisconnectTime = lastSwitch;

    if (windowInfo->ResourceClass == CHROME) {
        BrowserFile::GetChromeUrl(windowInfo->WindowTitle, windowInfo->URL);

    }

    if (windowInfo->ResourceClass == FIREFOX) {
        BrowserFile::GetUrl(windowInfo->WindowTitle, windowInfo->URL);
    }

    SubmitMetric();

    windowInfo.reset(new WindowInfo());

    lastWindow = window;

    XClassHint *cls = WindowHelper::GetWindowClass(display, window);
    string clsName = "";
    string clsClass = "";
    if (cls != NULL) {
        clsName = cls->res_name;
        clsClass = cls->res_class;
        XFree(cls);
    }

    string winName = WindowHelper::GetWindowNameMultiByte(display, window);
    int pid, height, width;
    WindowHelper::GetWindowHeightWidht(display, window, width, height);

    pid = WindowHelper::GetPid(display, lastWindow);
    char *path = WindowHelper::GetPathByPid(pid);

    windowInfo->FocusedWindow = focusedWindow;
    windowInfo->ClientWindow = lastWindow;
    windowInfo->WindowTitle = winName;
    windowInfo->ResourceName = clsName;
    windowInfo->ResourceClass = clsClass;
    windowInfo->Path = path ? path : "";
    windowInfo->Pid = pid;
    windowInfo->ConnectTime = lastSwitch;

    delete[] path;

    if (clsClass == CHROME) {
//        windowMap[window] = BROWSER_CHROME;
//        windowMap[focusedWindow] = BROWSER_CHROME;
        windowInfo->Type = BROWSER_CHROME;
    } else if (clsClass == FIREFOX) {
//        windowMap[window] = BROWSER;
//        windowMap[focusedWindow] = BROWSER;
        windowInfo->Type = BROWSER;
    } else {
//        windowMap[focusedWindow] = NOT_DEFINED;
//        windowMap[window] = NOT_DEFINED;
        windowInfo->Type = NOT_DEFINED;
    }

    WindowHelper::xerror = 0;
}

void HandleFocusOut(Display *display, Window &focusedWindow, int *revert) {
    //Get new Focus
    XGetInputFocus(display, &focusedWindow, revert);

    //Handling window log-> console, sqlite, file
    LogWindowInfo(display, focusedWindow);

    //New Mask for window
    XSelectInput(display, focusedWindow, EVENT_MASK);
}


sqlite3 *dbch;

static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
    int i;
    for (i = 0; i < argc; i++) {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

int measurements_main() {
    setlocale(LC_ALL, "");

    HandleSaveInitialization();
    //ShowMetric();
    SystemInfo systemInfo;

    SystemInfoHelper::GetSystemInfo(systemInfo);
    printf("%s, %s\n", systemInfo.UserName.c_str(), systemInfo.HostName.c_str());

    for (int i = 0; i < systemInfo.NetInterfaces.size(); i++) {
        printf("net%d: %s, %s, %s\n", i, systemInfo.NetInterfaces[i].Name.c_str(),
               systemInfo.NetInterfaces[i].Ip.c_str(), systemInfo.NetInterfaces[i].Mac.c_str());
    }

    Display *display = XOpenDisplay(0);
    Window root = DefaultRootWindow(display);
    Window focusedWindow;
    int revert;

    XSetErrorHandler(WindowHelper::HandleX11Error);

    XGetInputFocus(display, &focusedWindow, &revert);
    unsigned long len;
    Window *windows = WindowHelper::GetAllWindows(display, root, &len);

    //Register events for all exist windows
    for (unsigned long i = 0; i < len; i++) {
//        windowMap[windows[i]] = NOT_DEFINED;
        XSelectInput(display, windows[i], EVENT_MASK);
    }
//    Register Input device events version 1
//    if (XInputListener::SelectXInputEvents(display)) {
//        return -1;
//    }

    //Register Input device events version 2
    if (XInputListener::RegisterEventsXi2(display)) {
        return -1;
    }

    delete[] windows;

    lastSwitch = getMilliseconds();
    while (true) {
        XEvent event;
        XGenericEventCookie *cookie = (XGenericEventCookie *) &event.xcookie;
        XNextEvent(display, &event);
        switch (event.type) {
            case GenericEvent: {
                //cout << cookie->evtype << "\n";
                switch (cookie->evtype) {
                    case XI_KeyPress:
                    case XI_ButtonPress:
                        break;
                    case XI_KeyRelease:
                    case XI_ButtonRelease: {
                        if (windowInfo->Type >= BROWSER) {//(windowMap[lastWindow] >= BROWSER) {
                            string winName = WindowHelper::GetWindowNameMultiByte(display, lastWindow);
                            if (windowInfo->WindowTitle != winName) {
                                LogWindowInfo(display, focusedWindow, true);
                            }
                        }
                        break;
                    }
                    case XI_RawButtonRelease:
                    case XI_RawKeyRelease: {
                        if (windowInfo->Type == BROWSER_CHROME) {//(windowMap[lastWindow] == BROWSER_CHROME) {
                            string winName = WindowHelper::GetWindowNameMultiByte(display, lastWindow);
                            if (windowInfo->WindowTitle != winName) {
                                LogWindowInfo(display, focusedWindow, true);
                            }
                        }
                        break;
                    }
                    case XI_DeviceChanged:
                        //printf("Device changed\n");
                        break;
                    case XI_HierarchyChanged:
                        //printf("HierarchyChanged\n");;
                        break;
                    case XI_PropertyEvent:
                        //printf("PropertyEvent\n");
                        break;
                    default:
                        break;

                }
                break;
            }
            case FocusIn:
                break;
            case FocusOut:
                HandleFocusOut(display, focusedWindow, &revert);
                break;
            default:
                /*               // The older version of events
                               if (event.type == XInputListener::ButtonPressType || event.type == XInputListener::KeyPressType) {
                                   //There is the event of pressed button. For now I skipped because of performance
                               } else if (event.type == XInputListener::ButtonReleaseType ||
                                          event.type == XInputListener::KeyReleaseType) {
                                   cout << lastWindow << "\n";
                                   if (windowMap[lastWindow] == BROWSER) {
                                       //LogWindowInfo(display, focusedWindow, true);
                                       string winName = WindowHelper::GetWindowNameMultiByte(display, lastWindow);
                                       if (windowInfo->WindowTitle != winName) {
                                           LogWindowInfo(display, focusedWindow, true);
                                       }
                                   }
                               }*/
                break;
        }
        XFreeEventData(display, cookie);
    }
    return 0;
}

int mm(){
    printf("123");
}
#pragma clang diagnostic pop
