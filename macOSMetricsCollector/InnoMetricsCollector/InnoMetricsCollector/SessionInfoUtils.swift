//
//  SessionInfoUtils.swift
//  InnoMetricsCollector
//
//  Created by Denis Zaplatnikov on 11/01/2017.
//  Copyright © 2017 Denis Zaplatnikov. All rights reserved.
//

import Foundation

class SessionInfoUtils {
    
    // Get IP-address utils
    static func getIPAddress() -> String {
        var addresses = [String]()
        
        // Get list of all interfaces on the local machine:
        var ifaddr : UnsafeMutablePointer<ifaddrs>?
        guard getifaddrs(&ifaddr) == 0 else { return "-" }
        guard let firstAddr = ifaddr else { return "-" }
        
        // For each interface ...
        for ptr in sequence(first: firstAddr, next: { $0.pointee.ifa_next }) {
            let flags = Int32(ptr.pointee.ifa_flags)
            var addr = ptr.pointee.ifa_addr.pointee
            
            // Check for running IPv4, IPv6 interfaces. Skip the loopback interface.
            if (flags & (IFF_UP|IFF_RUNNING|IFF_LOOPBACK)) == (IFF_UP|IFF_RUNNING) {
                if addr.sa_family == UInt8(AF_INET) || addr.sa_family == UInt8(AF_INET6) {
                    let name = String(cString: ptr.pointee.ifa_name)
                    if  name == "en0" {
                        // Convert interface address to a human readable string:
                        var hostname = [CChar](repeating: 0, count: Int(NI_MAXHOST))
                        if (getnameinfo(&addr, socklen_t(addr.sa_len), &hostname, socklen_t(hostname.count),
                                        nil, socklen_t(0), NI_NUMERICHOST) == 0) {
                            let address = String(cString: hostname)
                            addresses.append(address)
                        }
                    }
                }
            }
        }
        
        freeifaddrs(ifaddr)
        
        for address in addresses {
            if address.contains(".") {
                return address
            }
        }
        return "-"
    }
    

    // Get MAC Address Utilities
    static func findEthernetInterfaces() -> io_iterator_t? {
        
        let matchingDict = IOServiceMatching("IOEthernetInterface") as NSMutableDictionary
        matchingDict["IOPropertyMatch"] = [ "IOPrimaryInterface" : true]
        
        var matchingServices : io_iterator_t = 0
        if IOServiceGetMatchingServices(kIOMasterPortDefault, matchingDict, &matchingServices) != KERN_SUCCESS {
            return nil
        }
        
        return matchingServices
    }
    
    static func getMACAddress(_ intfIterator : io_iterator_t) -> [UInt8]? {
        
        var macAddress : [UInt8]?
        
        var intfService = IOIteratorNext(intfIterator)
        while intfService != 0 {
            
            var controllerService : io_object_t = 0
            if IORegistryEntryGetParentEntry(intfService, "IOService", &controllerService) == KERN_SUCCESS {
                
                let dataUM = IORegistryEntryCreateCFProperty(controllerService, "IOMACAddress" as CFString, kCFAllocatorDefault, 0)
                if let data = dataUM?.takeRetainedValue() as? NSData {
                    macAddress = [0, 0, 0, 0, 0, 0]
                    data.getBytes(&macAddress!, length: macAddress!.count)
                }
                IOObjectRelease(controllerService)
            }
            
            IOObjectRelease(intfService)
            intfService = IOIteratorNext(intfIterator)
        }
        
        return macAddress
    }

}
