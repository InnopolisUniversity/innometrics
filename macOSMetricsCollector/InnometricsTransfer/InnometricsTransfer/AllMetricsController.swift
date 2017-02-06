//
//  AllMetricsController.swift
//  InnometricsTransfer
//
//  Created by Denis Zaplatnikov on 05/02/2017.
//  Copyright © 2017 Denis Zaplatnikov. All rights reserved.
//

import Cocoa
import SQLite

class AllMetricsController: NSViewController, NSTableViewDataSource, NSTableViewDelegate {
    @IBOutlet weak var allMetricsTableView: NSTableView!
    
    private var metrics: [Metric] = []
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        fetchAllMetricsAndRefreshTable()
    }

    public func fetchAllMetricsAndRefreshTable() {
        if allMetricsTableView == nil {
            return
        }
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
            
            for row in try db.prepare(metricsTable) {
                if row[timestampEndColumn] == nil {
                    continue
                }
                
                let metric: Metric = Metric()
                metric.id = row[metricIdColumn]
                metric.appName = row[appNameColumn]
                metric.bundleIdentifier = row[bundleIdentifierColumn]
                metric.bundleURL = row[bundleURLColumn]
                metric.duration = row[durationColumn]
                metric.tabName = row[tabNameColumn]
                metric.tabUrl = row[tabUrlColumn]
                if row[timestampEndColumn] != nil {
                    metric.timestampEnd = NSDate(timeIntervalSinceReferenceDate:row[timestampEndColumn]!)
                }
                metric.timestampStart = NSDate(timeIntervalSinceReferenceDate:row[timestampStartColumn])
                metrics.insert(metric, at: 0)
            }
        } catch {
        }
        allMetricsTableView.reloadData()
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
