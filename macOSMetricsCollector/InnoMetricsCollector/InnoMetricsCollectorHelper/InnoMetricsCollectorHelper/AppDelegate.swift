//
//  AppDelegate.swift
//  InnoMetricsCollectorHelper
//
//  Created by Denis Zaplatnikov on 23/01/2017.
//  Copyright © 2017 Denis Zaplatnikov. All rights reserved.
//

import Cocoa

@NSApplicationMain
class AppDelegate: NSObject, NSApplicationDelegate {

    func applicationDidFinishLaunching(_ aNotification: Notification) {
        // Insert code here to initialize your application
        let path = NSString(string: (NSString(string: (NSString(string: (NSString(string: Bundle.main.bundlePath).deletingLastPathComponent)).deletingLastPathComponent)).deletingLastPathComponent)).deletingLastPathComponent
        
        NSWorkspace.shared().launchApplication(path)
        NSApplication.shared().terminate(self)
    }

    func applicationWillTerminate(_ aNotification: Notification) {
        // Insert code here to tear down your application
    }


}

