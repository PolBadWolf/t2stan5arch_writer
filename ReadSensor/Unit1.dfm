object Form1: TForm1
  Left = 202
  Top = 123
  Width = 762
  Height = 201
  HorzScrollBar.Visible = False
  VertScrollBar.Visible = False
  Caption = 'Form1'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnCloseQuery = FormCloseQuery
  PixelsPerInch = 96
  TextHeight = 13
  object Shape_TUBE_HERE_L: TShape
    Left = 200
    Top = 24
    Width = 17
    Height = 33
    Brush.Color = clMedGray
    Shape = stRoundRect
  end
  object Shape_TUBE_HERE_R: TShape
    Left = 264
    Top = 24
    Width = 17
    Height = 33
    Brush.Color = clMedGray
    Shape = stRoundRect
  end
  object Shape_WELD_DEFECT: TShape
    Left = 224
    Top = 24
    Width = 33
    Height = 33
    Brush.Color = clMedGray
  end
  object Shape_MODE_SAMPLE: TShape
    Left = 552
    Top = 8
    Width = 153
    Height = 57
    Brush.Color = clMedGray
    Shape = stRoundRect
  end
  object Shape_SENSOR_AT_TOP: TShape
    Left = 224
    Top = 4
    Width = 33
    Height = 17
    Brush.Color = clMedGray
    Shape = stRoundRect
  end
  object Shape_SENSOR_AT_BOTTOM: TShape
    Left = 224
    Top = 60
    Width = 33
    Height = 17
    Brush.Color = clMedGray
    Shape = stRoundRect
  end
  object Shape_Circle: TShape
    Left = 224
    Top = 24
    Width = 33
    Height = 33
    Brush.Color = clMedGray
    Shape = stRoundRect
  end
  object ImageVisual: TImage
    Left = 8
    Top = 80
    Width = 729
    Height = 73
  end
  object Shape_SENSOR_AT: TShape
    Left = 264
    Top = 0
    Width = 25
    Height = 25
    Shape = stCircle
  end
  object Label1: TLabel
    Left = 552
    Top = 24
    Width = 153
    Height = 23
    Alignment = taCenter
    AutoSize = False
    Caption = #1054#1073#1088#1072#1079#1077#1094
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindow
    Font.Height = -21
    Font.Name = 'Times New Roman'
    Font.Style = []
    ParentFont = False
    Transparent = True
  end
  object Label2: TLabel
    Left = 808
    Top = 16
    Width = 32
    Height = 13
    Caption = 'Label2'
  end
  object Label3: TLabel
    Left = 808
    Top = 40
    Width = 32
    Height = 13
    Caption = 'Label3'
  end
  object Panel1: TPanel
    Left = 8
    Top = 8
    Width = 89
    Height = 57
    Caption = 'Panel1'
    Color = clSkyBlue
    TabOrder = 0
    object Panel2: TPanel
      Left = 8
      Top = 8
      Width = 73
      Height = 41
      TabOrder = 0
      object Label_NumberTube: TLabel
        Left = 1
        Top = 1
        Width = 71
        Height = 39
        Align = alClient
        Alignment = taCenter
        Caption = '----'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -32
        Font.Name = 'Times New Roman'
        Font.Style = []
        ParentFont = False
      end
    end
  end
  object Panel3: TPanel
    Left = 104
    Top = 8
    Width = 89
    Height = 57
    Caption = 'Panel1'
    Color = clSkyBlue
    TabOrder = 1
    object Panel4: TPanel
      Left = 8
      Top = 8
      Width = 73
      Height = 41
      TabOrder = 0
      object Label_dTube: TLabel
        Left = 1
        Top = 1
        Width = 71
        Height = 39
        Align = alClient
        Alignment = taCenter
        Caption = '---,-'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -32
        Font.Name = 'Times New Roman'
        Font.Style = []
        ParentFont = False
      end
    end
  end
  object Memo1: TMemo
    Left = 304
    Top = 8
    Width = 233
    Height = 57
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -19
    Font.Name = 'Times New Roman'
    Font.Style = []
    Lines.Strings = (
      #1044#1083#1080#1085#1072' '#1089#1077#1075#1084#1077#1085#1090#1072' : '
      #1044#1083#1080#1085#1072' '#1090#1088#1091#1073#1099' : ')
    ParentFont = False
    TabOrder = 2
  end
  object Button1: TButton
    Left = 712
    Top = 8
    Width = 25
    Height = 65
    Caption = 'E'
    Enabled = False
    TabOrder = 3
    Visible = False
    OnClick = Button1Click
  end
  object ADOConnRead: TADOConnection
    ConnectionString = 'Provider=MSDASQL.1;Persist Security Info=False;Data Source=loc2'
    ConnectOptions = coAsyncConnect
    LoginPrompt = False
    Mode = cmRead
    Provider = 'MSDASQL.1'
    Left = 744
    Top = 72
  end
  object TimerStart: TTimer
    Interval = 1
    OnTimer = TimerStartTimer
    Left = 744
    Top = 8
  end
  object ADOConnWrite: TADOConnection
    ConnectOptions = coAsyncConnect
    LoginPrompt = False
    Mode = cmShareDenyWrite
    Left = 744
    Top = 104
  end
  object TimerSecRead: TTimer
    Enabled = False
    OnTimer = TimerSecReadTimer
    Left = 776
    Top = 72
  end
  object TimerSecWrite: TTimer
    Enabled = False
    OnTimer = TimerSecWriteTimer
    Left = 776
    Top = 104
  end
end
