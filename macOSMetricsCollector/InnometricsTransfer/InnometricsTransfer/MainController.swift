//
//  MainController.swift
//  InnometricsTransfer
//
//  Created by Denis Zaplatnikov on 03.11.16.
//  Copyright © 2016 Denis Zaplatnikov. All rights reserved.
//

import Cocoa

class MainController: NSViewController {
    @IBOutlet weak var spinnerView: NSProgressIndicator!
    
    @IBOutlet weak var fromDateCheckBox: NSButton!
    @IBOutlet weak var fromDate: NSDatePicker!
    @IBOutlet weak var toDateCheckBox: NSButton!
    @IBOutlet weak var toDate: NSDatePicker!
    
    @IBOutlet weak var keywordsTableView: NSTableView!
    @IBOutlet var keywordsArrayController: NSArrayController!
    dynamic var keywordsArray: Array<Keyword> = []
    private var prefsKeywordsArray: [String] = []
    @IBOutlet weak var removeKeywordBtn: NSButton!
    @IBOutlet weak var addKeywordBtn: NSButton!
    
    @IBOutlet weak var appsTableView: NSTableView!
    @IBOutlet var appsArrayController: NSArrayController!
    dynamic var appsArray: Array<Application> = []
    private var prefsAppsArray: [String] = []
    @IBOutlet weak var addAppBtn: NSButton!
    @IBOutlet weak var removeAppBtn: NSButton!
    
    @IBOutlet weak var sendMetricsBtn: NSButton!
    @IBOutlet weak var refreshTableBtn: NSButton!
    @IBOutlet weak var clearDBBtn: NSButton!
    
    @IBOutlet weak var lastTableRefreshTextField: NSTextField!
    private var metricsController: MetricsController!
    
    override func viewDidAppear() {
        self.view.window?.titleVisibility = .hidden
        self.view.window?.titlebarAppearsTransparent = true
        self.view.window?.isMovableByWindowBackground = true
        self.view.window?.backgroundColor = NSColor.gray
        self.view.window?.styleMask = [(self.view.window?.styleMask)!, NSFullSizeContentViewWindowMask]
        
        let window = NSApplication.shared().windows[0]
        if let screen = NSScreen.main() {
            window.setFrame(screen.visibleFrame, display: true, animate: true)
        }
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
        
        if (UserPrefs.isNeedFromDateFilter()) {
            fromDateCheckBox.state = NSOnState
        }
        if (UserPrefs.isNeedToDateFilter()) {
            toDateCheckBox.state = NSOnState
        }
        
        fromDate.dateValue = UserPrefs.getExludedFromDate() as Date
        toDate.dateValue = UserPrefs.getExludedToDate() as Date
        
        self.keywordsArrayController.addObserver(self, forKeyPath: "arrangedObjects.keyword", options: .new, context: nil)
        self.appsArrayController.addObserver(self, forKeyPath: "arrangedObjects.application", options: .new, context: nil)

        let dateFormatter = DateFormatter()
        dateFormatter.dateFormat = "HH:mm:ss dd-MM-yyyy"
        lastTableRefreshTextField.stringValue = "(Last update: \(dateFormatter.string(from: Date())))"
    }

    override var representedObject: Any? {
        didSet {
        // Update the view, if already loaded.
        }
    }
    
    @IBAction func refreshMetricsTables(_ sender: AnyObject) {
        metricsController.fetchNewMetricsAndRefreshTable()
        let dateFormatter = DateFormatter()
        dateFormatter.dateFormat = "HH:mm:ss dd-MM-yyyy"
        lastTableRefreshTextField.stringValue = "(Last update: \(dateFormatter.string(from: Date())))"
    }
    
    @IBAction func clearDataBase(_ sender: AnyObject) {
        
        let alert: NSAlert = NSAlert()
        alert.messageText = "Would you like to delete metrics data and clear the local database?"
        alert.informativeText = "This action cannot be undone."
        alert.alertStyle = NSAlertStyle.critical
        alert.addButton(withTitle: "Delete")
        alert.addButton(withTitle: "Cancel")
        
        let answer = alert.runModal()
        if answer == NSAlertFirstButtonReturn {
            clearDatabase()
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
            if (keywordsTableView.selectedRow >= 0) {
                prefsKeywordsArray.remove(at: keywordsTableView.selectedRow)
                keywordsArrayController.removeObject(selectedKeyword)
                UserPrefs.saveUserExludedKeywords(keywords: prefsKeywordsArray)
            }
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
            if (appsTableView.selectedRow >= 0) {
                prefsAppsArray.remove(at: appsTableView.selectedRow)
                appsArrayController.removeObject(selectedApp)
                UserPrefs.saveUserExludedApps(apps: prefsAppsArray)
            }
        }
    }
    
    override func prepare(for segue: NSStoryboardSegue, sender: Any?) {
        if segue.identifier == "MetricsController" {
            metricsController = segue.destinationController as! MetricsController
        }
    }
    
    override func observeValue(forKeyPath keyPath: String?, of object: Any?, change: [NSKeyValueChangeKey : Any]?, context: UnsafeMutableRawPointer?) {
        
        if keyPath == "arrangedObjects.keyword" {
            prefsKeywordsArray = []
            for keyword in keywordsArray {
                prefsKeywordsArray.append(keyword.keyword)
            }
            UserPrefs.saveUserExludedKeywords(keywords: prefsKeywordsArray)
        
            refreshMetricsTables(self)
        } else if keyPath == "arrangedObjects.application" {
            prefsAppsArray = []
            for app in appsArray {
                prefsAppsArray.append(app.application)
            }
            UserPrefs.saveUserExludedApps(apps: prefsAppsArray)
            
            refreshMetricsTables(self)
        }
    }
    
    @IBAction func fromDateFilterChanged(_ sender: AnyObject) {
        UserPrefs.saveExludedFromDate(date: fromDate.dateValue as NSDate)
        if (fromDateCheckBox.state == NSOnState) {
            refreshMetricsTables(self)
        }
    }
    
    @IBAction func toDateFilterChanged(_ sender: AnyObject) {
        UserPrefs.saveExludedToDate(date: toDate.dateValue as NSDate)
        if (toDateCheckBox.state == NSOnState) {
            refreshMetricsTables(self)
        }
    }
    
    @IBAction func sendMetricsBtnClicked(_ sender: AnyObject) {
        if metricsController.metrics.count == 0 {
            dialogOKCancel(question: "Warning", text: "There are no metrics data to send.")
            return
        }
        
        let alert: NSAlert = NSAlert()
        alert.messageText = "Would you like to send metrics to the remote server?"
        alert.informativeText = "All metrics that you see on the screen would be send to the remote server."
        alert.alertStyle = NSAlertStyle.warning
        alert.addButton(withTitle: "Send")
        alert.addButton(withTitle: "Cancel")
        
        let answer = alert.runModal()
        if answer == NSAlertFirstButtonReturn {
            disableAllElements()
            spinnerView.isHidden = false
            spinnerView.startAnimation(self)
        
            metricsController.sendMetrics() { (response) in
                DispatchQueue.main.async {
                    self.enableAllElements()
                    self.spinnerView.stopAnimation(self)
                    self.spinnerView.isHidden = true
                    if (response == 1) {
                        self.clearDatabase()
                        self.dialogOKCancel(question: "Success", text: "Data have been sent successfully.")
                    } else {
                        self.dialogOKCancel(question: "Error", text: "Something went wrong during sending.")
                    }
                }
            }
        }
    }
    
    @IBAction func fromDateFilterChecked(_ sender: AnyObject) {
        UserPrefs.saveNeedFromDateFilter(isNeeded: fromDateCheckBox.state == NSOnState)
        refreshMetricsTables(self)
    }
    
    @IBAction func toDateFilterChecked(_ sender: AnyObject) {
        UserPrefs.saveNeedToDateFilter(isNeeded: toDateCheckBox.state == NSOnState)
        refreshMetricsTables(self)
    }
    
    func clearDatabase() {
        let startChangingDbNotificationName = Notification.Name("db_start_changing")
        let endChangingDbNotificationName = Notification.Name("db_end_changing")
        DistributedNotificationCenter.default().postNotificationName(startChangingDbNotificationName, object: Bundle.main.bundleIdentifier, deliverImmediately: true)
        do {
            let appDelegate = NSApplication.shared().delegate as! AppDelegate
            let context = appDelegate.managedObjectContext
            
            let metricsFetch: NSFetchRequest<Metric> = Metric.fetchRequest()
            metricsFetch.includesPropertyValues = false
            let metricsToDelete = try context.fetch(metricsFetch as! NSFetchRequest<NSFetchRequestResult>) as! [NSManagedObject]
            
            for metric in metricsToDelete {
                context.delete(metric)
            }
            
            let sessionsFetch: NSFetchRequest<Session> = Session.fetchRequest()
            sessionsFetch.includesPropertyValues = false
            let sessionsToDelete = try context.fetch(sessionsFetch as! NSFetchRequest<NSFetchRequestResult>) as! [NSManagedObject]
            
            for session in sessionsToDelete {
                context.delete(session)
            }
            
            // Save Changes
            try context.save()
            
            refreshMetricsTables(self)
        } catch {
            print (error)
        }
        DistributedNotificationCenter.default().postNotificationName(endChangingDbNotificationName, object: Bundle.main.bundleIdentifier, deliverImmediately: true)
    }
    
    func disableAllElements() {
        fromDateCheckBox.isEnabled = false
        fromDate.isEnabled = false
        toDateCheckBox.isEnabled = false
        toDate.isEnabled = false
        keywordsTableView.isEnabled = false
        removeKeywordBtn.isEnabled = false
        addKeywordBtn.isEnabled = false
        appsTableView.isEnabled = false
        addAppBtn.isEnabled = false
        removeAppBtn.isEnabled = false
        sendMetricsBtn.isEnabled = false
        refreshTableBtn.isEnabled = false
        clearDBBtn.isEnabled = false
        metricsController.newMetricsTableView.isEnabled = false
    }
    
    func enableAllElements() {
        fromDateCheckBox.isEnabled = true
        fromDate.isEnabled = true
        toDateCheckBox.isEnabled = true
        toDate.isEnabled = true
        keywordsTableView.isEnabled = true
        removeKeywordBtn.isEnabled = true
        addKeywordBtn.isEnabled = true
        appsTableView.isEnabled = true
        addAppBtn.isEnabled = true
        removeAppBtn.isEnabled = true
        sendMetricsBtn.isEnabled = true
        refreshTableBtn.isEnabled = true
        clearDBBtn.isEnabled = true
        metricsController.newMetricsTableView.isEnabled = true
    }
    
    func dialogOKCancel(question: String, text: String) {
        let myPopup: NSAlert = NSAlert()
        myPopup.messageText = question
        myPopup.informativeText = text
        myPopup.alertStyle = NSAlertStyle.informational
        myPopup.addButton(withTitle: "OK")
        myPopup.runModal()
    }
    
    private func stringFromTimeInterval(interval: TimeInterval) -> NSString {
        
        let ti = NSInteger(interval)
        
        let seconds = ti % 60
        let minutes = (ti / 60) % 60
        let hours = (ti / 3600)
        
        return NSString(format: "%0.2d:%0.2d:%0.2d",hours,minutes,seconds)
    }
}
