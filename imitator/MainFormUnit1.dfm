object Form1: TForm1
  Left = 236
  Top = 221
  Width = 542
  Height = 530
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
  object Shape_CircleZ1: TShape
    Left = 24
    Top = 216
    Width = 177
    Height = 177
    Shape = stCircle
  end
  object Shape_CircleZ2: TShape
    Left = 106
    Top = 206
    Width = 10
    Height = 200
  end
  object Shape_CircleZ3: TShape
    Left = 17
    Top = 305
    Width = 200
    Height = 10
  end
  object Shape_CircleMark0: TShape
    Left = 136
    Top = 328
    Width = 33
    Height = 33
    Brush.Color = clBlue
    Shape = stCircle
  end
  object Shape_Circle1: TShape
    Left = 97
    Top = 296
    Width = 25
    Height = 25
    Brush.Color = clSilver
    Shape = stCircle
  end
  object Shape_CircleMark1: TShape
    Left = 56
    Top = 328
    Width = 33
    Height = 33
    Brush.Color = clBlue
    Shape = stCircle
    Visible = False
  end
  object Shape_CircleMark2: TShape
    Left = 56
    Top = 256
    Width = 33
    Height = 33
    Brush.Color = clBlue
    Shape = stCircle
    Visible = False
  end
  object Shape_CircleMark3: TShape
    Left = 136
    Top = 256
    Width = 33
    Height = 33
    Brush.Color = clBlue
    Shape = stCircle
    Visible = False
  end
  object Shape_Tube: TShape
    Left = 80
    Top = 160
    Width = 321
    Height = 33
  end
  object Shape_StangaNiz: TShape
    Left = 344
    Top = 128
    Width = 65
    Height = 33
  end
  object Shape_ShtangaVerh: TShape
    Left = 368
    Top = 56
    Width = 17
    Height = 73
  end
  object Shape_DefPosDn: TShape
    Left = 416
    Top = 104
    Width = 41
    Height = 41
  end
  object Shape_DefPosUp: TShape
    Left = 416
    Top = 16
    Width = 41
    Height = 41
  end
  object Shape_Sensor1: TShape
    Left = 272
    Top = 184
    Width = 41
    Height = 121
    Shape = stEllipse
  end
  object Shape_Sensor2: TShape
    Left = 456
    Top = 184
    Width = 41
    Height = 121
    Shape = stEllipse
  end
  object Button_DefPosUp: TSpeedButton
    Left = 264
    Top = 24
    Width = 73
    Height = 25
    AllowAllUp = True
    GroupIndex = 1
    Down = True
    Caption = #1042#1077#1088#1093
    OnClick = Button_DefPosUpClick
  end
  object Button_DefPosDn: TSpeedButton
    Left = 264
    Top = 112
    Width = 73
    Height = 25
    AllowAllUp = True
    GroupIndex = 2
    Caption = #1042#1085#1080#1079
    OnClick = Button_DefPosDnClick
  end
  object Shape_SetDefect: TShape
    Left = 360
    Top = 272
    Width = 33
    Height = 33
    Shape = stCircle
  end
  object SpeedButton_Obrazec: TSpeedButton
    Left = 328
    Top = 376
    Width = 105
    Height = 57
    AllowAllUp = True
    GroupIndex = 3
    Caption = #1054#1073#1088#1072#1079#1077#1094
    OnClick = SpeedButton_ObrazecClick
  end
  object Button_CircleBack: TButton
    Left = 56
    Top = 416
    Width = 33
    Height = 25
    Caption = '<-'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -21
    Font.Name = 'Times New Roman'
    Font.Style = []
    ParentFont = False
    TabOrder = 0
    OnClick = Button_CircleBackClick
  end
  object Button_CircleStop: TButton
    Left = 96
    Top = 416
    Width = 33
    Height = 25
    Caption = '--'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -21
    Font.Name = 'Times New Roman'
    Font.Style = []
    ParentFont = False
    TabOrder = 1
    OnClick = Button_CircleStopClick
  end
  object Button_CircleForev: TButton
    Left = 136
    Top = 416
    Width = 33
    Height = 25
    Caption = '->'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -21
    Font.Name = 'Times New Roman'
    Font.Style = []
    ParentFont = False
    TabOrder = 2
    OnClick = Button_CircleForevClick
  end
  object Button_Defect: TButton
    Left = 344
    Top = 200
    Width = 75
    Height = 73
    Caption = #1076#1077#1092#1077#1082#1090
    TabOrder = 3
    OnClick = Button_DefectClick
  end
  object Button2: TButton
    Left = 16
    Top = 8
    Width = 75
    Height = 25
    Caption = #1074' '#1085#1072#1095#1072#1083#1086
    TabOrder = 4
    OnClick = Button2Click
  end
  object Edit_TubeLen: TEdit
    Left = 8
    Top = 56
    Width = 121
    Height = 21
    TabOrder = 5
    Text = '300'
  end
  object Edit_TubeShift: TEdit
    Left = 56
    Top = 448
    Width = 121
    Height = 21
    TabOrder = 6
    Text = '9'
  end
  object Timer_Circle: TTimer
    Enabled = False
    Interval = 350
    OnTimer = Timer_CircleTimer
    Left = 96
    Top = 448
  end
  object Timer_SetDefect: TTimer
    Enabled = False
    Interval = 200
    OnTimer = Timer_SetDefectTimer
    Left = 368
    Top = 304
  end
end
