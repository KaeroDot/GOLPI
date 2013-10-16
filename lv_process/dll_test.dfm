object Form1: TForm1
  Left = 0
  Top = 0
  ClientHeight = 432
  ClientWidth = 752
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object BtnRun: TButton
    Left = 8
    Top = 16
    Width = 75
    Height = 25
    Caption = 'Run'
    TabOrder = 0
    OnClick = BtnRunClick
  end
  object Memo: TMemo
    Left = 8
    Top = 47
    Width = 736
    Height = 350
    Align = alCustom
    Anchors = [akLeft, akTop, akRight, akBottom]
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
    ReadOnly = True
    ScrollBars = ssVertical
    TabOrder = 1
  end
  object Epath: TEdit
    Left = 89
    Top = 20
    Width = 473
    Height = 21
    Align = alCustom
    Anchors = [akLeft, akTop, akRight]
    TabOrder = 2
    Text = 'C:\Octave\Octave3.6.4_gcc4.6.2\bin\'
  end
  object Ecmd: TEdit
    Left = 568
    Top = 20
    Width = 176
    Height = 21
    Align = alCustom
    Anchors = [akTop, akRight]
    TabOrder = 3
    Text = 'octave.exe -q'
  end
  object Einput: TEdit
    Left = 8
    Top = 403
    Width = 580
    Height = 21
    Align = alCustom
    Anchors = [akLeft, akRight, akBottom]
    TabOrder = 4
    OnKeyPress = EinputKeyPress
  end
  object BtnCmd: TButton
    Left = 594
    Top = 403
    Width = 150
    Height = 21
    Align = alCustom
    Anchors = [akRight, akBottom]
    Caption = 'Command (write + read)'
    TabOrder = 5
    OnClick = BtnCmdClick
  end
  object Button1: TButton
    Left = 594
    Top = 360
    Width = 75
    Height = 25
    Caption = 'Terminate'
    TabOrder = 6
    OnClick = Button1Click
  end
  object timer: TTimer
    Interval = 50
    OnTimer = timerTimer
    Left = 16
    Top = 56
  end
end
