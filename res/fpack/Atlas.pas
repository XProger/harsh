unit Atlas;

interface

uses
  Windows;

type
  TRGB = record
    b, g, r : Byte;
  end;

  TArrayRGB = array [0..1] of TRGB;
  PArrayRGB = ^TArrayRGB;

  TRGBA = record
    b, g, r, a : Byte;
  end;

  TArrayRGBA = array [0..1023] of TRGBA;
  PArrayRGBA = ^TArrayRGBA;

  TByteArray = array [0..1023] of Byte;
  PByteArray = ^TByteArray;

  TLongWordArray = array [0..1023] of LongWord;
  PLongWordArray = ^TLongWordArray;

  TAtlasImage = class
    constructor Create(const Name: WideString; Margin: Boolean; Width, Height: Integer; Data: PByteArray);
    destructor Destroy; override;
  public
    Flag : Boolean;
    Name : WideString;
    Data : PByteArray;
    Margin : Boolean;
    X, Y   : LongInt;
    Width, Height : LongInt;
    Link : TAtlasImage;
  end;

  TAtlasImageArray = array of TAtlasImage;

  TAtlasNode = class
    constructor Create(const Rect: TRect);
    destructor Destroy; override;
  public
    Block : Boolean;
    Node  : array [0..1] of TAtlasNode;
    Rect  : TRect;
    function Insert(Width, Height, mx, my: LongInt; out X, Y: LongInt): Boolean;
  end;

  TAtlas = class
    destructor Destroy; override;
  public
    Images : TAtlasImageArray;
    Width, Height : LongInt;
    Data  : PByteArray;
    Node  : TAtlasNode;
    MinID, MaxID : Integer;
    Name, Shader, Normal : String;
    Square : Boolean;
    BackColor : LongWord;
    procedure BeginUpdate(ImageArray: TAtlasImageArray);
    procedure EndUpdate;
    procedure LinkUpdate;
    procedure AddImage(Image: TAtlasImage);
    procedure DelImage(Image: TAtlasImage);
    function GetIndex(Image: TAtlasImage): LongInt;
    function Pack: Boolean;
  end;

  function SFH(Data: PAnsiChar; len: Integer): LongWord;

implementation

type
  TCompareFunc = function (a, b: TAtlasImage): Integer;

function SFH(Data: PAnsiChar; len: Integer): LongWord;
var
  tmp : LongWord;
  rem : Integer;
begin
  if (len <= 0) or (data = nil) then
  begin
    Result := 0;
    Exit;
  end else
    Result := len;

  rem := len and 3;
  len := len shr 2;

  while len > 0 do
  begin
    Result := Result + Word(Pointer(Data)^);
    tmp    := (Word(Pointer(@Data[2])^) shl 11) xor Result;
    Result := (Result shl 16) xor tmp;
    Inc(Data, 4);
    Result := Result + (Result shr 11);
    Dec(len);
  end;

  case rem of
    3 :
      begin
        Result := Result + Word(Pointer(Data)^);
        Result := Result xor (Result shl 16);
        Result := Result xor (Byte(Data[2]) shl 18);
        Result := Result + Result shr 11;
      end;
    2 :
      begin
        Result := Result + Word(Pointer(Data)^);
        Result := Result xor (Result shl 11);
        Result := Result + (Result shr 17);
      end;
    1 :
      begin
        Result := Result + Byte(Data[0]);
        Result := Result xor (Result shl 10);
        Result := Result + (Result shr 1);
      end;
  end;

  Result := Result xor (Result shl 3);
  Result := Result + (Result shr 5);
  Result := Result xor (Result shl 4);
  Result := Result + (Result shr 17);
  Result := Result xor (Result shl 25);
  Result := Result + (Result shr 6);
end;

function RectI(Left, Top, Right, Bottom: LongInt): TRect;
begin
  Result.Left   := Left;
  Result.Top    := Top;
  Result.Right  := Right;
  Result.Bottom := Bottom;
end;

function ToPow2(x: LongInt): LongInt;
begin
  Result := x - 1;
  Result := Result or (Result shr 1);
  Result := Result or (Result shr 2);
  Result := Result or (Result shr 4);
  Result := Result or (Result shr 8);
  Result := Result or (Result shr 16);
  Result := Result + 1;
end;

function Max(x, y: Integer): Integer;
begin
  if x > y then
    Result := x
  else
    Result := y;
end;

function Min(x, y: Integer): Integer;
begin
  if x < y then
    Result := x
  else
    Result := y;
end;

function CompareStr(a, b: TAtlasImage): Integer;
begin
  if a.Name < b.Name then
    Result := 1
  else
    if a.Name > b.Name then
      Result := -1
    else
      Result := 0;
end;

function CompareMax(a, b: TAtlasImage): Integer;
begin
  if Max(a.Width, a.Height) < Max(b.Width, b.Height) then
    Result := 1
  else
    if Max(a.Width, a.Height) > Max(b.Width, b.Height) then
      Result := -1
    else
      Result := CompareStr(a, b);
end;

function CompareMin(a, b: TAtlasImage): Integer;
begin
  if Min(a.Width, a.Height) < Min(b.Width, b.Height) then
    Result := 1
  else
    if Min(a.Width, a.Height) > Min(b.Width, b.Height) then
      Result := -1
    else
      Result := CompareStr(a, b);
end;

function CompareWidth(a, b: TAtlasImage): Integer;
begin
  if a.Width < b.Width then
    Result := 1
  else
    if a.Width > b.Width then
      Result := -1
    else
      Result := CompareStr(a, b);
end;

function CompareHeight(a, b: TAtlasImage): Integer;
begin
  if a.Height < b.Height then
    Result := 1
  else
    if a.Height > b.Height then
      Result := -1
    else
      Result := CompareStr(a, b);
end;

function CompareArea(a, b: TAtlasImage): Integer;
begin
  if (a.Width * a.Height) < (b.Width * b.Height) then
    Result := 1
  else
    if (a.Width * a.Height) > (b.Width * b.Height) then
      Result := -1
    else
      Result := CompareStr(a, b);
end;

function ComparePerim(a, b: TAtlasImage): Integer;
begin
  if (a.Width + a.Height) < (b.Width + b.Height) then
    Result := 1
  else
    if (a.Width + a.Height) > (b.Width + b.Height) then
      Result := -1
    else
      Result := CompareStr(a, b);
end;

procedure QSort(Items: PPointerArray; L, R: LongInt; CompareFunc: TCompareFunc);
var
  i, j : LongInt;
  P, T : Pointer;
begin
  repeat
    i := L;
    j := R;
    P := Items[(L + R) div 2];
    repeat
      while CompareFunc(Items[i], P) < 0 do
        Inc(i);
      while CompareFunc(Items[j], P) > 0 do
        Dec(j);
      if i <= j then
      begin
        if i <> j then
        begin
          T := Items^[i];
          Items^[i] := Items^[j];
          Items^[j] := T;
        end;
        Inc(i);
        Dec(j);
      end;
    until i > j;
    if L < j then
      QSort(Items, L, j, CompareFunc);
    L := i;
  until i >= R;
end;

{$REGION 'TAtlasImage'}
constructor TAtlasImage.Create(const Name: WideString; Margin: Boolean; Width, Height: Integer; Data: PByteArray);
begin
  Self.Name   := Name;
  Self.Width  := Width;
  Self.Height := Height;
  Self.Data   := Data;
  Self.Margin := Margin;
end;

destructor TAtlasImage.Destroy;
begin
  FreeMemory(Data);
  inherited;
end;
{$ENDREGION}

{$REGION 'TAtlasNode'}
constructor TAtlasNode.Create(const Rect: TRect);
begin
  Self.Rect := Rect;
end;

destructor TAtlasNode.Destroy;
begin
  if Node[0] <> nil then Node[0].Free;
  if Node[1] <> nil then Node[1].Free;
end;

function TAtlasNode.Insert(Width, Height, mx, my: LongInt; out X, Y: LongInt): Boolean;
var
  dw, dh : LongInt;
begin
  if (Node[0] <> nil) and (Node[1] <> nil) then
  begin
    Result := Node[0].Insert(Width, Height, mx, my, X, Y);
    if not Result then
      Result := Node[1].Insert(Width, Height, mx, my, X, Y);
  end else
  begin
    dw := Rect.Right - Rect.Left;
    dh := Rect.Bottom - Rect.Top;
    if (dw < Width) or (dh < Height) or Block then
    begin
      Result := False;
      Exit;
    end else
      if (dw = Width) and (dh = Height) then
      begin
        X := mx + Rect.Left;
        Y := my + Rect.Top;
        Block := True;
        Result := True;
        Exit;
      end else
        with Rect do
          if dw - Width > dh - Height then
          begin
            Node[0] := TAtlasNode.Create(RectI(Left, Top, Left + Width, Bottom));
            Node[1] := TAtlasNode.Create(RectI(Left + Width, Top, Right, Bottom));
          end else
          begin
            Node[0] := TAtlasNode.Create(RectI(Left, Top, Right, Top + Height));
            Node[1] := TAtlasNode.Create(RectI(Left, Top + Height, Right, Bottom));
          end;
    Result := Node[0].Insert(Width, Height, mx, my, X, Y);
  end;
end;
{$ENDREGION}

{$REGION 'TAtlas'}
destructor TAtlas.Destroy;
var
  i : LongInt;
begin
  if Data <> nil then
    FreeMemory(Data);
  for i := 0 to Length(Images) - 1 do
    Images[i].Free;
  inherited;
end;

procedure TAtlas.BeginUpdate(ImageArray: TAtlasImageArray);
var
  i : Integer;
begin
  for i := 0 to Length(ImageArray) - 1 do
    ImageArray[i].Flag := True;
end;

procedure TAtlas.EndUpdate;
var
  i : Integer;
begin
  i := Length(Images) - 1;
  while i >= 0 do
  begin
    if Images[i].Flag then
      DelImage(Images[i]);
    Dec(i);
  end;
end;

procedure TAtlas.LinkUpdate;
var
  i, j : Integer;
  Hash : LongWord;
begin
  for i := 0 to Length(Images) - 1 do
    Images[i].Link := nil;

  for i := 0 to Length(Images) - 1 do
    if Images[i].Link = nil then
    begin
      Hash := SFH(PAnsiChar(Images[i].Data), Images[i].Width * Images[i].Height * 4);
      for j := Length(Images) - 1 downto 0 do
        if (i <> j) and (Images[j].Link = nil) and
           (Images[i].Width = Images[j].Width) and
           (Images[i].Height = Images[j].Height) and
           (Hash = SFH(PAnsiChar(Images[j].Data), Images[j].Width * Images[j].Height * 4)) then
        begin
          Images[i].Link := Images[j];
          break;
        end;
    end;
end;

procedure TAtlas.AddImage(Image: TAtlasImage);
begin
  DelImage(Image);
  SetLength(Images, Length(Images) + 1);
  Images[Length(Images) - 1] := Image;
end;

procedure TAtlas.DelImage(Image: TAtlasImage);
var
  i : LongInt;
begin
  i := GetIndex(Image);
  if i = -1 then
    Exit;

  Images[i] := Images[Length(Images) - 1];
  SetLength(Images, Length(Images) - 1);
end;

function TAtlas.GetIndex(Image: TAtlasImage): LongInt;
var
  i : Integer;
begin
  Result := -1;
  for i := 0 to Length(Images) - 1 do
    if Images[i] = Image then
    begin
      Result := i;
      break;
    end;
end;

function TAtlas.Pack: Boolean;
var
  MinSize : Integer;

  function Pack(Image: TAtlasImage): Boolean;
  begin
    with Image do
      Result := Node.Insert(Width + Ord(Margin) * 2, Height + Ord(Margin) * 2, Ord(Margin), Ord(Margin), X, Y);
  end;

  procedure Draw(Image: TAtlasImage);
  var
    i, x, y, px, py : LongInt;
  begin
    for y := -Ord(Image.Margin) to Image.Height - 1 + Ord(Image.Margin) do
      for x := -Ord(Image.Margin) to Image.Width - 1 + Ord(Image.Margin) do
      begin
        px := Min(Max(0, x), Image.Width - 1);
        py := Min(Max(0, y), Image.Height - 1);
        i := (Image.Y + y) * Width + (Image.X + x);
        LongInt(Pointer(@Data[i * 4])^) := LongInt(Pointer(@Image.Data[(py * Image.Width + px) * 4])^);
      end;
  end;

  function TestPack(CompareFunc: TCompareFunc): Boolean;
  var
    Count, i : Integer;
  begin
    QSort(PPointerArray(@Images[0]), 0, Length(Images) - 1, CompareFunc);

    Width  := MinSize;
    Height := MinSize div 2;

    Count := Length(Images);
    while Height < 4096 do
    begin
      Count := Length(Images);

      Node := TAtlasNode.Create(RectI(0, 0, Width, Height));
      for i := 0 to Length(Images) - 1 do
        if (Images[i].Link <> nil) or Pack(Images[i]) then
          Dec(Count)
        else
          break;
      Node.Free;

      if Count = 0 then
        break;

      if Square then
      begin
        Height := Height * 2;
        Width  := Height;
      end else
        if Height > Width * 2 then
          Width := Width * 2
        else
          Height := Height + 16;
    end;

    Result := Count = 0;
  end;

  procedure GetBBox(out mX, mY: Integer);
  var
    i : Integer;
  begin
    mX := 0;
    mY := 0;
    for i := 0 to Length(Images) - 1 do
      if Images[i].Link = nil then
      begin
        mX := Max(mX, Images[i].X + Images[i].Width);
        mY := Max(mY, Images[i].Y + Images[i].Height);
      end;
  end;

const
  CompareList : array [0..5] of TCompareFunc = (
    CompareMax,
    CompareMin,
    CompareWidth,
    CompareHeight,
    ComparePerim,
    CompareArea
  );

var
  i : Integer;
  mX, mY, mW, mH : Integer;
  BestFunc : TCompareFunc;
begin
  if Data <> nil then
  begin
    FreeMemory(Data);
    Data := nil;
  end;

  MinID := High(Word);
  MaxID := 0;

  for i := 0 to Length(Images) - 1 do
  begin
    MinID := Min(MinID, Ord(Images[i].Name[1]));
    MaxID := Max(MaxID, Ord(Images[i].Name[1]));
  end;

  if Length(Images) = 0 then
    Exit(False);

  LinkUpdate; // поиск ссылок на дублирующиеся глифы
// Get minimum texture area
  MinSize := 0;
  for i := 0 to Length(Images) - 1 do
    if Images[i].Link = nil then
      Inc(MinSize, Images[i].Width * Images[i].Height);
  MinSize := ToPow2(Round(sqrt(MinSize)));

  mW := 4096;
  mH := 4096;
  BestFunc := nil;
  for i := 0 to Length(CompareList) - 1 do
    if TestPack(CompareList[i]) then
    begin
      GetBBox(mX, mY);
      if (mX <= mW) and (mY <= mH) then
      begin
        BestFunc := CompareList[i];
        mW := mX;
        mH := mY;
      end;
    end;

  Result := @BestFunc <> nil;
  if Result then
  begin
    TestPack(BestFunc);

    GetBBox(mX, mY);
    Width  := ToPow2(mX);
    Height := ToPow2(mY);
    Data := GetMemory(Width * Height * 4);

    for i := 0 to Width * Height - 1 do
      PLongWordArray(Data)[i] := BackColor;

    for i := 0 to Length(Images) - 1 do
      if Images[i].Link = nil then
        Draw(Images[i]);
  end else
    Writeln('Can''t find place for ', Result, ' images');
end;
{$ENDREGION}

end.
