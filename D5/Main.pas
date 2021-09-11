unit Main;

interface

uses
  Windows, Messages, SysUtils, Classes, Graphics, Controls, Forms, Dialogs,
  math,
  utFreqCalc,
  HsMoveForm,
  StdCtrls, ExtCtrls, HsSkinPanel, MySlider;

type

  THs = class
    a : integer ;
  private
      b : integer ;
  end ;

  TTAF_dataValue = function ( dataName, dataValue: pchar ):integer ; stdcall;

  TfrmMain = class(TForm)
    Panel_Base: THsSkinPanel;
    Edit1: TEdit;
    Panel_Info: TPanel;
    Timer_Close: TTimer;
    Button1: TButton;
    Button2: TButton;
    Button3: TButton;
    Button4: TButton;
    Panel_Msg: TPanel;
    Panel_WaveFormBase: TPanel;
    ScrollBarWaveForm: TScrollBar;
    Image_WaveForm: TImage;
    Panel_WaveForm: TMyPanel0;
    EditZoomValue: TEdit;
    Edit2: TEdit;
    Panel_DataInfoBase: TPanel;
    Panel_DataInfo: TMyPanel0;
    imgNoteInfo: TImage;
    Panel_KeyboardBase: TPanel;
    procedure FormCreate(Sender: TObject);
    procedure FormShow(Sender: TObject);
    procedure FormResize(Sender: TObject);
    procedure Panel_BaseCloseClick(Sender: TObject);
    procedure Panel_BaseMinClick(Sender: TObject);
    procedure Panel_BaseMaxClick(Sender: TObject);
    procedure Panel_BaseMenuClick(Sender: TObject);
    procedure Panel_BaseNorClick(Sender: TObject);
    procedure FormClose(Sender: TObject; var Action: TCloseAction);
    procedure Timer_CloseTimer(Sender: TObject);
    procedure FormDestroy(Sender: TObject);
    procedure Button1Click(Sender: TObject);
    procedure Panel_DataInfoResize(Sender: TObject);
    procedure Panel_WaveFormResize(Sender: TObject);
    procedure Panel_DataInfoRefresh(Sender: TObject);
    procedure Button2Click(Sender: TObject);
    procedure Button3Click(Sender: TObject);
    procedure Button4Click(Sender: TObject);
    procedure Panel_WaveFormRefresh(Sender: TObject);
    procedure ScrollBarWaveFormScroll(Sender: TObject; ScrollCode: TScrollCode; var ScrollPos: Integer);
    procedure EditZoomValueExit(Sender: TObject);
    procedure EditZoomValueEnter(Sender: TObject);
    procedure Panel_DataInfoBaseResize(Sender: TObject);
    procedure Panel_WaveFormBaseResize(Sender: TObject);
  private

    FWndProc  : TWndMethod ;

    procedure ReadIniValue;
    procedure WriteIniValue;
    procedure InitHsZip;
    procedure Init_View;
    procedure FormResize_Proc;
    procedure show_info(info: string);
    procedure FormCloseProc;
    procedure Reset_Other_Panels_Position;
    procedure Init_taf_DLL;
    procedure taf_open_proc;
    function TAF_Get_DV(sn: string): string;
    function TAF_Set_DV(sn, sv: string): integer;
    procedure FireEvent(sn, sv: integer);
    procedure WndProc0(var Message: TMessage);
    procedure do_test_proc;
    procedure DataInfo_Resize_proc;
    procedure WaveForm_Resize_proc;
    procedure do_all_wave_test;
    procedure do_save_waveForm_bmpFile( sv: integer );
    procedure Wave_and_Data_Resize_proc;
    procedure Reset_ScrollBarVar;
    procedure ResizeIamge_Proc;
    procedure SetImageSizeVars;
    procedure freq_and_pcm_event(sv: integer);
    procedure show_freq(freq_ss: string);
    procedure draw_note_info;
    procedure ResizeDataInfoBaseProc;
    procedure draw_pcm_waveForm;
    { Private declarations }
  public
    { Public declarations }
  end;

var
  frmMain: TfrmMain;

implementation
uses
  HsZip,
  HsUtils ;

var

  f_CaptionHeight : integer=28 ;
  f_UI_BorderWidth: integer=12 ;
  f_freq_show_fmt : string='3.2' ;

  f_ZoomValue : string ;

  f_taf_dataValue : TTAF_dataValue=nil ;

  f_audio_open_flag : integer=0 ;

  f_NoteInfo  : TStringList ;
  f_PcmBuf  : PSmallInteger ;
  f_PcmLen  : integer ;

  hs  : THs ;

{$R *.DFM}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
procedure TfrmMain.do_save_waveForm_bmpFile( sv: integer ) ;
var
  bmpFile : string ;
begin
  bmpFile := pchar(sv) ;
  Image_WaveForm.Picture.LoadFromFile( bmpFile ) ;
end ;
////////////////////////////////////////////////////////////////////////////////
procedure TfrmMain.freq_and_pcm_event( sv: integer ) ;
var
  pp    : pchar ;
  sm,
  ss    : string ;
begin
  pp := pchar(sv) ;
  ss := pp ;
  sm := get_ss_name0( ss, ',' ) ; // 频率

  show_freq( sm ) ;

  sm := get_ss_name0( ss, ',' ) ;
  f_PcmLen := strtoint(sm) ;
  f_PcmBuf := PSmallInteger( strtoint(ss) ) ;

  draw_pcm_waveForm ;

end ;
////////////////////////////////////////////////////////////////////////////////
procedure TfrmMain.FireEvent( sn, sv: integer );
begin
  if ( sn=0 ) then begin

  end else if ( sn=5001 ) then begin // 保存 waveForm BMP
    do_save_waveForm_bmpFile( sv ) ;

  end else if ( sn=5002 ) then begin // 检测频率
    freq_and_pcm_event( sv ) ;

  end else if ( sn=2011 ) then begin //音频打开标志
    f_audio_open_flag := sv ;
{
  end else if ( sn=200000+1001 ) then begin
  end else if ( sn=200000+1002 ) then begin
    ii := sv mod 1000 ;
    sv := sv div 1000 ;
    f_FileData[ii] := sv ;
    ss := 'OpType=DataRef'+ CR0 +
          'IPC_II='+inttostr(ii)+CR0 +
          'Data='+inttostr(f_FileData[ii])+'-'+inttostr(f_SockData[ii])+CR0 +
          '' ;
    RunName_Data( 0, 'ShowDataInfo='+ss ) ;

  end else if ( sn=200000+2001 ) then begin
    pp := f_ipc_simu_vars( 101, sv, 'PEER_INFO' ) ;
    if ( pp<>nil ) then begin
      ss := pp ;
      ii := sv ;
      ss := 'OpType=DataRef'+ CR0 +
            'IPC_II='+inttostr(ii)+CR0 +
            'Desc='+ss+CR0 +
            '' ;
      RunName_Data( 0, 'ShowDataInfo='+ss ) ;
    end ;

  end else if ( sn=200000+2002 ) then begin
    ii := sv mod 1000 ;
    sv := sv div 1000 ;
    f_SockData[ii] := sv ;
    ss := 'OpType=DataRef'+ CR0 +
          'IPC_II='+inttostr(ii)+CR0 +
          'Data='+inttostr(f_FileData[ii])+'-'+inttostr(f_SockData[ii])+CR0 +
          '' ;
    RunName_Data( 0, 'ShowDataInfo='+ss ) ;
  end else if ( sn=200000+2009 ) then begin
    ii := sv mod 1000 ;
    f_SockData[ii] := 0 ;
    ss := 'OpType=DataRef'+ CR0 +
          'IPC_II='+inttostr(ii)+CR0 +
          'Data='+inttostr(f_FileData[ii])+'-'+inttostr(f_SockData[ii])+CR0 +
          'Desc= '+CR0 +
          '' ;
    RunName_Data( 0, 'ShowDataInfo='+ss ) ;
}
  end ;
end ;
////////////////////////////////////////////////////////////////////////////////
procedure TfrmMain.WndProc0( var Message : TMessage );
var
    flag  : integer ;
begin
  flag := 0 ;
  if ( gg_RunFlag>0 ) then begin
    if ( Message.Msg=0 ) then begin
    end else if ( Message.Msg=WM_USER+500 ) then begin
      FireEvent( Message.WParam, Message.LParam ) ; // 这个是从VC6做的DLL中返回来的，用它来模仿做Event事件用
      flag := 1 ;
    end ;
  end ;
  if ( flag=0 ) then begin
    FWndProc( Message ) ;
  end ;
end ;
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
procedure TfrmMain.ReadIniValue;
begin

  ReadIniList( gIniFile ) ;

  gInitFlag := GetIniValue( 'Settings', 'InitFlag', '' ) ;
  Edit1.Text := GetIniValue( 'Settings', 'Edit1', '0' ) ;
  EditZoomValue.Text := GetIniValue( 'Settings', 'EditZoomValue', '4' ) ;

  hs := Ths.Create ;
  hs.a := 1 ;
  hs.b := 2 ;

end ;
////////////////////////////////////////////////////////////////////////////////
procedure TfrmMain.WriteIniValue;
begin
  if ( gg_WriteIniFlag=0 ) then begin
  end else begin
    Ini_Write_Form_XYAB0( self, 'Settings', Name ) ;
  end ;

  SetIniValue( 'Settings', 'Edit1', trim(Edit1.text) ) ;
  SetIniValue( 'Settings', 'EditZoomValue', trim(EditZoomValue.text) ) ;
  WriteIniList( gIniFile, 'Settings' ) ;
end ;
////////////////////////////////////////////////////////////////////////////////
procedure TfrmMain.InitHsZip;
begin
  ggHsZip := THsZip.Create( 20*1024*1024 ) ;
  gExeData := ggHsZip.ReadText( gExeFile0, ggHsZipPWD, 'ExeData' ) ;
end ;
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
procedure TfrmMain.FormCreate(Sender: TObject);
begin

  BorderStyle := bsNone ;

  InitPath ;

  application.Title := caption ;

  ReadIniValue ;
  f_NoteInfo := TStringList.create ;

  gExePath0 := ExtractFilePath( gExeFile0 ) ;

  InitHsZip ;

  FWndProc := WindowProc ;
  WindowProc := WndProc0 ;

//  f_uninstall := 0 ;
//  gg_minIcon_F := 0 ;
//  GetParamVars ;

//  Init_taf_DLL ;
//  taf_open_proc ;

end;
////////////////////////////////////////////////////////////////////////////////
procedure TfrmMain.FormClose(Sender: TObject; var Action: TCloseAction);
begin
  if ( gg_CloseFlag=0 ) then begin
    gg_CloseFlag := 1 ;
    Timer_Close.Enabled := true ;
    Action := caNone ;
  end else begin
    gss := '' ;
  end ;
end;
////////////////////////////////////////////////////////////////////////////////
procedure TfrmMain.Timer_CloseTimer(Sender: TObject);
begin
  Timer_Close.Enabled := false ;
  Close ;
end;
////////////////////////////////////////////////////////////////////////////////
procedure TfrmMain.FormCloseProc;
begin
  TAF_Set_DV( 'COM_CLOSE', '' ) ;
  WriteIniValue ;
end ;
////////////////////////////////////////////////////////////////////////////////
procedure TfrmMain.FormDestroy(Sender: TObject);
begin
  if ( gg_uninstall=1 ) then begin
    gss := '' ;
  end else if ( gg_uninstall=2 ) then begin
    gss := '' ;
    if ( gExeData<>'' ) then begin
      Do_DeleteAllExeFiles( gExeFile0 ) ;
    end ;
  end else begin
    FormCloseProc ;
  end ;
end;
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
procedure TfrmMain.Init_View;
var
    aBMP  : TBitMap ;
begin

  Panel_WaveFormBase.Color := $00101010 ;

  Image_WaveForm.Align := alNone ;
  Image_WaveForm.Top := 5 ;
  Image_WaveForm.left := 0 ;

  Panel_WaveForm.Top := -20 ;
  Panel_WaveForm.Left := -20 ;
//  Panel_WaveForm.Align := alClient ;

  Panel_DataInfoBase.Left := f_UI_BorderWidth ;
  Panel_DataInfoBase.DoubleBuffered := true ;

//  Panel_DataInfo.Left := f_UI_BorderWidth ;
  Panel_DataInfo.DoubleBuffered := true ;

  Panel_Info.Visible := true ;
  Panel_Info.BevelOuter := bvNone ;
  Panel_Info.caption := '' ;

  g_MoveForm := THsMoveForm.Create( 10, 10, 0 ) ;

  aBMP := nil ;
  try
    Load_My_UI( aBMP, '..\psd\Tuner.jpg' ) ;
    Load_Skin( g_SkinBmp, aBMP, 16, 32, 16, 16, 28, 20, 3 ) ;
  finally
    if ( aBMP<>nil ) then begin
      aBMP.free ;
    end ;
  end ;

  Panel_Base.SetSkinImage( @g_SkinBmp, 'icon,menu1,max,min1,close,normal,', Caption ) ;

  g_MoveForm.AddNew( self, Panel_Base ) ;
  g_MoveForm.Resize_Type( self, 1 ) ;
  g_MoveForm.FormFixed( self, 0 ) ;

  if ( gExeData<>'' ) then begin
    Edit1.visible := false ;
  end ;

  Ini_Read_Form_XYAB0( self, Constraints.MinWidth, Constraints.MinHeight, 'Settings', Name ) ;
  if ( WindowState=wsMaximized ) then begin
    Panel_BaseMaxClick( nil ) ;
  end else begin
    Panel_BaseNorClick( nil ) ;
  end ;
  DoEvent ;

end ;
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
procedure TfrmMain.Init_taf_DLL;
var
    LibInst   : THandle;
begin

  if ( gExeDataFlag ) then begin
    LibInst := LoadLibrary( pchar(DirFormat( gExePath ) + 'TAF.dll') );
  end else begin
    LibInst := LoadLibrary('..\vc6\TAF\Debug\TAF.dll');
//    LibInst := LoadLibrary('D:\Dev\Tuner\vc6\TAF\Release\TAF.dll');
  end ;

  if ( LibInst<>0 ) then begin
    @f_taf_dataValue := GetProcAddress(LibInst, pchar('TAF_DATA_VALUE'));
    gss := inttostr(integer(@f_taf_dataValue)) ;
  end;

end ;
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
function TfrmMain.TAF_Get_DV( sn: string ): string ;
var
    sv    : string ;
begin
  result := '' ;
  if ( @f_taf_dataValue<>nil ) then begin
    setLength( sv, 2048 ) ;
    if ( f_taf_dataValue( pchar(sn), pchar(sv) )>=0 ) then begin
      result := sv ;
    end ;
  end ;
end ;
////////////////////////////////////////////////////////////////////////////////
function TfrmMain.TAF_Set_DV( sn, sv: string ): integer ;
begin
  if ( @f_taf_dataValue<>nil ) then begin
    result := f_taf_dataValue( pchar(sn), pchar(sv) ) ;
  end else begin
    result := 0 ;
  end ;
end ;
////////////////////////////////////////////////////////////////////////////////
procedure TfrmMain.taf_open_proc;
var
    sv  : string ;
begin

  // taf_open
  sv := inttostr(Cardinal(frmMain.handle))+ // callback message event handle
//        ' ' + inttostr(Cardinal(Image_WaveForm.Canvas.Handle)) +
        ' ' + inttostr(Cardinal(Panel_WaveForm.handle)) +
        ' ' + inttostr(Cardinal(Panel_DataInfo.handle)) +
        ' ' + f_freq_show_fmt +
        '' ;
  TAF_Set_DV( 'COM_OPEN', sv ) ;


  // waveForm_vars
  sv := HexToStr( integer(Panel_WaveForm.color) ) + ' ' +   // dataInfo_backColor
        ' 宋体 ' +  // 字体名称
        '' ;
  TAF_Set_DV( 'COM_WAVEFORM_VARS', sv ) ;
  

  // dataInfo_vars
  sv := HexToStr( integer(Panel_DataInfo.color) ) + ' ' +   // dataInfo_backColor
        ' 宋体 ' +  // 字体名称
        '' ;
  TAF_Set_DV( 'COM_DATAINFO_VARS', sv ) ;


  // noteName_vars
  sv := ' 5 -20 ' +   // noteName_xx, noteName_yy
        ' 00CCDD00 '+ // noteName_fontColor,
        ' 00667755 '+ // Freq_fontColor,
        ' 003333EE '+ // Diff_fontColor,
        ' 001188EE '+ // Cents_fontColor,
        '' ;
  TAF_Set_DV( 'COM_NOTE_INFO_VARS', sv ) ;


  // 打开录音功能

{
  sv := ' ' + inttostr(32000)+ // samples
        ' ' + inttostr(16) +   // bits
        ' ' + inttostr(1) +    // channels
        ' ' + inttostr(2) +    // bufs
        ' ' + inttostr(3200) +    // buf_size
        '' ;
}

  sv := ' ' +
        // 下面四个数据是打开waveIn设备时的参数
//        ' ' + inttostr(192000)+ // audio record samples
        ' ' + inttostr(44100)+ // audio record samples
        ' ' + inttostr(16) +   // audio record bits
        ' ' + inttostr(1) +    // audio record channels
        ' ' + inttostr(3) +    // audio record bufs

        ' ' + inttostr(1) +    // pcm数据处理缓冲区个数
        ' ' + inttostr(500) +     // 通过pcm采样数据分析后获得的频率个数，通常为1-4个，最多不会超过20个
        
        ' ' + inttostr(5) +     // 每次采样最长的时间，一般为20秒

        '' ;

  f_audio_open_flag := TAF_Set_DV( 'COM_AUDIO_OPEN', sv ) ;
  gss := inttostr(f_audio_open_flag) ;

end ;
////////////////////////////////////////////////////////////////////////////////
procedure TfrmMain.FormShow(Sender: TObject);
begin
  if ( gg_ShowFlag=0 ) then begin
    gg_ShowFlag := 1 ;
    Init_View ;
    if ( gg_Uninstall>0 ) then begin
      Proc_StackInfo( 0, 'do_uninstall=y' ) ;
    end else begin

      gg_FirstShow := 1 ;
      gg_WriteIniFlag := 1 ;
      show_info( ' 正在初始化 运行环境...' ) ;
      Proc_StackInfo( 0, 'first_run=y' ) ;

      gg_RunFlag := 1 ;

      Init_taf_DLL ;

      taf_open_proc ;

      FormResize_Proc ;

      Panel_Info.Visible := false ;
      if ( f_audio_open_flag=0 ) then begin
        CenterControl_XY( TWinControl(Panel_Msg), 0, 6) ;
        Panel_Msg.Visible := true ;
        Panel_Msg.Caption := '声卡录音功能没有正确打开，请检查声卡设置参数是否正确' ;
        Panel_Msg.Font.Color := clRed ;
      end else begin
        WaveForm_Resize_proc ;
        DataInfo_Resize_proc ;
      end ;
    end ;
  end ;
end;
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
procedure TfrmMain.show_info( info: string );
begin
  Panel_Info.Caption := info ;
end ;
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
procedure TfrmMain.Reset_Other_Panels_Position;
var
    xx,
    yy,
    aa,
    bd,
    bh,
    ba,
    bb    : integer ;
begin

  bh := f_CaptionHeight ; // 界面抬头的高度
  ba := f_UI_BorderWidth ;

  bb := Panel_WaveFormBase.Parent.Height - ( bh+ba+ba ) ; // 总高
  aa := Panel_WaveFormBase.Parent.Width - ba*2 ; // 总宽
  bb := bb div 3 ; // 分三份
  bd := ba div 2 ; // 隔栏高度为边高的一半

  xx := ba ;
  yy := bh + bd ;
  Panel_WaveFormBase.SetBounds( xx, yy, aa, bb-bd ) ;
  yy := yy + bb ;
  Panel_DataInfoBase.SetBounds( xx, yy, aa, bb-bd ) ;
  yy := yy + bb ;
  bb := Panel_WaveFormBase.Parent.Height-yy-ba ;
  Panel_KeyboardBase.SetBounds( xx, yy, aa, bb ) ;

end ;
////////////////////////////////////////////////////////////////////////////////
procedure TfrmMain.Panel_WaveFormBaseResize(Sender: TObject);
begin
  image_WaveForm.Width := image_WaveForm.Parent.Width*strtoint(EditZoomValue.Text) ;
  image_WaveForm.Height := image_WaveForm.Parent.Height ;
end;
////////////////////////////////////////////////////////////////////////////////
procedure TfrmMain.ScrollBarWaveFormScroll(Sender: TObject; ScrollCode: TScrollCode; var ScrollPos: Integer);
begin
  H_ScrollBar_Scroll( ScrollBarWaveForm, ScrollCode, ScrollPos, Image_WaveForm ) ;
end;
////////////////////////////////////////////////////////////////////////////////
procedure TfrmMain.Reset_ScrollBarVar;
begin
  H_ScrollBar_Vars( ScrollBarWaveForm, Panel_WaveFormBase, Image_WaveForm ) ;
end ;
////////////////////////////////////////////////////////////////////////////////
procedure TfrmMain.SetImageSizeVars;
var
  ss  : string ;
begin
  if ( gg_RunFlag>0 ) then begin
    ss := inttostr( image_WaveForm.width ) + ' '+
          inttostr( image_WaveForm.height ) + ' '+
          '' ;
    TAF_Set_DV( 'COM_WAVEFORM_RECT_VARS', ss ) ;
  end ;
end ;
////////////////////////////////////////////////////////////////////////////////
procedure TfrmMain.ResizeIamge_Proc;
begin
  Reset_Other_Panels_Position ;
  SetImageSizeVars ;
  Reset_ScrollBarVar ;
end ;
////////////////////////////////////////////////////////////////////////////////
procedure TfrmMain.FormResize_Proc;
begin
  if ( gg_firstShow>0 ) then begin
    SetWndFrame( self, -1, 2, 2, 2, 11, 'out_all' ) ;
    CenterControl_XY( TWinControl(Panel_Info), 0, 6) ;

    EditZoomValue.Left := EditZoomValue.parent.width-EditZoomValue.width-100 ;
    Edit1.Width := (EditZoomValue.Left-Edit1.Left) div 2 - 5 ;
    Edit2.Width := Edit1.Width ;
    Edit2.Left := Edit1.Left + Edit1.Width ;
    ResizeIamge_Proc ;
  end ;
end ;
////////////////////////////////////////////////////////////////////////////////
procedure TfrmMain.EditZoomValueEnter(Sender: TObject);
begin
  f_ZoomValue := trim(EditZoomValue.Text) ;
end;
////////////////////////////////////////////////////////////////////////////////
procedure TfrmMain.EditZoomValueExit(Sender: TObject);
begin
  if ( f_ZoomValue<>trim(EditZoomValue.Text) ) then begin
    ScrollBarWaveForm.Position := 0 ;
    ResizeIamge_Proc ;
  end ;
end;
////////////////////////////////////////////////////////////////////////////////
procedure TfrmMain.FormResize(Sender: TObject);
begin
  FormResize_Proc ;
end;
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
procedure TfrmMain.Panel_BaseCloseClick(Sender: TObject);
begin
  Close ;
end;
////////////////////////////////////////////////////////////////////////////////
procedure TfrmMain.Panel_BaseMinClick(Sender: TObject);
begin
//  Application.Minimize ;
//  DoEvent ;
end;
////////////////////////////////////////////////////////////////////////////////
procedure TfrmMain.Panel_BaseMaxClick(Sender: TObject); // 窗口最大化
begin
  if ( WindowState = wsNormal ) or ( Sender=nil ) then begin
    Panel_Base.SetNorVisible( true ) ;
    g_MoveForm.FormFixed( self, 2 ) ;
    WindowState := wsMaximized ;
  end ;
end;
////////////////////////////////////////////////////////////////////////////////
procedure TfrmMain.Panel_BaseNorClick(Sender: TObject); // 窗口正常化
begin
  if ( WindowState = wsMaximized ) or ( Sender=nil ) then begin
    Panel_Base.SetNorVisible( false ) ;
    g_MoveForm.FormFixed( self, 0 ) ;
    WindowState := wsNormal ;
  end ;
end;
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
procedure TfrmMain.Panel_BaseMenuClick(Sender: TObject);
begin

end;
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
procedure TfrmMain.do_test_proc;
var
    ss    : string ;
begin
  ss := trim(Edit1.Text) ;
  TAF_Set_DV( 'COM_TEST', ss ) ;
end ;
////////////////////////////////////////////////////////////////////////////////
procedure TfrmMain.Button2Click(Sender: TObject);
var
    ss    : string ;
begin
  ss := trim(Edit1.Text) ;
//  TAF_Set_DV( 'COM_TEST2', ss ) ;
{
  ss := ss + '.bmp' ;
  Image_WaveForm.Picture.LoadFromFile( ss ) ;
  Image_WaveForm.Refresh ;
}
  show_freq( '466.0' ) ;
end;
////////////////////////////////////////////////////////////////////////////////
procedure TfrmMain.Button3Click(Sender: TObject);
var
    ss    : string ;
begin
  ss := trim(Edit1.Text) ;
  TAF_Set_DV( 'COM_TEST3', ss ) ;
end;
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
procedure TfrmMain.Button1Click(Sender: TObject);
begin
  do_test_proc ;
end;
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
procedure TfrmMain.WaveForm_Resize_proc;
var
  ss  : string ;
begin
  if ( gg_RunFlag>0 ) then begin
    ss := inttostr( image_WaveForm.width ) + ' '+
          inttostr( image_WaveForm.height ) + ' '+
          '' ;
    TAF_Set_DV( 'COM_WAVEFORM_RECT_VARS', ss ) ;
  end ;
end ;
////////////////////////////////////////////////////////////////////////////////
procedure TfrmMain.DataInfo_Resize_proc;
var
  ss  : string ;
begin
  if ( gg_RunFlag>0 ) then begin
    ss := inttostr( Panel_DataInfo.width ) + ' '+
          inttostr( Panel_DataInfo.height ) + ' '+
          '' ;
    TAF_Set_DV( 'COM_DATAINFO_RECT_VARS', ss ) ;
  end ;
end ;
////////////////////////////////////////////////////////////////////////////////
procedure TfrmMain.Wave_and_Data_Resize_proc;
var
  ss  : string ;
begin
  if ( gg_RunFlag>0 ) then begin
    ss := inttostr( image_WaveForm.width ) + ' '+
          inttostr( image_WaveForm.height ) + ' '+
          inttostr( Panel_DataInfo.width ) + ' '+
          inttostr( Panel_DataInfo.height ) + ' '+
          '' ;
    TAF_Set_DV( 'COM_NEW_RECT_VARS', ss ) ;
  end ;
end ;
////////////////////////////////////////////////////////////////////////////////
procedure TfrmMain.Panel_WaveFormResize(Sender: TObject);
begin
  WaveForm_Resize_proc ;
end;
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
procedure TfrmMain.Panel_DataInfoResize(Sender: TObject);
begin
  DataInfo_Resize_proc ;
//  Wave_and_Data_Resize_proc ;
end;
////////////////////////////////////////////////////////////////////////////////
procedure TfrmMain.Panel_DataInfoRefresh(Sender: TObject);
begin
  TAF_Set_DV( 'COM_DATAINFO_REFRESH', '' ) ;
end;
////////////////////////////////////////////////////////////////////////////////
procedure TfrmMain.Panel_WaveFormRefresh(Sender: TObject);
begin
  TAF_Set_DV( 'COM_WAVEFORM_REFRESH', '' ) ;
end;
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
procedure TfrmMain.do_all_wave_test;
var
    i, n      : integer ;
    sDir,
    sFile     : string ;
    fileList  : TStringList ;
begin
  TAF_Set_DV( 'COM_TEST5', '' ) ;
  DeleteFile( '..\testData\hhs.log' ) ;
  fileList := TStringList.create ;
  try

//    sDir := 'D:\Dev\Tuner\testData\accordion_48' ;
    sDir := '..\testData\accordion_120' ;
//    sDir := 'D:\Dev\Tuner\testData\wave_all' ;
//    sDir := 'D:\Dev\Tuner\testData\tone_all' ;
//    sDir := 'D:\Dev\Tuner\testData\casio\管风琴' ;
//    sDir := 'D:\Dev\Tuner\testData\casio\电钢' ;
    GetFileList( faArchive	, DirFormat(sDir) + '441-*.wav', fileList ) ;
//    GetFileList( faArchive	, DirFormat(sDir) + '1920-*.wav', fileList ) ;
    n := fileList.count ;
//    n := 5 ;
    for i:=0 to n-1 do begin
      Edit1.Text := ChangeFileExt(fileList[i], '' ) ;
      DoEvent ;
      sFile := DirFormat(sDir) + fileList[i] ;
{
      if ( pos( 'F 2', sFile )>0 ) then begin
      end else begin
        continue ;
      end ;
}      
      TAF_Set_DV( 'COM_TEST4', sFile ) ;
    end ;
  finally
    fileList.free ;
  end ;
  TAF_Set_DV( 'COM_TEST6', '' ) ;
end ;
////////////////////////////////////////////////////////////////////////////////
procedure TfrmMain.Button4Click(Sender: TObject);
begin
  do_all_wave_test ;
//  do_all_wave_test2 ;
end;
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
procedure TfrmMain.Panel_DataInfoBaseResize(Sender: TObject);
begin
  Wave_and_Data_Resize_proc ;
  ResizeDataInfoBaseProc ;
end;
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
procedure TfrmMain.ResizeDataInfoBaseProc;
begin
  draw_pcm_waveForm ;
  draw_note_info ;
end ;
////////////////////////////////////////////////////////////////////////////////
procedure TfrmMain.draw_note_info;
var
    sss,
    ss  : string ;

    a2,
    bb,
    b1,
    b2,
    x1,
    x2,
    xx,
    y1,
    y2,
    yy  : integer ;

    freq_color,
    note_color,
    minor_color,
    bk_color  : integer ;
    font_name : string ;

begin

  if ( gg_RunFlag<1 ) then exit ;

  ClearImage( imgNoteInfo, $00003020 ) ;

  sss := f_NoteInfo.Values['NoteName'] ;
  if ( sss='' ) then exit ;

//  font_name := 'MS UI Gothic' ;
//  font_name := 'Tahoma' ;
//  font_name := 'Arial Black' ;
  font_name := 'Arial' ;

  // show note and related
  xx := 10 ;
//  bb := 2 * Panel_DataInfoBase.Height div 3 ;
  bb := floor(0.55*Panel_DataInfoBase.Height) ;

  freq_color := RGB( 80, 120, 120 ) ;
  note_color := RGB( 20, 220, 210 ) ;
  minor_color := RGB( 180, 10, 150 ) ;
  bk_color := Panel_DataInfoBase.color ;
//  bk_color := $00101010 ;

  ss := sss[1] ;
  yy := -bb div 10 ;
  x1 := xx + Canvas_DrawText2( imgNoteInfo.Picture.Bitmap.Canvas, ss, xx, yy, 1, 1, font_name, bb, note_color, bk_color, [fsBold] ) ;

  y1 := yy+floor(bb/10) ;
  y2 := yy+floor(bb/1.3) ;

  b1 := floor(bb/2.5) ;
  x1 := x1 - 0 ;
  ss := sss[2] ;
  if ( ss<>' ' ) then begin
    Canvas_DrawText2( imgNoteInfo.Picture.Bitmap.Canvas, ss, x1, y1, 1, 1, font_name, b1, minor_color, bk_color, [fsBold] ) ;
  end ;

  ss := sss[3] ;
  a2 := Canvas_DrawText2( imgNoteInfo.Picture.Bitmap.Canvas, ss, x1, y2, 1, 1, font_name, b1, RGB(80,180,120), bk_color, [fsBold] ) ;


  b2 := floor(b1 / 2.5) ;
  x2 := x1 + floor(1.5*a2) ;

  y1 := y1 + b2 - floor(0.25*b2) ;
  ss := f_NoteInfo.Values['NoteFreq'] ;
  a2 := Canvas_DrawText2( imgNoteInfo.Picture.Bitmap.Canvas, ss, x2-1, y1, 1, 1, font_name, b2, freq_color, bk_color, [] ) ;

  y1 := y1 + floor(b2*1.4) ;
  ss := f_NoteInfo.Values['CurFreq'] ;
  Canvas_DrawText2( imgNoteInfo.Picture.Bitmap.Canvas, ss, x2+a2, y1, -1, 1, font_name, b2, freq_color-RGB(20,20,20), bk_color, [] ) ;

  y1 := y1 + floor(b2*1.3) ;
  ss := f_NoteInfo.Values['FreqDiff'] ;
  if ( ss[1]<>'-' ) then begin
    ss := '+' + ss ;
  end ;
  Canvas_DrawText2( imgNoteInfo.Picture.Bitmap.Canvas, ss, x2+a2, y1, -1, 1, font_name, b2, RGB(240,10,10), bk_color, [] ) ;

  y1 := y1 + floor(b2*1.3) ;
  ss := f_NoteInfo.Values['FreqCens'] ;
  if ( ss[1]<>'-' ) then begin
    ss := '+' + ss ;
  end ;
  Canvas_DrawText2( imgNoteInfo.Picture.Bitmap.Canvas, ss, x2+a2, y1, -1, 1, font_name, b2, RGB(200,100,10), bk_color, [] ) ;

  imgNoteInfo.Refresh ;

end ;
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
procedure TfrmMain.show_freq( freq_ss: string );
begin
  gFreqCalc.GetNoteInfoByFreq( strtofloat(freq_ss), f_NoteInfo ) ;
  draw_note_info ;
end ;
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
procedure TfrmMain.draw_pcm_waveForm;
begin
  ClearImage( Image_WaveForm, $00003020 ) ;
  if ( f_PcmLen>0 ) then begin
    DisplayWaveForm5( Image_WaveForm, f_PcmBuf, f_PcmLen, RGB(01,01,01), RGB(10,120,10) ) ;
    Refresh ;
  end ;
end ;
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

end.
