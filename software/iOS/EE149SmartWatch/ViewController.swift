//
//  ViewController.swift
//  EE149SmartWatch
//
//  Created by Michael on 10/24/18.
//  Copyright © 2018 Michael. All rights reserved.
//

import UIKit
import CoreBluetooth

class ViewController: UICollectionViewController, UICollectionViewDelegateFlowLayout, CBCentralManagerDelegate, CBPeripheralDelegate {
    private let cellId = "cellId"
    private var manager : CBCentralManager!
    
    private lazy var dataForGraphCells : [[CGPoint]?] = {
        var array = [[CGPoint]?]()
        for _ in names {
            array.append(nil)
        }
        return array
    }()
    
    enum ServiceID: String {
        case TIMER    = "F3641400-00B0-4240-BA50-05CA45BF8ABC"
        case STEPS    = "C4851500-EAF6-4540-9CFA-903DC7037320"
        case TEMP     = "05051600-484D-5987-969E-B6D3E4E7FDFE"
        case PRESSURE = "00041600-3542-5A73-86A4-B0C1C4CACFE2"
        case HUMIDITY = "181A1600-4346-5E75-798D-A0AEB8B8DFEB"
        case TIME     = "1A1A1600-4E58-5C7F-92A9-AFB7B7BFDADA"
        case RANDOM   = "010E1600-5561-7287-8889-A3ADBAC5E1FD"
    }

    
    private let serviceIds = [
        ServiceID.STEPS,
        ServiceID.TEMP,
        ServiceID.PRESSURE,
        ServiceID.HUMIDITY,
        ServiceID.RANDOM,
        ServiceID.TIMER
    ]
    private let names = [ServiceID.STEPS: "Footsteps",
                         ServiceID.TEMP: "Temperature",
                         ServiceID.PRESSURE: "Pressure",
                         ServiceID.HUMIDITY: "Humidity",
                         ServiceID.RANDOM: "Random",
                         ServiceID.TIMER: "Timer"]
    
    var services = [ServiceID:CBService]()
    
//    var history : [ServiceID:[UInt32]] = [
//        ServiceID.TIMER: [UInt32](),
//        ServiceID.STEPS: [UInt32](),
//        ServiceID.TEMP: [UInt32](),
//        ServiceID.PRESSURE: [UInt32](),
//        ServiceID.HUMIDITY: [UInt32]()
//    ]
    
    var buckler : CBPeripheral! {
        didSet {
            manager.connect(buckler, options: nil)
            buckler.delegate = self
        }
    }
    
    var timer = Timer()
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        collectionView?.backgroundColor = UIColor.white
        
        collectionView?.register(GraphCell.self, forCellWithReuseIdentifier: CellId.GraphCell.rawValue)
        collectionView?.register(TimerCell.self, forCellWithReuseIdentifier: CellId.TimerCell.rawValue)
        collectionView?.register(DebuggingCell.self, forCellWithReuseIdentifier: CellId.DebugCell.rawValue)
        collectionView?.register(BasicCell.self, forCellWithReuseIdentifier: CellId.BasicCell.rawValue)
        
        manager = CBCentralManager.init(delegate: self, queue: nil)
        
        // Add to Navigaition Bar
        
        self.title = "Connecting…"
    }

    override func collectionView(_ collectionView: UICollectionView, cellForItemAt indexPath: IndexPath) -> UICollectionViewCell {
        let serviceId = serviceIds[indexPath.item]
        if serviceId == ServiceID.TIMER {
            let cell = collectionView.dequeueReusableCell(withReuseIdentifier: CellId.TimerCell.rawValue, for: indexPath) as! TimerCell
            cell.collectionView = self
            return cell
        }
        
        let cell = collectionView.dequeueReusableCell(withReuseIdentifier: CellId.BasicCell.rawValue, for: indexPath) as! BasicCell
        cell.parameter = names[serviceId] ?? "Fuck"
        cell.viewController = self
        cell.serviceId = serviceId
        cell.notificationOn = true
    
        return cell
    }
    
    override func collectionView(_ collectionView: UICollectionView, numberOfItemsInSection section: Int) -> Int {
        return serviceIds.count
    }
    
    func collectionView(_ collectionView: UICollectionView, layout collectionViewLayout: UICollectionViewLayout, sizeForItemAt indexPath: IndexPath) -> CGSize {
        return CGSize(width: view.frame.width, height: 200)
    }
    
    @objc
    func printToDebug(str: String) {
        let cells = self.collectionView?.visibleCells
        guard let cellsThatExist = cells else { return }
        for cell in cellsThatExist {
            if let debugCell = cell as? DebuggingCell {
                debugCell.textView.text += str
                return
            }
        }
    }
    
    func printValue(str: String, forCell serviceId: ServiceID) {
        let cells = self.collectionView.visibleCells
        for cell in cells {
            if let basicCell = cell as? BasicCell {
                if basicCell.serviceId == serviceId {
                    basicCell.valueTextView.text = str
//                    basicCell.setupViews()
                    return
                }
            }
        }
    }
    
    @objc
    func sendUpdatedTime() {
        let characteristicOpt = services[ServiceID.TIME]?.characteristics?[0]
        if let characteristic = characteristicOpt {
            let date = Date()
            let calendar = Calendar.current
            let components = calendar.dateComponents([.hour, .minute, .second], from: date)
            var data = Data(capacity: 3)
            if let hour = components.hour, let minute = components.minute, let second = components.second {
                data.append(contentsOf: [UInt8(min(second, 59)), UInt8(min(minute, 59)), UInt8(min(hour, 24))])
                buckler.writeValue(data, for: characteristic, type: .withResponse)
            }
            Timer.scheduledTimer(timeInterval: 60, target: self, selector: #selector(ViewController.sendUpdatedTime), userInfo: nil, repeats: false)
        }
    }
    
    /********* CBManagerDelegate **************/
    
    func centralManagerDidUpdateState(_ central: CBCentralManager) {
        print("state: \(central.state)")
        if central.state == .poweredOn {
            central.scanForPeripherals(withServices: nil, options: nil)
        }
    }
    
    func centralManager(_ central: CBCentralManager, didDiscover peripheral: CBPeripheral, advertisementData: [String : Any], rssi RSSI: NSNumber) {
        
        if let manuData = advertisementData[CBAdvertisementDataManufacturerDataKey],
            let data = manuData as? Data {
            let dataBytes = [UInt8](data)
            if dataBytes[1] == 0xe7 && dataBytes[0] == 0x13 && // LAB11 COMPANY IDENTIFIER
                dataBytes[2] == 0x23
            {
                // Buckler found
                print("buckler found")
                buckler = peripheral
            }
        }
    }
    
    func centralManager(_ central: CBCentralManager, didConnect peripheral: CBPeripheral) {
        self.title = "Discovering Services"
        peripheral.discoverServices(nil);
    }
    
    func centralManager(_ central: CBCentralManager, didDisconnectPeripheral peripheral: CBPeripheral, error: Error?) {
        self.title = "Connecting…"
        if central.state == .poweredOn {
            central.scanForPeripherals(withServices: nil, options: nil)
        }
    }
    
    /** Peripheral Delegate **/
    func peripheral(_ peripheral: CBPeripheral, didDiscoverServices error: Error?) {
        print("Discover Services")
        if let discoveredServices = peripheral.services {
            for service in discoveredServices {
                print(service.uuid.uuidString)
                let serviceIdEnum = ServiceID(rawValue: service.uuid.uuidString)
                if let serviceId = serviceIdEnum {
                    services[serviceId] = service
                    peripheral.discoverCharacteristics(nil, for: service)
                }
            }
        }
        
        self.title = "BearWatch"
    }
    
    func peripheral(_ peripheral: CBPeripheral, didDiscoverCharacteristicsFor service: CBService, error: Error?) {
        print("Discovered Characteristics \t \(service.uuid.uuidString)")
        if let discoveredCharacteristic = service.characteristics?[0] {
            peripheral.setNotifyValue(true, for: discoveredCharacteristic)
            if let serviceId = ServiceID(rawValue: service.uuid.uuidString) {
                if serviceId == .TIME {
                    sendUpdatedTime()
                }
            }
        }
        
        
    }
    
    let file = "avg_sum4.csv"
    func peripheral(_ peripheral: CBPeripheral, didUpdateValueFor characteristic: CBCharacteristic, error: Error?) {
        if let readData = characteristic.value {
            let value = UInt32(littleEndian: readData.withUnsafeBytes( { $0.pointee }))
            
            if let serviceId = ServiceID(rawValue: characteristic.service.uuid.uuidString) {
//                print(names[serviceId]!)
                if serviceId == .RANDOM {
                    if let dir = FileManager.default.urls(for: .documentDirectory, in: .userDomainMask).first {
                        let fileUrl = dir.appendingPathComponent(file)
                        let dataS = "\(value),".data(using: .utf8)
                        if let fileHandle = try? FileHandle(forWritingTo: fileUrl), let data = dataS {
                            fileHandle.seekToEndOfFile()
                            fileHandle.write(data)
                            fileHandle.closeFile()
                        }
                        else {
                            FileManager.default.createFile(atPath: fileUrl.path, contents: nil, attributes: nil)
                        }
                    }
                }
                if serviceId == ServiceID.TEMP ||
                    serviceId == ServiceID.PRESSURE ||
                    serviceId == ServiceID.HUMIDITY{
                    let valueF = Float32(bitPattern: value)
                    let valueS = String(format: "%.2f", valueF)
                    
                    if serviceId == .TEMP {
                        printValue(str: valueS + "˚C", forCell: serviceId)
                    } else if serviceId == .PRESSURE {
                         printValue(str: valueS + " mbar", forCell: serviceId)
                    } else {
                        printValue(str: valueS + "%", forCell: serviceId)
                    }
                } else {
                    printValue(str: "\(value)\n", forCell: serviceId)
                }
            } else {
                print("didUpdateValueFor Not working")
            }
        }
    }
}

