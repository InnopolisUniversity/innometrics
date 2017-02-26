//
//  Session+CoreDataProperties.swift
//  InnoMetricsCollector
//
//  Created by Denis Zaplatnikov on 11/01/2017.
//  Copyright © 2017 Denis Zaplatnikov. All rights reserved.
//

import Foundation
import CoreData

extension Session {

    @nonobjc public class func fetchRequest() -> NSFetchRequest<Session> {
        return NSFetchRequest<Session>(entityName: "Session");
    }

    @NSManaged public var ipAddress: String?
    @NSManaged public var macAddress: String?
    @NSManaged public var operatingSystem: String?
    @NSManaged public var userLogin: String?
    @NSManaged public var userName: String?
    @NSManaged public var metric: NSSet?

}

// MARK: Generated accessors for metric
extension Session {

    @objc(addMetricObject:)
    @NSManaged public func addToMetric(_ value: Metric)

    @objc(removeMetricObject:)
    @NSManaged public func removeFromMetric(_ value: Metric)

    @objc(addMetric:)
    @NSManaged public func addToMetric(_ values: NSSet)

    @objc(removeMetric:)
    @NSManaged public func removeFromMetric(_ values: NSSet)

}
