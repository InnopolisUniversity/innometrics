//
//  MainController.swift
//  InnometricsTransfer
//
//  Created by Denis Zaplatnikov on 03.11.16.
//  Copyright © 2016 Denis Zaplatnikov. All rights reserved.
//

import Cocoa
import SQLite

class MainController: NSViewController {

    private var metrics: [Metric] = []
    private var sessions: [Session] = []
    
    @IBOutlet weak var fromDate: NSDatePicker!
    @IBOutlet weak var toDate: NSDatePicker!
    
    @IBOutlet weak var keywordsTableView: NSTableView!
    @IBOutlet var keywordsArrayController: NSArrayController!
    dynamic var keywordsArray: Array<Keyword> = []
    private var prefsKeywordsArray: [String] = []
    
    @IBOutlet weak var appsTableView: NSTableView!
    @IBOutlet var appsArrayController: NSArrayController!
    dynamic var appsArray: Array<Application> = []
    private var prefsAppsArray: [String] = []
    
    private var allMetricsController: AllMetricsController!
    private var newMetricsController: NewMetricsController!
    
    override func viewDidAppear() {
        self.view.window?.titleVisibility = .hidden
        self.view.window?.titlebarAppearsTransparent = true
        self.view.window?.isMovableByWindowBackground = true
        self.view.window?.backgroundColor = NSColor.gray
        self.view.window?.styleMask = [(self.view.window?.styleMask)!, NSFullSizeContentViewWindowMask]
    }
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        // load excluded keywords
        prefsKeywordsArray = UserPrefs.getUserExludedKeywords()
        for keyword in prefsKeywordsArray {
            keywordsArray.append(Keyword(keyword: keyword))
        }
        
        // load excluded apps
        prefsAppsArray = UserPrefs.getUserExludedApps()
        for app in prefsAppsArray {
            appsArray.append(Application(application: app))
        }
        
        
        fromDate.dateValue = NSDate() as Date
        toDate.dateValue = NSDate() as Date
        
        self.keywordsArrayController.addObserver(self, forKeyPath: "arrangedObjects.keyword", options: .new, context: nil)
        self.appsArrayController.addObserver(self, forKeyPath: "arrangedObjects.application", options: .new, context: nil)

    }

    override var representedObject: Any? {
        didSet {
        // Update the view, if already loaded.
        }
    }
    
    @IBAction func refreshMetricsTables(_ sender: AnyObject) {
        allMetricsController.fetchAllMetricsAndRefreshTable()
        newMetricsController.fetchNewMetricsAndRefreshTable()
    }
    
    @IBAction func clearDataBase(_ sender: AnyObject) {
        do {
            let urls = FileManager.default.urls(for: .applicationSupportDirectory, in: .userDomainMask)
            let appSupportURL = urls[urls.count - 1]
            let dbPath = appSupportURL.appendingPathComponent("com.apple.toolsQA.CocoaApp_CD").appendingPathComponent("InnoMetricsCollector.storedata")
            
            let db = try Connection(dbPath.absoluteString)
            
            let metricsTable = Table("ZMETRIC")
            let sessionTable = Table("ZSESSION")
            try db.run(metricsTable.delete())
            try db.run(sessionTable.delete())
            
            newMetricsController.fetchNewMetricsAndRefreshTable()
            allMetricsController.fetchAllMetricsAndRefreshTable()
        } catch {
            print (error)
        }
    }
    
    @IBAction func addKeywordsButtonClicked(_ sender: AnyObject) {
        keywordsArray.append(Keyword())
        prefsKeywordsArray.append(Keyword().keyword)
        UserPrefs.saveUserExludedKeywords(keywords: prefsKeywordsArray)
        DispatchQueue.main.async {
            let rowIndex = self.keywordsArray.count - 1
            self.keywordsTableView.selectRowIndexes(NSIndexSet(index: rowIndex) as IndexSet, byExtendingSelection: false)
            self.keywordsTableView.scrollRowToVisible(rowIndex)
        }
    }
   
    @IBAction func removeKeywordsButtonClicked(_ sender: AnyObject) {
        if let selectedKeyword = keywordsArrayController.selectedObjects.first as? Keyword {
            prefsKeywordsArray.remove(at: keywordsTableView.selectedRow)
            keywordsArrayController.removeObject(selectedKeyword)
            UserPrefs.saveUserExludedKeywords(keywords: prefsKeywordsArray)
        }
    }
    
    @IBAction func addAppsButtonClicked(_ sender: AnyObject) {
        appsArray.append(Application())
        prefsAppsArray.append(Application().application)
        UserPrefs.saveUserExludedApps(apps: prefsAppsArray)
        DispatchQueue.main.async {
            let rowIndex = self.appsArray.count - 1
            self.appsTableView.selectRowIndexes(NSIndexSet(index: rowIndex) as IndexSet, byExtendingSelection: false)
            self.appsTableView.scrollRowToVisible(rowIndex)
        }
    }
    
    @IBAction func removeAppsButtonClicked(_ sender: AnyObject) {
        if let selectedApp = appsArrayController.selectedObjects.first as? Application {
            prefsAppsArray.remove(at: appsTableView.selectedRow)
            appsArrayController.removeObject(selectedApp)
            UserPrefs.saveUserExludedApps(apps: prefsAppsArray)
        }
    }
    
    override func prepare(for segue: NSStoryboardSegue, sender: Any?) {
        if segue.identifier == "MetricsTabSegue" {
            let barViewControllers = segue.destinationController as! NSTabViewController
            newMetricsController = barViewControllers.childViewControllers[0] as! NewMetricsController
            allMetricsController = barViewControllers.childViewControllers[1] as! AllMetricsController
        }
    }
    
    override func observeValue(forKeyPath keyPath: String?, of object: Any?, change: [NSKeyValueChangeKey : Any]?, context: UnsafeMutableRawPointer?) {
        
        if keyPath == "arrangedObjects.keyword" {
            prefsKeywordsArray = []
            for keyword in keywordsArray {
                prefsKeywordsArray.append(keyword.keyword)
            }
            UserPrefs.saveUserExludedKeywords(keywords: prefsKeywordsArray)
        
            newMetricsController.fetchNewMetricsAndRefreshTable()
        } else if keyPath == "arrangedObjects.application" {
            prefsAppsArray = []
            for app in appsArray {
                prefsAppsArray.append(app.application)
            }
            UserPrefs.saveUserExludedApps(apps: prefsAppsArray)
            
            newMetricsController.fetchNewMetricsAndRefreshTable()
        }
    }
    
    @IBAction func sendMetricsBtnClicked(_ sender: AnyObject) {
        newMetricsController.sendMetrics()
    }
    
     func excludeBtnPressed(_ sender: NSButton) {
        /*metrics = []
        do {
            let urls = FileManager.default.urls(for: .applicationSupportDirectory, in: .userDomainMask)
            let appSupportURL = urls[urls.count - 1]
            let dbPath = appSupportURL.appendingPathComponent("com.apple.toolsQA.CocoaApp_CD").appendingPathComponent("InnoMetricsTestApp.sqlite")
            
            let db = try Connection(dbPath.absoluteString)
            
            let metricsTable = Table("ZMETRIC")
            let timestampEndColumn = Expression<Double?>("ZTIMESTAMPEND")
            let timestampStartColumn = Expression<Double>("ZTIMESTAMPSTART")
            let appNameColumn = Expression<String>("ZAPPNAME")
            let bundleIdentifierColumn = Expression<String>("ZBUNDLEIDENTIFIER")
            let bundleURLColumn = Expression<String>("ZBUNDLEURL")
            let durationColumn = Expression<Double?>("ZDURATION")
            let tabNameColumn = Expression<String?>("ZTABNAME")
            let tabUrlColumn = Expression<String?>("ZTABURL")
            //let session: Session?
            //timestampStartColumn > fromDate.dateValue.timeIntervalSinceReferenceDate
            
            let timeWhereSql = timestampEndColumn < fromDate.dateValue.timeIntervalSinceReferenceDate || timestampStartColumn > toDate.dateValue.timeIntervalSinceReferenceDate
            
            var appNameWhereSql = appNameColumn.like("%")
            let appNames = apps.stringValue.components(separatedBy: ", ")
            if (apps.stringValue != "") {
                for appName in appNames {
                    appNameWhereSql = appNameWhereSql && !appNameColumn.like("%" + appName + "%")
                }
            }
            
            var keywordsWhereSql = appNameColumn.like("%") && bundleIdentifierColumn.like("%")  && bundleURLColumn.like("%") && (tabNameColumn.like("%") || tabNameColumn == nil)  && (tabUrlColumn.like("%") || tabUrlColumn == nil)
            let keywordsValues = keywords.stringValue.components(separatedBy: ", ")
            if (keywords.stringValue != "") {
                for keyword in keywordsValues {
                    keywordsWhereSql = keywordsWhereSql && !appNameColumn.like("%" + keyword + "%") && !bundleIdentifierColumn.like("%" + keyword + "%") && !bundleURLColumn.like("%" + keyword + "%") && (!tabNameColumn.like("%" + keyword + "%") || tabNameColumn == nil) && (!tabUrlColumn.like("%" + keyword + "%") || tabUrlColumn == nil)
                }
            }
            
            for row in try db.prepare(metricsTable.filter(timeWhereSql && appNameWhereSql && keywordsWhereSql)) {
                let metric: Metric = Metric()
                metric.appName = row[appNameColumn]
                metric.bundleIdentifier = row[bundleIdentifierColumn]
                metric.bundleURL = row[bundleURLColumn]
                metric.duration = row[durationColumn]
                metric.tabName = row[tabNameColumn]
                metric.tabUrl = row[tabUrlColumn]
                if (row[timestampEndColumn] != nil) {
                    metric.timestampEnd = NSDate(timeIntervalSinceReferenceDate:row[timestampEndColumn]!)
                }
                metric.timestampStart = NSDate(timeIntervalSinceReferenceDate:row[timestampStartColumn])
                metrics.insert(metric, at: 0)
            }
        } catch {
        }
        metricsTableView.reloadData()*/
    }
    
    private func stringFromTimeInterval(interval: TimeInterval) -> NSString {
        
        let ti = NSInteger(interval)
        
        let seconds = ti % 60
        let minutes = (ti / 60) % 60
        let hours = (ti / 3600)
        
        return NSString(format: "%0.2d:%0.2d:%0.2d",hours,minutes,seconds)
    }
}
