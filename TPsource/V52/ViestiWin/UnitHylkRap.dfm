object FormHylkRap: TFormHylkRap
  Left = 0
  Top = 0
  Caption = 'Hylk'#228'ysraportti'
  ClientHeight = 504
  ClientWidth = 619
  Color = clWindow
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Arial'
  Font.Style = []
  Menu = MainMenu1
  OldCreateOrder = False
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 14
  object Label1: TLabel
    Left = 13
    Top = 15
    Width = 42
    Height = 16
    Caption = 'KilpNo'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Arial'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label17: TLabel
    Left = 17
    Top = 37
    Width = 38
    Height = 16
    Caption = 'Osuus'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Arial'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object LblSeura: TLabel
    Left = 7
    Top = 60
    Width = 48
    Height = 16
    Caption = 'Joukkue'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Arial'
    Font.Style = []
    ParentFont = False
  end
  object Nimi: TLabel
    Left = 29
    Top = 83
    Width = 26
    Height = 16
    Caption = 'Nimi'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Arial'
    Font.Style = []
    ParentFont = False
  end
  object Sarja: TLabel
    Left = 25
    Top = 106
    Width = 30
    Height = 16
    Caption = 'Sarja'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Arial'
    Font.Style = []
    ParentFont = False
  end
  object Label2: TLabel
    Left = 234
    Top = 43
    Width = 90
    Height = 15
    Caption = 'Asian avausaika'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Arial'
    Font.Style = []
    ParentFont = False
  end
  object Label3: TLabel
    Left = 234
    Top = 67
    Width = 77
    Height = 15
    Caption = 'Tallennusaika'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Arial'
    Font.Style = []
    ParentFont = False
  end
  object Label4: TLabel
    Left = 22
    Top = 375
    Width = 54
    Height = 15
    Caption = 'Lis'#228'tietoa'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Arial'
    Font.Style = []
    ParentFont = False
  end
  object Label5: TLabel
    Left = 22
    Top = 327
    Width = 148
    Height = 15
    Caption = 'Lis'#228'tietoa (max. 9 merkki'#228')'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Arial'
    Font.Style = []
    ParentFont = False
  end
  object Label6: TLabel
    Left = 25
    Top = 127
    Width = 27
    Height = 16
    Caption = 'Emit'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Arial'
    Font.Style = []
    ParentFont = False
  end
  object Label7: TLabel
    Left = 270
    Top = 271
    Width = 177
    Height = 15
    Caption = 'Toimitsijan nimi tai nimikirjaimet'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Arial'
    Font.Style = []
    ParentFont = False
  end
  object Label8: TLabel
    Left = 133
    Top = 106
    Width = 27
    Height = 16
    Caption = 'Rata'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Arial'
    Font.Style = []
    ParentFont = False
  end
  object LblRapLkm: TLabel
    Left = 512
    Top = 112
    Width = 66
    Height = 49
    AutoSize = False
    Caption = 'Raportti tallennettu 2 kansioon'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Arial'
    Font.Style = []
    ParentFont = False
    Visible = False
    WordWrap = True
  end
  object EdtKilpno: TEdit
    Left = 63
    Top = 12
    Width = 61
    Height = 26
    Alignment = taRightJustify
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -15
    Font.Name = 'Arial'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 0
    Text = '0'
    OnExit = EdtKilpnoExit
  end
  object CBOsuus: TComboBox
    Left = 63
    Top = 35
    Width = 73
    Height = 23
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Arial'
    Font.Style = []
    ParentFont = False
    TabOrder = 1
    Text = '1. osuus'
    OnChange = CBOsuusChange
    OnExit = CBOsuusExit
    Items.Strings = (
      '1. osuus'
      '2. osuus'
      '3. osuus'
      '4. osuus'
      '5. osuus'
      '6. osuus'
      '7. osuus')
  end
  object EdtSeura: TEdit
    Left = 62
    Top = 57
    Width = 156
    Height = 24
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Arial'
    Font.Style = []
    ParentFont = False
    ReadOnly = True
    TabOrder = 2
  end
  object EdtNimi: TEdit
    Left = 62
    Top = 80
    Width = 156
    Height = 24
    TabStop = False
    Enabled = False
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Arial'
    Font.Style = []
    ParentFont = False
    ReadOnly = True
    TabOrder = 3
  end
  object CBNimihaku: TCheckBox
    Left = 181
    Top = 10
    Width = 140
    Height = 17
    Caption = 'Hae joukkueen nimell'#228
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Arial'
    Font.Style = []
    ParentFont = False
    TabOrder = 4
    Visible = False
    OnClick = CBNimihakuClick
  end
  object EdtSarja: TEdit
    Left = 62
    Top = 103
    Width = 68
    Height = 23
    TabStop = False
    Enabled = False
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Arial'
    Font.Style = []
    ParentFont = False
    ReadOnly = True
    TabOrder = 5
  end
  object RGLuonne: TRadioGroup
    Left = 22
    Top = 155
    Width = 226
    Height = 159
    Caption = ' Tapauksen luonne '
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Arial'
    Font.Style = []
    Items.Strings = (
      'Aiheeton (poistettu k'#228'sittelyst'#228')'
      'Keskeytt'#228'nyt'
      'Ep'#228'onnistunut kortin luenta'
      'Puuttuva leima'
      'V'#228#228'r'#228' leima'
      'V'#228#228'r'#228' leimausj'#228'rjestys'
      'Muu syy')
    ParentFont = False
    TabOrder = 6
  end
  object EdtAvausaika: TEdit
    Left = 329
    Top = 40
    Width = 120
    Height = 22
    HelpContext = 9
    TabOrder = 7
  end
  object EdtTallennusaika: TEdit
    Left = 329
    Top = 64
    Width = 120
    Height = 22
    HelpContext = 9
    TabOrder = 8
  end
  object RGRatk_1: TRadioGroup
    Left = 270
    Top = 174
    Width = 226
    Height = 93
    Caption = ' Toimitsijan ratkaisu '
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Arial'
    Font.Style = []
    ItemIndex = 0
    Items.Strings = (
      'Ei ratkaisua'
      'Hyv'#228'ksytty'
      'Hyl'#228'tty'
      'Keskeytt'#228'nyt')
    ParentFont = False
    TabOrder = 9
  end
  object RGKilpKanta: TRadioGroup
    Left = 270
    Top = 316
    Width = 226
    Height = 72
    Caption = ' Kilpailijan kanta '
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Arial'
    Font.Style = []
    ItemIndex = 0
    Items.Strings = (
      'Ei ilmoittanut'
      'Kilpailija hyv'#228'ksyy'
      'Kilpailija kiist'#228#228)
    ParentFont = False
    TabOrder = 10
  end
  object Memo1: TMemo
    Left = 22
    Top = 397
    Width = 226
    Height = 88
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Arial'
    Font.Style = []
    ParentFont = False
    TabOrder = 11
  end
  object EdtSelitys: TEdit
    Left = 22
    Top = 349
    Width = 102
    Height = 22
    HelpContext = 9
    TabOrder = 12
  end
  object RGTuom: TRadioGroup
    Left = 270
    Top = 394
    Width = 226
    Height = 91
    Caption = ' Tuomarineuvoston ratkaisu '
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Arial'
    Font.Style = []
    ItemIndex = 0
    Items.Strings = (
      'Ei ratkaisua'
      'Hyv'#228'ksytty'
      'Hyl'#228'tty'
      'Keskeytt'#228'nyt')
    ParentFont = False
    TabOrder = 13
  end
  object BtnTallennaTulosta: TButton
    Left = 511
    Top = 350
    Width = 99
    Height = 137
    Caption = 'Tallenna tiedot ja tulosta raportti'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Arial'
    Font.Style = []
    ParentFont = False
    TabOrder = 14
    WordWrap = True
    OnClick = BtnTallennaTulostaClick
  end
  object BtnTulosta: TButton
    Left = 511
    Top = 186
    Width = 99
    Height = 62
    Caption = 'Tulosta raportti tallentamatta tietoja'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Arial'
    Font.Style = []
    ParentFont = False
    TabOrder = 15
    WordWrap = True
    OnClick = BtnTulostaClick
  end
  object BtnTallenna: TButton
    Left = 512
    Top = 264
    Width = 99
    Height = 63
    Caption = 'Tallenna tiedot'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Arial'
    Font.Style = []
    ParentFont = False
    TabOrder = 16
    WordWrap = True
    OnClick = BtnTallennaClick
  end
  object RGXmlLuku: TRadioGroup
    Left = 460
    Top = 10
    Width = 150
    Height = 81
    Caption = ' XML-raportin lukeminen '
    Enabled = False
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Arial'
    Font.Style = []
    ItemIndex = 0
    Items.Strings = (
      'Ei yritetty'
      'Onnistunut'
      'Ei onnistunut')
    ParentFont = False
    TabOrder = 17
  end
  object RGPiikit: TRadioGroup
    Left = 270
    Top = 95
    Width = 226
    Height = 75
    Caption = ' Piikkikuvio '
    Columns = 2
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Arial'
    Font.Style = []
    ItemIndex = 0
    Items.Strings = (
      'Ei tarkastettu'
      'Ei korttia'
      'OK'
      'J'#228'lki puuttuu'
      'Ep'#228'selv'#228)
    ParentFont = False
    TabOrder = 18
  end
  object EdtBadge: TEdit
    Left = 62
    Top = 124
    Width = 90
    Height = 23
    TabStop = False
    Enabled = False
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Arial'
    Font.Style = []
    ParentFont = False
    TabOrder = 19
  end
  object BitBtn1: TBitBtn
    Left = 413
    Top = 5
    Width = 31
    Height = 31
    Margins.Left = 0
    Margins.Right = 0
    Margins.Bottom = 0
    Glyph.Data = {
      FE0A0000424DFE0A00000000000036000000280000001E0000001E0000000100
      180000000000C80A000000000000000000000000000000000000FFFFFFFFFFFF
      FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFCBB5A9C3A89EB297
      8DA689849B857F968D839391869C9A90ABA8A0BFB9B4FFFFFFFFFFFFFFFFFFFF
      FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF0000FFFFFFFFFFFFFFFFFFFF
      FFFFFFFFFFFFFFFFFFFFFFFFFFFFB3A393A48C78A88B76BC9C89D8B7A7F1D2C3
      FFE5D5FBEAD7EADCC9CFC1AFAEA191988D7F9C9288B7ACA4FFFFFFFFFFFFFFFF
      FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF0000FFFFFFFFFFFFFFFFFFFFFFFFFFFF
      FFFFFFFFB5A69D9F8E7BB9A489DFC8A8FFEDCAFFFFDFFFFFE3FFFFE3FFFFE4FF
      FDDFFFFFE7FFFFE9FFFFE9FDE9D0D4C3AEAFA18F978C7EFFFFFFFFFFFFFFFFFF
      FFFFFFFFFFFFFFFFFFFFFFFF0000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFCCB8AD
      B19786FBDDC0FFEFCBFFFFD7FFFFD5FFECC0EFD7ADE9D4AEF6DCB7FFDFB9FFDD
      B6FFE4BEFFF8D4FFFFE3FFFFE0F6E1C5D3C3ACB2A793C4BBADFFFFFFFFFFFFFF
      FFFFFFFFFFFFFFFF0000FFFFFFFFFFFFFFFFFFFFFFFFD9C8BFCAAE9DF4CFB3FF
      F7D1FFE9BBFFD29FF1B884D8A370CE9D6DD6A97DE8B98DF2BC8DFCC493FECA9B
      FACC9DF8CFA2FFDEB3FFF8D3FFFFE4DAC9AEC7BAA4D7CEC0FFFFFFFFFFFFFFFF
      FFFFFFFF0000FFFFFFFFFFFFFDFFFFD4CAC3BCA798F5D2B8FFFFDFFFC695EFAC
      75DD9559DB9357E19C63DF9F69CF9564C78C5BDC9A65DF9A61DC9C65E2A76FEB
      B782F7CA97FFD9A9FFE0B5FFFFE3D9C7AACBBDAAF7EEE1FFFFFBFFFEFFFFFDFF
      0000FFFFFFFFFFFFF6F1EEAB9B8FFFF5DEFFF9D7F3B587D48A50DF8F4EE4914D
      E59451D28749DA975EF9BC8AF3B481EDA468DE9152EBA161DF9A5BCD8E51DEA6
      6BEDBC84F1C693F8D5A9FFFFE3F1DCC1B4A694FFFEF5FFFBFAFFFBFF0000FFFF
      FFFCF6F7CCBAB3DCBEADFFFBDFF2B48BD0854DDB8543E28841E38A40E6944CE0
      9956FFC185FFFFCCFFFFD2FFD092F3A05CEC9B57E99A57DC934FE4A05FE5A669
      E4AD74F9CA97FDD7ADFFF6D7D1BCA6EFE1D5FFFCF9FFFFFF0000FFFFFFF5DAD6
      D1AB9FFFF1DCFFD3B0C67746C36A2DDF8138E48738E28A3DE09448D39853ECBF
      82FFFECCFFFFD4FFEAACF9A35DE7914BEE9852EB9650E89851DD924ED89354EF
      B17BDCAA7CFFF5D2FFEACFCBB9A8FFFBF3FFFFFF0000FFFFFFC7AAA1FFDAC9FF
      FEE1CA8056BD642BDB7834EC8536ED8734E48535DB883DC68645DDAE76FFF3C6
      FFFFD0FFCE8FEA924CE68C45F2964FF2944BEF934AE28A43DD8947DF965AE1A0
      6DFAC69EFFFFE6C6AD99F2E4D8FFFFFF0000F9F9EDA79B89FFFFE8FAC6A19C55
      23BF6525EA8137E3701FF5802BFC8D3DF49048CB793FBE7C4CCC9874C48D66E4
      9A60D37C39E68B46F4954CEE8B41F48E45F08A42E88743D98043F0A36BD29367
      FFF8D6F3D5BCC5B5A4FFFFFF0000DEDECCCDBFA8FFFAD9B67F54B2672FC66521
      E57628EE7420F57A24EC7929E17A35C7743DE19C74FFE5CAFFFAD9DC925CD07A
      38E68B46F18F47EB8239F3863CF3843BEF833CDB7737EB945BC78251F4C098FF
      FFE7A89782FCF6E90000D0BAA8FFEAD0FFE0BCAA6435C76F33D36922DF6918EA
      6F17EF7620E47827D57A35BC7942D6A57DFFF6DBFFFFE6FFBF8ADE8A49DE8340
      EB8641EC7E36F27F36F27A32F07C35DD722FD7793ED38655CF976EFFFFE6C1AF
      98E2DDCE0000C1A08CFFF7DBFFD1ADB96B3CB05419D36A27D16317D36716D873
      24D77E34D18747BE8450D8AC83FFF6D7FFFFE6FFEFC4D99764C4773FD77D42E4
      7E3DEC7B38EB722CF07731E16E2BCB6628DF8B57CC8C63FFEDCEF1D9C1CAC0AF
      0000B39278FFFFE2FFC199A65627C26A34E3874CDB8245EFA061DA9458ECAF77
      DEA873D09C6DD09B70FFDCB3FFFEDCFFFFEAFFECD4CC9A7CB6734CD78250E680
      45EA7937EA712CE66F2AD6692BCA6C37C67A50FFF9DAF0CBB7E8D0C40000BEA0
      85FFFEDEFFC8A2B86A3FEE9A6AFFAD7AFFB783E6A775E2AE7FBE9165B3895ECC
      9C72D6A176EEAE85FFE8C7FFFCECFFFEF3F8D4C2E5B193C77E52D1763FE87E3E
      E97633EB7633D26527C66833CB7D52FFFBDEF2CAB7DBC0B60000BA9F8AFFF2D7
      F7BE9EB7704AF5A77CFFB486FBB78AE7AD83D8AA81CAA37DDBB592C19774C797
      73CB916ED1A185F6DFD0FFFBEEFFFFEDFFF0D0D69267CD7741E58343DE7532E1
      7633D26D2FC16B37BE794EFFFCE0E9C8B5D5C0B80000C6AE9CFFF3DBFFD3B7BA
      7B59F5B089FFBE97E7A882DBA783DDB291FEDABCFFF1D4D6B195BE9477BE8C6E
      B3876FBFAA9BE2D2C5FFFFECFFFFE3FFC89CD48651DC8144D2712FD16F2FD77B
      40BF703DB5774FFFFCE1D3B7A6E7D8CF0000D4C1B4EFCFBCFFF4DDAA7456F9BC
      9CFFC7A7E7B190D1A286FAD5BBFFF9E2FFF5E0FFE9D2C9A48EB78B74C9A49091
      8171C2B3A3FFF3DBFFFFE2FFFBD1D7915CCE7C42D37C3ECE763ACF7E45B26F3E
      C7926DFFFFE8B09C8BF8EFE60000ECDBD2B99F8FFFFFEBA5765BE6B196EDB99B
      EDBDA1DDB39CECC9B5FFF8E7FFFFEEFFFFEEDEBDAAC8A08DB796839C897ACFBF
      AEFFF7DFFFFFE2FFFFD7D39160C87D45DF8E55D7884FBB723EAD7044F4C6A4FF
      F7DEA49484FFFFFF0000FFFFFFBCA294FFF0DDEAC0A9C7987DE4B59AF8CAB2E7
      C0AAD1B09DFADDCEFFFFF1FFF9EAFFE1D1F2CFBBC5A592D8C1B1FFE9D7FFFDE4
      FFFFE3FFD4ACD39567CE8853DA925CD58F5AB17141BC885FFFF2D2E7CCB7C6B6
      A9FFFFFF0000FFFFFFC7B2A3E8CAB7FFFFEA9C735ADFB39BF3C9B2E0B8A5E3C2
      AFDCBBACFFE7D8FFFFF0FFFFEEFFF0D9FFFEE7FFF8E4FFFFEEFFF8E0FFFFE0DA
      A580D99C70D59363CB8857CA8A5AB2784ED6A682FFFFE7B89F8BF2E4D8FFFFFF
      0000FFFFFFFFFFFFB89F8BFFF7DEFFE3C7A37C60F4CCB3F0C9B3E4BEACD8B4A2
      E0BCAAFCD9C5FFF8E0FFFFE6FFFEE4FFFBE5FFF5E2FFDFC7ECBA9CD39B78D093
      6BD8986FDE9E75B97C54AF7A55FFFFE4FFDCC4B89F8FFFFFF7FFFFFF0000FFFF
      FFFFFFFFD7C2ADCCB197FFFFE3DDBB9EB08B71F7D0BAF2CAB7E9C1AEE2BAA7E0
      BAA2E4BDA1E5BF9FE5BE9EECC2ABEABBA5D4A28BBE8A6CCA9171E1A581D79975
      B67A56B57F5CFFF1D1FFE1C8C7A794EFD9CDFFFFFFFFFFFF0000FFFFFFFFFFFF
      FFFFFFBCA88FEED6BAFEE1C6E2C0A8B08973E7BDAAFFDFCCF9CFB8E2B79CE2B9
      99E9C29CE7BF9CD7A88DD7A38CE3AD94F5BFA1F1BA9BD59C7CB98362AE7958FF
      DFC0FFFCE0C8A793D5BCACFFFFFFFFFFFFFFFFFF0000FFFFFFFFFFFFFFFFFFFF
      FFFFA39278F1D9C1FFF5DEE8C3AFC09683B38875DFB39CFFDFC2FFDEBDF2CBA4
      E9C099FBC9ADF6C0A7EFBA9FDBA68BB58163A67456D7A889FFECCFFFF8DFCFAF
      98D0B6A5FFFEF1FFFFFFFFFFFFFFFFFF0000FFFFFFFFFFFFFFFFFFFFFFFFFFFF
      FFBDAE9BE1C9B7FFFFF0FFE9DAE2B6A5B98C77B18469C19774D2AC82DDB58BD6
      A486CE997EB47F64A27054C19276FCD2B5FFF4D8FFF2D8C8AC94CEB9A4FFF5E5
      FFFFFFFFFFFFFFFFFFFFFFFF0000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
      CEBBACB29587FAD4C8FFFFF0FFFCEAFFDEC3F0C7A6D7B489C29C72C19070DFAA
      8FFFCDB1FFEBCFFFFFE2FFF7DBE6C7AEAD957DDFCCB7FFFFFFFFFFFFFFFFFFFF
      FFFFFFFFFFFFFFFF0000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
      FFFFCAA59DB1877BEFC3B2FFFCE3FFFBDBFFF4CCFFFFD7FFFFE1FFFFE3FFFFE5
      FFE9CCD7B296B29479D3BBA3FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
      FFFFFFFF0000F1F8FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFD
      F7FFEFE6EFC4B5CAA089C19B7BC6A27CC7A47CCEA180BD8F70BA8E71D9B395FF
      E2C8FFFFE7FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
      0000}
    TabOrder = 20
    TabStop = False
    OnClick = BitBtn1Click
  end
  object EdtToimitsija: TEdit
    Left = 270
    Top = 289
    Width = 181
    Height = 22
    HelpContext = 9
    TabOrder = 21
  end
  object EdtRata: TEdit
    Left = 165
    Top = 103
    Width = 65
    Height = 23
    TabStop = False
    Enabled = False
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Arial'
    Font.Style = []
    ParentFont = False
    ReadOnly = True
    TabOrder = 22
  end
  object RELisaTieto: TRichEdit
    Left = 162
    Top = 348
    Width = 102
    Height = 53
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Arial'
    Font.Style = []
    ParentFont = False
    TabOrder = 23
    Visible = False
    Zoom = 100
  end
  object MainMenu1: TMainMenu
    Left = 376
    Top = 16
    object iedosto1: TMenuItem
      Caption = '&Tiedosto'
      object ValitseHakemisto1: TMenuItem
        Caption = 'Valitse hylk'#228'ysraporttien hakemisto'
        OnClick = ValitseHakemisto1Click
      end
      object Avaahylkysraportti1: TMenuItem
        Caption = 'Avaa hylk'#228'ysraportti'
        OnClick = Avaahylkysraportti1Click
      end
      object allennanytetyttiedothylkysraporttiin1: TMenuItem
        Caption = 'Tallenna n'#228'ytetyt tiedot hylk'#228'ysraporttiin'
        OnClick = allennanytetyttiedothylkysraporttiin1Click
      end
      object Suljekaavake1: TMenuItem
        Caption = 'Sulje kaavake'
        OnClick = Suljekaavake1Click
      end
    end
    object Valinnat1: TMenuItem
      Caption = 'Valinnat'
      object Valitsekirjoitin1: TMenuItem
        Caption = 'Valitse kirjoitin'
        OnClick = Valitsekirjoitin1Click
      end
    end
  end
  object OpenDialog1: TOpenDialog
    Left = 328
    Top = 16
  end
end
