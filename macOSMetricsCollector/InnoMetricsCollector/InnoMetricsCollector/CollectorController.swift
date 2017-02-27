//
//  CollectorController.swift
//  InnoMetricsCollector
//
//  Created by Denis Zaplatnikov on 11/01/2017.
//  Copyright © 2017 Denis Zaplatnikov. All rights reserved.
//

import Cocoa
import ServiceManagement

class CollectorController: NSObject {
    
    @IBOutlet weak var statusMenu: NSMenu!
    
    @IBOutlet weak var currentWorkingApplicationView: CurrentWorkingApplicationController!
    var currentWorkingApplicationMenuItem: NSMenuItem!
    
    @IBOutlet weak var currentWorkingSessionView: CurrentWorkingSessionController!
    
    var currentWorkingSessionMenuItem: NSMenuItem!
    
    @IBOutlet weak var pausePlayBtn: NSButton!
    @IBOutlet weak var pausePlayLabel: NSTextField!
    
    private var currentSession: Session!
    private var currentMetric: Metric?
    private var prevMetric: Metric?
    private var context: NSManagedObjectContext!
    private var isPaused: Bool = false
    
    private var isCollectingBrowserInfo: Bool = false
    private var isCollecting: Bool = true
    
    private let browsersId: [String] = ["org.chromium.Chromium", "com.google.Chrome.canary", "com.google.Chrome", "com.apple.Safari"]
    
    let statusItem = NSStatusBar.system().statusItem(withLength: NSVariableStatusItemLength)
    
    override func awakeFromNib() {
        
        setUpLaunchAtLogin()
        
        let icon = NSImage(named: "statusIcon")
        icon?.isTemplate = true // best for dark mode
        statusItem.image = icon
        statusItem.menu = statusMenu
        
        currentWorkingApplicationMenuItem = statusMenu.item(withTitle: "CurrentWorkingApplication")
        currentWorkingApplicationMenuItem.view = currentWorkingApplicationView
        
        currentWorkingSessionMenuItem = statusMenu.item(withTitle: "CurrentWorkingSession")
        currentWorkingSessionMenuItem.view = currentWorkingSessionView
        
        // set up the NSManagedObjectContext
        let appDelegate = NSApplication.shared().delegate as! AppDelegate
        context = appDelegate.managedObjectContext
        
        let transferAppIdentifier = "com.denzap.InnometricsTransfer"
        let startChangingDbNotificationName = Notification.Name("db_start_changing")
        let endChangingDbNotificationName = Notification.Name("db_end_changing")
        
        DistributedNotificationCenter.default().addObserver(self, selector: #selector(dbChangeBegin), name: startChangingDbNotificationName, object: transferAppIdentifier)
        DistributedNotificationCenter.default().addObserver(self, selector: #selector(dbChangeEnd), name: endChangingDbNotificationName, object: transferAppIdentifier)
        
        NSWorkspace.shared().notificationCenter.addObserver(self, selector: #selector(applicationSwitchTriggered), name: NSNotification.Name.NSWorkspaceDidActivateApplication, object: nil)
        
        startMetricCollection()
    }
    
    func startMetricCollection() {
        isCollecting = true
        handleApplicationSwitch()
    }
    
    func stopMetricCollection() {
        isCollecting = false
        isCollectingBrowserInfo = false
        setEndTimeOfPrevMetric()
    }
    
    func applicationSwitchTriggered(notification: NSNotification) {
        handleApplicationSwitch()
    }
    
    func handleApplicationSwitch() {
        if (!isCollecting) {
            return
        }
        
        let fronmostApp = NSWorkspace.shared().frontmostApplication
        if (fronmostApp == nil) {
            return
        }
        
        let foregroundWindowBundleId = fronmostApp?.bundleIdentifier
        if (foregroundWindowBundleId == "com.denzap.InnoMetricsCollector") {
            return
        }
        
        setEndTimeOfPrevMetric()
        currentWorkingApplicationView.update(application: fronmostApp!)
        
        createAndSaveMetric(fronmostApp: fronmostApp!)
        
        if (browsersId.contains(foregroundWindowBundleId!)) {
            
            // background func
            let backgroundQueue = DispatchQueue(label: "com.app.InnoMetricsCollector", qos: .background, target: nil)
            
            backgroundQueue.async {
                self.isCollectingBrowserInfo = true
                while (self.isCollectingBrowserInfo) {
                    sleep(5)
                    let fronmostApp = NSWorkspace.shared().frontmostApplication
                    if (fronmostApp == nil) {
                        self.isCollectingBrowserInfo = false
                        break
                    }
                    let foregroundWindowBundleId = fronmostApp?.bundleIdentifier
                    
                    if (!self.browsersId.contains(foregroundWindowBundleId!)) {
                        self.isCollectingBrowserInfo = false
                        break
                    }
                    
                    let foregroundWindowTabUrl = BrowserInfoUtils.activeTabURL(bundleIdentifier: foregroundWindowBundleId!)
                    
                    if (self.prevMetric != nil) {
                        if (foregroundWindowTabUrl == self.prevMetric!.tabUrl) {
                            continue
                        }
                    }
                    
                    self.setEndTimeOfPrevMetric()
                    self.createAndSaveMetric(fronmostApp: fronmostApp!)
                }
            }
        } else {
            self.isCollectingBrowserInfo = false
        }

    }
    
    func createAndSaveMetric(fronmostApp: NSRunningApplication) {
        let foregroundWindowBundleId = fronmostApp.bundleIdentifier
        
        let metric = NSEntityDescription.insertNewObject(forEntityName: "Metric", into: context) as! Metric
        metric.bundleIdentifier = foregroundWindowBundleId
        metric.appName = fronmostApp.localizedName
        metric.bundleURL = fronmostApp.executableURL?.absoluteString
        let foregroundWindowLaunchDate =  NSDate()
        
        metric.timestampStart = foregroundWindowLaunchDate
        
        createAndSaveCurrentSession()
        metric.session = self.currentSession
        
        if (self.browsersId.contains(foregroundWindowBundleId!)) {
            let foregroundWindowTabUrl = BrowserInfoUtils.activeTabURL(bundleIdentifier: foregroundWindowBundleId!)
            
            if (foregroundWindowTabUrl != nil) {
                metric.tabUrl = foregroundWindowTabUrl!
            }
            
            let foregroundWindowTabTitle = BrowserInfoUtils.activeTabTitle(bundleIdentifier: foregroundWindowBundleId!)
            
            if (foregroundWindowTabTitle != nil) {
                metric.tabName = foregroundWindowTabTitle!
            }
            prevMetric = currentMetric
            currentMetric = metric//self.metrics.insert(metric, at: 0)
        }
        
        do {
            try self.context.save()
            prevMetric = currentMetric
            currentMetric = metric
            //self.metrics.insert(metric, at: 0)
        } catch {
            print("An error occurred")
        }
    }
    
    func setEndTimeOfPrevMetric() {
        if currentMetric != nil {
            if (currentMetric!.timestampEnd == nil) {
                let metric = currentMetric!
                let endTime = NSDate()
                metric.timestampEnd = endTime
                
                metric.duration = (metric.timestampEnd?.timeIntervalSinceReferenceDate)! - (metric.timestampStart?.timeIntervalSinceReferenceDate)!
                do {
                    try context.save()
                } catch {
                    print("An error occurred")
                }
            }
        }
    }
    
    func createAndSaveCurrentSession() {
        // save current session
        let session = NSEntityDescription.insertNewObject(forEntityName: "Session", into: context) as! Session
        
        session.operatingSystem = "macOS " + ProcessInfo().operatingSystemVersionString
        if #available(OSX 10.12, *) {
            session.userName = ProcessInfo().fullUserName
        } else {
            session.userName = NSUserName()
        }
        
        session.userLogin = NSUserName()
        session.ipAddress = SessionInfoUtils.getIPAddress()
        
        if let intfIterator = SessionInfoUtils.findEthernetInterfaces() {
            if let macAddress = SessionInfoUtils.getMACAddress(intfIterator) {
                let macAddressAsString = macAddress.map( { String(format:"%02x", $0) } )
                    .joined(separator: ":")
                session.macAddress = macAddressAsString
            } else {
                session.macAddress = ""
            }
            
            IOObjectRelease(intfIterator)
        }
        
        let isNewSession = currentSession == nil || session.operatingSystem != currentSession.operatingSystem || session.userName != currentSession.userName || session.userLogin != currentSession.userLogin || session.ipAddress != currentSession.ipAddress || session.macAddress != currentSession.macAddress
        if (isNewSession) {
            do {
                try context.save()
                currentSession = session
                self.currentWorkingSessionView.updateSession(session: currentSession)
            } catch {
                print("An error occurred")
            }
        } else {
            context.delete(session)
            do {
                try context.save()
            } catch {
                print("An error occured")
            }
        }
    }
    
    @IBAction func quitCliked(_ sender: AnyObject) {
        setEndTimeOfPrevMetric()
        NSApplication.shared().terminate(self)
    }
    
    @IBAction func pausePlayClicked(_ sender: AnyObject) {
        if (isPaused) {
            pausePlayBtn.image = #imageLiteral(resourceName: "pauseIcon")
            pausePlayLabel.stringValue = "Pause"
            isPaused = false
            startMetricCollection()
        } else {
            currentWorkingApplicationView.pauseTime()
            pausePlayBtn.image = #imageLiteral(resourceName: "playIcon")
            pausePlayLabel.stringValue = "Start"
            isPaused = true
            stopMetricCollection()
        }
    }
    
    func dbChangeBegin() {
        pausePlayBtn.isEnabled = false
        
        isCollecting = false
        if (!isPaused) {
            currentWorkingApplicationView.pauseTime()
            pausePlayBtn.image = #imageLiteral(resourceName: "playIcon")
            pausePlayLabel.stringValue = "Start"
        }
    }
    
    func dbChangeEnd() {
        context.reset()
        currentSession = nil
        currentMetric = nil
        prevMetric = nil
        
        pausePlayBtn.isEnabled = true
        startMetricCollection()
        if (!isPaused) {
            pausePlayBtn.image = #imageLiteral(resourceName: "pauseIcon")
            pausePlayLabel.stringValue = "Pause"
            isPaused = false
        }
    }
    
    private func setUpLaunchAtLogin() {
        let appBundleIdentifier = "com.denzap.InnoMetricsCollectorHelper"
        if SMLoginItemSetEnabled(appBundleIdentifier as CFString, true) {
            //if autoLaunch {
                NSLog("Successfully add login item.")
            //} else {
                //NSLog("Successfully remove login item.")
            //}
            
        } else {
            NSLog("Failed to add login item.")
        }
    }
    
    deinit {
        NotificationCenter.default.removeObserver(self)
    }
}
