object frmMain: TfrmMain
  Left = 548
  Top = 238
  Width = 653
  Height = 242
  Caption = '手风琴调音软件'
  Color = clBtnFace
  Constraints.MinHeight = 200
  Constraints.MinWidth = 400
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnClose = FormClose
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  OnResize = FormResize
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Panel_Base: THsSkinPanel
    Left = 0
    Top = 0
    Width = 645
    Height = 215
    Align = alClient
    BevelOuter = bvNone
    Color = 147767
    Constraints.MinHeight = 200
    Constraints.MinWidth = 400
    UseDockManager = False
    FullRepaint = False
    Font.Charset = GB2312_CHARSET
    Font.Color = 7034689
    Font.Height = -12
    Font.Name = '宋体'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 0
    OnCloseClick = Panel_BaseCloseClick
    OnMaxClick = Panel_BaseMaxClick
    OnNorClick = Panel_BaseNorClick
    OnMinClick = Panel_BaseMinClick
    OnMenuClick = Panel_BaseMenuClick
    object Panel_DataInfo: TMyPanel0
      Left = 216
      Top = 72
      Width = 40
      Height = 30
      Color = 12320
      OnResize = Panel_DataInfoResize
      OnRefresh = Panel_DataInfoRefresh
    end
    object Button1: TButton
      Left = 89
      Top = 3
      Width = 55
      Height = 22
      Caption = 'Test'
      TabOrder = 3
      OnClick = Button1Click
    end
    object Edit1: TEdit
      Left = 311
      Top = 4
      Width = 98
      Height = 20
      TabOrder = 0
      Text = 'Edit1'
    end
    object Button2: TButton
      Left = 144
      Top = 3
      Width = 55
      Height = 22
      Caption = 'Test2'
      TabOrder = 4
      OnClick = Button2Click
    end
    object Button3: TButton
      Left = 199
      Top = 3
      Width = 55
      Height = 22
      Caption = 'Test3'
      TabOrder = 5
      OnClick = Button3Click
    end
    object Button4: TButton
      Left = 254
      Top = 3
      Width = 55
      Height = 22
      Caption = 'Test4'
      TabOrder = 6
      OnClick = Button4Click
    end
    object Panel_Info: TPanel
      Left = 28
      Top = 120
      Width = 340
      Height = 29
      Caption = 'Panel_Info'
      Font.Charset = GB2312_CHARSET
      Font.Color = clGray
      Font.Height = -12
      Font.Name = '宋体'
      Font.Style = [fsBold]
      ParentColor = True
      ParentFont = False
      TabOrder = 1
    end
    object Panel_Msg: TPanel
      Left = 28
      Top = 116
      Width = 553
      Height = 89
      BevelInner = bvLowered
      BorderWidth = 5
      Caption = 'Panel_Msg'
      Color = clSilver
      TabOrder = 7
      Visible = False
    end
    object Panel_WaveFormBase: TPanel
      Left = 312
      Top = 30
      Width = 193
      Height = 59
      BevelOuter = bvNone
      Caption = 'Panel_WaveFormBase'
      Color = clGray
      TabOrder = 8
      object Image_WaveForm: TImage
        Left = 0
        Top = 16
        Width = 105
        Height = 105
      end
      object ScrollBarWaveForm: TScrollBar
        Left = 0
        Top = 0
        Width = 193
        Height = 16
        Align = alTop
        PageSize = 0
        TabOrder = 0
        OnScroll = ScrollBarWaveFormScroll
      end
      object Panel_WaveForm: TMyPanel0
        Left = 153
        Top = 29
        Width = 10
        Height = 10
        Color = 1052688
        OnResize = Panel_DataInfoResize
        OnRefresh = Panel_DataInfoRefresh
      end
    end
    object EditZoomValue: TEdit
      Left = 558
      Top = 3
      Width = 48
      Height = 20
      TabOrder = 9
      Text = 'EditZoomValue'
      OnEnter = EditZoomValueEnter
      OnExit = EditZoomValueExit
    end
    object Edit2: TEdit
      Left = 411
      Top = 4
      Width = 146
      Height = 20
      TabOrder = 10
      Text = 'Edit2'
    end
  end
  object Timer_Close: TTimer
    Enabled = False
    Interval = 10
    OnTimer = Timer_CloseTimer
    Left = 8
    Top = 20
  end
end
