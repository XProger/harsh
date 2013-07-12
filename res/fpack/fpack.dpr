program fpack;

{$APPTYPE CONSOLE}

uses
  Windows,
  jxml, Atlas, ZLib, TARGA;

type
  TVertex = record
    x, y, s, t : Single;
  end;

  TSprite = class
    parent : TSprite;
    frame  : TSprite;
    name   : string;
    src    : string;
    x, y   : Integer;
    cx, cy : Integer;
    margin : Boolean;
    frames : Integer;
    Image  : TAtlasImage;
    Ref    : TSprite;
    IndexStart, IndexCount : Integer;
    Hash   : LongWord;
  end;

  TFileItem = record
    Name   : AnsiString;
    Hash   : LongWord;
    Offset : Integer;
    Size   : Integer;
    CSize  : Integer;
  end;

  TFileItemList = array of TFileItem;

  TImageInfo = record
    Width, Height : Integer;
    Data : Pointer;
  end;

function Vertex(x, y, s, t: Single): TVertex;
begin
  Result.x := x;
  Result.y := y;
  Result.s := s;
  Result.t := t;
end;

function StrToInt(const Str: string): Integer;
var
  Code : LongInt;
begin
  Val(Str, Result, Code);
  if Code <> 0 then
    Result := 0;
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

function LoadImage(const FileName: string; var Image: TImageInfo): Boolean;
var
  tga  : TGA_Header;
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

    Image.Width  := tga.Width;
    Image.Height := tga.Height;
    Image.Data   := tga.Data;
    Result := True;
  end else
  begin
    Result := False;
    Writeln('error: invalid tga image "', FileName, '"');
  end;
end;

procedure PVR2ATF(const FileName: string);
var
  PVR : record
    dwHeaderSize,
    dwHeight,
    dwWidth,
    dwMipMapCount,
    dwpfFlags,
    dwDataSize,
    dwBitCount,
    dwRBitMask,
    dwGBitMask,
    dwBBitMask,
    dwABitMask,
    dwPVR,
    dwNumSurfs : LongWord;
  end;
  F : File;
begin
  AssignFile(F, FileName);
  Reset(F, 1);
  BlockRead(F, PVR, SizeOf(PVR));


  Rewrite(F, 1);


  CloseFile(F);
end;

function StripPath(const Path: AnsiString): AnsiString;
var
  i : Integer;
begin
  i := 1;
  Result := '';
  while i <= Length(Path) do
  begin
    if Path[i] <> '-' then
      Result := Result + Path[i]
    else
      while i <= Length(Path) do
      begin
        if Path[i] = '/' then
          break;
        Inc(i);
      end;
    Inc(i);
  end;
end;

procedure List(const Path, OS, BasePath: string; var Items: TFileItemList);
var
  i : Integer;
  h  : THandle;
  fd : TWIN32FindData;
  str, strOS : AnsiString;
begin
  strOS := '_' + AnsiString(OS) + '/';
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
        str := StripPath(str);
        Items[Length(Items) - 1].Hash := SFH(PAnsiChar(str), Length(str));
      end else
        if (fd.cFileName[0] <> '-') or (Pos(OS, fd.cFileName) > 0) then
          List(Path + fd.cFileName + '/', OS, BasePath, Items);
    until (not FindNextFile(h, fd));
    Windows.FindClose(h);
  end;
end;

procedure PackData(const OS, BasePath, FileName: string);
const
  NULL : Byte = 0;
var
  Items : TFileItemList;

  procedure Remap(const FileName: string);
  var
    F : File;
    map : array of LongWord;
    i, j, k : Integer;
    tmp  : TFileItem;
    NewItems : TFileItemList;
  begin
    AssignFile(F, FileName);
    Reset(F, 1);
    SetLength(map, FileSize(F) div 4);
    BlockRead(F, map[0], Length(map) * 4);
    CloseFile(F);

    SetLength(NewItems, Length(Items));
    k := 0;
    for i := 0 to Length(Map) - 1 do
      for j := 0 to Length(Items) - 1 do
        if Map[i] = Items[j].Hash then
        begin
          NewItems[k] := Items[j];
          Items[j].Hash := 0;
          Inc(k);
          break;
        end;

    Writeln('unused:');
    for i := 0 to Length(Items) - 1 do
      if Items[i].Hash <> 0 then
      begin
        Writeln(Items[i].Name);
        NewItems[k] := Items[i];
        Inc(k);
      end;

    Readln;
    Items := NewItems;
  end;

var
  i, j, HSize : Integer;
  Fout, Fin : File;
  Data, CData : Pointer;
  str : AnsiString;
begin
  Items := nil;
  List('', OS, BasePath, Items);
// hash check
  for i := 0 to Length(Items) - 1 do
  begin
    if (Items[i].Hash = 0) then
    begin
      Writeln('! hash null: "', Items[i].Name, '"');
      Readln;
    end;

    for j := 0 to Length(Items) - 1 do
      if (i <> j) and (Items[i].Hash = Items[j].Hash) then
      begin
        Writeln('! hash conflict: "', Items[i].Name, '" & "', Items[j].Name, '"');
        Readln;
      end;
  end;

  Remap('pack.list');

  AssignFile(Fout, FileName);
  Rewrite(Fout, 1);

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

procedure PackAtlas(const OS, FileName: string);
const
  EPS = 0.00001;
var
  XML : TXML;
  i, j : Integer;
  atlas : TAtlas;
  Img : TImageInfo;
  Sprites : array of TSprite;
  Name, Format : string;
  Flag : Boolean;
  v : TVertex;
  aw, ah, tx, ty : Single;
  Sprite : TSprite;
  merge, compressed : Boolean;

  Indices  : array of Word;
  Vertices : array of TVertex;

  function AddVertex(x, y, s, t: Single): Integer;
  var
    i : Integer;
  begin
    Result := -1;
    for i := 0 to Length(Vertices) - 1 do
      if (abs(Vertices[i].x - x) < EPS) and
         (abs(Vertices[i].y - y) < EPS) and
         (abs(Vertices[i].s - s) < EPS) and
         (abs(Vertices[i].t - t) < EPS) then
      begin
        Result := i;
        break;
      end;
    if Result = -1 then
    begin
      Result := Length(Vertices);
      SetLength(Vertices, Result + 1);
      Vertices[Result] := Vertex(x,y, s, t);
    end;
    SetLength(Indices, Length(Indices) + 1);
    Indices[Length(Indices) - 1] := Result;
  end;


  procedure ParseSprite(Parent: TSprite; XML: TXML);
  var
    Sprite : TSprite;
    i : Integer;
  begin
    if XML = nil then
      Exit;
    Sprite := TSprite.Create;
    Sprite.parent := Parent;
    Sprite.name   := XML.Params['name'];
    Sprite.src    := XML.Params['src'];
    Sprite.x  := StrToInt(XML.Params['x']);
    Sprite.y  := StrToInt(XML.Params['y']);
    Sprite.cx := StrToInt(XML.Params['cx']);
    Sprite.cy := StrToInt(XML.Params['cy']);
    Sprite.margin := XML.Params['margin'] <> 'false';
    Sprite.frames := StrToInt(XML.Params['frames']);

    SetLength(Sprites, Length(Sprites) + 1);
    Sprites[Length(Sprites) - 1] := Sprite;

    if Parent <> nil then
      Writeln('sprite: ', Parent.name, '::', Sprite.src)
    else
      Writeln('sprite: ', Sprite.name);

    for i := 0 to XML.Count - 1 do
      ParseSprite(Sprite, XML.NodeI[i]);
  end;

  function SaveAtlasImage(const FileName: string): Boolean;
  begin
    Result := SaveTGA(FileName, atlas.Width, atlas.Height, atlas.Data);
  end;

  function GetHash(const Str: string): LongWord;
  begin
    if Str <> '' then
      Result := SFH(PAnsiChar(AnsiString(Str)), Length(Str))
    else
      Result := 0;
  end;

  function SaveAtlasData(const FileName: string): Boolean;
  var
    F : File;
    i, j, count : Integer;
    error : Boolean;
    HashShader, HashNormal : LongWord;
    descSize : Integer;

    Desc : record
      Hash : LongWord;
      IndexStart, IndexCount : Word;
      StateCount, FPS        : Word;
    end;

  begin
    Result := False;

    error := False;
    count := 0;

  // validate names & indices
    for i := 0 to Length(Sprites) - 1 do
      if Sprites[i].name <> '' then
        if Sprites[i].IndexCount = 0 then
        begin
          Writeln('error: sprite indices "', Sprites[i].name, '"');
          error := True;
        end else
        begin
          Sprites[i].Hash := SFH(PAnsiChar(AnsiString(Sprites[i].name)), Length(Sprites[i].name));
          Inc(count);
        end;

  // validate hashes
    for i := 0 to Length(Sprites) - 1 do
      for j := 0 to Length(Sprites) - 1 do
        if (Sprites[i].name <> '') and
           (Sprites[j].name <> '') and
           (Sprites[i] <> Sprites[j]) and
           (Sprites[i].Hash = Sprites[j].Hash) then
        begin
          Writeln('error: hash collision ', Sprites[i].hash, ' ', Sprites[i].name, ' - ', Sprites[j].name);
          error := True;
        end;

    if error then
      Exit;

    if count = 0 then
    begin
      Writeln('error: sprite count');
      Exit;
    end;

    AssignFile(F, FileName);
    Rewrite(F, 1);

    HashShader := GetHash(atlas.Shader);
    HashNormal := GetHash(atlas.Normal);

    descSize := count * SizeOf(Desc);

    BlockWrite(F, HashShader, SizeOf(HashShader));
    BlockWrite(F, HashNormal, SizeOf(HashNormal));
    i := count;
    BlockWrite(F, i, SizeOf(i)); // descCount

    i := Length(Vertices);
    BlockWrite(F, i, SizeOf(i)); // vertexCount
    i := Length(Indices);
    BlockWrite(F, i, SizeOf(i)); // indexCount

    i := SizeOf(i) * 8;
    BlockWrite(F, i, SizeOf(i)); // desc
    i := SizeOf(i) * 8 + descSize;
    BlockWrite(F, i, SizeOf(i));
    i := SizeOf(i) * 8 + descSize + Length(Vertices) * SizeOf(TVertex);
    BlockWrite(F, i, SizeOf(i));

    for i := 0 to Length(Sprites) - 1 do
      if Sprites[i].name <> '' then
      begin
        Desc.Hash := Sprites[i].Hash;
        Desc.IndexStart := Sprites[i].IndexStart;
        Desc.IndexCount := Sprites[i].IndexCount;
        Desc.StateCount := 0;
        Desc.FPS := 0;
        BlockWrite(F, Desc, SizeOf(Desc));
      end;

    BlockWrite(F, Vertices[0], Length(Vertices) * SizeOf(TVertex));
    BlockWrite(F, Indices[0], Length(Indices) * SizeOf(Word));

    CloseFile(F);
    Result := True;
  end;

  procedure MergeAtlasData(const Name: string);
  var
    Data : Pointer;
    Size : Integer;
    F : File;
  begin
    FileMode := 0;
    AssignFile(F, Name + '.dat');
    Reset(F, 1);
    Size := FileSize(F);
    Data := GetMemory(Size);
    BlockRead(F, Data^, Size);
    CloseFile(F);

    FileMode := 2;
    AssignFile(F, Name + '.pvr');
    Reset(F, 1);
    Seek(F, FileSize(F));
    BlockWrite(F, Size, SizeOf(Size));
    BlockWrite(F, Data^, Size);
    CloseFile(F);

    FreeMemory(Data);
  end;

begin
  XML := TXML.Load(FileName);

  if XML = nil then
  begin
    Writeln('error: invalid xml file');
    Exit;
  end;

  Sprites  := nil;
  Vertices := nil;
  Indices  := nil;

  atlas := TAtlas.Create;

  atlas.Square := XML.Params['square'] = 'true';
  atlas.Name   := XML.Params['name'];
  atlas.Shader := XML.Params['shader'];
  atlas.Normal := XML.Params['normal'];

  if XML.Params['isNormal'] = 'true' then
    atlas.backColor := $FF8080FF
  else
    atlas.backColor := $00000000;

  Merge        := XML.Params['merge'] = 'true';
  Compressed   := XML.Params['compressed'] = 'true';

  if atlas.Shader <> '' then
    Writeln('atlas shader "', atlas.Shader, '"');
  if atlas.Normal <> '' then
    Writeln('atlas normal "', atlas.Normal, '"');

  for i := 0 to XML.Count - 1 do
    ParseSprite(nil, XML.NodeI[i]);

  for i := 0 to Length(Sprites) - 1 do
    if Sprites[i].src <> '' then
      if Sprites[i].src[1] = '#' then
      begin
        Name := Copy(Sprites[i].src, 2, 255);
        Flag := False;
        for j := 0 to Length(Sprites) - 1 do
          if Name = Sprites[j].name then
          begin
            Sprites[i].Ref := Sprites[j];
            Flag := True;
            break;
          end;
        if not Flag then
          Writeln('error: invalid link "', Sprites[i].src, '"');
      end else
        if LoadImage(Sprites[i].src, Img) then
        begin
          Sprites[i].Image := TAtlasImage.Create(Sprites[i].name, Sprites[i].margin, Img.Width, Img.Height, Img.Data);
          atlas.AddImage(Sprites[i].Image);
        end;
  XML.Free;

  if atlas.Pack then
  begin
    j := 0;
    for i := 0 to Length(Atlas.Images) - 1 do
      if Atlas.Images[i].Link <> nil then
        Inc(j);
    Writeln('atlas size: ', atlas.Width, ' x ', atlas.Height, ' [ ', j, ' linked ]');
  end else
    Writeln('error: packing atlas');

  aw := 1 / atlas.Width;
  ah := 1 / atlas.Height;
// build vertices for image sprites
  for i := 0 to Length(Sprites) - 1 do
    if Sprites[i].Image <> nil then
      with Sprites[i], Image do
      begin
        IndexStart := Length(Indices);
        IndexCount := 6;

        tx := X;// + 0.5;
        ty := Y;// + 0.5;
      // 1
        AddVertex(-cx, -cy, tx * aw, (ty + Height) * ah);
        AddVertex(-cx + Width, -cy, (tx + Width) * aw, (ty + Height) * ah);
        AddVertex(-cx + Width, -cy + Height, (tx + Width) * aw, ty * ah);
      // 2
        AddVertex(-cx, -cy, tx * aw, (ty + Height) * ah);
        AddVertex(-cx + Width, -cy + Height, (tx + Width) * aw, ty * ah);
        AddVertex(-cx, -cy + Height, tx * aw, ty * ah);
      end;

// build vertices for assets of sprites
  for i := 0 to Length(Sprites) - 1 do
    if Sprites[i].parent <> nil then
      with Sprites[i].parent do
      begin
        if Sprites[i].Ref <> nil then
          Sprite := Sprites[i].Ref
        else
          Sprite := Sprites[i];

        if IndexStart = 0 then
          IndexStart := Length(Indices);
        for j := Sprite.IndexStart to Sprite.IndexStart + Sprite.IndexCount - 1 do
        begin
          v := Vertices[Indices[j]];
          AddVertex(v.x - cx + Sprites[i].x, v.y - cy + Sprites[i].y, v.s, v.t);
          Inc(IndexCount);
        end;
      end;

  Writeln('i: ', Length(Indices), ' v: ', Length(Vertices));

// save atlas
  Name := atlas.Name;//Copy(FileName, 1, Pos('.xml', FileName));
  if SaveAtlasImage(Name + '.tga') then
    Writeln('save atlas image "', Name, '.tga"')
  else
    Writeln('error: save atlas image "', Name, '.tga"');

// save atlas data file
  if SaveAtlasData(Name + '.dat') then
    Writeln('save atlas data "', Name, '.dat"')
  else
    Writeln('error: save atlas data "', Name, '.dat"');

  for i := 0 to Length(Sprites) - 1 do
    Sprites[i].Free;
  atlas.Free;

// convert to pvr
  Write('Convert to "', Name, '.pvr"... ');
  if Compressed then
  begin
    if OS = 'ios'  then
      Format := '-fOGLPVRTC4 -yflip0'
    else
      if OS = 'android' then
        Format := '-fETC -yflip0'
      else
        if OS = 'flash' then
          Format := '-fOGL8888 -yflip0'
        else
          Format := '-fDXT1 -yflip0';
  end else
    Format := '-fOGL8888 -yflip0';

  Writeln('PVRTexTool.exe ' + Format + ' -i' + Name +'.tga');

  RunCmd('PVRTexTool.exe ' + Format + ' -i' + Name +'.tga');
  Writeln('ok');

  if Merge then
    MergeAtlasData(Name);
end;

procedure Convert(const OS, BasePath, DestPath: string);
var
  Items : TFileItemList;
  i : Integer;
  Compressed : Boolean;
  Format : string;
begin
  Items := nil;
  List('', OS, BasePath, Items);
  for i := 0 to Length(Items) - 1 do
  begin
    Compressed := True;

    if Compressed then
    begin
      if OS = 'ios' then
        Format := '-fOGLPVRTC4 -yflip0'
      else
        if OS = 'android' then
          Format := '-fETC -yflip0'
        else
          if OS = 'flash' then
            Format := '-fOGL8888 -yflip0'
          else
            Format := '-fDXT1 -yflip0';
    end else
      Format := '-fOGL8888 -yflip0';

    RunCmd('PVRTexTool.exe ' + Format + ' -m -i' + BasePath + Items[i].Name + ' -o' + DestPath + Items[i].Name );
    Writeln(Items[i].Name);
  end;
end;

begin
  ReportMemoryLeaksOnShutdown := True;
//  Convert('win', 'raw/', 'tmp/');
//  PackData('win', 'tmp\', '..\sys_win\bin\data.jet');
//  AllocConsole();
  if ParamStr(1) = 'atlas' then
    PackAtlas(ParamStr(2), ParamStr(3));
  if ParamStr(1) = 'pack' then
    PackData(ParamStr(2), ParamStr(3), ParamStr(4));
  if ParamStr(1) = 'convert' then
    Convert(ParamStr(2), ParamStr(3), ParamStr(4));
end.
