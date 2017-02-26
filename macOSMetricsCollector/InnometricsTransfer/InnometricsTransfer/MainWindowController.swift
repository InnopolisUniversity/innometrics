//
//  MainWindowController.swift
//  InnometricsTransfer
//
//  Created by Denis Zaplatnikov on 25/02/2017.
//  Copyright © 2017 Denis Zaplatnikov. All rights reserved.
//

import Cocoa

class MainWindowController: NSWindowController {

    override func windowDidLoad() {
        super.windowDidLoad()
    
        // Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
        let storyboard = NSStoryboard(name: "Main", bundle: nil)
        let mvc = storyboard.instantiateController(withIdentifier:"MainController") as! MainController
        let avc = storyboard.instantiateController(withIdentifier:"AuthorizationController") as! AuthorizationController
        
        if AuthorizationUtils.isAuthorized() {
            self.contentViewController = mvc
        } else {
            let appDelegate = NSApplication.shared().delegate as! AppDelegate
            appDelegate.logOutMenuItem.isEnabled = false
            self.contentViewController = avc
        }
    }
}
