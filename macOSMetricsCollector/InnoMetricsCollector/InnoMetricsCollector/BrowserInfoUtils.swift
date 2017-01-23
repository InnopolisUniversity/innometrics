//
//  BrowserInfoUtils.swift
//  InnoMetricsCollector
//
//  Created by Denis Zaplatnikov on 18/01/2017.
//  Copyright © 2017 Denis Zaplatnikov. All rights reserved.
//

import Foundation

class BrowserInfoUtils {
    
    // Get browser tab url
    static func activeTabURL(bundleIdentifier: String) -> String? {
        var code: String?
        switch(bundleIdentifier){
        case "org.chromium.Chromium":
            code = "tell application \"Chromium\" to return URL of active tab of front window"
        case "com.google.Chrome.canary":
            code =  "tell application \"Google Chrome Canary\" to return URL of active tab of front window"
        case "com.google.Chrome":
            code =  "tell application \"Google Chrome\" to return URL of active tab of front window"
        case "com.apple.Safari":
            code = "tell application \"Safari\" to return URL of front document"
        default:
            code = nil
        }
        if (code == nil) {
            return nil;
        }
        return appleScripExec(code: code!)
    }
    
    // Get browser tab name
    static func activeTabTitle(bundleIdentifier: String) -> String? {
        var code: String?
        switch(bundleIdentifier){
        case "org.chromium.Chromium":
            code =  "tell application \"Chromium\" to return title of active tab of front window"
        case "com.google.Chrome.canary":
            code =  "tell application \"Google Chrome Canary\" to return title of active tab of front window"
        case "com.google.Chrome":
            code =  "tell application \"Google Chrome\" to return title of active tab of front window"
        case "com.apple.Safari":
            code =  "tell application \"Safari\" to return name of front document"
        default:
            code =  nil
        }
        if (code == nil) {
            return nil;
        }
        return appleScripExec(code: code!)
    }
    
    private static func appleScripExec(code: String) -> String? {
        var errorInfo: NSDictionary?
        let script = NSAppleScript(source: code)
        let scriptOutput = script?.executeAndReturnError(&errorInfo)
        if ((errorInfo) != nil) {
            print(errorInfo)
            return nil;
        } else {
            return scriptOutput?.stringValue
        }
    }
}
