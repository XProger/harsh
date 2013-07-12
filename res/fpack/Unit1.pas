unit Unit1;

interface

uses
  jxml, jimg, jutils,
  Windows, Messages, PNGImage, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, ZLib, ComCtrls, ExtCtrls, Buttons, StdCtrls, ExtDlgs, Font, Atlas,
  AppEvnts, Spin, Menus, ImgList, Grids, ValEdit, SysUtils, ShellApi, Level, TARGA;

type
  TRGB = record
    b, g, r : Byte;
  end;

  TArrayRGB = array [0..1] of TRGB;
  PArrayRGB = ^TArrayRGB;

  TRGBA = record
    b, g, r, a : Byte;
  end;

  TArrayRGBA = array [0..1] of TRGBA;
  PArrayRGBA = ^TArrayRGBA;

  TItemsArray = array of string;

  TFileItem = record
    Name   : AnsiString;
    Hash   : LongWord;
    Offset : Integer;
    Size   : Integer;
    CSize  : Integer;
  end;

  TSubFrame = record
    pl, pt, pr, pb : Single;
    sl, st, sr, sb : Single;
  end;

  TSubData = class
    Flags  : LongWord;
    Hash   : LongWord;
    Name   : string;
    Images : TAtlasImageArray;
    constructor Create;
    destructor Destroy; override;
    procedure Clear;
    procedure AddImage(Image: TAtlasImage);
    procedure Update; virtual;
    function GetSize: Integer; virtual;
    procedure Save(Stream: TStream); virtual;
  end;

  TFontData = class(TSubData)
    Font : record
        Name   : string;
        Size   : Integer;
        Italic : Boolean;
        Bold   : Boolean;
      end;
    Kerning : Integer;
    Margin  : TRect;
    GlyphStr : WideString;
    Graph    : array of string;
    constructor Create;
    procedure Update; override;
    function GetSize: Integer; override;
    procedure Save(Stream: TStream); override;
  end;

  TSpriteData = class(TSubData)
    FPS, Loop, X, Y : Integer;
    FrameSize : TSize;
    constructor Create;
    function GetSize: Integer; override;
    procedure SaveFrames(Stream: TStream);
  end;

  TCategoryPanelGroup = class(ExtCtrls.TCategoryPanelGroup)
  protected
    procedure CreateParams(var Params: TCreateParams); override;
  end;

  TForm1 = class(TForm)
    StatusBar1: TStatusBar;
    PageControl1: TPageControl;
    TabSheet1: TTabSheet;
    Splitter1: TSplitter;
    FontDialog1: TFontDialog;
    OpenPictureDialog1: TOpenPictureDialog;
    SaveDialog1: TSaveDialog;
    ApplicationEvents1: TApplicationEvents;
    PopupMenu1: TPopupMenu;
    Add1: TMenuItem;
    Font1: TMenuItem;
    Sprite1: TMenuItem;
    Remove1: TMenuItem;
    ImageList1: TImageList;
    TreeView1: TTreeView;
    Pack: TTabSheet;
    CategoryFont: TCategoryPanelGroup;
    CategoryPanel1: TCategoryPanel;
    CategoryPanel2: TCategoryPanel;
    SpeedButton1: TSpeedButton;
    Label1: TLabel;
    Edit1: TEdit;
    Memo1: TMemo;
    CategoryPanel3: TCategoryPanel;
    ListBox1: TListBox;
    CategorySprite: TCategoryPanelGroup;
    CategoryPanel6: TCategoryPanel;
    Edit2: TEdit;
    Label2: TLabel;
    CategoryPanel4: TCategoryPanel;
    PopupMenu2: TPopupMenu;
    Add2: TMenuItem;
    Remove2: TMenuItem;
    Label3: TLabel;
    SpinEdit1: TSpinEdit;
    SpinEdit2: TSpinEdit;
    SpinEdit3: TSpinEdit;
    SpinEdit4: TSpinEdit;
    SpinEdit5: TSpinEdit;
    Label4: TLabel;
    CategoryPanel5: TCategoryPanel;
    SpinEdit6: TSpinEdit;
    Label5: TLabel;
    ListBox2: TListBox;
    PopupMenu3: TPopupMenu;
    Add3: TMenuItem;
    Remove3: TMenuItem;
    OpenDialog1: TOpenDialog;
    SpeedButton2: TSpeedButton;
    ApplicationEvents2: TApplicationEvents;
    N1: TMenuItem;
    Save1: TMenuItem;
    ScrollBox1: TScrollBox;
    Image1: TImage;
    CheckBox1: TCheckBox;
    Label6: TLabel;
    SpinEdit7: TSpinEdit;
    SpinEdit8: TSpinEdit;
    TabSheet2: TTabSheet;
    CategoryPanelGroup1: TCategoryPanelGroup;
    CategoryPanel7: TCategoryPanel;
    ComboBox1: TComboBox;
    Label15: TLabel;
    Label16: TLabel;
    ComboBox2: TComboBox;
    PopupMenu4: TPopupMenu;
    Open1: TMenuItem;
    OpenBack1: TMenuItem;
    OpenDialog2: TOpenDialog;
    OpenDialog3: TOpenDialog;
    SpeedButton3: TSpeedButton;
    Load1: TMenuItem;
    OpenDialog4: TOpenDialog;
    SpeedButton4: TSpeedButton;
    New1: TMenuItem;
    GroupBox1: TGroupBox;
    SpinEdit18: TSpinEdit;
    Label18: TLabel;
    Label7: TLabel;
    SpinEdit9: TSpinEdit;
    GroupBox2: TGroupBox;
    SpinEdit17: TSpinEdit;
    Label17: TLabel;
    Label8: TLabel;
    SpinEdit10: TSpinEdit;
    SpinEdit11: TSpinEdit;
    Label9: TLabel;
    Label10: TLabel;
    SpinEdit12: TSpinEdit;
    SpinEdit13: TSpinEdit;
    Label11: TLabel;
    Label12: TLabel;
    SpinEdit14: TSpinEdit;
    SpinEdit15: TSpinEdit;
    Label13: TLabel;
    Label14: TLabel;
    SpinEdit16: TSpinEdit;
    Label19: TLabel;
    ComboBox3: TComboBox;
    ScrollBox2: TScrollBox;
    PaintBox1: TPaintBox;
    N2: TMenuItem;
    RemoveObject1: TMenuItem;
    AddObject1: TMenuItem;
    Acid1: TMenuItem;
    PortalA1: TMenuItem;
    GroupBox3: TGroupBox;
    ComboBox4: TComboBox;
    Label20: TLabel;
    ComboBox5: TComboBox;
    Label21: TLabel;
    Flower1: TMenuItem;
    Grass1: TMenuItem;
    Stone1: TMenuItem;
    Crystal1: TMenuItem;
    Label22: TLabel;
    ComboBox6: TComboBox;
    Bomb1: TMenuItem;
    DZone1: TMenuItem;
    procedure SpeedButton1Click(Sender: TObject);
    procedure FormCreate(Sender: TObject);
    procedure AtlasPack;
    procedure AtlasDraw;
    procedure FontDraw;
    procedure SpriteDraw;
    procedure Draw;
    procedure FormDestroy(Sender: TObject);
    procedure Memo1Change(Sender: TObject);
    procedure SpeedButton2Click(Sender: TObject);
    procedure BitBtn1Click(Sender: TObject);
    procedure SpinEditChange(Sender: TObject);
    procedure PopupMenu1Popup(Sender: TObject);
    procedure Font1Click(Sender: TObject);
    function AddSprite(const Name: string): TSpriteData;
    procedure Sprite1Click(Sender: TObject);
    procedure TreeView1Change(Sender: TObject; Node: TTreeNode);
    procedure PopupMenu2Popup(Sender: TObject);
    procedure SpinEdit5Change(Sender: TObject);
    procedure Add3Click(Sender: TObject);
    procedure Remove3Click(Sender: TObject);
    procedure PopupMenu3Popup(Sender: TObject);
    procedure ListBox2KeyDown(Sender: TObject; var Key: Word;
      Shift: TShiftState);
    procedure ListBox2Click(Sender: TObject);
    procedure ApplicationEvents2Idle(Sender: TObject; var Done: Boolean);
    procedure Save1Click(Sender: TObject);
    procedure Edit2Change(Sender: TObject);
    procedure SpinEdit6Change(Sender: TObject);
    procedure CheckBox1Click(Sender: TObject);
    procedure SpinEdit7Change(Sender: TObject);
    procedure Open1Click(Sender: TObject);
    procedure OpenBack1Click(Sender: TObject);
    procedure SpeedButton3Click(Sender: TObject);
    procedure ComboBox2Change(Sender: TObject);
    procedure ChangeLevelParams(Sender: TObject);
    procedure Remove1Click(Sender: TObject);
    procedure Load1Click(Sender: TObject);
    procedure SpeedButton4Click(Sender: TObject);
    procedure New1Click(Sender: TObject);
    procedure PaintBox1Paint(Sender: TObject);
    procedure PaintBox1MouseDown(Sender: TObject; Button: TMouseButton;
      Shift: TShiftState; X, Y: Integer);
    procedure PaintBox1MouseMove(Sender: TObject; Shift: TShiftState; X,
      Y: Integer);
    procedure PaintBox1MouseUp(Sender: TObject; Button: TMouseButton;
      Shift: TShiftState; X, Y: Integer);
    procedure RemoveObject1Click(Sender: TObject);
    procedure PopupMenu4Popup(Sender: TObject);
    procedure Acid1Click(Sender: TObject);
    procedure ComboBox4Change(Sender: TObject);
    procedure Image1MouseDown(Sender: TObject; Button: TMouseButton;
      Shift: TShiftState; X, Y: Integer);
    procedure PaintBox1DblClick(Sender: TObject);
  private
    { Private declarations }
  public
    { Public declarations }
  end;

  TLevelParams = record
    front, back : string;
    start, finish : TPoint;
    count, time, dir, cancel, bomb, puckman, crusher, brick, jumper, sticky, bridge : Integer;
    obj : array of record
      id   : Integer;
      Flag : Integer;
      Pos  : TPoint;
      case Integer of
        0 : (Size : TSize);
        1 : (Scale, Angle : Single);
      end;
  end;

var
  Form1 : TForm1;
  Atlas : TAtlas;

  NoUpdate : Boolean;

  LastTime : LongWord;

  NodeFonts, NodeSprites : TTreeNode;

  MouseDelta  : TPoint;
  MouseSender : TObject = nil;

  CurLevel : ^TLevelParams;

  level : array [1..3, 1..16] of TLevelParams;

  NewObjID : TLevelObjectID = OID_ACID;

  LevelObj : TLevel;
  CurObj   : TLevelObject;
  DragObj  : TLevelObject;
  DragMove : Boolean = False;
  DragPos  : TPoint;
  DragTrans : Boolean;
  DragAngle : Single;
  DragScale : Single;

procedure LevelStrip(const FileName, Dir: string; ConvData: Boolean);
procedure Convert(const PlatformName, DataPath, ResultDataPath: string; NoPlatformDir: Boolean = False);
procedure Pack(const BasePath, FileName: string);

implementation

{$R *.dfm}

function ExtractFileName(const Path: string): string;
var
  i : Integer;
begin
  Result := Path;
  for i := Length(Path) downto 1 do
    if (Path[i] = '\') or (Path[i] = '/') then
    begin
      Result := Copy(Path, i + 1, Length(Path));
      Exit;
    end;
end;

function GetItems(const BaseDir: string): TItemsArray;

  procedure AddDir(const Dir: string);
  var
    SR : TSearchRec;
  begin
    if FindFirst(BaseDir + Dir + '*', faAnyFile , SR) = 0 then
    begin
      repeat
        if (SR.Name = '') or (SR.Name[1] = '_') or (SR.Name[1] = '.') then
          continue;
        if SR.Attr and faDirectory = 0 then
        begin
          SetLength(Result, Length(Result) + 1);
          Result[Length(Result) - 1] := Dir + SR.Name;
        end else
          AddDir(Dir + SR.Name + '\');
      until FindNext(SR) <> 0;
      FindClose(SR);
    end;
  end;

begin
  Result := nil;
  AddDir('');
end;

procedure RunCmd(const cmd: string; show: Boolean = False);
var
  StartupInfo : TStartupInfo;
  ProcessInfo : TProcessInformation;
begin
  GetStartupInfo(StartupInfo);
  with StartupInfo do
  begin
    if show then
      wShowWindow := SW_SHOW
    else
      wShowWindow := SW_HIDE;
    dwFlags     := STARTF_USESHOWWINDOW;
  end;
  CreateProcess(nil, PChar(cmd),
    nil, nil, FALSE, CREATE_NEW_CONSOLE, nil, nil, StartupInfo, ProcessInfo);

  WaitForSingleObject(ProcessInfo.hProcess, INFINITE);
  CloseHandle(ProcessInfo.hProcess);
end;

procedure CheckDir(Path: string);
var
  i, j: Integer;
  CurDir: string;
begin
  if ExcludeTrailingBackslash(Path) = '' then
    Exit;
  Path := IncludeTrailingBackslash(Path);
  j := Length(Path);
  for i := 1 to j do
  begin
    CurDir := CurDir + Path[i];
    if Path[i] = '\' then
    begin
      if not DirectoryExists(CurDir) then
        if not CreateDir(CurDir) then
          Exit;
    end;
  end;
end;

procedure CleanBitmap(bmp: PArrayRGBA; bWidth, bHeight: Integer);
const
  SIZE = 4;
var
  x, y, ix, iy : Integer;
  Flag : Boolean;
begin
  for y := 0 to bHeight - 1 do
    for x := 0 to bWidth - 1 do
    begin
      Flag := True;

      for iy := y - SIZE to y + SIZE do
        if (iy >= 0) and (iy < bHeight) and Flag then
          for ix := x - SIZE to x + SIZE do
            if (ix >= 0) and (ix < bWidth) and (bmp[iy * bWidth + ix].a > 0) then
            begin
              Flag := False;
              break;
            end;

      if Flag then
        with bmp[y * bWidth + x] do
        begin
          r := 0;
          g := 0;
          b := 0;
        end;

    end;
end;

procedure LevelStrip(const FileName, Dir: string; ConvData: Boolean);
var
  bWidth, bHeight : Integer;
  bmp : PArrayRGBA;
  Name : string;
  HasAlpha : Boolean;

  function GetData(px, py, pw, ph: Integer; Seams: Boolean = False): PByteArray;
  var
    x, y, i : Integer;
    p, SeamData : PArrayRGBA;
    d : PByteArray;
  begin
    Result := GetMemory(pw * ph * 4);
    d := Result;
    for y := py to py + ph - 1 do
    begin
      p := @bmp[(bHeight - y - 1) * bWidth];
      for x := px to px + pw - 1 do
      begin
        d[0] := p[x].r;
        d[1] := p[x].g;
        d[2] := p[x].b;
        d[3] := p[x].a;
        d := @d[4];
      end;
    end;
  end;

  procedure SaveData(Data: PByteArray; pw, ph: Integer; const Name: string);
  var
    DestPNG : TPNGImage;
    x, y : Integer;
    p, d, a : PByteArray;
  begin
    d := Data;
    DestPNG := TPNGImage.CreateBlank(COLOR_RGB, 8, pw, ph);
    for y := 0 to ph - 1 do
    begin
      p := DestPNG.Scanline[y];
      for x := 0 to pw - 1 do
      begin
        p[0] := d[2];
        p[1] := d[1];
        p[2] := d[0];
        p := @p[3];
        d := @d[4];
      end;
    end;
    DestPNG.SaveToFile(Dir + Name + '.png');
    DestPNG.CompressionLevel := 9;
    DestPNG.Free;
    Writeln('- save part ', Name);

    if HasAlpha then
    begin
      d := Data;
{
      DestPNG := TPNGImage.CreateBlank(COLOR_RGBALPHA, 8, pw, ph);
      DestPNG.CreateAlpha;
      for y := 0 to ph - 1 do
      begin
        p := DestPNG.Scanline[y];
        a := DestPNG.AlphaScanline[y];
        for x := 0 to pw - 1 do
        begin
          p[x * 3 + 0] := 255;
          p[x * 3 + 1] := 255;
          p[x * 3 + 2] := 255;
          a[x] := d[3];
          d := @d[4];
        end;
      end;
}
      DestPNG := TPNGImage.CreateBlank(COLOR_GRAYSCALE, 8, pw, ph);
      for y := 0 to ph - 1 do
      begin
        p := DestPNG.Scanline[y];
        for x := 0 to pw - 1 do
        begin
          p[x] := d[3];
          d := @d[4];
        end;
      end;

      DestPNG.SaveToFile(Dir + Name + '_mask.png');
      DestPNG.CompressionLevel := 9;
      DestPNG.Free;
      Writeln('- save mask ', Name);
    end;
    FreeMemory(Data);
  end;

  function AppendData(Data1, Data2: PByteArray; pw, ph: Integer): PByteArray;
  var
    d : PByteArray;
  begin
    Result := GetMemory(pw * (ph + ph) * 4);
    d := Result;
    Move(Data1[0], d[0], pw * ph * 4);
    d := @d[pw * ph * 4];
    Move(Data2[0], d[0], pw * ph * 4);
    FreeMemory(Data1);
    FreeMemory(Data2);
  end;

  procedure BinMaskData(Data: PByteArray; pw, ph: Integer; const Name: string);
  const
    SCALE = 2;
  var
    i, x, y : Integer;
    Mask : PIntegerArray;
    m : PInteger;
    F : File;

    function Blocked(x, y: Integer): Boolean;
    var
      bx, by, c : Integer;
    begin
      c := 0;
      for by := 0 to SCALE - 1 do
        for bx := 0 to SCALE - 1 do
          if Data[((x * SCALE + bx) + (y * SCALE + by) * pw) * 4 + 3] > 25 then
            Inc(c);
      Result := c / SCALE * SCALE > 0.25;
    end;

  var
    MaskSize : Integer;
  begin
    MaskSize := (pw div SCALE) * ((ph div SCALE) div 8);
    Mask := GetMemory(MaskSize);
    m := PInteger(Mask);
    for x := 0 to (pw div SCALE) - 1 do
    begin
      for y := 0 to (ph div SCALE) div 32 - 1 do
      begin
        m^ := 0;
        for i := 0 to 31 do
          m^ := m^ or (Ord(Blocked(x, y * 32 + i)) shl i);
        Inc(m);
      end;
    end;

    AssignFile(F, Dir + Name + '.dat');
    Rewrite(F, 1);
    i := SCALE;
    BlockWrite(F, i, 4);
    BlockWrite(F, pw, 4);
    BlockWrite(F, ph, 4);
    BlockWrite(F, Mask^, MaskSize);
    CloseFile(F);

    Writeln('- save data ', Name);

    FreeMemory(Mask);
    FreeMemory(Data);
  end;

const
  S = 1024;
  H = 512;
var
  Stream : jutils.TStream;
begin
  Writeln('load image ', FileName);

  Stream := jutils.TFileStream.Create(FileName);
  LoadIMG(Stream, PByteArray(bmp), bWidth, bHeight);
  Stream.Free;
  CleanBitmap(bmp, bWidth, bHeight);

  HasAlpha := True;
  {
  png := TPNGImage.Create;
  png.LoadFromFile(FileName);
  HasAlpha := png.Header.ColorType = COLOR_RGBALPHA;
//  png.CreateAlpha;

  bmp := TBitmap.Create;
  bmp.Assign(png);
  bmp.PixelFormat := pf32bit;
}


  Name := ExtractFileName(FileName);
  Delete(Name, Length(Name) - 3, 4);

  CheckDir(ExtractFileDir(Dir));

  SaveData(GetData(0, 0, S, S, True), S, S, Name + '_0');
  SaveData(GetData(S, 0, S, S, True), S, S, Name + '_1');
// нижн€€ полоска уровн€ разбиваетс€ на куски 1024x512 которые попарно склеиваютс€
// в одну 1024x1024, эта полоска включает 2px верхних тайлов, чтобы не поиметь жопу при
// фильтрации при выборке из текстур на стыках
  SaveData(AppendData(GetData(0, S - 2, S, H, True), GetData(S, S - 2, S, H, True), S, H), S, S, Name + '_2');

  if ConvData then
    BinMaskData(GetData(0, 0, bWidth, bHeight), bWidth, bHeight, Name);

  FreeMemory(bmp);
  //bmp.Free;
  //png.Free;
end;

procedure Convert(const PlatformName, DataPath, ResultDataPath: string; NoPlatformDir: Boolean);
var
  TexTool, CFormat : string;
  CompFlip : Boolean;

  procedure ConvertItems(const Items: TItemsArray; const DataPath, PlatformName: string);
  var
    i : Integer;
    cmd, Path : string;
  begin
    if NoPlatformDir then
      Path := ResultDataPath
    else
      Path := ResultDataPath + PlatformName + '\';

    for i := 0 to Length(Items) - 1 do
    begin
      CheckDir(ExtractFileDir(Path + Items[i]));

      if ExtractFileExt(Items[i]) = '.png' then
      begin
        Writeln('- convert: ', Items[i]);
        cmd := TexTool;

        if Pos('_mask.png', Items[i]) > 0 then
          cmd := cmd + ' -yflip0 -f8'//' -fOGL8888 -yflip0'
        else
          if Pos('_raw.png', Items[i]) > 0 then
          begin
            cmd := cmd + ' -yflip0 -fOGL8888';
          end else
          begin
            if CompFlip then
              cmd := cmd + ' -yflip1'
            else
              cmd := cmd + ' -yflip0';
            cmd := cmd + ' -f' + CFormat;
          end;

        cmd := cmd + ' -i' + DataPath + Items[i] + ' -o' + Path + Items[i];
        RunCmd(cmd);
      end else
      begin
        Writeln('- copy: ', Items[i]);
        CopyFile(PChar(DataPath + Items[i]), PChar(Path + Items[i]), False);
      end;
    end;
  end;

begin
  TexTool := ExtractFileDir(ParamStr(0)) + '\PVRTexTool.exe';

  CFormat := '';

  CompFlip  := False;

  if (PlatformName = 'win') or
     (PlatformName = 'nix') or
     (PlatformName = 'osx') or
     (PlatformName = 'wp') then
    CFormat := 'DXT1';

  if PlatformName = 'ios' then
    CFormat := 'OGLPVRTC4';

  if PlatformName = 'android' then
  begin
    CompFlip := False;
    CFormat := 'ETC';
  end;

  if CFormat = '' then
  begin
    Writeln('Invalid platform (win, nix, osx, ios, android, wp)');
    Exit;
  end;

  Writeln('Get _all');
  ConvertItems(GetItems(DataPath + '_all\'), DataPath + '_all\', PlatformName);
  Writeln('Get platform');
  ConvertItems(GetItems(DataPath), DataPath, PlatformName);
end;

procedure Pack(const BasePath, FileName: string);
const
  NULL : Byte = 0;
var
  Items : array of TFileItem;

  procedure List(const Path: string);
  var
    i : Integer;
    h  : THandle;
    fd : TWIN32FindData;
    str : AnsiString;
  begin
    h := FindFirstFile(PChar(BasePath + Path + '*'), fd);
    if h <> 0 then
    begin
      repeat
        if fd.cFileName[0] = '.' then
          continue;
        if fd.dwFileAttributes and FILE_ATTRIBUTE_DIRECTORY = 0 then
        begin
          SetLength(Items, Length(Items) + 1);
          str := AnsiString(Path + fd.cFileName);
          for i := 1 to Length(str) do
            if str[i] = '\' then
              str[i] := '/';
          Items[Length(Items) - 1].Name := str;
          Items[Length(Items) - 1].Hash := SFH(PAnsiChar(str), Length(str));
        end else
          List(Path + fd.cFileName + '\');
      until (not FindNextFile(h, fd));
      Windows.FindClose(h);
    end;
  end;

var
  i, j, HSize : Integer;
  Fout, Fin : File;
  Data, CData : Pointer;
begin
  List('');
  AssignFile(Fout, FileName);
  Rewrite(Fout, 1);
// hash check
  for i := 0 to Length(Items) - 1 do
    for j := 0 to Length(Items) - 1 do
      if (i <> j) and (Items[i].Hash = Items[j].Hash) then
      begin
        Writeln('! hash conflict: "', Items[i].Name, '" & "', Items[j].Name, '"');
        Readln;
      end;

// get header size
  HSize := (SizeOf(Items[0]) - SizeOf(Items[i].Name)) * Length(Items);

// data
  Seek(Fout, SizeOf(i) + SizeOf(HSize) + HSize);
  for i := 0 to Length(Items) - 1 do
  begin
    FileMode := 0;
    AssignFile(Fin, BasePath + string(Items[i].Name));
    Reset(Fin, 1);
    Items[i].Size   := FileSize(Fin);
    Items[i].Offset := FilePos(Fout);

    if Items[i].Size > 0 then
    begin
      Data := GetMemory(Items[i].Size);
      BlockRead(Fin, Data^, Items[i].Size);

      Write(Items[i].Hash, #9, Items[i].Name, #9, Items[i].Size, #9);
      ZCompress(Data, Items[i].Size, CData, Items[i].CSize, zcMax);
//      CompressBuf(Data, Items[i].Size, CData, Items[i].CSize);
      Write(Items[i].CSize, #9, Round(Items[i].CSize / Items[i].Size * 100), '%');

      //Items[i].CSize := Items[i].Size;

      if (Items[i].CSize / Items[i].Size) > 0.9 then  // if less 10% - don't need compression
      begin
        Items[i].CSize := Items[i].Size;
        BlockWrite(Fout, Data^, Items[i].Size);
        Write(#9'raw');
      end else
      begin
        Dec(Items[i].CSize, 6); // zlib header
        BlockWrite(Fout, PByteArray(CData)[2], Items[i].CSize);
      end;
      FreeMemory(CData);
      FreeMemory(Data);
    end else
      Items[i].CSize  := Items[i].Size;

    CloseFile(Fin);

    Writeln;
  end;

// header
  Seek(Fout, 0);
  i := Length(Items);
  BlockWrite(Fout, i, SizeOf(i));
// write items info
  for i := 0 to Length(Items) - 1 do
  begin
    BlockWrite(Fout, Items[i].Hash, 4);
    BlockWrite(Fout, Items[i].Offset, 4);
    BlockWrite(Fout, Items[i].Size, 4);
    BlockWrite(Fout, Items[i].CSize, 4);
  end;
  Writeln('---- ', FileName, #9, FileSize(Fout));
  CloseFile(Fout);
end;

function GetHash(const s: string): LongWord;
begin
  Result := SFH(PAnsiChar(AnsiString(s)), Length(s));
end;

{ TCategoryPanelGroup }
procedure TCategoryPanelGroup.CreateParams(var Params: TCreateParams);
begin
  inherited;
  Params.Style := Params.Style and not WS_BORDER;
end;

{$REGION 'TSubData'}
constructor TSubData.Create;
begin
  Update;
end;

destructor TSubData.Destroy;
begin
  Clear;
  inherited;
end;

procedure TSubData.Clear;
var
  i : Integer;
begin
  for i := 0 to Length(Images) - 1 do
  begin
    Atlas.DelImage(Images[i]);
    Images[i].Free;
  end;
  Images := nil;
end;

procedure TSubData.AddImage(Image: TAtlasImage);
begin
  SetLength(Images, Length(Images) + 1);
  Images[Length(Images) - 1] := Image;
  Atlas.AddImage(Images[Length(Images) - 1]);
end;

procedure TSubData.Update;
begin
  Form1.AtlasPack;
end;

function TSubData.GetSize: Integer;
begin
  Result := 0;
end;

procedure TSubData.Save(Stream: TStream);
var
  Hash : LongWord;
  Size : LongInt;
begin
  Hash := GetHash(Name);
  Size := GetSize;
  Stream.Write(Hash, SizeOf(Hash));
  Stream.Write(Size, SizeOf(Size));
end;
{$ENDREGION}

{$REGION 'TFontData'}
constructor TFontData.Create;
begin
  Font.Name   := 'Arial';
  Font.Size   := 16;
  Font.Bold   := False;
  Font.Italic := False;
  GlyphStr := ' :;=+-_()[]{}'#13 +
              '*"''%@!?,.<>/|\'#13 +
              '0123456789'#13 +
              'abcdefghijklmnopqrstuvwxyz'#13 +
              'ABCDEFGHIJKLMNOPQRSTUVWXYZ'#13 +
              'абвгдеЄжзийклмнопрстуфхцчшщъыьэю€'#13 +
              'јЅ¬√ƒ≈®∆«»… ЋћЌќѕ–—“”‘’÷„ЎўЏџ№Ёёя';
  inherited;
end;

procedure TFontData.Update;
var
  i : Integer;
  FontGen : TFontGen;
  Glyph   : TFontGlyph;
begin
  Name := Font.Name + '_' + IntToStr(Font.Size);
  if Font.Bold or Font.Italic then
  begin
    Name := Name + '_';
    if Font.Bold then Name := Name + 'b';
    if Font.Italic then Name := Name + 'i';
  end;

  Clear;

  FontGen := TFontGen.Create(Font.Name, Font.Size, Font.Bold, Font.Italic);
  for i := 1 to Length(GlyphStr) do
  begin
    if CharInSet(GlyphStr[i], [#10, #13]) then
      continue;
    Glyph := FontGen.GetGlyph(GlyphStr[i]);
    AddImage(TAtlasImage.Create(Glyph.ID, GetHash(Name + '\' + Glyph.ID), 0, Glyph.OffsetY, Glyph.Width, Glyph.Height, Glyph.Data));
  end;
  FontGen.Free;

  inherited;
end;

function TFontData.GetSize: Integer;
begin
  Result := 2 + 2 + SizeOf(Kerning) + 2 + Length(GlyphStr) * 2;
end;

procedure TFontData.Save(Stream: TStream);
var
  i, j, Count : LongInt;
  Frame : TSubFrame;
  MinID, MaxID : Integer;
  img : TAtlasImage;
begin
  inherited;
  MinID := High(Word);
  MaxID := 0;
  for i := 1 to Length(GlyphStr) do
  begin
    MinID := Min(MinID, Ord(GlyphStr[i]));
    MaxID := Max(MaxID, Ord(GlyphStr[i]));
  end;

  Stream.Write(MinID, SizeOf(MinID));
  Stream.Write(MinID, SizeOf(MinID));

  Count := Length(GlyphStr);
  Stream.Write(Kerning, SizeOf(Kerning));
  Stream.Write(Count, SizeOf(Count));
  Stream.Write(GlyphStr[1], Count * 2);

  for i := 1 to Length(GlyphStr) do
    for j := 0 to Length(Images) - 1 do
      if GlyphStr[i] = Images[j].Name then
      begin
        img := Images[i];
        if img.Link <> nil then
          img := img.Link;
        with img do
        begin
          Frame.pl := ox;
          Frame.pt := oy;
          Frame.pr := Frame.pl + Width;
          Frame.pb := Frame.pt + Height;
          Frame.sl := X / Atlas.Width;
          Frame.st := Y / Atlas.Height;
          Frame.sr := Frame.sl + Width / Atlas.Width;
          Frame.sb := Frame.st + Height / Atlas.Height;

          Frame.st := 1 - Frame.st;
          Frame.sb := 1 - Frame.sb;

          Stream.Write(Frame, SizeOf(Frame));
          break;
        end;
      end;
end;
{$ENDREGION}

{$REGION 'TSpriteData'}
constructor TSpriteData.Create;
begin
  FPS  := 25;
  Loop := 1;
end;

function TSpriteData.GetSize: Integer;
begin
  Result := SizeOf(FPS) + 4{Count} + SizeOf(Loop) + Length(Images) * SizeOf(TSubFrame);
end;

procedure TSpriteData.SaveFrames(Stream: TStream);
var
  i : LongInt;
  Frame : TSubFrame;
  img : TAtlasImage;
begin
  for i := 0 to Length(Images) - 1 do
  begin
    img := Images[i];
    if img.Link <> nil then
      img := img.Link;
    with img do
    begin
      Frame.pl := ox - Self.X;
      Frame.pt := oy - Self.Y;
      Frame.pr := Frame.pl + Width;
      Frame.pb := Frame.pt + Height;
      Frame.sl := X / Atlas.Width;
      Frame.st := Y / Atlas.Height;
      Frame.sr := Frame.sl + Width / Atlas.Width;
      Frame.sb := Frame.st + Height / Atlas.Height;

//      Frame.st := 1 - Frame.st;
//      Frame.sb := 1 - Frame.sb;

      Stream.Write(Frame, SizeOf(Frame));
    end;
  end;
end;
{$ENDREGION}

{ TForm1 }
procedure LoadLevelsXML;
var
  F : TextFile;
  XML : TXML;
  i, j : Integer;
  Text, s : string;
begin
  AssignFile(F, ExtractFileDir(ParamStr(0)) + '\levels.xml');
  Reset(F);
  Text := '';
  while not eof(F) do
  begin
    Readln(F, s);
    Text := Text + s;
  end;
  CloseFile(F);

  for i := 1 to 3 do
    for j := 1 to 16 do
      with level[i][j] do
      begin
        front := 'def_front.png';
        back  := 'def_back.png';
        obj   := nil;
        count := 0;
      end;


  XML := TXML.Create(Text, 1);
  for i := 0 to XML.Count - 1 do
    with XML.NodeI[i] do
      with level[StrToInt(Params['pack']), StrToInt(Params['index'])] do
      begin
        start   := Point(StrToInt(Params['sx']), StrToInt(Params['sy']));
        finish  := Point(StrToInt(Params['fx']), StrToInt(Params['fy']));
        front   := Params['front'];
        back    := Params['back'];
        count   := StrToInt(Params['count']);
        time    := StrToIntDef(Params['time'], 3);
        dir     := StrToIntDef(Params['dir'], 1);

        cancel  := StrToIntDef(Params['cancel'], 0);
        bomb    := StrToInt(Params['bomb']);
        puckman := StrToInt(Params['puckman']);
        crusher := StrToInt(Params['crusher']);
        brick   := StrToInt(Params['brick']);
        jumper  := StrToInt(Params['jumper']);
        sticky  := StrToInt(Params['sticky']);
        bridge  := StrToInt(Params['bridge']);

        obj := nil;
        for j := 0 to XML.NodeI[i].Count - 1 do
          if NodeI[j].Tag = 'obj' then
          begin
            SetLength(obj, Length(obj) + 1);
            with obj[Length(obj) - 1] do
            begin
              id      := StrToIntDef(NodeI[j].Params['id'], 0);
              Flag    := StrToIntDef(NodeI[j].Params['flag'], 0);
              Pos.X   := StrToIntDef(NodeI[j].Params['px'], 0);
              Pos.Y   := StrToIntDef(NodeI[j].Params['py'], 0);

              if TLevelObjectID(id) in [OID_STONE, OID_GRASS, OID_FLOWER, OID_BOMB, OID_DZONE] then
              begin
                Angle := StrToFloatDef(NodeI[j].Params['angle'], 0);
                Scale := StrToFloatDef(NodeI[j].Params['scale'], 1);
              end else
              begin
                Size.cx := StrToIntDef(NodeI[j].Params['sx'], 0);
                Size.cy := StrToIntDef(NodeI[j].Params['sy'], 0);
              end;
            {
              if id = ord(OID_PORTAL) then
              begin
                Size.cx := Pos.X + Size.cx;
                Size.cy := Pos.Y + Size.cy;
              end;
            }
            end;
          end;

      end;
  XML.Free;
end;

procedure UpdateLevelObj;
var
  i : Integer;
begin
  if CurLevel = nil then
    Exit;

  CurLevel^.obj := nil;
  SetLength(CurLevel^.obj, Length(LevelObj.Objects) - 2);

  CurLevel^.start  := LevelObj.Objects[0].Pos;
  CurLevel^.finish := LevelObj.Objects[1].Pos;

  for i := 2 to Length(LevelObj.Objects) - 1 do
  begin
    CurLevel^.obj[i - 2].id   := Ord(LevelObj.Objects[i].ID);
    CurLevel^.obj[i - 2].flag := Ord(LevelObj.Objects[i].Flag);
    CurLevel^.obj[i - 2].Pos  := LevelObj.Objects[i].Pos;
    CurLevel^.obj[i - 2].Size := LevelObj.Objects[i].Param.Size;
  end;
end;

procedure SaveLevelsXML;
var
  i, j, k : Integer;
  F : TextFile;
begin
  UpdateLevelObj;

  AssignFile(F, ExtractFileDir(ParamStr(0)) + '\levels.xml');
  Rewrite(F);

  Writeln(F, '<levels>');
  for i := 1 to 3 do
  begin
    for j := 1 to 16 do
      with level[i][j] do
      begin
        Writeln(F, '  <level pack="', i,
                '" index="', j,
                '" sx="', start.X,
                '" sy="', start.Y,
                '" fx="', finish.X,
                '" fy="', finish.Y,
                '" front="', front,
                '" back="', back,
                '" count="', count,
                '" time="', time,
                '" dir="', dir,

                '" cancel="', cancel,
                '" bomb="', bomb,
                '" puckman="', puckman,
                '" crusher="', crusher,
                '" brick="', brick,
                '" jumper="', jumper,
                '" sticky="', sticky,
                '" bridge="', bridge, '">');
        for k := 0 to Length(obj) - 1 do
        begin
          Write(F, '    <obj id="', obj[k].id,
                            '" flag="', obj[k].Flag,
                            '" px="', obj[k].Pos.X,
                            '" py="', obj[k].Pos.Y);
          if TLevelObjectID(obj[k].id) in [OID_STONE, OID_GRASS, OID_FLOWER, OID_BOMB, OID_DZONE] then
            Writeln(F, '" scale="', obj[k].Scale:0:4, '" angle="', obj[k].Angle:0:4, '" />')
          else
            Writeln(F, '" sx="', obj[k].Size.cx, '" sy="', obj[k].Size.cy, '" />')
        end;
        Writeln(F, '  </level>');
      end;
  end;
  Writeln(F, '</levels>');

  CloseFile(F);
end;

procedure TForm1.FormCreate(Sender: TObject);
begin
  DecimalSeparator := '.';

  TreeView1.Items.Add(nil, 'Atlas');
  NodeFonts   := TreeView1.Items.AddChild(TreeView1.Items[0], 'Fonts');
  NodeSprites := TreeView1.Items.AddChild(TreeView1.Items[0], 'Sprites');
  TreeView1.Items[0].Expanded := True;

  NodeFonts.ImageIndex    := 1;
  NodeFonts.SelectedIndex := 1;

  NodeSprites.ImageIndex    := 3;
  NodeSprites.SelectedIndex := 3;

  Atlas := TAtlas.Create;
  lastTime := GetTickCount;

  LevelObj := TLevel.Create;

  LoadLevelsXML;
  ComboBox2Change(Sender);
end;

procedure TForm1.FormDestroy(Sender: TObject);
begin
  while NodeFonts.Count > 0 do
  begin
    TFontData(NodeFonts[0].Data).Free;
    NodeFonts[0].Free;
  end;

  while NodeSprites.Count > 0 do
  begin
    TSpriteData(NodeSprites[0].Data).Free;
    NodeSprites[0].Free;
  end;

  Atlas.Free;

  LevelObj.Free;
end;

procedure TForm1.Image1MouseDown(Sender: TObject; Button: TMouseButton;
  Shift: TShiftState; X, Y: Integer);
begin
  if TreeView1.Selected.Parent = NodeSprites then
  begin
    TSpriteData(TreeView1.Selected.Data).X := X;
    TSpriteData(TreeView1.Selected.Data).Y := Y;
    SpriteDraw;
  end;
end;

(*
procedure TForm1.Image3MouseDown(Sender: TObject; Button: TMouseButton;
  Shift: TShiftState; X, Y: Integer);
begin
  MouseDelta.X := X;
  MouseDelta.Y := Y;
  MouseSender := Sender;
end;

procedure TForm1.Image3MouseMove(Sender: TObject; Shift: TShiftState; X,
  Y: Integer);
var
  p : TPoint;
begin
  if MouseSender = Sender then
  begin
    GetCursorPos(p);
    p := ScrollBox2.ScreenToClient(p);
    TImage(Sender).Left := p.X - MouseDelta.X;
    TImage(Sender).Top  := p.Y - MouseDelta.Y;
  end;
end;

procedure TForm1.Image3MouseUp(Sender: TObject; Button: TMouseButton;
  Shift: TShiftState; X, Y: Integer);
begin
  if MouseSender <> nil then
  begin
    with level[ComboBox2.ItemIndex + 1, ComboBox1.ItemIndex + 1], ScrollBox2 do
    begin
      start  := Point(Image3.Left + HorzScrollBar.Position, Image3.Top + VertScrollBar.Position);
      finish := Point(Image4.Left + HorzScrollBar.Position, Image4.Top + VertScrollBar.Position);
    end;
    SaveLevelsXML;
  end;
  MouseSender := nil;
end;
*)

procedure TForm1.ListBox2Click(Sender: TObject);
begin
  SpriteDraw;
end;

procedure TForm1.ListBox2KeyDown(Sender: TObject; var Key: Word;
  Shift: TShiftState);
begin
  if Key = VK_DELETE then
    Remove3.onClick(Sender);
end;

procedure TForm1.Memo1Change(Sender: TObject);
begin
  if (not NoUpdate) and (TreeView1.Selected <> nil) and (TreeView1.Selected.Parent = NodeFonts) then
    with TFontData(TreeView1.Selected.Data) do
    begin
      GlyphStr := Memo1.Text;
      Update;
    end;
end;

procedure TForm1.New1Click(Sender: TObject);
var
  i : Integer;
begin
  while NodeFonts.Count > 0 do
  begin
    TFontData(NodeFonts[0].Data).Free;
    NodeFonts[0].Delete;
  end;

  while NodeSprites.Count > 0 do
  begin
    TSpriteData(NodeSprites[0].Data).Free;
    NodeSprites[0].Delete;
  end;

  Atlas.Pack;
  Draw;
end;

procedure TForm1.Open1Click(Sender: TObject);
begin
  if OpenDialog2.Execute and LevelObj.SetFront(OpenDialog2.FileName) then
  begin
    CurLevel.front := OpenDialog2.FileName;
    PaintBox1.Repaint;
    SaveLevelsXML;
  end;
end;

procedure TForm1.OpenBack1Click(Sender: TObject);
begin
  if OpenDialog3.Execute and LevelObj.SetBack(OpenDialog3.FileName) then
  begin
    CurLevel.back := OpenDialog3.FileName;
    PaintBox1.Repaint;
    SaveLevelsXML;
  end;
end;

procedure TForm1.PaintBox1DblClick(Sender: TObject);
begin
  LevelObj.ToFront(CurObj);
end;

procedure TForm1.PaintBox1MouseDown(Sender: TObject; Button: TMouseButton;
  Shift: TShiftState; X, Y: Integer);
var
  i, tCount : Integer;
begin
  if Button = mbRight then
    Exit;

  if ssCtrl in Shift then
  begin
    DragPos := Point(X, Y);
    DragMove := True;
    Exit;
  end;

  if ssShift in Shift then
  begin
    DragObj := LevelObj.AddObj(Point(X, Y), NewObjID);
    if DragObj <> nil then
    begin
      if DragObj.ID in [OID_PORTAL, OID_ACID] then
      begin
        DragObj.Param.Size.cx := X;
        DragObj.Param.Size.cy := Y;
      end;

      if DragObj.ID = OID_ACID then
        DragObj.SelectID := 1;
    end;
  end else
    DragObj := LevelObj.GetObject(Point(X, Y));

  if DragObj <> nil then
    case DragObj.SelectID of
      0 : DragPos := Point(DragObj.Pos.X - X, DragObj.Pos.Y - Y);
      1 : DragPos := Point(DragObj.Param.Size.cx - X, DragObj.Param.Size.cy - Y);
      2 : DragPos := Point(DragObj.Param.Size.cx - X, DragObj.Pos.Y - Y);
      3 : DragPos := Point(DragObj.Pos.X - X, DragObj.Param.Size.cy - Y);
    end;

  DragTrans := Button = mbMiddle;

  if DragTrans and (DragObj <> nil) then
    if DragObj.ID in [OID_STONE, OID_GRASS, OID_FLOWER, OID_BOMB, OID_DZONE] then
    begin
      DragPos   := Point(X, Y);
      DragAngle := DragObj.Param.Angle;
      DragScale := DragObj.Param.Scale;
    end else
      DragObj := nil;

  CurObj := DragObj;
  if CurObj <> nil then
  begin
    GroupBox3.Visible := True;

    ComboBox4.Enabled := False;
    ComboBox5.Enabled := False;
    ComboBox6.Enabled := False;

    case CurObj.ID of
      OID_PORTAL :
        begin
          ComboBox4.Enabled := True;
          ComboBox5.Enabled := True;
          ComboBox4.ItemIndex := Ord(CurObj.Flag and 3);
          ComboBox5.ItemIndex := Ord((CurObj.Flag shr 2) and 3);
        end;
      OID_GRASS, OID_FLOWER :
        begin
          ComboBox6.Enabled := True;
          ComboBox6.Clear;
          ComboBox6.Items.BeginUpdate;
          if CurObj.ID = OID_GRASS then
            tCount := 12
          else
            tCount := 3;
          for i := 0 to tCount - 1 do
            ComboBox6.Items.Add(IntToStr(i));
          ComboBox6.Items.EndUpdate;
          ComboBox6.ItemIndex := CurObj.Flag;
          ComboBox6.Parent.Refresh;
        end
    else
      GroupBox3.Visible := False;
    end;
  end else
    GroupBox3.Visible := False;

  PaintBox1.Repaint;
end;

procedure TForm1.PaintBox1MouseMove(Sender: TObject; Shift: TShiftState; X,
  Y: Integer);
begin
  if (ssCtrl in Shift) and DragMove then
  begin
    ScrollBox2.HorzScrollBar.Position := ScrollBox2.HorzScrollBar.Position - (X - DragPos.X);
    ScrollBox2.VertScrollBar.Position := ScrollBox2.VertScrollBar.Position - (Y - DragPos.Y);
    DragPos := Point(X - (X - DragPos.X), Y - (Y - DragPos.Y));
  end;

  if DragObj = nil then
    Exit;

  if X < ScrollBox2.HorzScrollBar.Position + 100 then ScrollBox2.HorzScrollBar.Position := ScrollBox2.HorzScrollBar.Position - 25;
  if Y < ScrollBox2.VertScrollBar.Position + 100 then ScrollBox2.VertScrollBar.Position := ScrollBox2.VertScrollBar.Position - 25;
  if X > ScrollBox2.HorzScrollBar.Position + ScrollBox2.Width - 100 then ScrollBox2.HorzScrollBar.Position  := ScrollBox2.HorzScrollBar.Position + 25;
  if Y > ScrollBox2.VertScrollBar.Position + ScrollBox2.Height - 100 then ScrollBox2.VertScrollBar.Position := ScrollBox2.VertScrollBar.Position + 25;

  if DragTrans then
  begin
    DragObj.Param.Angle := DragAngle + (X - DragPos.X) * 0.01;
    if (DragObj.ID <> OID_BOMB) and (DragObj.ID <> OID_DZONE) then
    begin
      DragObj.Param.Scale := DragScale + (DragPos.Y - Y) * 0.01;
      if DragObj.Param.Scale < 0.1 then
        DragObj.Param.Scale := 0.1;
    end else
    begin
      DragObj.Param.Scale := DragScale + (DragPos.Y - Y);
      if DragObj.Param.Scale < 32 then
        DragObj.Param.Scale := 32;
    end;
  end else
    case DragObj.SelectID of
      0 : DragObj.Pos := Point(X + DragPos.X, Y + DragPos.Y);
      1 :
        begin
          DragObj.Param.Size.cx := X + DragPos.X;
          DragObj.Param.Size.cy := Y + DragPos.Y;
        end;
      2 :
        begin
          DragObj.Param.Size.cx := X + DragPos.X;
          DragObj.Pos.Y         := Y + DragPos.Y;
        end;
      3 :
        begin
          DragObj.Pos.X         := X + DragPos.X;
          DragObj.Param.Size.cy := Y + DragPos.Y;
        end;
    end;

  if DragObj.ID = OID_ACID then
  begin
    DragObj.Pos.X := min(DragObj.Pos.X, DragObj.Param.Size.cx - 5);
    DragObj.Pos.Y := min(DragObj.Pos.Y, DragObj.Param.Size.cy - 5);
//    DragObj.Param.Size.cx := max(DragObj.Pos.X, DragObj.Param.Size.cx - 5);
//    DragObj.Param.Size.cx := max(DragObj.Pos.Y, DragObj.Param.Size.cy - 5);
    if DragObj.Param.Size.cy - DragObj.Pos.Y > 255 then
      if DragObj.SelectID in [0, 2] then
        DragObj.Param.Size.cy := DragObj.Pos.Y + 255
      else
        DragObj.Pos.Y := DragObj.Param.Size.cy - 255;
  end;
       {
    if not (DragObj.ID in [OID_START, OID_FINISH, OID_FLOWER, OID_GRASS, OID_STONE, OID_CRYSTAL]) then
    begin
      if DragObj.ID = OID_PORTAL then
      begin
        with TPortal(DragObj) do
          if SelectID = 0 then
          begin
            DragObj.Pos.X := ;
            DragObj.Pos.Y := ;
          end else
          begin

          end;
      end else
      begin
        if X < DragPos.X then DragObj.Pos.X := X;
        DragObj.Param.Size.cx := abs(X - DragPos.X);

        if Y < DragPos.Y then DragObj.Pos.Y := Y;
        DragObj.Param.Size.cy := abs(Y - DragPos.Y);
      end;
    end else
      DragObj.Pos := Point(X + DragPos.X, Y + DragPos.Y);
           }

  PaintBox1.Repaint;
end;

procedure TForm1.PaintBox1MouseUp(Sender: TObject; Button: TMouseButton;
  Shift: TShiftState; X, Y: Integer);
begin
  if Button <> mbRight then
  begin
    if DragObj <> nil then
      SaveLevelsXML;
    DragObj := nil;
  end;
  DragMove := False;
end;

procedure TForm1.PaintBox1Paint(Sender: TObject);
var
  rMin, rMax : TPoint;
begin
  LevelObj.Draw(PaintBox1.Canvas);
  if CurObj <> nil then
    with PaintBox1.Canvas do
    begin
      Brush.Color := clYellow;
      FrameRect(CurObj.Bounds);
    end;
end;

procedure TForm1.PopupMenu1Popup(Sender: TObject);
begin
  Remove1.Enabled := (TreeView1.Selected <> nil) and (
    (TreeView1.Selected.Parent = NodeFonts) or
    (TreeView1.Selected.Parent = NodeSprites));
end;

procedure TForm1.PopupMenu2Popup(Sender: TObject);
begin
  Remove2.Enabled := False;
end;

procedure TForm1.PopupMenu3Popup(Sender: TObject);
begin
  Remove3.Enabled := ListBox2.ItemIndex > -1;
end;

procedure TForm1.PopupMenu4Popup(Sender: TObject);
begin
  RemoveObject1.Enabled :=
    (CurObj <> nil) and
    (CurObj <> LevelObj.Objects[0]) and
    (CurObj <> LevelObj.Objects[1]);
end;

procedure TForm1.Remove1Click(Sender: TObject);
begin
  TAtlasNode(TreeView1.Selected.Data).Free;
  TreeView1.Selected.Delete;
  AtlasPack;
end;

procedure TForm1.Remove3Click(Sender: TObject);
var
  i, j : Integer;
  str : string;
begin
  if ListBox2.ItemIndex > -1 then
    with TSpriteData(TreeView1.Selected.Data) do
    begin
      i := ListBox2.ItemIndex;
      Atlas.DelImage(Images[i]);
      Images[i].Free;
      for j := i to Length(Images) - 2 do
        Images[j] := Images[j + 1];
      SetLength(Images, Length(Images) - 1);
      Update;
      ListBox2.DeleteSelected;
    end;
end;

procedure TForm1.RemoveObject1Click(Sender: TObject);
begin
  LevelObj.DelObj(CurObj);
  DragObj := nil;
  CurObj := nil;
  PaintBox1.Repaint;
  SaveLevelsXML;
end;

procedure Crop(Image: TAtlasImage);
const
  SENS = 8;
var
  x, y, cl, ct, cr, cb, cw, ch : Integer;
  Data : PByteArray;
begin
  cl := Image.Width - 1;
  ct := Image.Height - 1;
  cr := 0;
  cb := 0;

  for y := 0 to Image.Height - 1 do
    for x := 0 to Image.Width - 1 do
      if Image.Data[(y * Image.Width + x) * 4 + 3] > SENS then
      begin
        cl := Min(cl, x);
        cr := Max(cr, x);
        ct := Min(ct, y);
        cb := Max(cb, y);
      end;

  cw := Max(1, cr - cl + 1);
  ch := Max(1, cb - ct + 1);
  Data := GetMemory(cw * ch * 4);

  for y := 0 to ch - 1 do
    Move(Image.Data[((ct + y) * Image.Width + cl) * 4], Data[y * cw * 4], cw * 4);

  FreeMemory(Image.Data);

  Image.ox     := cl;
  Image.oy     := ct;
  Image.Width  := cw;
  Image.Height := ch;
  Image.Data   := Pointer(Data);
  Writeln('crop to : ', cw, ' ', ch);
end;

procedure TForm1.Load1Click(Sender: TObject);
var
  F : TFileStream;
  M : TMemoryStream;

  Hash, Size, Count, SCount, Flags, Loop, FPS : LongWord;
  p : TPoint;
  Frames : array of TSubFrame;
  png : TPNGImage;
  i, j : Integer;
  n : TSpriteData;
  Image : TAtlasImage;
  Data : Pointer;
  img : TAtlasImage;

  procedure GetImage(const Name: string; const Frame: TSubFrame);
  var
    x, y, yb, Width, Height : Integer;
    i, j : Integer;
    d : Pointer;
    p : ^TRGBA;
    b : PArrayRGB;
    a : PByteArray;
  begin
    Width  := Round(Frame.pr - Frame.pl);
    Height := Round(Frame.pb - Frame.pt);
    x := Round(Frame.sl * png.Width);
    y := Round(Frame.st * png.Height);

    d := GetMemory(Width * Height * 4);
    p := Pointer(d);
    for j := y to y + Height - 1 do
    begin
      b := png.Scanline[j];
      a := PByteArray(png.AlphaScanline[j]);
      for i := x to x + Width - 1 do
      begin
        p^.b := b[i].b;
        p^.g := b[i].g;
        p^.r := b[i].r;
        p^.a := a[i];
        Inc(p);
      end;
    end;
    img := TAtlasImage.Create(Name, 0, 0, 0, Width, Height, d);
    Crop(img);
    Inc(img.ox, Round(Frame.pl) + n.X);
    Inc(img.oy, Round(Frame.pt) + n.Y);
    n.AddImage(img);
  end;

begin
  if OpenDialog4.Execute then
  begin
    png := TPNGImage.Create;
    SaveDialog1.FileName := Copy(OpenDialog4.FileName, 1, Length(OpenDialog4.FileName) - 4) + '.png';
    png.LoadFromFile(SaveDialog1.FileName);

    F := TFileStream.Create(OpenDialog4.FileName, fmOpenRead);
    M := TMemoryStream.Create;
    M.CopyFrom(F, F.Size);
    M.Position := 0;
    M.Read(SCount, SizeOf(SCount));

    for i := 0 to SCount - 1 do
    begin
      M.Read(Hash, SizeOf(Hash));
      M.Read(Count, SizeOf(Count));
      M.Read(FPS, SizeOf(FPS));
      M.Read(Loop, SizeOf(Loop));
      M.Read(Flags, SizeOf(Flags));
      M.Read(p, SizeOf(p));

      M.ReadBuffer(j, SizeOf(j));

      SetLength(Frames, Count);
      Move(Pointer(Integer(M.Memory) + j)^, Frames[0], Count * SizeOf(TSubFrame));

      M.ReadBuffer(j, SizeOf(j));
      n := AddSprite(PAnsiChar(Integer(M.Memory) + j));
      n.X := p.x;
      n.Y := p.y;
      n.Loop := Loop;
      n.Hash := Hash;
      n.FPS  := FPS;

      for j := 0 to Length(Frames) - 1 do
      begin
        n.FrameSize.cx := Max(n.FrameSize.cx, n.X + Round(Frames[j].pr));
        n.FrameSize.cy := Max(n.FrameSize.cy, n.Y + Round(Frames[j].pb));
      end;

      for j := 0 to Length(Frames) - 1 do
        GetImage('frame_' + IntToStr(j), Frames[j]);
    end;

    M.Free;
    F.Free;

    png.Free;
  end;
  Atlas.Pack;
  NodeSprites.Selected := True;
end;

procedure TForm1.Save1Click(Sender: TObject);
var
  i, x, y, Count : Integer;
  d : PArrayRGBA;
  b : PArrayRGB;
  a : PByteArray;
  sd : array of TSubData;
  str, dstr : string;
  png : TPNGImage;
  Stream : TStream;
  Offset, Align, FramesSize, OffsetFrames, OffsetNames : Integer;
begin
  if SaveDialog1.Execute then
  begin
    dstr := SaveDialog1.FileName;
    Delete(dstr, Length(dstr) - 3, 4);

    str := ExtractFileName(SaveDialog1.FileName);
    Delete(str, Length(str) - 3, 4);

    Stream := TFileStream.Create(SaveDialog1.FileName, fmCreate);
{
    SetLength(sd, NodeFonts.Count + NodeSprites.Count);

    for i := 0 to NodeFonts.Count - 1 do
      sd[i] := TSubData(NodeFonts[i].Data);

    for i := 0 to NodeSprites.Count - 1 do
      sd[i + NodeFonts.Count] := TSubData(NodeSprites[i].Data);

    for i := 0 to Length(sd) - 1 do
      sd[i].Save(Stream);
}
  // sprites count
    i := NodeSprites.Count;
    Stream.WriteBuffer(i, SizeOf(i));

    Offset := 4 + 9 * 4 * NodeSprites.Count;// + 2 + Length(str);
    Align  := 0;//4 - Offset mod 4;

    OffsetFrames := Offset + Align;
    OffsetNames  := Offset + Align;

    for i := 0 to NodeSprites.Count - 1 do
      with TSpriteData(NodeSprites[i].Data) do
        Inc(OffsetNames, 32 * Length(Images));

    for i := 0 to NodeSprites.Count - 1 do
      with TSpriteData(NodeSprites[i].Data) do
      begin
        Hash := GetHash(Name);
        Stream.WriteBuffer(Hash, SizeOf(Hash));
        Count := Length(Images);
        Stream.WriteBuffer(Count, SizeOf(Count));
        Stream.WriteBuffer(FPS, SizeOf(FPS));
        Stream.WriteBuffer(Loop, SizeOf(Loop));
        Stream.WriteBuffer(Flags, SizeOf(Flags));
        Stream.WriteBuffer(X, SizeOf(X));
        Stream.WriteBuffer(Y, SizeOf(Y));

        Stream.WriteBuffer(OffsetFrames, SizeOf(OffsetFrames));
        Stream.WriteBuffer(OffsetNames, SizeOf(OffsetNames));

        Inc(OffsetFrames, 32 * Length(Images));
        Inc(OffsetNames, Length(Name) + 1);
      end;
{
  // texture name
    i := Length(str);
    Stream.WriteBuffer(i, 2);
    Stream.WriteBuffer(AnsiString(str)[1], i);

  // data align (4 bytes)
    i := 0;
    Stream.WriteBuffer(i, Align);
}
  // frames
    for i := 0 to NodeSprites.Count - 1 do
      with TSpriteData(NodeSprites[i].Data) do
        SaveFrames(Stream);
  // names
    for i := 0 to NodeSprites.Count - 1 do
      with TSpriteData(NodeSprites[i].Data) do
      begin
        Count := 0;
        Stream.WriteBuffer(AnsiString(Name)[1], Length(Name));
        Stream.WriteBuffer(Count, 1);
      end;
    Stream.Free;

    png := TPNGImage.CreateBlank(COLOR_RGBALPHA, 8, Atlas.Width, Atlas.Height);
    png.CreateAlpha;
    for y := 0 to Atlas.Height - 1 do
    begin
      d := @Atlas.Data[y * Atlas.Width * 4];
      b := png.Scanline[y];
      a := PByteArray(png.AlphaScanline[y]);
      for x := 0 to Atlas.Width - 1 do
      begin
        b[x].b := d[x].b;
        b[x].g := d[x].g;
        b[x].r := d[x].r;
        a[x] := d[x].a;
      end;
    end;
    png.CompressionLevel := 9;

    png.SaveToFile(dstr + '.png');
    png.Free;
  end;
end;

procedure TForm1.SpeedButton1Click(Sender: TObject);
begin
  if FontDialog1.Execute then
  begin
    with TFontData(TreeView1.Selected.Data) do
    begin
      Font.Name := FontDialog1.Font.Name;
      Font.Size := FontDialog1.Font.Size;
      Font.Bold   := fsBold in FontDialog1.Font.Style;
      Font.Italic := fsItalic in FontDialog1.Font.Style;
      Update;
      SpeedButton1.Caption := Name; // TFontData.Name
      TreeView1.Selected.Text := Name;
    end;
  end;
end;

procedure TForm1.SpeedButton2Click(Sender: TObject);
begin
  with TSpeedButton(Sender) do
    if Down then
      Caption := 'Pause'
    else
      Caption := 'Play'
end;

procedure TForm1.SpeedButton3Click(Sender: TObject);
var
  F : File;
  i : Integer;
  s : Single;
  world, stage : Integer;
begin
  ChDir(ExtractFileDir(ParamStr(0)));

  AssignFile(F, 'data/level.cfg');
  Rewrite(F, 1);

  for world := 1 to 3 do
    for stage := 1 to 16 do
      with level[world, stage] do
      begin
        BlockWrite(F, start, SizeOf(level[world, stage]) - 12);
        i := Length(obj);
        BlockWrite(F, i, SizeOf(i));
        for i := 0 to Length(obj) - 1 do
        begin
          BlockWrite(F, obj[i].id, SizeOf(obj[i].id) + SizeOf(obj[i].Flag));
          s := obj[i].Pos.X;   BlockWrite(F, s, SizeOf(s));
          s := obj[i].Pos.Y;   BlockWrite(F, s, SizeOf(s));
          if TLevelObjectID(obj[i].id) in [OID_GRASS, OID_STONE, OID_FLOWER, OID_BOMB, OID_DZONE] then
          begin
            BlockWrite(F, obj[i].Scale, SizeOf(s));
            BlockWrite(F, obj[i].Angle, SizeOf(s));
          end else
          begin
            s := obj[i].Size.cx; BlockWrite(F, s, SizeOf(s));
            s := obj[i].Size.cy; BlockWrite(F, s, SizeOf(s));
          end;
        end;
      end;

  CloseFile(F);

  with CurLevel^ do
  begin
    CopyFile(PChar(front), PChar(ExtractFileDir(ParamStr(0)) + '\raw\front.png'), False);
    CopyFile(PChar(back), PChar(ExtractFileDir(ParamStr(0)) + '\raw\back.png'), False);
  end;

  LevelStrip('raw\front.png', 'raw\level\', True);
  LevelStrip('raw\back.png', 'raw\back\', False);

  DeleteFile(ExtractFileDir(ParamStr(0)) + '\raw\front.png');
  DeleteFile(ExtractFileDir(ParamStr(0)) + '\raw\back.png');

  Convert('win', 'raw\', 'data\', True);
  ShellExecute(0, 'open', 'plastiland.exe', PChar(ComboBox2.Text + ' ' + ComboBox1.Text), nil, SW_SHOW);
//  WinExec('plastiland.exe', SW_SHOW);
end;

procedure TForm1.SpeedButton4Click(Sender: TObject);
var
  t : TAtlasImage;
  i : Integer;
begin
  ListBox2.Clear;
  with TSpriteData(TreeView1.Selected.Data) do
  begin
    for i := 0 to Length(Images) div 2 - 1 do
    begin
      t := Images[i];
      Images[i] := Images[Length(Images) - i - 1];
      Images[Length(Images) - i - 1] := t;
    end;
    for i := 0 to Length(Images) - 1 do
      ListBox2.Items.Add(Images[i].Name);
  end;
end;

procedure TForm1.SpinEdit5Change(Sender: TObject);
begin
  TFontData(TreeView1.Selected.Data).Kerning := TSpinEdit(Sender).Value;
end;

procedure TForm1.SpinEdit6Change(Sender: TObject);
begin
  TSpriteData(TreeView1.Selected.Data).FPS := Max(1, StrToIntDef(SpinEdit6.Text, 0));
end;

procedure TForm1.SpinEdit7Change(Sender: TObject);
begin
  if TreeView1.Selected.Parent = NodeSprites then
  begin
    if Sender = SpinEdit7 then
      TSpriteData(TreeView1.Selected.Data).X := StrToIntDef(SpinEdit7.Text, 0);
    if Sender = SpinEdit8 then
      TSpriteData(TreeView1.Selected.Data).Y := StrToIntDef(SpinEdit8.Text, 0);
  end;
  SpriteDraw;
end;

procedure TForm1.SpinEditChange(Sender: TObject);
begin
  AtlasPack;
end;

function TForm1.AddSprite(const Name: string): TSpriteData;
var
  n : TTreeNode;
begin
  n := TreeView1.Items.AddChild(NodeSprites, Name);
  Result := TSpriteData.Create;
  n.Data := Result;
  TSpriteData(n.Data).Name := Name;
  n.ImageIndex    := 4;
  n.SelectedIndex := 4;
  n.Selected      := True;
end;

procedure TForm1.Sprite1Click(Sender: TObject);
begin
  AddSprite('Sprite');
end;

procedure TForm1.TreeView1Change(Sender: TObject; Node: TTreeNode);
var
  i : Integer;
  str : string;
begin
  CategoryFont.Visible   := Node.Parent = NodeFonts;
  CategorySprite.Visible := Node.Parent = NodeSprites;

  if Node.Parent = NodeFonts then
    with TFontData(Node.Data) do
    begin
      NoUpdate := True;
      Memo1.Text := GlyphStr;
      NoUpdate := False;

      FontDialog1.Font.Name := Font.Name;
      FontDialog1.Font.Size := Font.Size;
      FontDialog1.Font.Style := [];
      if Font.Bold then
        FontDialog1.Font.Style := FontDialog1.Font.Style + [fsBold];
      if Font.Italic then
        FontDialog1.Font.Style := FontDialog1.Font.Style + [fsItalic];
      SpinEdit5.Value := Kerning;
      SpeedButton1.Caption := Name;
    end;

  if Node.Parent = NodeSprites then
    with TSpriteData(Node.Data) do
    begin
      ListBox2.Clear;
      for i := 0 to Length(Images) - 1 do
      begin
        str := ExtractFileName(Images[i].Name);
        if LowerCase(ExtractFileExt(str)) = '.png' then
          Delete(str, Length(str) - 3, 4);
        ListBox2.Items.Add(str);
      end;
      SpinEdit6.Value := FPS;
      SpinEdit7.Value := TSpriteData(Node.Data).X;
      SpinEdit8.Value := TSpriteData(Node.Data).Y;
      CheckBox1.Checked := TSpriteData(TreeView1.Selected.Data).Loop <> 0;
      Edit2.Text := Name;
      if ListBox2.Count > 0 then
        ListBox2.ItemIndex := 0;
    end;

  Draw;
  Image1.Refresh;
end;

procedure TForm1.BitBtn1Click(Sender: TObject);
var
  png : TPNGImage;
  i, x, y : Integer;
  d : PArrayRGBA;
  b : PArrayRGB;
  a : PByteArray;
  F : File;
  img : TAtlasImage;

  FileChar : record
    ID : WideChar;
    ox, oy, x, y, w, h : Word;
  end;

begin
  if Atlas.Data = nil then
    Exit;

  if SaveDialog1.Execute then
  begin
    png := TPNGImage.CreateBlank(COLOR_RGBALPHA, 8, Atlas.Width, Atlas.Height);
    png.CreateAlpha;
    for y := 0 to Atlas.Height - 1 do
    begin
      d := @Atlas.Data[y * Atlas.Width * 4];
      b := png.Scanline[y];
      a := PByteArray(png.AlphaScanline[y]);
      for x := 0 to Atlas.Width - 1 do
      begin
        b[x].b := d[x].b;
        b[x].g := d[x].g;
        b[x].r := d[x].r;
        a[x] := d[x].a;
      end;
    end;
    png.CompressionLevel := 9;
    png.SaveToFile(SaveDialog1.FileName);
    png.Free;

    AssignFile(F, Copy(SaveDialog1.FileName, 1, Length(SaveDialog1.FileName) - 4) + '.atl');
    Rewrite(F, 1);

    BlockWrite(F, Atlas.Width,  SizeOf(Integer));
    BlockWrite(F, Atlas.Height, SizeOf(Integer));

    BlockWrite(F, Atlas.MinID, SizeOf(Atlas.MinID));
    BlockWrite(F, Atlas.MaxID, SizeOf(Atlas.MaxID));
    i := Length(Atlas.Images);
    BlockWrite(F, i, SizeOf(i)); // count

    Writeln('Font save');

    for i := 0 to Length(Atlas.Images) - 1 do
    begin
      img := Atlas.Images[i];
      if img.Link <> nil then
        img := img.Link;
      with img do
      begin
        FileChar.ID := Atlas.Images[i].Name[1];
        FileChar.ox := ox; // 0
        FileChar.oy := oy; // Glyph.OffsetY
        FileChar.x  := X;
        FileChar.y  := Y;
        FileChar.w  := Width;
        FileChar.h  := Height;
        BlockWrite(F, FileChar, SizeOf(FileChar));
      end;
    end;
    CloseFile(F);
  end;
end;

procedure TForm1.Font1Click(Sender: TObject);
var
  n : TTreeNode;
begin
  n := TreeView1.Items.AddChild(NodeFonts, FontDialog1.Font.Name + '_' + IntToStr(FontDialog1.Font.Size));
  n.Data := TFontData.Create;
  n.ImageIndex    := 2;
  n.SelectedIndex := 2;
  n.Selected      := True;
end;

procedure TForm1.AtlasPack;
var
  i, j : Integer;
begin
  Atlas.Margin := Rect(SpinEdit3.Value, SpinEdit1.Value, SpinEdit4.Value, SpinEdit2.Value);
  Atlas.Pack;
  j := 0;
  for i := 0 to Length(Atlas.Images) - 1 do
    if Atlas.Images[i].Link <> nil then
      Inc(j);
  Writeln('Atlas pack (chars: ', Length(Atlas.Images), ' [', j, ' linked]) ', Atlas.Width, 'x', Atlas.Height);
  StatusBar1.Panels[0].Text := IntToStr(Atlas.Width) + 'x' + IntToStr(Atlas.Height);
  AtlasDraw;
end;

function Blend(a, b, alpha: Byte): Byte; inline;
const
  INV_BYTE = 1 / 255;
begin
  Result := Round(a + (Integer(b) - Integer(a)) * ((255 - alpha) * INV_BYTE));
end;

function LoadTGA(const FileName: string): TAtlasImage;
var
  tga : TGA_Header;
  Data : PArrayRGBA;
  i : Integer;
begin
  tga := TARGA.LoadTGA(FileName);
  if tga.Data <> nil then
  begin
    if tga.BPP = 24 then
    begin
      Data := GetMemory(tga.Width * tga.Height * 4);
      for i := 0 to tga.Width * tga.Height - 1 do
        with PArrayRGB(tga.Data)[i] do
        begin
          Data[i].b := b;
          Data[i].g := g;
          Data[i].r := r;
          Data[i].a := 255;
        end;
      FreeMemory(tga.Data);
      tga.Data := Data;
    end;

    Result := TAtlasImage.Create(FileName, GetHash(FileName), 0, 0, tga.Width, tga.Height, tga.Data);
  end else
  begin
    Result := nil;
    Writeln('error loading tga image');
  end;

end;

function LoadPNG(const FileName: string): TAtlasImage;
var
  png : TPNGImage;
  x, y : Integer;
  b : ^TRGB;
  a : ^Byte;
  d : PByteArray;

//  Stream : jutils.TStream;
  Data : Pointer;
//  Width, Height : Integer;
begin
  Result := nil;
{
  Stream := jutils.TFileStream.Create(FileName);
  if jimg.LoadIMG(Stream, PByteArray(Data), Width, Height) then
  begin
    //
    Result := TAtlasImage.Create(FileName, GetHash(FileName), 0, 0, Width, Height, Data);
  end else
    MessageBox(0, 'Ќеверный фоормат изображени€', nil, MB_ICONHAND);
  Stream.Free;
}
  png := TPNGImage.Create;
  png.LoadFromFile(FileName);
  png.CreateAlpha;

  Data := GetMemory(png.Width * png.Height * 4);
  d := Data;

  for y := 0 to png.Height - 1 do
  begin
    b := Pointer(png.Scanline[y]);
    a := Pointer(png.AlphaScanline[y]);
    for x := 0 to png.Width - 1 do
    begin
      d[0] := b^.b;
      d[1] := b^.g;
      d[2] := b^.r;
      d[3] := a^;
      Inc(a);
      Inc(b);
      d := @d[4];
    end;
  end;

  Result := TAtlasImage.Create(FileName, GetHash(FileName), 0, 0, png.Width, png.Height, Data);
  png.Free;
end;

procedure TForm1.Acid1Click(Sender: TObject);
begin
  NewObjID := TLevelObjectID(TComponent(Sender).Tag);
  CurObj := nil;
  TMenuItem(Sender).Checked := True;
end;

procedure TForm1.Add3Click(Sender: TObject);
var
  i : Integer;
  str : string;
  img : TAtlasImage;
begin
  with OpenDialog1 do
    if Execute then
    begin
      for i := 0 to Files.Count - 1 do
      begin
        str := ExtractFileName(Files[i]);
        Delete(str, Length(str) - 3, 4);
        ListBox2.Items.Add(str);

        if LowerCase(ExtractFileExt(Files[i])) = '.tga' then
          img := LoadTGA(Files[i])
        else
          img := LoadPNG(Files[i]);
        TSpriteData(TreeView1.Selected.Data).FrameSize.cx := img.Width;
        TSpriteData(TreeView1.Selected.Data).FrameSize.cy := img.Height;

        Crop(img);
        // transform center

        TSpriteData(TreeView1.Selected.Data).AddImage(img);
      end;
      TSpriteData(TreeView1.Selected.Data).Update;
    end;
end;

procedure TForm1.ApplicationEvents2Idle(Sender: TObject; var Done: Boolean);
begin
  Done := True;
  if (TreeView1.Selected <> nil) and (TreeView1.Selected.Parent = NodeSprites) and (ListBox2.Count > 0) then
    if SpeedButton2.Down then
    begin
      if (GetTickCount - lastTime) >= (1000 / TSpriteData(TreeView1.Selected.Data).FPS) then
      begin
        lastTime := GetTickCount;
        if not CheckBox1.Checked then
        begin
          ListBox2.ItemIndex := Min(ListBox2.ItemIndex + 1, ListBox2.Count - 1);
          if ListBox2.ItemIndex = ListBox2.Count then
            SpeedButton2.onClick(Sender);
        end else
          ListBox2.ItemIndex := (ListBox2.ItemIndex + 1) mod ListBox2.Count;
        Draw;
      end;
      Done := False;
    end;
end;

procedure TForm1.AtlasDraw;
var
  i, j : LongInt;
  a : PArrayRGBA;
  b : PArrayRGB;
  bmp : TBitmap;
begin
  if Atlas.Data <> nil then
  begin
    bmp := Image1.Picture.Bitmap;
    bmp.SetSize(Atlas.Width, Atlas.Height);
    bmp.PixelFormat := pf24bit;

    for j := 0 to Atlas.Height - 1 do
    begin
      a := @Atlas.Data[(j * Atlas.Width) * 4];
      b := bmp.ScanLine[j];
      for i := 0 to Atlas.Width - 1 do
      begin
        b[i].b := Blend(a[i].b, 0, a[i].a);
        b[i].g := Blend(a[i].g, 0, a[i].a);
        b[i].r := Blend(a[i].r, 0, a[i].a);
      end;
    end;

    Image1.Width  := bmp.Width;
    Image1.Height := bmp.Height;
  end;
end;

procedure TForm1.FontDraw;
begin
  //
end;

procedure TForm1.SpriteDraw;
var
  bmp : TBitmap;
  x, y : Integer;
  px, py, tx : Integer;
  img  : TAtlasImage;
  b : PArrayRGB;
  a : PArrayRGBA;
begin
  if ListBox2.ItemIndex < 0 then
    Exit;

  Image1.Width := TSpriteData(TreeView1.Selected.Data).FrameSize.cx;
  Image1.Height := TSpriteData(TreeView1.Selected.Data).FrameSize.cy;


  bmp := Image1.Picture.Bitmap;
  bmp.PixelFormat := pf24bit;
  bmp.SetSize(Image1.Width, Image1.Height);
  bmp.Canvas.Brush.Color := clBlack;
  bmp.Canvas.FillRect(Rect(0, 0, bmp.Width, bmp.Height));

  with TSpriteData(TreeView1.Selected.Data) do
  begin
    img := Images[ListBox2.ItemIndex];
    if img.Link <> nil then
      img := img.Link;
  end;

  for y := 0 to img.Height - 1 do
  begin
    py := img.oy + y;
    if (py < 0) or (py >= bmp.Height) then
      continue;

    a := @Atlas.Data[((img.y + y) * Atlas.Width) * 4];
    b := bmp.ScanLine[py];
    for x := 0 to img.Width - 1 do
    begin
      px := img.ox + x;
      if (px < 0) or (px >= bmp.Width) then
        continue;
      tx := x + img.X;
      b[px].b := Blend(a[tx].b, 0, a[tx].a);
      b[px].g := Blend(a[tx].g, 0, a[tx].a);
      b[px].r := Blend(a[tx].r, 0, a[tx].a);
    end;
  end;

  with TSpriteData(TreeView1.Selected.Data) do
  begin
    bmp.Canvas.Pen.Color := clYellow;
    bmp.Canvas.Brush.Color := clRed;
    bmp.Canvas.Ellipse(X - 2, Y - 2, X + 2, Y + 2);
  end;

  bmp.Canvas.Brush.Color := clYellow;
  bmp.Canvas.FrameRect(Rect(img.ox - 1, img.oy - 1, img.ox + img.Width + 1, img.oy + img.Height + 1));

  Image1.Repaint;
end;

procedure TForm1.Draw;
begin
  if TreeView1.Selected.Parent = NodeFonts then
    FontDraw
  else
    if TreeView1.Selected.Parent = NodeSprites then
      SpriteDraw
    else
      AtlasDraw;
end;

procedure TForm1.Edit2Change(Sender: TObject);
begin
  if Edit2.Text = '' then
    Exit;


  TSpriteData(TreeView1.Selected.Data).Name := Edit2.Text;
  TreeView1.Selected.Text := Edit2.Text;
end;

procedure TForm1.ChangeLevelParams(Sender: TObject);
begin
  with CurLevel^ do
  begin
    if Sender = SpinEdit9  then count   := SpinEdit9.Value;
    if Sender = SpinEdit18 then time    := SpinEdit18.Value;
    if Sender = ComboBox3  then
      if ComboBox3.ItemIndex = 0 then dir := 1 else dir := -1;

    if Sender = SpinEdit10 then bomb    := SpinEdit10.Value;
    if Sender = SpinEdit11 then puckman := SpinEdit11.Value;
    if Sender = SpinEdit12 then crusher := SpinEdit12.Value;
    if Sender = SpinEdit13 then brick   := SpinEdit13.Value;
    if Sender = SpinEdit14 then jumper  := SpinEdit14.Value;
    if Sender = SpinEdit15 then sticky  := SpinEdit15.Value;
    if Sender = SpinEdit16 then bridge  := SpinEdit16.Value;
    if Sender = SpinEdit17 then cancel  := SpinEdit17.Value;
  end;
  SaveLevelsXML;
end;

procedure TForm1.CheckBox1Click(Sender: TObject);
begin
  TSpriteData(TreeView1.Selected.Data).Loop := Ord(CheckBox1.Checked);
end;

procedure TForm1.ComboBox2Change(Sender: TObject);
var
  i : Integer;
begin
  ScrollBox2.VertScrollBar.Position := 0;
  ScrollBox2.HorzScrollBar.Position := 0;

  GroupBox3.Visible := False;
  CurObj := nil;

  SaveLevelsXML;

  CurLevel := @level[ComboBox2.ItemIndex + 1, ComboBox1.ItemIndex + 1];
  with CurLevel^ do
  begin
    LevelObj.Clear;
    for i := 0 to Length(obj) - 1 do
      with LevelObj.AddObj(obj[i].Pos, TLevelObjectID(obj[i].id)) do
      begin
        Param.Size := obj[i].Size;
        Flag := obj[i].Flag;
      end;

    LevelObj.Objects[0].Pos := start;
    LevelObj.Objects[1].Pos := finish;

    SpinEdit9.Value  := count;
    SpinEdit18.Value := time;
    if dir > 0 then
      ComboBox3.ItemIndex := 0
    else
      ComboBox3.ItemIndex := 1;

    SpinEdit10.Value := bomb;
    SpinEdit11.Value := puckman;
    SpinEdit12.Value := crusher;
    SpinEdit13.Value := brick;
    SpinEdit14.Value := jumper;
    SpinEdit15.Value := sticky;
    SpinEdit16.Value := bridge;
    SpinEdit17.Value := cancel;

    LevelObj.SetBack(back);
    LevelObj.SetFront(front);
    PaintBox1.Repaint;
  end;
end;

procedure TForm1.ComboBox4Change(Sender: TObject);
begin
  if CurObj = nil then
    Exit;

  if (Sender = ComboBox4) or (Sender = ComboBox5) then
    CurObj.Flag := (CurObj.Flag and (not 15)) or (ComboBox4.ItemIndex or (ComboBox5.ItemIndex shl 2));

  if Sender = ComboBox6 then
    CurObj.Flag := ComboBox6.ItemIndex;

  PaintBox1.Repaint;
  SaveLevelsXML;
end;

end.
