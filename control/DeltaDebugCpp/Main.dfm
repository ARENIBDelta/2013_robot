object FMain: TFMain
  Left = 0
  Top = 0
  Caption = 'DeltaControl'
  ClientHeight = 485
  ClientWidth = 735
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object GroupBox1: TGroupBox
    Left = 0
    Top = 0
    Width = 735
    Height = 382
    Align = alClient
    Caption = 'Control'
    TabOrder = 0
    object tbQEI1: TTrackBar
      Left = 55
      Top = 16
      Width = 35
      Height = 150
      Enabled = False
      Max = 10000
      Orientation = trVertical
      TabOrder = 0
    end
    object tbQEI2: TTrackBar
      Left = 304
      Top = 16
      Width = 31
      Height = 150
      Enabled = False
      Max = 10000
      Orientation = trVertical
      TabOrder = 1
    end
    object tbQEI6: TTrackBar
      Left = 548
      Top = 180
      Width = 29
      Height = 150
      Max = 10000
      Orientation = trVertical
      TabOrder = 2
    end
    object tbQEI4: TTrackBar
      Left = 55
      Top = 172
      Width = 30
      Height = 150
      Max = 10000
      Orientation = trVertical
      TabOrder = 3
    end
    object tbQEI5: TTrackBar
      Left = 304
      Top = 172
      Width = 45
      Height = 150
      Max = 10000
      Orientation = trVertical
      TabOrder = 5
    end
    object leKp1: TLabeledEdit
      Left = 139
      Top = 40
      Width = 86
      Height = 21
      EditLabel.Width = 12
      EditLabel.Height = 13
      EditLabel.Caption = 'Kp'
      TabOrder = 6
    end
    object leKd1: TLabeledEdit
      Left = 139
      Top = 80
      Width = 86
      Height = 21
      EditLabel.Width = 12
      EditLabel.Height = 13
      EditLabel.Caption = 'Kd'
      TabOrder = 7
    end
    object leKi1: TLabeledEdit
      Left = 139
      Top = 120
      Width = 86
      Height = 21
      EditLabel.Width = 8
      EditLabel.Height = 13
      EditLabel.Caption = 'Ki'
      TabOrder = 8
    end
    object leKp3: TLabeledEdit
      Left = 623
      Top = 40
      Width = 86
      Height = 21
      EditLabel.Width = 12
      EditLabel.Height = 13
      EditLabel.Caption = 'Kp'
      TabOrder = 9
    end
    object leKd3: TLabeledEdit
      Left = 623
      Top = 80
      Width = 86
      Height = 21
      EditLabel.Width = 12
      EditLabel.Height = 13
      EditLabel.Caption = 'Kd'
      TabOrder = 10
    end
    object leKi3: TLabeledEdit
      Left = 623
      Top = 120
      Width = 86
      Height = 21
      EditLabel.Width = 8
      EditLabel.Height = 13
      EditLabel.Caption = 'Ki'
      TabOrder = 11
    end
    object leKp4: TLabeledEdit
      Left = 139
      Top = 192
      Width = 86
      Height = 21
      EditLabel.Width = 12
      EditLabel.Height = 13
      EditLabel.Caption = 'Kp'
      TabOrder = 12
    end
    object leKd4: TLabeledEdit
      Left = 139
      Top = 232
      Width = 86
      Height = 21
      EditLabel.Width = 12
      EditLabel.Height = 13
      EditLabel.Caption = 'Kd'
      TabOrder = 13
    end
    object leKi4: TLabeledEdit
      Left = 139
      Top = 272
      Width = 86
      Height = 21
      EditLabel.Width = 8
      EditLabel.Height = 13
      EditLabel.Caption = 'Ki'
      TabOrder = 14
    end
    object leKp5: TLabeledEdit
      Left = 387
      Top = 192
      Width = 86
      Height = 21
      EditLabel.Width = 12
      EditLabel.Height = 13
      EditLabel.Caption = 'Kp'
      TabOrder = 15
    end
    object leKd5: TLabeledEdit
      Left = 387
      Top = 232
      Width = 86
      Height = 21
      EditLabel.Width = 12
      EditLabel.Height = 13
      EditLabel.Caption = 'Kd'
      TabOrder = 16
    end
    object leKi5: TLabeledEdit
      Left = 387
      Top = 272
      Width = 86
      Height = 21
      EditLabel.Width = 8
      EditLabel.Height = 13
      EditLabel.Caption = 'Ki'
      TabOrder = 17
    end
    object leKp6: TLabeledEdit
      Left = 623
      Top = 192
      Width = 86
      Height = 21
      EditLabel.Width = 12
      EditLabel.Height = 13
      EditLabel.Caption = 'Kp'
      TabOrder = 18
    end
    object leKd6: TLabeledEdit
      Left = 623
      Top = 232
      Width = 86
      Height = 21
      EditLabel.Width = 12
      EditLabel.Height = 13
      EditLabel.Caption = 'Kd'
      TabOrder = 19
    end
    object leKi6: TLabeledEdit
      Left = 623
      Top = 272
      Width = 86
      Height = 21
      EditLabel.Width = 8
      EditLabel.Height = 13
      EditLabel.Caption = 'Ki'
      TabOrder = 20
    end
    object tbQEI3: TTrackBar
      Left = 583
      Top = 16
      Width = 34
      Height = 150
      Max = 10000
      Orientation = trVertical
      TabOrder = 4
    end
    object tbGoal3: TTrackBar
      Left = 548
      Top = 16
      Width = 29
      Height = 150
      Enabled = False
      Max = 10000
      Orientation = trVertical
      TabOrder = 21
    end
    object tbQEISet3: TTrackBar
      Left = 510
      Top = 16
      Width = 32
      Height = 150
      Max = 10000
      Orientation = trVertical
      TabOrder = 22
    end
    object tbQEISet2: TTrackBar
      Left = 267
      Top = 16
      Width = 31
      Height = 150
      Max = 10000
      Orientation = trVertical
      TabOrder = 23
    end
    object tbQEISet1: TTrackBar
      Left = 19
      Top = 16
      Width = 40
      Height = 150
      Max = 10000
      Orientation = trVertical
      TabOrder = 24
    end
    object tbQEISet4: TTrackBar
      Left = 19
      Top = 172
      Width = 30
      Height = 150
      Max = 10000
      Orientation = trVertical
      TabOrder = 25
    end
    object tbQEISet5: TTrackBar
      Left = 267
      Top = 172
      Width = 31
      Height = 150
      Max = 10000
      Orientation = trVertical
      TabOrder = 26
    end
    object tbQEISet6: TTrackBar
      Left = 510
      Top = 180
      Width = 29
      Height = 150
      Max = 10000
      Orientation = trVertical
      TabOrder = 27
    end
  end
  object tbGoal1: TTrackBar
    Left = 94
    Top = 16
    Width = 31
    Height = 150
    Max = 10000
    Orientation = trVertical
    TabOrder = 1
  end
  object tbGoal4: TTrackBar
    Left = 91
    Top = 172
    Width = 34
    Height = 150
    Max = 10000
    Orientation = trVertical
    TabOrder = 2
  end
  object tbGoal5: TTrackBar
    Left = 341
    Top = 172
    Width = 40
    Height = 150
    Max = 10000
    Orientation = trVertical
    TabOrder = 3
  end
  object tbGoal2: TTrackBar
    Left = 341
    Top = 16
    Width = 32
    Height = 150
    Max = 10000
    Orientation = trVertical
    TabOrder = 4
  end
  object tbGoal6: TTrackBar
    Left = 583
    Top = 180
    Width = 34
    Height = 150
    Max = 10000
    Orientation = trVertical
    TabOrder = 5
  end
  object leKp2: TLabeledEdit
    Left = 387
    Top = 40
    Width = 86
    Height = 21
    EditLabel.Width = 12
    EditLabel.Height = 13
    EditLabel.Caption = 'Kp'
    TabOrder = 6
  end
  object leKd2: TLabeledEdit
    Left = 387
    Top = 80
    Width = 86
    Height = 21
    EditLabel.Width = 12
    EditLabel.Height = 13
    EditLabel.Caption = 'Kd'
    TabOrder = 7
  end
  object leKi2: TLabeledEdit
    Left = 387
    Top = 120
    Width = 86
    Height = 21
    EditLabel.Width = 8
    EditLabel.Height = 13
    EditLabel.Caption = 'Ki'
    TabOrder = 8
  end
  object Panel1: TPanel
    Left = 0
    Top = 382
    Width = 735
    Height = 103
    Align = alBottom
    TabOrder = 9
    object GroupBox4: TGroupBox
      Left = 1
      Top = 1
      Width = 193
      Height = 101
      Align = alLeft
      Caption = 'Serial'
      TabOrder = 0
      object ComLed2: TComLed
        Left = 12
        Top = 24
        Width = 25
        Height = 25
        ComPort = ComPort1
        LedSignal = lsConn
        Kind = lkRedLight
      end
      object Button7: TButton
        Left = 12
        Top = 55
        Width = 75
        Height = 25
        Caption = 'Connect'
        TabOrder = 0
        OnClick = Button1Click
      end
      object Button8: TButton
        Left = 99
        Top = 55
        Width = 75
        Height = 25
        Caption = 'Disconnect'
        TabOrder = 1
        OnClick = Button2Click
      end
      object Button9: TButton
        Left = 99
        Top = 24
        Width = 75
        Height = 25
        Caption = 'Flush'
        TabOrder = 2
        OnClick = Button9Click
      end
    end
    object GroupBox5: TGroupBox
      Left = 194
      Top = 1
      Width = 540
      Height = 101
      Align = alClient
      Caption = 'Amour'
      TabOrder = 1
      object Button10: TButton
        Left = 193
        Top = 24
        Width = 75
        Height = 25
        Caption = 'SetGoals'
        TabOrder = 0
        OnClick = Button5Click
      end
      object Button11: TButton
        Left = 14
        Top = 23
        Width = 75
        Height = 25
        Caption = 'GetQEIs'
        TabOrder = 1
        OnClick = Button4Click
      end
      object Button12: TButton
        Left = 14
        Top = 54
        Width = 75
        Height = 25
        Caption = 'GetkPDIs'
        TabOrder = 2
        OnClick = Button6Click
      end
    end
    object Button1: TButton
      Left = 298
      Top = 24
      Width = 75
      Height = 25
      Caption = 'SetQEIS'
      TabOrder = 2
      OnClick = SetQEIS
    end
    object Button2: TButton
      Left = 510
      Top = 56
      Width = 75
      Height = 25
      Caption = 'Button2'
      TabOrder = 3
    end
    object Button3: TButton
      Left = 298
      Top = 55
      Width = 75
      Height = 25
      Caption = 'SetkPDIs'
      TabOrder = 4
      OnClick = Button3Click
    end
  end
  object ComPort1: TComPort
    BaudRate = br38400
    Port = 'COM3'
    Parity.Bits = prNone
    StopBits = sbOneStopBit
    DataBits = dbEight
    Events = [evRxChar, evTxEmpty, evRxFlag, evRing, evBreak, evCTS, evDSR, evError, evRLSD, evRx80Full]
    FlowControl.OutCTSFlow = False
    FlowControl.OutDSRFlow = False
    FlowControl.ControlDTR = dtrDisable
    FlowControl.ControlRTS = rtsDisable
    FlowControl.XonXoffOut = False
    FlowControl.XonXoffIn = False
    StoredProps = []
    TriggersOnRxChar = True
    OnRxChar = ComPort1RxChar
    Left = 128
    Top = 432
  end
end