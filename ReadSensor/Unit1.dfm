object Form1: TForm1
  Left = 207
  Top = 124
  Width = 728
  Height = 196
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
    Width = 697
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
  object ADOConnection1: TADOConnection
    ConnectionString = 
      'Provider=MSDASQL.1;Persist Security Info=False;Data Source=T2Sta' +
      'n5MyCon'
    LoginPrompt = False
    Provider = 'MSDASQL.1'
    Left = 728
    Top = 24
  end
  object ADOQuery1: TADOQuery
    Connection = ADOConnection1
    Parameters = <>
    Left = 760
    Top = 24
  end
  object TimerStart: TTimer
    Interval = 1
    OnTimer = TimerStartTimer
    Left = 792
    Top = 24
  end
end
