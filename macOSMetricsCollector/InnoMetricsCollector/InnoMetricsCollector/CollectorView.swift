//
//  CollectorView.swift
//  InnoMetricsCollector
//
//  Created by Denis Zaplatnikov on 11/01/2017.
//  Copyright © 2017 Denis Zaplatnikov. All rights reserved.
//

import Cocoa

class CollectorView: NSView, NSTableViewDataSource, NSTableViewDelegate {
    
    @IBOutlet weak var metricsTableView: NSTableView!
    private var metrics: [Metric] = []
    
    func populateMetricsFromData(metricData: [Metric]) {
        metrics = metricData
        reloadTable()
    }
    
    func addNewMetric(metric: Metric) {
        metrics.insert(metric, at: 0)
        reloadTable()
    }
    
    func updateMetric(metric: Metric, at: Int) {
        metrics[at] = metric
        reloadTable()
    }
    
    func reloadTable() {
        DispatchQueue.main.async {
            self.metricsTableView.reloadData()
        }
    }
    
    // Table view utilities
    func numberOfRows(in tableView: NSTableView) -> Int {
        return metrics.count
    }
    
    func tableView(_ tableView: NSTableView, viewFor tableColumn: NSTableColumn?, row: Int) -> NSView? {
        let columnId = tableColumn?.identifier
        let dateFormatter = DateFormatter()
        dateFormatter.dateFormat = "yyyy-MM-dd HH:mm:ss Z"
        
        let cell: NSTableCellView = tableView.make(withIdentifier: tableColumn!.identifier, owner: self) as! NSTableCellView
        
        if (columnId == "id") {
            cell.textField?.stringValue = metrics[row].objectID.uriRepresentation().absoluteString
        } else if (columnId == "timestampStart") {
            cell.textField?.stringValue = dateFormatter.string(from: metrics[row].timestampStart as! Date)
        } else if (columnId == "timestampEnd") {
            if (metrics[row].timestampEnd != nil) {
                cell.textField?.stringValue = dateFormatter.string(from: metrics[row].timestampEnd as! Date)
            } else {
                cell.textField?.stringValue = ""
            }
        } else if (columnId == "bundleIdentifier") {
            cell.textField?.stringValue = metrics[row].bundleIdentifier!
        } else if (columnId == "name") {
            cell.textField?.stringValue = metrics[row].appName!
            let path = NSWorkspace.shared().absolutePathForApplication(withBundleIdentifier: metrics[row].bundleIdentifier!)
            cell.imageView?.image = NSWorkspace.shared().icon(forFile: path!)
        } else if (columnId == "bundleURL") {
            cell.textField?.stringValue = metrics[row].bundleURL!
        } else if (columnId == "tabUrl") {
            if metrics[row].tabUrl != nil {
                cell.textField?.stringValue = metrics[row].tabUrl!
            }
        } else if (columnId == "tabName") {
            cell.textField?.stringValue = metrics[row].tabName!
        } else if (columnId == "duration"){
            cell.textField?.stringValue = stringFromTimeInterval(interval: metrics[row].duration) as String
        }
        
        return cell
    }
        
    private func stringFromTimeInterval(interval: TimeInterval) -> NSString {
        
        let ti = NSInteger(interval)
        
        let seconds = ti % 60
        let minutes = (ti / 60) % 60
        let hours = (ti / 3600)
        
        return NSString(format: "%0.2d:%0.2d:%0.2d",hours,minutes,seconds)
    }
}
