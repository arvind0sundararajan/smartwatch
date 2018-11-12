//
//  CollectionViewCells.swift
//  EE149SmartWatch
//
//  Created by Michael on 10/24/18.
//  Copyright © 2018 Michael. All rights reserved.
//

import UIKit

enum CellId : String {
    case GraphCell = "graphCellId"
    case TimerCell = "timerCellId"
    case DebugCell = "debugCellId"
}

class GraphCell: UICollectionViewCell {
    
    private let cellId = CellId.GraphCell.rawValue
    var name : String = "" {
        didSet {
            nameLabel.text = name
        }
    }
    
    var pointsToDraw = Array<CGPoint>() {
        didSet {
            graphView.pointsToDraw = pointsToDraw
        }
    }
    
    override init(frame: CGRect) {
        super.init(frame: frame)
        setupViews()
    }
    
    required init?(coder aDecoder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
    
    let nameLabel: UILabel = {
        let label = UILabel()
        label.text = "Best New Apps"
        label.font = UIFont.systemFont(ofSize: 16)
        label.translatesAutoresizingMaskIntoConstraints = false
        return label
    }()
    
    let graphView : GraphView = {
        let view = GraphView()
        view.backgroundColor = UIColor.clear
        view.translatesAutoresizingMaskIntoConstraints = false
        return view
    }()
    
    let appsCollectionView: UICollectionView = {
        let layout = UICollectionViewFlowLayout()
        layout.scrollDirection = .horizontal
        let collectionView = UICollectionView(frame: .zero, collectionViewLayout: layout)
        
        collectionView.backgroundColor = UIColor.clear
        collectionView.translatesAutoresizingMaskIntoConstraints = false
        
        return collectionView
    }()
    
    let dividerLineView: UIView = {
        let view = UIView()
        view.backgroundColor = UIColor(white: 0.4, alpha: 0.4)
        view.translatesAutoresizingMaskIntoConstraints = false
        return view
    }()
    
    func setupViews() {
        backgroundColor = UIColor.clear
        
        addSubview(nameLabel)
        addSubview(graphView)
        
        addConstraints(NSLayoutConstraint.constraints(withVisualFormat: "H:|-14-[v0]|", options: .alignAllCenterY, metrics: nil, views: ["v0": nameLabel]))
        addConstraints(NSLayoutConstraint.constraints(withVisualFormat: "H:|[v0]|", options: .alignAllCenterY, metrics: nil, views: ["v0": graphView]))
        addConstraints(NSLayoutConstraint.constraints(withVisualFormat: "V:|-5-[v0][v1]|", options: .alignAllTrailing, metrics: nil, views: ["v0": nameLabel, "v1": graphView]))
    }
}

class GraphView : UIView {
    
    var pointsToDraw = Array<CGPoint>() {
        didSet {
            setNeedsDisplay()
        }
    }
    
    override func draw(_ rect: CGRect) {
        let path = UIBezierPath()
        for (i, pt) in pointsToDraw.enumerated() {
            if i == 0 {
                path.move(to: pt)
            } else {
                path.addLine(to: pt)
            }
        }
        UIColor.red.setFill()
        UIColor.red.setStroke()
        path.fill()
        path.stroke()
        path.close()
    }
}

class TimerCell: UICollectionViewCell {
    
    private let cellId = CellId.GraphCell.rawValue
    var collectionView : UICollectionViewController? {
        didSet {
            startTimerButton.addTarget(collectionView, action: #selector(ViewController.startTimer), for: .touchUpInside)
        }
    }
    
    override init(frame: CGRect) {
        super.init(frame: frame)
        setupViews()
    }
    
    required init?(coder aDecoder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
    
    let nameLabel: UILabel = {
        let label = UILabel()
        label.text = "Timer"
        label.font = UIFont.systemFont(ofSize: 16)
        label.translatesAutoresizingMaskIntoConstraints = false
        return label
    }()
    
    let startTimerButton : UIButton = {
        let button = UIButton()
        button.setTitle("Start Timer", for: UIControl.State.normal)
        button.setTitleColor(.black, for: .normal)
        button.translatesAutoresizingMaskIntoConstraints = false
        
        button.setTitle("Starting", for: UIControl.State.selected)
        button.setTitleColor(.black, for: .focused)
        
        button.backgroundColor = .clear
        button.layer.borderColor = UIColor.blue.cgColor
        
        return button
    }()
    
    let timePicker : UIDatePicker = {
        let picker = UIDatePicker()
        picker.datePickerMode = .time
        picker.translatesAutoresizingMaskIntoConstraints = false
        return picker
    }()
    
    func setupViews() {
        backgroundColor = UIColor.clear
        
        addSubview(nameLabel)
        addSubview(startTimerButton)
        addSubview(timePicker)

        addConstraints(NSLayoutConstraint.constraints(withVisualFormat: "V:|-5-[v0][v1]|", options: .directionLeadingToTrailing, metrics: nil, views: ["v0": nameLabel, "v1": timePicker]))
        addConstraints(NSLayoutConstraint.constraints(withVisualFormat: "H:|-5-[v0][v1]-5-|", options: .alignAllCenterY, metrics: nil, views: ["v0": nameLabel, "v1": startTimerButton]))
        addConstraints(NSLayoutConstraint.constraints(withVisualFormat: "H:|[v0]|", options: .alignAllCenterY, metrics: nil, views: ["v0": timePicker]))
//
    }
}

class DebuggingCell :  UICollectionViewCell {
    private let cellId = CellId.DebugCell.rawValue
    
    override init(frame: CGRect) {
        super.init(frame: frame)
        setupViews()
    }
    
    required init?(coder aDecoder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
    
    let textView : UITextView = {
        let view = UITextView()
        view.translatesAutoresizingMaskIntoConstraints = false
        
        return view
    }()
    
    func setupViews() {
        backgroundColor = UIColor.clear
        addSubview(textView)
        
        addConstraints(NSLayoutConstraint.constraints(withVisualFormat: "V:|-0-[v0]-0-|", options: .alignAllTop, metrics: nil, views: ["v0": textView]))
        addConstraints(NSLayoutConstraint.constraints(withVisualFormat: "V:|-0-[v0]-0-|", options: .alignAllBottom, metrics: nil, views: ["v0": textView]))
        addConstraints(NSLayoutConstraint.constraints(withVisualFormat: "H:|-0-[v0]-0-|", options: .alignAllLeft, metrics: nil, views: ["v0": textView]))
        addConstraints(NSLayoutConstraint.constraints(withVisualFormat: "H:|-0-[v0]-0-|", options: .alignAllRight, metrics: nil, views: ["v0": textView]))
    }
}

