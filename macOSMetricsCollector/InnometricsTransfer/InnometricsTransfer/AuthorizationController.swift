//
//  AuthorizationController.swift
//  InnometricsTransfer
//
//  Created by Denis Zaplatnikov on 25/02/2017.
//  Copyright © 2017 Denis Zaplatnikov. All rights reserved.
//

import Cocoa

class AuthorizationController: NSViewController {

    @IBOutlet weak var loginBtn: NSButton!
    @IBOutlet weak var loginTextField: NSTextField!
    @IBOutlet weak var passwordTextField: NSSecureTextField!
    
    @IBOutlet weak var loaderIndicator: NSProgressIndicator!
    
    override func viewDidAppear() {
        self.view.window?.titleVisibility = .hidden
        self.view.window?.titlebarAppearsTransparent = true
        self.view.window?.isMovableByWindowBackground = true
        self.view.window?.backgroundColor = NSColor.gray
        self.view.window?.styleMask = [(self.view.window?.styleMask)!, NSFullSizeContentViewWindowMask]
    }
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        let attrs = [NSForegroundColorAttributeName : NSColor.white, NSFontAttributeName : NSFont.systemFont(ofSize: 17.0)] as [String : Any]
        let buttonTitleStr = NSMutableAttributedString(string:"Login", attributes:attrs)
        let attributedString = NSMutableAttributedString(string: "")
        attributedString.append(buttonTitleStr)
        loginBtn.setValue(attributedString, forKey: "attributedTitle")
    }
    
    @IBAction func loginBtn_Clicked(_ sender: AnyObject) {
        let login = loginTextField.stringValue
        let password = passwordTextField.stringValue
        if (((login.range(of: "\\s+", options: .regularExpression) != nil) || login.characters.count == 0) && ((password.range(of: "\\s+", options: .regularExpression) != nil) || password.characters.count == 0)) {
            dialogOKCancel(question: "Warning", text: "Enter your login and password")
        } else if (((login.range(of: "\\s+", options: .regularExpression) != nil) || login.characters.count == 0)) {
            dialogOKCancel(question: "Warning", text: "Enter your login")
        } else if (((password.range(of: "\\s+", options: .regularExpression) != nil) || password.characters.count == 0)) {
            dialogOKCancel(question: "Warning", text: "Enter your password")
        } else {
            loaderIndicator.isHidden = false
            loaderIndicator.startAnimation(self)
            disableInputElements()
            
            AuthorizationUtils.authorization(username: login, password: password) { (token) in
                DispatchQueue.main.async {
                    self.enableInputElements()
                    self.loaderIndicator.stopAnimation(self)
                    self.loaderIndicator.isHidden = true
                    if (token == nil) {
                        self.dialogOKCancel(question: "Error", text: "Wrong input authorization data")
                    } else {
                        AuthorizationUtils.saveAuthorizationToken(token: token!)
                        AuthorizationUtils.saveIsAuthorized(isAuthorized: true)
                        let storyboard = NSStoryboard(name: "Main", bundle: nil)
                        let mvc = storyboard.instantiateController(withIdentifier:"MainController") as! MainController
                        self.view.window?.contentViewController = mvc
                    }
                }
            }
        }
    }
    
    func enableInputElements() {
        loginBtn.isEnabled = true
        loginTextField.isEnabled = true
        passwordTextField.isEnabled = true
    }
    
    func disableInputElements() {
        loginBtn.isEnabled = false
        loginTextField.isEnabled = false
        passwordTextField.isEnabled = false
    }
    
    func dialogOKCancel(question: String, text: String) {
        let myPopup: NSAlert = NSAlert()
        myPopup.messageText = question
        myPopup.informativeText = text
        myPopup.alertStyle = NSAlertStyle.informational
        myPopup.addButton(withTitle: "OK")
        myPopup.runModal()
    }
    
    override var representedObject: Any? {
        didSet {
            // Update the view, if already loaded.
        }
    }
}
