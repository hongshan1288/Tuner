unit utFreqCalc;

interface
uses
  classes,
  sysutils,
  math ;

type

  TFreqInfo = packed record
    NoteName  : string ;
    NoteFreq  : double ;
    NoteCns : double ;
  end ;

  TFreqCalc = class
  private
  
    FHalf_II : integer ;
    FFreqInfo : array of TFreqInfo ;
    procedure make_note_pcm_data;
    procedure make_pcm_freq(i: integer);
    procedure set_note_name(i, di: integer);

    function get_freq_by_note(music_note: string): double;
    function get_idx_by_freq(note_freq: double): integer;

  public

    constructor Create ;
    destructor Destroy; override ;
    procedure GetNoteInfoByFreq(aFreq: double; aNoteInfoList: TStringList) ;

    function get_Note_by_idx( idx: integer): string;
    function get_idx_by_note( aNote: string): integer;

  end ;

var
  gFreqCalc : TFreqCalc ;
    
implementation

const
  NOTE_INFO_MAX = 128 ; // 音符个数，按MIDI最大个数计算
  NoteInfoFmt = '%1.3f' ;

{ TFreqCalc }
////////////////////////////////////////////////////////////////////////////////
constructor TFreqCalc.Create;
begin
  FHalf_II := 59 ;
  SetLength( FFreqInfo, 128 ) ;
  make_note_pcm_data ;
end;
////////////////////////////////////////////////////////////////////////////////
destructor TFreqCalc.Destroy;
begin
  inherited;

end;
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
procedure TFreqCalc.set_note_name( i, di: integer );
var
	ci, dm, dv : integer ; //short	dm, dv ;
	cn, ca: char ; //char	cn, ci, ca ;
begin
	dm := di ;
	dv := dm mod 12 ;
	dm := dm div 12 ;
  cn := 'a' ;
  ca := 'a' ;
	case dv of
    0 :  begin cn := 'C'; ca := ' ' end ;
    1 :  begin cn := 'C'; ca := '#' end ;
    2 :  begin cn := 'D'; ca := ' ' end ;
    3 :  begin cn := 'E'; ca := 'b' end ;
    4 :  begin cn := 'E'; ca := ' ' end ;
    5 :  begin cn := 'F'; ca := ' ' end ;
    6 :  begin cn := 'F'; ca := '#' end ;
    7 :  begin cn := 'G'; ca := ' ' end ;
    8 :  begin cn := 'G'; ca := '#' end ;
    9 :  begin cn := 'A'; ca := ' ' end ;
    10 : begin cn := 'B'; ca := 'b' end ;
    11 : begin cn := 'B'; ca := ' ' end ;
  end ;
	ci := dm + 48 ;
  FFreqInfo[i].NoteName := cn + ca + chr(ci) ;
end ;
////////////////////////////////////////////////////////////////////////////////
procedure TFreqCalc.make_pcm_freq( i: integer );
var
	di : integer ;
  df, // 标准音a=440
  vf, // 指定音的频率
	dfi : double ;
begin
	df := 440 ;	// 标准音
	di := i-FHalf_II ; // 标准音的起始位置 (440在数组中的起始位置)
	dfi := di ;
	vf := df * power( 2, dfi/12 ) ; // 得到指定音的频率
  FFreqInfo[i].NoteFreq := vf ;
  if ( i=0 ) then begin
    FFreqInfo[i].NoteCns := 1 ;
  end else begin
    FFreqInfo[i].NoteCns := ( FFreqInfo[i].NoteFreq-FFreqInfo[i-1].NoteFreq) / 100 ;
  end ;
  set_note_name( i, di+(9+4*12) ) ; //FHalf_II-2 or 59 - 2 ;  // 从标准音到C1的差是9+4*12
end ;
//////////////////////////////////////////////////////////////////////
procedure TFreqCalc.make_note_pcm_data ;
var
	i : integer;
begin
	for i:=0 to NOTE_INFO_MAX-1 do begin
		make_pcm_freq( i ) ;
  end ;
end ;
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
function TFreqCalc.get_freq_by_note( music_note: string ) : double ;
var
    i : integer ;
begin
  i := get_idx_by_note( music_note ) ;
  if ( i>=0 ) then begin
    result := FFreqInfo[i].NoteFreq ;
  end else begin
    result := -1 ;
  end ;
end ;
//////////////////////////////////////////////////////////////////////
function TFreqCalc.get_idx_by_freq( note_freq: double ): integer ;
var
    i, ni   : integer ;
begin
  ni := -1 ;
  for i:=0 to NOTE_INFO_MAX-2 do begin
    if ( note_freq>=FFreqInfo[i].NoteFreq ) and ( note_freq<FFreqInfo[i+1].NoteFreq ) then begin // 确定在某个NoteFreq的范围内
      if ( abs(note_freq-FFreqInfo[i].NoteFreq)<abs(note_freq-FFreqInfo[i+1].NoteFreq) ) then begin // 看看更靠近哪个就用那个。
        ni := i ;
      end else begin
        ni := i+1 ;
      end ;
      break ;
    end ;
  end ;
  result := ni ;
end ;
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
procedure TFreqCalc.GetNoteInfoByFreq(aFreq: double; aNoteInfoList: TStringList);
var
  idx   : integer ;
begin
  aNoteInfoList.clear ;
  idx := get_idx_by_freq( aFreq ) ;
  if ( idx>=0 ) then begin
    aNoteInfoList.Add( 'NoteName='+FFreqInfo[idx].NoteName ) ;
    aNoteInfoList.Add( 'NoteFreq='+format( '%1.3f', [FFreqInfo[idx].NoteFreq] ) + 'Hz' ) ;
    aNoteInfoList.Add( 'CurFreq='+format( '%1.3f', [aFreq] ) + 'Hz' ) ;
    aNoteInfoList.Add( 'FreqDiff='+format( '%1.3f', [aFreq-FFreqInfo[idx].NoteFreq] ) + 'Hz' ) ;
    aNoteInfoList.Add( 'FreqCens='+format( '%1.3f', [(aFreq-FFreqInfo[idx].NoteFreq)/FFreqInfo[idx].NoteCns] ) + 'Cn' ) ;
  end ;
end;
////////////////////////////////////////////////////////////////////////////////
function TFreqCalc.get_Note_by_idx(idx: integer): string;
begin
  result := FFreqInfo[idx].NoteName ;
end;
////////////////////////////////////////////////////////////////////////////////
function TFreqCalc.get_idx_by_note(aNote: string): integer;
var
    i : integer ;
begin
  result := -1 ;
  for i:=0 to NOTE_INFO_MAX-1 do begin
    if ( aNote=FFreqInfo[i].NoteName ) then begin
      result := i ;
      exit ;
    end ;
  end ;
end;
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

initialization
  gFreqCalc := TFreqCalc.Create ;
finalization
  gFreqCalc.Free ;
end.
