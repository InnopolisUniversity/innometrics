//
//  UserPrefs.swift
//  InnometricsTransfer
//
//  Created by Denis Zaplatnikov on 05/02/2017.
//  Copyright © 2017 Denis Zaplatnikov. All rights reserved.
//

import Cocoa

class UserPrefs {
    private static var keywordsAlias: String = "UserExludedKeywords"
    private static var appsAlias: String = "UserExludedApps"
    private static var lastDataSendAlias: String = "LastDataSend"
    
    public static func saveUserExludedKeywords(keywords: [String]) {
        let defaults = UserDefaults.standard
        defaults.set(keywords, forKey: keywordsAlias)
    }
    
    public static func getUserExludedKeywords() -> [String] {
        let defaults = UserDefaults.standard
        return defaults.array(forKey: keywordsAlias) as? [String] ?? [String]()
    }
    
    public static func saveLastDataSendTime(time: Double) {
        let defaults = UserDefaults.standard
        defaults.set(time, forKey: lastDataSendAlias)
    }
    
    public static func getLastDataSendTime() -> Double {
        let defaults = UserDefaults.standard
        return defaults.double(forKey: lastDataSendAlias) as Double
    }
    
    public static func saveUserExludedApps(apps: [String]) {
        let defaults = UserDefaults.standard
        defaults.set(apps, forKey: appsAlias)
    }
    
    public static func getUserExludedApps() -> [String] {
        let defaults = UserDefaults.standard
        return defaults.array(forKey: appsAlias) as? [String] ?? [String]()
    }

}
