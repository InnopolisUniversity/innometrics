//
//  AuthorizationUtils.swift
//  InnometricsTransfer
//
//  Created by Denis Zaplatnikov on 25/02/2017.
//  Copyright © 2017 Denis Zaplatnikov. All rights reserved.
//

import Foundation

public class AuthorizationUtils {
    private static var isAuthorizedAlias: String = "isAuthorized"
    private static var authorizationTokenAlias: String = "authorizationToken"

    public static func authorization(username: String, password: String, completion: @escaping (_ token: String?) -> Void) {
        let authorizationJson: [String: String] = ["username": username, "password": password]
        do {
            let jsonData = try! JSONSerialization.data(withJSONObject: authorizationJson, options: .prettyPrinted)
            
            // create post request
            var request = URLRequest(url: URL(string: "\(ServerPrefs.getServerUrl())/api-token-auth/")!)
            request.addValue("application/json", forHTTPHeaderField: "Content-Type")
            request.httpMethod = "POST"
            
            // insert json data to the request
            request.httpBody = jsonData
            
            let task = URLSession.shared.dataTask(with: request) { data, response, error in
                if error != nil
                {
                    print("\(error)")
                    completion(nil)
                    return
                }
                
                let responseCode = (response as! HTTPURLResponse).statusCode
                
                if (responseCode == 200) {
                    if data != nil {
                        if let responseJson = try? JSONSerialization.jsonObject(with: data!) as! [String: Any] {
                            if let token = responseJson["token"] as? String {
                                completion(token)
                                return
                            }
                        }
                    }
                }
                
                completion(nil)
                return
            }
            
            task.resume()
        }
    }
    
    public static func saveIsAuthorized(isAuthorized: Bool) {
        let defaults = UserDefaults.standard
        defaults.set(isAuthorized, forKey: isAuthorizedAlias)
    }
    
    public static func isAuthorized() -> Bool {
        let defaults = UserDefaults.standard
        return defaults.bool(forKey: isAuthorizedAlias)
    }
    
    public static func saveAuthorizationToken(token: String?) {
        let defaults = UserDefaults.standard
        defaults.set(token, forKey: authorizationTokenAlias)
    }
    
    public static func getAuthorizationToken() -> String? {
        let defaults = UserDefaults.standard
        return defaults.string(forKey: authorizationTokenAlias)
    }
}
