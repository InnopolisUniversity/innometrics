//
//  CurrentWorkingApplication.swift
//  InnoMetricsCollector
//
//  Created by Denis Zaplatnikov on 12/01/2017.
//  Copyright © 2017 Denis Zaplatnikov. All rights reserved.
//

import Cocoa

class CurrentWorkingApplicationController: NSView {
    
    @IBOutlet weak var appImage: NSImageView!
    @IBOutlet weak var appName: NSTextField!
    @IBOutlet weak var appTimeUsage: NSTextField!
    private var timer = Timer()
    private var counter = 0.0
    
    func update(application: NSRunningApplication) {
        // do UI updates on the main thread
        DispatchQueue.main.async {
            self.timer.invalidate()
            self.counter = 0
            
            let path = NSWorkspace.shared().absolutePathForApplication(withBundleIdentifier: application.bundleIdentifier!)
            if (path != nil) {
                self.appImage.image = NSWorkspace.shared().icon(forFile: path!)
            }
            
            if (application.localizedName != nil) {
                self.appName.stringValue = application.localizedName!
                self.appName.sizeToFit()
            }
            
            self.timer = Timer.scheduledTimer(timeInterval: 1.0, target: self, selector: #selector(self.updateTime), userInfo: nil, repeats: true)
            
            RunLoop.main.add(self.timer, forMode: .commonModes)
        }
    }
    
    func updateTime() {
        DispatchQueue.main.async {
            self.counter += 1
            self.appTimeUsage.stringValue = self.stringFromTimeInterval(interval: self.counter)
        }
    }
    
    func pauseTime() {
        DispatchQueue.main.async {
            self.timer.invalidate()
        }
    }
    
    private func stringFromTimeInterval(interval: TimeInterval) -> String {
        let ti = NSInteger(interval)
        
        let seconds = ti % 60
        let minutes = (ti / 60) % 60
        let hours = (ti / 3600)
        
        return String(format: "%0.2d:%0.2d:%0.2d",hours,minutes,seconds)
    }
}
