//
//  ViewController.swift
//  EE149SmartWatch
//
//  Created by Michael on 10/24/18.
//  Copyright © 2018 Michael. All rights reserved.
//

import UIKit
import CoreBluetooth

class ViewController: UICollectionViewController, UICollectionViewDelegateFlowLayout, CBCentralManagerDelegate {
    private let cellId = "cellId"
    
    private let names = ["Steps", "Temperature", "Sensor3", "Sensor4"]
    
    private var manager : CBCentralManager!
    
    private lazy var dataForGraphCells : [[CGPoint]?] = {
        var array = [[CGPoint]?]()
        for _ in names {
            array.append(nil)
        }
        return array
    }()
    
    let supportedServices : [CBUUID] = {
        let servicesUUID = [
            "BA49D63B-6FF0-4886-857B-43E0A95EB5EF"
        ]
        
        var services = [CBUUID]()
        for uuid in servicesUUID {
            let service = CBUUID(string: uuid)
            services.append(service)
        }
        return services
    }()
    
    var buckler : CBPeripheral! {
        didSet {
            manager.connect(buckler, options: nil)
        }
    }
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        collectionView?.backgroundColor = UIColor.white
        
        collectionView?.register(GraphCell.self, forCellWithReuseIdentifier: CellId.GraphCell.rawValue)
        collectionView?.register(TimerCell.self, forCellWithReuseIdentifier: CellId.TimerCell.rawValue)
        collectionView?.register(DebuggingCell.self, forCellWithReuseIdentifier: CellId.DebugCell.rawValue)
        
        manager = CBCentralManager.init(delegate: self, queue: nil)
    }
    
    override func collectionView(_ collectionView: UICollectionView, cellForItemAt indexPath: IndexPath) -> UICollectionViewCell {
        if indexPath.item == 1 {
            let cell = collectionView.dequeueReusableCell(withReuseIdentifier: CellId.TimerCell.rawValue, for: indexPath) as! TimerCell
            cell.collectionView = self
            return cell
        }
        let cell = collectionView.dequeueReusableCell(withReuseIdentifier: CellId.DebugCell.rawValue, for: indexPath) as! DebuggingCell
        
//        let cell = collectionView.dequeueReusableCell(withReuseIdentifier: CellId.GraphCell.rawValue, for: indexPath) as! GraphCell
//        if let data = dataForGraphCells[indexPath.item] {
//            cell.pointsToDraw = data
//        } else {
//            dataForGraphCells[indexPath.item] = createRandomData(for: cell)
//            cell.pointsToDraw = dataForGraphCells[indexPath.item]!
//        }
//        cell.name = names[indexPath.item]
        return cell
    }
    
    override func collectionView(_ collectionView: UICollectionView, numberOfItemsInSection section: Int) -> Int {
        return 2
    }
    
    func collectionView(_ collectionView: UICollectionView, layout collectionViewLayout: UICollectionViewLayout, sizeForItemAt indexPath: IndexPath) -> CGSize {
        return CGSize(width: view.frame.width, height: 300)
    }
    
    @objc
    func startTimer(sender: UIButton) {
        if sender.title(for: .normal) == "Start Timer" {
            sender.setTitle("Starting", for: .normal)
        } else {
            sender.setTitle("Start Timer", for: .normal)
        }
        guard let title = sender.title(for: .normal) else {
            return
        }
        printToDebug(str: "\(title)\n")
        
        if manager.isScanning {
            manager.stopScan()
        } else {
            manager.scanForPeripherals(withServices: nil, options: nil)
        }
    }
    
    func createRandomData(for cell: GraphCell) -> [CGPoint] {
        var pointsToDraw = Array<CGPoint>()
        var randomYValues = Array<Float>()
        let height = Float(cell.bounds.height)
        let width = Float(cell.bounds.width)
        
        for i in 0..<Int(300) {
            randomYValues.append(Float(arc4random_uniform(1000) * UInt32(i)))
        }
        let max = randomYValues.reduce(-Float.greatestFiniteMagnitude) {
            if $0 > $1 {
                return $0
            }
            return $1
        }
        let min = randomYValues.reduce(Float.greatestFiniteMagnitude) {
            if $0 > $1 {
                return $1
            }
            return $0
        }
        
        let numPoints = Float(randomYValues.count)
        
        let b = height/(1 - min/max)
        let a = -b/max
        
        for (index, yPt) in randomYValues.enumerated() {
            let xPt  = Float(index)/numPoints*width
            pointsToDraw.append(CGPoint(x: CGFloat(xPt), y: CGFloat(a*yPt + b)))
        }
        return pointsToDraw
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
            if dataBytes[0] == 0xe0 && dataBytes[1] == 0x02 && // LAB11 COMPANY IDENTIFIER
                dataBytes[2] == 0x23
            {
                // Buckler found
                print(dataBytes[3..<dataBytes.count])
                buckler = peripheral
            }
        }
    }
    
    func centralManager(_ central: CBCentralManager, didConnect peripheral: CBPeripheral) {
        print("Connected")
    }
    
    func centralManager(_ central: CBCentralManager, didDisconnectPeripheral peripheral: CBPeripheral, error: Error?) {
        print("Disconnected")
    }
    
}

