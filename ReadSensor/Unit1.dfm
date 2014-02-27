object Form1: TForm1
  Left = -987
  Top = 102
  Width = 854
  Height = 479
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
  object Shape_KEY_BACK: TShape
    Left = 320
    Top = 304
    Width = 33
    Height = 33
    Brush.Color = clMedGray
    Shape = stCircle
  end
  object Shape_KEY_FORWARD: TShape
    Left = 360
    Top = 304
    Width = 33
    Height = 33
    Brush.Color = clMedGray
    Shape = stCircle
  end
  object Shape_TUBE_HERE_L: TShape
    Left = 368
    Top = 40
    Width = 33
    Height = 41
    Brush.Color = clMedGray
  end
  object Shape_TUBE_HERE_R: TShape
    Left = 464
    Top = 40
    Width = 33
    Height = 33
    Brush.Color = clMedGray
  end
  object Shape5: TShape
    Left = 56
    Top = 336
    Width = 33
    Height = 33
    Brush.Color = clMedGray
    Shape = stCircle
  end
  object Shape6: TShape
    Left = 96
    Top = 336
    Width = 33
    Height = 33
    Brush.Color = clMedGray
    Shape = stCircle
  end
  object Shape_WELD_DEFECT: TShape
    Left = 504
    Top = 40
    Width = 33
    Height = 33
    Brush.Color = clMedGray
  end
  object Shape_MODE_CALIBROVKA: TShape
    Left = 312
    Top = 40
    Width = 33
    Height = 41
    Brush.Color = clMedGray
  end
  object Shape_SENSOR_AT_TOP: TShape
    Left = 416
    Top = 16
    Width = 33
    Height = 17
    Brush.Color = clMedGray
  end
  object Shape_SENSOR_AT_BOTTOM: TShape
    Left = 416
    Top = 80
    Width = 33
    Height = 17
    Brush.Color = clMedGray
  end
  object Shape_Circle: TShape
    Left = 416
    Top = 40
    Width = 33
    Height = 33
    Brush.Color = clMedGray
    Shape = stRoundRect
  end
  object Label1: TLabel
    Left = 320
    Top = 344
    Width = 33
    Height = 24
    Alignment = taCenter
    AutoSize = False
    Caption = 'K<'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label2: TLabel
    Left = 360
    Top = 344
    Width = 33
    Height = 24
    Alignment = taCenter
    AutoSize = False
    Caption = 'K>'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label3: TLabel
    Left = 368
    Top = 48
    Width = 33
    Height = 24
    Alignment = taCenter
    AutoSize = False
    Caption = 'stL'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label4: TLabel
    Left = 464
    Top = 80
    Width = 33
    Height = 24
    Alignment = taCenter
    AutoSize = False
    Caption = 'stR'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label5: TLabel
    Left = 56
    Top = 376
    Width = 33
    Height = 24
    Alignment = taCenter
    AutoSize = False
    Caption = '--'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label6: TLabel
    Left = 96
    Top = 376
    Width = 33
    Height = 24
    Alignment = taCenter
    AutoSize = False
    Caption = '--'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label7: TLabel
    Left = 504
    Top = 80
    Width = 33
    Height = 24
    Alignment = taCenter
    AutoSize = False
    Caption = 'DF'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label8: TLabel
    Left = 312
    Top = 48
    Width = 33
    Height = 24
    Alignment = taCenter
    AutoSize = False
    Caption = 'Obr'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label9: TLabel
    Left = 616
    Top = 320
    Width = 33
    Height = 24
    Alignment = taCenter
    AutoSize = False
    Caption = 'Dup'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label10: TLabel
    Left = 744
    Top = 344
    Width = 33
    Height = 24
    Alignment = taCenter
    AutoSize = False
    Caption = 'Ddn'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label13: TLabel
    Left = 672
    Top = 336
    Width = 33
    Height = 24
    Alignment = taCenter
    AutoSize = False
    Caption = 'K'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label14: TLabel
    Left = 448
    Top = 328
    Width = 38
    Height = 13
    Caption = 'Label14'
  end
  object ImageVisual: TImage
    Left = 8
    Top = 136
    Width = 681
    Height = 160
  end
  object Label17: TLabel
    Left = 448
    Top = 360
    Width = 38
    Height = 13
    Caption = 'Label17'
  end
  object ShapeCircle1: TShape
    Left = 176
    Top = 320
    Width = 33
    Height = 33
    Brush.Color = clMedGray
    Shape = stCircle
  end
  object ShapeCircle2: TShape
    Left = 224
    Top = 320
    Width = 33
    Height = 33
    Brush.Color = clMedGray
    Shape = stCircle
  end
  object Label11: TLabel
    Left = 176
    Top = 360
    Width = 33
    Height = 24
    Alignment = taCenter
    AutoSize = False
    Caption = 'D1'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label12: TLabel
    Left = 224
    Top = 360
    Width = 33
    Height = 24
    Alignment = taCenter
    AutoSize = False
    Caption = 'D2'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label_temp: TLabel
    Left = 96
    Top = 312
    Width = 55
    Height = 13
    Caption = 'Label_temp'
  end
  object Label15: TLabel
    Left = 40
    Top = 312
    Width = 38
    Height = 13
    Caption = 'Label15'
  end
  object Panel1: TPanel
    Left = 8
    Top = 8
    Width = 89
    Height = 89
    Caption = 'Panel1'
    Color = clSkyBlue
    TabOrder = 0
    object Panel2: TPanel
      Left = 8
      Top = 8
      Width = 73
      Height = 73
      Caption = 'Panel2'
      TabOrder = 0
      object Label_NumberTube: TLabel
        Left = 1
        Top = 1
        Width = 71
        Height = 71
        Align = alClient
        Alignment = taCenter
        Caption = '99'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -64
        Font.Name = 'Times New Roman'
        Font.Style = []
        ParentFont = False
      end
    end
  end
  object Panel3: TPanel
    Left = 104
    Top = 8
    Width = 177
    Height = 89
    Caption = 'Panel1'
    Color = clSkyBlue
    TabOrder = 1
    object Panel4: TPanel
      Left = 8
      Top = 8
      Width = 161
      Height = 73
      Caption = 'Panel2'
      TabOrder = 0
      object Label_dTube: TLabel
        Left = 1
        Top = 1
        Width = 159
        Height = 71
        Align = alClient
        Alignment = taCenter
        Caption = '279,5'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -64
        Font.Name = 'Times New Roman'
        Font.Style = []
        ParentFont = False
      end
    end
  end
  object ADOConnection1: TADOConnection
    ConnectionString = 
      'Provider=MSDASQL.1;Persist Security Info=False;Data Source=T2Sta' +
      'n5MyCon'
    LoginPrompt = False
    Provider = 'MSDASQL.1'
    Left = 528
    Top = 320
  end
  object ADOQuery1: TADOQuery
    Connection = ADOConnection1
    Parameters = <>
    Left = 560
    Top = 320
  end
  object TimerStart: TTimer
    OnTimer = TimerStartTimer
    Left = 8
    Top = 312
  end
end
