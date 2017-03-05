//
//  ServerPrefs.swift
//  InnometricsTransfer
//
//  Created by Denis Zaplatnikov on 26/02/2017.
//  Copyright © 2017 Denis Zaplatnikov. All rights reserved.
//

import Foundation

public class ServerPrefs {
    
     private static var serverUrlAlias: String = "serverUrl"

    public static func saveServerUrl(serverUrl: String) {
        let defaults = UserDefaults.standard
        defaults.set(serverUrl, forKey: serverUrlAlias)
    }
    
    public static func getServerUrl() -> String {
        let defaults = UserDefaults.standard
        return defaults.string(forKey: serverUrlAlias) ?? "https://aqueous-escarpment-80312.herokuapp.com"
    }
    
}
