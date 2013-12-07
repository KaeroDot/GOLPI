object Form1: TForm1
  Left = 0
  Top = 0
  ClientHeight = 419
  ClientWidth = 743
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
    Width = 727
    Height = 337
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
    ExplicitWidth = 736
    ExplicitHeight = 350
  end
  object Epath: TEdit
    Left = 89
    Top = 20
    Width = 464
    Height = 21
    Align = alCustom
    Anchors = [akLeft, akTop, akRight]
    TabOrder = 2
    Text = 'C:\Octave\Octave3.6.4_gcc4.6.2\bin\'
    ExplicitWidth = 473
  end
  object Ecmd: TEdit
    Left = 559
    Top = 20
    Width = 176
    Height = 21
    Align = alCustom
    Anchors = [akTop, akRight]
    TabOrder = 3
    Text = 'octave.exe -q -i'
    ExplicitLeft = 568
  end
  object Einput: TEdit
    Left = 8
    Top = 390
    Width = 571
    Height = 21
    Align = alCustom
    Anchors = [akLeft, akRight, akBottom]
    TabOrder = 4
    OnKeyPress = EinputKeyPress
    ExplicitTop = 403
    ExplicitWidth = 580
  end
  object BtnCmd: TButton
    Left = 585
    Top = 390
    Width = 150
    Height = 21
    Align = alCustom
    Anchors = [akRight, akBottom]
    Caption = 'Command (write + read)'
    TabOrder = 5
    OnClick = BtnCmdClick
    ExplicitLeft = 594
    ExplicitTop = 403
  end
  object CBconsole: TCheckBox
    Left = 559
    Top = 2
    Width = 97
    Height = 17
    Align = alCustom
    Anchors = [akTop, akRight]
    Caption = 'Create console'
    TabOrder = 6
    ExplicitLeft = 568
  end
  object timer: TTimer
    Interval = 50
    OnTimer = timerTimer
    Left = 16
    Top = 56
  end
end
