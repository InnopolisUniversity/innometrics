//
//  NewMetricsController.swift
//  InnometricsTransfer
//
//  Created by Denis Zaplatnikov on 05/02/2017.
//  Copyright © 2017 Denis Zaplatnikov. All rights reserved.
//

import Cocoa
import SQLite

class NewMetricsController: NSViewController, NSTableViewDataSource, NSTableViewDelegate {
    
    private var metrics: [Metric] = []
    @IBOutlet weak var newMetricsTableView: NSTableView!
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        fetchNewMetricsAndRefreshTable()
    }
    
    public func fetchNewMetricsAndRefreshTable() {
        metrics = []
        do {
            let urls = FileManager.default.urls(for: .applicationSupportDirectory, in: .userDomainMask)
            let appSupportURL = urls[urls.count - 1]
            let dbPath = appSupportURL.appendingPathComponent("com.apple.toolsQA.CocoaApp_CD").appendingPathComponent("InnoMetricsCollector.storedata")
            
            let db = try Connection(dbPath.absoluteString)
            
            let metricsTable = Table("ZMETRIC")
            let metricIdColumn = Expression<Int64>("Z_PK")
            let timestampEndColumn = Expression<Double?>("ZTIMESTAMPEND")
            let timestampStartColumn = Expression<Double>("ZTIMESTAMPSTART")
            let appNameColumn = Expression<String>("ZAPPNAME")
            let bundleIdentifierColumn = Expression<String>("ZBUNDLEIDENTIFIER")
            let bundleURLColumn = Expression<String>("ZBUNDLEURL")
            let durationColumn = Expression<Double?>("ZDURATION")
            let tabNameColumn = Expression<String?>("ZTABNAME")
            let tabUrlColumn = Expression<String?>("ZTABURL")
            let sessionColumn = Expression<Int64>("ZSESSION")
            
            let sessionTable = Table("ZSESSION")
            let sessionIdColumn = Expression<Int64>("Z_PK")
            let ipAddressColumn = Expression<String?>("ZIPADDRESS")
            let macAddressColumn = Expression<String>("ZMACADDRESS")
            let operatingSystemColumn = Expression<String>("ZOPERATINGSYSTEM")
            let userLoginColumn = Expression<String>("ZUSERLOGIN")
            let userNameColumn = Expression<String>("ZUSERNAME")
            
            let timeWhereSql = timestampStartColumn > UserPrefs.getLastDataSendTime()
            
            var appNameWhereSql = appNameColumn.like("%")
            let appNames = UserPrefs.getUserExludedApps()
            
            for appName in appNames {
                appNameWhereSql = appNameWhereSql && !appNameColumn.like("%" + appName + "%")
            }
            
            
            var keywordsWhereSql = appNameColumn.like("%") && bundleIdentifierColumn.like("%")  && bundleURLColumn.like("%") && (tabNameColumn.like("%") || tabNameColumn == nil)  && (tabUrlColumn.like("%") || tabUrlColumn == nil)
            let keywordsValues = UserPrefs.getUserExludedKeywords()
            
            for keyword in keywordsValues {
                keywordsWhereSql = keywordsWhereSql && !appNameColumn.like("%" + keyword + "%") && !bundleIdentifierColumn.like("%" + keyword + "%") && !bundleURLColumn.like("%" + keyword + "%") && (!tabNameColumn.like("%" + keyword + "%") || tabNameColumn == nil) && (!tabUrlColumn.like("%" + keyword + "%") || tabUrlColumn == nil)
            }
            
            for row in try db.prepare(metricsTable.filter(timeWhereSql && appNameWhereSql && keywordsWhereSql)) {
                if row[timestampEndColumn] == nil {
                    continue
                }
                
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
            print(error)
        }
        newMetricsTableView.reloadData()
    }
    
    public func sendMetrics () {
        dateFormatter.dateFormat = "yyyy-MM-dd'T'HH:mm:ssZ"
        
        var activitiesArrayJson: [[String: Any]] = []
        for metric in metrics {
            
            var measurementsArrayJson: [[String: Any]] = []
            let appBundleIdentifierJson: [String: Any] = ["name": "bundle_identifier", "type": "string", "value": metric.bundleIdentifier!]
            let appBundleURLJson: [String: Any] = ["name": "bundle_url", "type": "string", "value": metric.bundleURL!]
            let appDurationJson: [String: Any] = ["name": "activity_duration", "type": "int", "value": metric.duration!]
            if metric.tabName != nil {
                let appTabNameJson: [String: Any] = ["name": "browser_tab_name", "type": "string", "value": metric.tabName!]
                measurementsArrayJson.append(appTabNameJson)
            }
            if metric.tabUrl != nil {
                let appTabUrlJson: [String: Any] = ["name": "browser_tab_url", "type": "string", "value": metric.tabUrl!]
                measurementsArrayJson.append(appTabUrlJson)
            }
            let appTimestampStartJson: [String: Any] = ["name": "activity_end", "type": "string", "value": dateFormatter.string(from: metric.timestampStart as! Date)]
            if metric.timestampEnd != nil {
                let appTimestampEndJson: [String: Any] = ["name": "activity_start", "type": "string", "value":  dateFormatter.string(from: metric.timestampEnd as! Date)]
                
                measurementsArrayJson.append(appTimestampEndJson)
            }
            
            measurementsArrayJson.append(appBundleIdentifierJson)
            measurementsArrayJson.append(appBundleURLJson)
            measurementsArrayJson.append(appDurationJson)
            measurementsArrayJson.append(appTimestampStartJson)
            
            activitiesArrayJson.append(["name": metric.appName! + " application use", "comments": "macOS Environment Collection", "measurements": measurementsArrayJson])
        }
        
        let finalJson: [String: [Any]] = ["activities": activitiesArrayJson]
        do {
            let jsonData = try! JSONSerialization.data(withJSONObject: finalJson, options: .prettyPrinted)
            
            let jsonString = NSString(data: jsonData, encoding: String.Encoding.ascii.rawValue)
            print("jsonData: \(jsonString)")
            // create post request
            var request = URLRequest(url: URL(string: "https://aqueous-escarpment-80312.herokuapp.com/activities/")!)
            request.httpMethod = "POST"
            request.addValue("application/json", forHTTPHeaderField: "Content-Type")
            
            // insert json data to the request
            request.httpBody = jsonData
            
            let task = URLSession.shared.dataTask(with: request) { data, response, error in
                if error != nil
                {
                    print("\(error)")
                    self.dialogOKCancel(question: "Fail", text: "Data haven't been sent.")
                }
         
                // You can print out response object
                print("========\n========\nresponse = \(response)========\n")
                
                DispatchQueue.main.async {
                    self.dialogOKCancel(question: "Success", text: "Data have been sent successfully.")
                    UserPrefs.saveLastDataSendTime(time: Date().timeIntervalSinceReferenceDate)
                    self.fetchNewMetricsAndRefreshTable()
                }
            }
            
            task.resume()
        }
    }
    
    // Table view utilities
    func numberOfRows(in tableView: NSTableView) -> Int {
        return metrics.count
    }
    
    func tableView(_ tableView: NSTableView, objectValueFor tableColumn: NSTableColumn?, row: Int) -> Any? {
        let columnId = tableColumn?.identifier
        
        let dateFormatter = DateFormatter()
        dateFormatter.dateFormat = "yyyy-MM-dd HH:mm:ss Z"
        
        if (columnId == "timestampStart") {
            return dateFormatter.string(from: metrics[row].timestampStart as! Date)
        } else if (columnId == "timestampEnd") {
            if (metrics[row].timestampEnd != nil) {
                return dateFormatter.string(from: metrics[row].timestampEnd as! Date)
            } else {
                return ""
            }
        } else if (columnId == "bundleIdentifier") {
            return metrics[row].bundleIdentifier
        } else if (columnId == "name") {
            return metrics[row].appName
        } else if (columnId == "bundleURL") {
            return metrics[row].bundleURL
        } else if (columnId == "tabUrl") {
            return metrics[row].tabUrl
        } else if (columnId == "tabName") {
            return metrics[row].tabName
        } else if (columnId == "duration"){
            return stringFromTimeInterval(interval: metrics[row].duration!)
        } else {
            return ""
        }
    }
    
    private func stringFromTimeInterval(interval: TimeInterval) -> NSString {
        
        let ti = NSInteger(interval)
        
        let seconds = ti % 60
        let minutes = (ti / 60) % 60
        let hours = (ti / 3600)
        
        return NSString(format: "%0.2d:%0.2d:%0.2d",hours,minutes,seconds)
    }
    
    func dialogOKCancel(question: String, text: String) {
        let myPopup: NSAlert = NSAlert()
        myPopup.messageText = question
        myPopup.informativeText = text
        myPopup.alertStyle = NSAlertStyle.informational
        myPopup.addButton(withTitle: "OK")
        myPopup.runModal()
    }
    
}
