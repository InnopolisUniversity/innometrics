//
//  ViewController.swift
//  InnometricsTransfer
//
//  Created by Denis Zaplatnikov on 03.11.16.
//  Copyright © 2016 Denis Zaplatnikov. All rights reserved.
//

import Cocoa
import SQLite

class ViewController: NSViewController, NSTableViewDataSource, NSTableViewDelegate {

    @IBOutlet weak var metricsTableView: NSTableView!
    private var metrics: [Metric] = []
    @IBOutlet weak var apps: NSTextField!
    @IBOutlet weak var keywords: NSTextField!
    @IBOutlet weak var toDate: NSDatePicker!
    @IBOutlet weak var fromDate: NSDatePicker!
    
    override func viewDidLoad() {
        super.viewDidLoad()

        // Do any additional setup after loading the view.
        fromDate.dateValue = NSDate() as Date
        toDate.dateValue = NSDate() as Date

        fetchetricsAndRefreshTable()
    }

    override var representedObject: Any? {
        didSet {
        // Update the view, if already loaded.
        }
    }
    
    func fetchetricsAndRefreshTable() {
        metrics = []
        do {
            let urls = FileManager.default.urls(for: .applicationSupportDirectory, in: .userDomainMask)
            let appSupportURL = urls[urls.count - 1]
            let dbPath = appSupportURL.appendingPathComponent("com.apple.toolsQA.CocoaApp_CD").appendingPathComponent("InnoMetricsCollector.storedata")
            
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
            
            for row in try db.prepare(metricsTable) {
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
        metricsTableView.reloadData()
        //sendData()
    }
    
    func sendData() {
        let agent: [String: Any] = ["code_name": "macOSAgent", "full_name": "macOS Environment Collector", "version_name": "0.1", "version_code": 1, "secret_key": UUID().uuidString, "install_guid": UUID().uuidString]
        
        var jsonMetricArray: [[String: Any]] = []
        
        for metric in metrics {
            jsonMetricArray.append(["name": metric.appName!, "bundle_identifier": metric.bundleIdentifier!, "duration": stringFromTimeInterval(interval: metric.duration!)] as [String: Any])
        }
        
        let dateFormatter = DateFormatter()
        dateFormatter.dateFormat = "yyyy-MM-dd'T'HH:mm:ssZ"
        let json: [String: Any] = ["name": "Denis", "comments": "", "timestamp": dateFormatter.string(from: Date()), "agent": agent, "measurements": Array(jsonMetricArray.prefix(3))]
        
        //var jsonTest: [[String: String]] = []
        //jsonTest.append(["test_metric": "test"])
        //let json: [String: Any] = ["metrics": jsonTest]
        
        
        do {
            let jsonData = try! JSONSerialization.data(withJSONObject: json, options: .prettyPrinted)
            
            let kek1 = NSString(data: jsonData, encoding: String.Encoding.ascii.rawValue)
            //let kek = try! JSONSerialization.jsonObject(with: jsonData)
            print("jsonData: \(kek1)")
            // create post request
            var request = URLRequest(url: URL(string: "https://sleepy-lake-73434.herokuapp.com/activities/")!)
            request.httpMethod = "POST"
            request.addValue("application/json", forHTTPHeaderField: "Content-Type")
            //request.addValue("text/html", forHTTPHeaderField: "Accept")
            //request.setValue("Connection", forHTTPHeaderField: "close")
            //request.setValue("X-XSS-Protection", forHTTPHeaderField: "0")
            
            // insert json data to the request
            request.httpBody = jsonData
            
            let task = URLSession.shared.dataTask(with: request) { data, response, error in
                /*guard error == nil else {
                    print(error)
                    return
                }
                guard let data = data else {
                    print("Data is empty")
                    return
                }
                
                let json = try? JSONSerialization.jsonObject(with: data, options: [])
                print(response)*/
                if error != nil
                {
                    print("========\n========\nerror=\(error)========\n")
                }
                
                // You can print out response object
                print("========\n========\nresponse = \(response)========\n")
            }
            
            task.resume()
        } catch {
            print("Error info: \(error)")
        }
    }
    
    @IBAction func excludeBtnPressed(_ sender: NSButton) {
        metrics = []
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
        metricsTableView.reloadData()
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
}
