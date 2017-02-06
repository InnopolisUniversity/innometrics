//
//  Keyword.swift
//  InnometricsTransfer
//
//  Created by Denis Zaplatnikov on 05/02/2017.
//  Copyright © 2017 Denis Zaplatnikov. All rights reserved.
//

import Foundation

class Keyword: NSObject {
    
    var keyword: String
    
    override init() {
        self.keyword = "default_keyword"
    }
    
    init (keyword: String) {
        self.keyword = keyword
    }
}
