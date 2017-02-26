//
//  MetricsTransfer.swift
//  InnometricsTransfer
//
//  Created by Denis Zaplatnikov on 25/02/2017.
//  Copyright © 2017 Denis Zaplatnikov. All rights reserved.
//

import Foundation

public class MetricsTransfer {

    public static func sendMetrics(token: String, metrics: [Metric], completion: @escaping (_ response: Int) -> Void) {
        let dateFormatter = DateFormatter()
        dateFormatter.dateFormat = "yyyy-MM-dd'T'HH:mm:ssZ"
        
        var activitiesArrayJson: [[String: Any]] = []
        for metric in metrics {
            
            var measurementsArrayJson: [[String: Any]] = []
            let appBundleIdentifierJson: [String: String] = ["name": "bundle_identifier", "type": "string", "value": metric.bundleIdentifier ?? ""]
            let appBundleURLJson: [String: String] = ["name": "bundle_url", "type": "string", "value": metric.bundleURL ?? ""]
            let appDurationJson: [String: String] = ["name": "activity_duration", "type": "double", "value": String(format:"%f", metric.duration)]
            if metric.tabName != nil {
                let appTabNameJson: [String: String] = ["name": "browser_tab_name", "type": "string", "value": metric.tabName ?? ""]
                measurementsArrayJson.append(appTabNameJson)
            }
            if metric.tabUrl != nil {
                let appTabUrlJson: [String: String] = ["name": "browser_tab_url", "type": "string", "value": metric.tabUrl ?? ""]
                measurementsArrayJson.append(appTabUrlJson)
            }
            let appTimestampStartJson: [String: String] = ["name": "activity_end", "type": "string", "value": dateFormatter.string(from: metric.timestampStart as! Date)]
            if metric.timestampEnd != nil {
                let appTimestampEndJson: [String: String] = ["name": "activity_start", "type": "string", "value":  dateFormatter.string(from: metric.timestampEnd as! Date)]
                
                measurementsArrayJson.append(appTimestampEndJson)
            }
            measurementsArrayJson.append(appBundleIdentifierJson)
            measurementsArrayJson.append(appBundleURLJson)
            measurementsArrayJson.append(appDurationJson)
            measurementsArrayJson.append(appTimestampStartJson)
            
            if (metric.session != nil) {
                let appSessionIpAddress: [String: String] = ["name": "session_ip_address", "type": "string", "value": metric.session!.ipAddress ?? ""]
                let appSessionMacAddress: [String: String] = ["name": "session_mac_address", "type": "string", "value": metric.session!.macAddress ?? ""]
                let operatingSystem: [String: String] = ["name": "session_operating_system", "type": "string", "value": metric.session!.operatingSystem ?? ""]
                let userLogin: [String: String] = ["name": "session_user_login", "type": "string", "value": metric.session!.userLogin ?? ""]
                let userName: [String: String] = ["name": "session_user_name", "type": "string", "value": metric.session!.userName ?? ""]
            
                measurementsArrayJson.append(appSessionIpAddress)
                measurementsArrayJson.append(appSessionMacAddress)
                measurementsArrayJson.append(operatingSystem)
                measurementsArrayJson.append(userLogin)
                measurementsArrayJson.append(userName)
            }
            
            activitiesArrayJson.append(["name": metric.appName ?? "undefined" + " application use", "comments": "macOS Environment Collection", "measurements": measurementsArrayJson])
        }
        
        let finalJson: [String: [Any]] = ["activities": activitiesArrayJson]
        do {
            let jsonData = try! JSONSerialization.data(withJSONObject: finalJson, options: .prettyPrinted)
            
            //let jsonString = NSString(data: jsonData, encoding: String.Encoding.ascii.rawValue)
            //print("jsonData: \(jsonString)")
            // create post request
            var request = URLRequest(url: URL(string: "\(ServerPrefs.getServerUrl())/activities/")!)
            request.httpMethod = "POST"
            request.addValue("application/json", forHTTPHeaderField: "Content-Type")
            request.addValue("Token \(token)", forHTTPHeaderField: "Authorization")
            
            // insert json data to the request
            request.httpBody = jsonData
            
            let task = URLSession.shared.dataTask(with: request) { data, response, error in
                if error != nil
                {
                    print("\(error)")
                    completion(-1)
                    return
                }
                
                let responseCode = (response as! HTTPURLResponse).statusCode
                if (responseCode == 201) {
                    completion(1)
                    return
                } else {
                    completion(-1)
                    return
                }
            }
            
            task.resume()
        }
    }
}
