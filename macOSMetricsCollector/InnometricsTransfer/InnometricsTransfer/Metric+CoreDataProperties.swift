//
//  Metric+CoreDataProperties.swift
//  InnoMetricsCollector
//
//  Created by Denis Zaplatnikov on 11/01/2017.
//  Copyright © 2017 Denis Zaplatnikov. All rights reserved.
//

import Foundation
import CoreData

extension Metric {

    @nonobjc public class func fetchRequest() -> NSFetchRequest<Metric> {
        return NSFetchRequest<Metric>(entityName: "Metric");
    }

    @NSManaged public var appName: String?
    @NSManaged public var bundleIdentifier: String?
    @NSManaged public var bundleURL: String?
    @NSManaged public var duration: Double
    @NSManaged public var tabName: String?
    @NSManaged public var tabUrl: String?
    @NSManaged public var timestampEnd: NSDate?
    @NSManaged public var timestampStart: NSDate?
    @NSManaged public var session: Session?

}
