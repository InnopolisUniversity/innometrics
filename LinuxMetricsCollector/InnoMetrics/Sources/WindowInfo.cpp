/*
 * WindowInfo.cpp
 *
 *  Created on: Oct 17, 2016
 *      Author: alon
 */

#include "WindowInfo.h"

std::string WindowInfo::GetInfo() {
    if (Type >= BROWSER && Type <= LAST_BROWSER)
        return std::to_string(FocusedWindow) + ";" +
               std::to_string(ClientWindow) + ";" +
               WindowTitle + ";" + ResourceName + ";" + ResourceClass + ";" +
               std::to_string(Pid) + ";" + Path + ";" +
               std::to_string(ConnectTime) + ";" + std::to_string(DisconnectTime) + ";"
               + std::to_string(DisconnectTime - ConnectTime) + ";" + URL + ";\n";
    //else
    return std::to_string(FocusedWindow) + ";" +
           std::to_string(ClientWindow) + ";" +
           WindowTitle + ";" + ResourceName + ";" + ResourceClass + ";" +
           std::to_string(Pid) + ";" + Path + ";" +
           std::to_string(ConnectTime) + ";" + std::to_string(DisconnectTime) + ";"
           + std::to_string(DisconnectTime - ConnectTime) + ";\n";
}

WindowInfo::WindowInfo() {
}

WindowInfo::~WindowInfo() {
}