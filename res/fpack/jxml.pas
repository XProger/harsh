unit jxml;

interface


type
  TXMLParam = record
    Name  : WideString;
    Value : WideString;
  end;

  TXMLParams = class
    constructor Create(const Text: WideString);
  private
    FCount  : LongInt;
    FParams : array of TXMLParam;
    function GetParam(const Name: WideString): WideString;
    function GetParamI(Idx: LongInt): TXMLParam;
  public
    property Count: LongInt read FCount;
    property Param[const Name: WideString]: WideString read GetParam; default;
    property ParamI[Idx: LongInt]: TXMLParam read GetParamI;
  end;

  TXML = class
    class function Load(const FileName: string): TXML;
    constructor Create(const Text: WideString; BeginPos: LongInt);
    destructor Destroy; override;
  private
    FCount   : LongInt;
    FNode    : array of TXML;
    FTag     : WideString;
    FContent : WideString;
    FDataLen : LongInt;
    FParams  : TXMLParams;
    function GetNode(const TagName: WideString): TXML;
    function GetNodeI(Idx: LongInt): TXML;
    function GetValue(const TagName: WideString): WideString;
  public
    property Count: LongInt read FCount;
    property Tag: WideString read FTag;
    property Content: WideString read FContent;
    property DataLen: LongInt read FDataLen;
    property Params: TXMLParams read FParams;
    property Node[const TagName: WideString]: TXML read GetNode; default;
    property NodeI[Idx: LongInt]: TXML read GetNodeI;
    property Value[const TagName: WideString]: WideString read GetValue;
  end;

implementation

type
  TCharSet = set of AnsiChar;

function TrimChars(const Str: WideString; Chars: TCharSet): WideString;
var
  i, j : LongInt;
begin
  j := Length(Str);
  i := 1;
  while (i <= j) and (Ord(Str[i]) < $0100) and (AnsiChar(Str[i]) in Chars) do
    Inc(i);
  if i <= j then
  begin
    while (Ord(Str[j]) < $0100) and (AnsiChar(Str[j]) in Chars) do
      Dec(j);
    Result := Copy(Str, i, j - i + 1);
  end else
    Result := '';
end;

function Trim(const Str: WideString): WideString;
begin
  Result := TrimChars(Str, [#9, #10, #13, #32, #34, #39]);
end;

{$REGION 'TXMLParam'}
{ TXMLParam }
constructor TXMLParams.Create(const Text: WideString);
var
  i          : LongInt;
  Flag       : (F_BEGIN, F_NAME, F_VALUE);
  ParamIdx   : LongInt;
  IndexBegin : LongInt;
  ReadValue  : Boolean;
  TextTag    : WideChar;
begin
  Flag       := F_BEGIN;
  ParamIdx   := -1;
  IndexBegin := 1;
  ReadValue  := False;
  TextTag    := #0;
  for i := 1 to Length(Text) do
    case Flag of
      F_BEGIN :
        if Text[i] <> ' ' then
        begin
          ParamIdx := Length(FParams);
          SetLength(FParams, ParamIdx + 1);
          FParams[ParamIdx].Name  := '';
          FParams[ParamIdx].Value := '';
          Flag := F_NAME;
          IndexBegin := i;
        end;
      F_NAME :
        if Text[i] = '=' then
        begin
          FParams[ParamIdx].Name := Trim(Copy(Text, IndexBegin, i - IndexBegin));
          Flag := F_VALUE;
          IndexBegin := i + 1;
        end;
      F_VALUE :
        begin
          if (Text[i] = '''') or (Text[i] = '"') then
            if TextTag = #0 then
              TextTag := Text[i]
            else
              TextTag := #0;
          if (Text[i] <> ' ') and (TextTag = #0) then
            ReadValue := True
          else
            if ReadValue then
            begin
              FParams[ParamIdx].Value := TrimChars(Trim(Copy(Text, IndexBegin, i - IndexBegin)), ['''', '"']);
              Flag := F_BEGIN;
              ReadValue := False;
              ParamIdx := -1;
            end else
              continue;
        end;
    end;
  if ParamIdx <> -1 then
    FParams[ParamIdx].Value := Trim(Trim(Copy(Text, IndexBegin, Length(Text) - IndexBegin + 1)));
  FCount := Length(FParams);
end;

function TXMLParams.GetParam(const Name: WideString): WideString;
var
  i : LongInt;
begin
  for i := 0 to Count - 1 do
    if FParams[i].Name = Name then
    begin
      Result := FParams[i].Value;
      Exit;
    end;
  Result := '';
end;

function TXMLParams.GetParamI(Idx: LongInt): TXMLParam;
begin
  Result.Name  := FParams[Idx].Name;
  Result.Value := FParams[Idx].Value;
end;
{$ENDREGION}

{$REGION 'TXML'}
{ TXML }
class function TXML.Load(const FileName: string): TXML;
var
  io : LongInt;
  F : File;
  Text     : WideString;
  Size     : LongInt;
  UTF8Text : UTF8String;
begin
  {$I-}
  FileMode := 2;
  AssignFile(F, FileName);
  FileMode := 0;
  Reset(F, 1);
  io := IOResult;
  {$I+}
  if io = 0 then
  begin
    Size := FileSize(F) - 3;
    Seek(F, 3);
    SetLength(UTF8Text, Size);
    BlockRead(F, UTF8Text[1], Size);
    CloseFile(F);
    Text := UTF8ToWideString(UTF8Text);//UTF8ToString(UTF8Text);
    Result := Create(Text, 1);
  end else
    Result := nil;
end;

constructor TXML.Create(const Text: WideString; BeginPos: LongInt);
var
  i, j : LongInt;
  Flag : (F_COMMENT, F_BEGIN, F_TAG, F_PARAMS, F_CONTENT, F_END, F_CDATA);
  BeginIndex : LongInt;
  TextTag    : WideChar;
  Len : LongInt;
begin
  TextTag := #0;
  Flag    := F_BEGIN;
  BeginIndex := BeginPos;
  FContent   := '';
  i   := BeginPos - 1;
  Len := Length(Text);
  while i < Len do
  begin
    Inc(i);
    case Flag of
      F_CDATA :
        if (Text[i] = ']') and (Copy(Text, i, 2) = ']]') then
        begin
          Inc(i, 2);
          Flag := F_CONTENT;
        end;
      F_COMMENT :
        if Text[i] = '>' then
          Flag := F_BEGIN;
    // waiting for new tag '<...'
      F_BEGIN :
        case Text[i] of
          '<' :
            begin
              Flag := F_TAG;
              BeginIndex := i + 1;
            end;
          '>' :
            begin
              Flag := F_END;
              Dec(i);
            end;
        end;
    // waiting for tag name '... ' or '.../' or '...>'
      F_TAG :
        begin
          case Text[i] of
            '>' : Flag := F_CONTENT;
            '/' : Flag := F_END;
            ' ' : Flag := F_PARAMS;
            '?', '!' :
              begin
                if Copy(Text, i + 1, 7) = '[CDATA[' then
                  Flag := F_CDATA
                else
                  Flag := F_COMMENT;
                continue;
              end
          else
            continue;
          end;
          FTag := Trim(Copy(Text, BeginIndex, i - BeginIndex));
          BeginIndex := i + 1;
        end;
    // parse tag parameters
      F_PARAMS :
        begin
          if (Text[i] = '''') or (Text[i] = '"') then
            if TextTag = #0 then
              TextTag := Text[i]
            else
              TextTag := #0;
          if TextTag = #0 then
          begin
            case Text[i] of
              '>' : Flag := F_CONTENT;
              '/' : Flag := F_END;
            else
              continue;
            end;
            FParams := TXMLParams.Create(Trim(Copy(Text, BeginIndex, i - BeginIndex)));
            BeginIndex := i + 1;
          end;
        end;
    // parse tag content
      F_CONTENT :
        case Text[i] of
          '<' :
            begin
              FContent := FContent + Trim(Copy(Text, BeginIndex, i - BeginIndex));
            // is new tag or tag closing?
              for j := i + 1 to Length(Text) do
                if Text[j] = '>' then
                begin
                  if Trim(Copy(Text, i + 1, j - i - 1)) <> '/' + FTag then
                  begin
                    SetLength(FNode, Length(FNode) + 1);
                    FNode[Length(FNode) - 1] := TXML.Create(Text, i);
                    if FNode[Length(FNode) - 1].DataLen = 0 then
                      break;
                    i := i + FNode[Length(FNode) - 1].DataLen - 1;
                    BeginIndex := i + 1;
                  end else
                  begin
                    i := j - 1;
                    Flag := F_END;
                  end;
                  break;
                end;
            end
          end;
    // waiting for close tag
      F_END :
        if Text[i] = '>' then
        begin
          FDataLen := i - BeginPos + 1;
          break;
        end;
    end;
  end;
  if FParams = nil then
    FParams := TXMLParams.Create('');
  FCount := Length(FNode);
end;

destructor TXML.Destroy;
var
  i : LongInt;
begin
  for i := 0 to Count - 1 do
    NodeI[i].Free;
  Params.Free;
end;

function TXML.GetNode(const TagName: WideString): TXML;
var
  i : LongInt;
begin
  for i := 0 to Count - 1 do
    if FNode[i].Tag = TagName then
    begin
      Result := FNode[i];
      Exit;
    end;
  Result := nil;
end;

function TXML.GetNodeI(Idx: LongInt): TXML;
begin
  Result := FNode[Idx];
end;

function TXML.GetValue(const TagName: WideString): WideString;
var
  XML : TXML;
begin
  XML := Node[TagName];
  if XML <> nil then
    Result := XML.Content
  else
    Result := '';
end;
{$ENDREGION}

end.
