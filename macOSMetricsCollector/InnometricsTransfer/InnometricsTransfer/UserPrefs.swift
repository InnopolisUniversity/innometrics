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
    
    private static var fromDateAlias: String = "FromDate"
    private static var toDateAlias: String = "ToDate"
    
    private static var isNeededFromDateAlias: String = "isNeededFromDate"
    private static var isNeededToDateAlias: String = "isNeededToDate"

    
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
    
    public static func saveExludedFromDate(date: NSDate) {
        let defaults = UserDefaults.standard
        defaults.set(date, forKey: fromDateAlias)
    }

    public static func getExludedFromDate() -> NSDate {
        let defaults = UserDefaults.standard
        return defaults.object(forKey: fromDateAlias) as? NSDate ?? NSDate()
    }
    
    public static func saveExludedToDate(date: NSDate) {
        let defaults = UserDefaults.standard
        defaults.set(date, forKey: toDateAlias)
    }
    
    public static func getExludedToDate() -> NSDate {
        let defaults = UserDefaults.standard
        return defaults.object(forKey: toDateAlias) as? NSDate ?? NSDate.init(timeIntervalSince1970: 0)
    }
    
    public static func saveNeedFromDateFilter(isNeeded: Bool) {
        let defaults = UserDefaults.standard
        defaults.set(isNeeded, forKey: isNeededFromDateAlias)
    }
    
    public static func isNeedFromDateFilter() -> Bool {
        let defaults = UserDefaults.standard
        return defaults.bool(forKey: isNeededFromDateAlias)
    }
    
    public static func saveNeedToDateFilter(isNeeded: Bool) {
        let defaults = UserDefaults.standard
        defaults.set(isNeeded, forKey: isNeededToDateAlias)
    }
    
    public static func isNeedToDateFilter() -> Bool {
        let defaults = UserDefaults.standard
        return defaults.bool(forKey: isNeededToDateAlias)
    }
    
    
}
