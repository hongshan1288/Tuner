// TAF.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "TAF.h"
#include "AudioProc.h"
#include "ComUtils.h"
#include "FreqByPcm.h"
#include "TWavePeriod.h"
#include "TWavePeriod2.h"

//#define from_proc_prt	log_printf_ex
#define from_proc_prt	//

#define MAX_PCM_DIV		10
#define	MIN_AMP_MEAN	5


void init_fbp() ;

void do_new_show_vars_proc() ;
void do_refresh_proc() ;
void waveForm_show_proc( char *from_proc ) ;

void make_waveForm_show_data( char *from_proc ) ;
void draw_waveForm_show_data( long dot_show_flag, char *from_proc ) ;

//////////////////////////////////////////////////////////////////////
long	Set_Com_Data( char *DataName, char *DataValue ) ;
long test_proc( char *DataValue ) ;
long test2_proc( char *DataValue ) ;
void test3_proc( char *DataValue ) ;
void test4_proc( char *DataValue ) ;
void test5_proc( char *DataValue ) ;
void test6_proc( char *DataValue ) ;

long do_test_wave_proc( char *wav_file ) ;

void audio_close_proc() ;
void clear_wave_area() ;
void clear_freq_area() ;

void pcm_data_filter_1( short *pcm_data, long pcm_len, long yy_div ) ;
void do_pcmData_in( short *pcm_data, long pcm_len ) ;
void do_pcmData_in_new( short *pcm_data, long pcm_len ) ;

void show_freq_from_pcm_data( short *pcm_data, long pcm_len ) ;
void make_freq_from_pcm_data( short *pcm_data, long pcm_len ) ;
long make_freq_from_pcmData( long print_flag, long pcm_flag, short *pcm_data, long pcm_len, char *from_proc ) ;
void do_freq_compare_proc( long ni, double freq, double freq_nn ) ;
void waveForm_refresh() ;
void waveForm_reshow( long dot_show_flag ) ;
void make_noteShow_data( short ni, double note_freq ) ;
void draw_noteName_new( short ni, double note_freq ) ;
void noteShow_proc( long clear_flag ) ;
void make_waveForm() ;

void do_showFreq_proc() ;
void do_vars_proc() ;
void do_wave_data_proc( long waveFileNo ) ;
void do_wave_data_proc_2() ;
void do_wave_data_from_file2( char *sFile ) ;
long do_testFile_data_proc() ;

void show_WaveForm_proc( short *pcm_data, long pcm_len, long data_color ) ;
void push_pcm_proc( BYTE *pcm_buf, long pcm_len ) ;
void push_pcm_proc_new( BYTE *pcm_buf, long pcm_len ) ;

void print_tj_data() ;
void print_ok_data() ;
void print_filter_data() ;
void print_filter_data2() ;


void print_freq_tj_data() ;

void init_pmd() ;

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
#define NOTE_INFO_MAX	128
//////////////////////////////////////////////////////////////////////
struct	sort_Data
{
	long	ii ;
	long	vv ;
} ;

struct	Note_Info
{
	char	note_name[4] ;
	double	note_freq ;
} ;

struct	Show_Info
{
	char	show_name[20] ;

	long	show_x, show_y ;
	long	show_fx, show_fy ;

	long	font_color, back_color ;
	long	font_height, font_weight ;

} ;

struct	Note_Show
{
	// 音符信息
	Show_Info	note ;
	Show_Info	note_aux ;
	Show_Info	note_idx ;

	// 频率信息
	Show_Info	freq_std ;
	Show_Info	freq_cur ;
	Show_Info	freq_dif ;
	Show_Info	freq_cns ;

	// 辅助信息
	Show_Info	freq_pcnt ;

} ;


struct	FREQ_TJ_DATA
{
	long	ok_flag ;
	double	freq_dif ;
	double	freq_nn, freq_vv ;
} ;




struct	TJ_DATA
{
	long	tj_iii ;
	double	tj_freq_dif ;
} ;

struct	TEST_DATA
{
	char	music_note[10] ;
	double	music_note_freq ;
	long	wrong_note_nn, wrong_note_nn_tot ;
	double	note_freq_dif, note_freq_dif_tot ;

	long	freq_dif_change_nn_tot ;
	double	freq_dif_change_vv_tot ;


	long	filter_problem_0 ;
	long	filter_problem_1 ;
	long	filter_problem_2 ;

	long	tot_filter_problem_0 ;
	long	tot_filter_problem_1 ;
	long	tot_filter_problem_2 ;


} ;



struct	MM_DATA
{
	long	x, y, f ;
	long	freq_ff, freq_ii, freq_done ;
} ;

struct	ZMZ_DATA
{
	long	i1, i2, ff, ff_nn ;
	long	i0, max_ii, max_vv ;
	long	xx_div, max_xx ;
	long	ii ;

	long	freq_ff, freq_done ;
	long	freq_vv, freq_nn ;
	double	freq_xx ;


	long	freq_f2, freq_v2, freq_n2, freq_d2 ;

	long	tot_xx ;
	double	x1, x2, xx, yy ;
	double	tot_vv ;		// 积分值，也是面积值
	double	ff_tot_vv ;		// 积分值，但是乘上了极值变化个数的值
	long	pole_xx ;		// 极值点与中心点所在的相对位置（如果是多个极值点，此时只取最后一个极值点的位置）
	long	first_ff, first_prev_ii, first_freq_nn ;
	double	first_vv ;


} ;

struct	FREQ_DATA
{
	short	freq_ff ; // 指定的极值参数标识
	short	freq_is ; // 起始x点
	short	freq_ie ; // 结束x点
	long	freq_nn ; // 连续个数

	double	freq_vv ; // 指定极值的频率值

	double	freq_avg ; // 指定极值的频率的平均值

	long	near_same ;
	long	freq_flag ;


	double	freq_max ; // 指定极值的频率的最大值
	double	freq_min ; // 指定极值的频率的最小值

	double	freq_dif ; // 指定极值的平均值与总平均值之差

	double	each_vvv ;	// 相互差分值
	double	each_dif ;	// 相互差分值之差



} ;

//////////////////////////////////////////////////////////////////////


short	g_pcm_data_half_ii=59 ;
//short	g_pcm_data_half_ii=69 ;

Note_Info	g_note_info[NOTE_INFO_MAX] ;


long	g_ExitFlag = 0 ;
char	g_waveForm_fontName[50], g_dataInfo_fontName[50] ;

long	g_dataInfo_cx ; // 数据显示区的中间分隔位置
long	g_NoteName_fontColor ;

long	g_NoteName_xx, g_NoteName_yy, g_NoteName_fontHeight, g_NoteAux_fontHeight, g_NoteIdx_fontHeight ;
long	g_Freq_fontColor, g_Freq_fontHeight, g_Diff_fontColor, g_Cents_fontColor ;
long	g_dataInfo_refresh=0, g_waveForm_refresh=0 ;

char	g_dataInfo_rect_vars[100], g_waveForm_rect_vars[100] ;
char	g_new_rect_vars[100] ;


short	*g_wave_data_buf, *g_wave_show_buf ;

long	g_wave_data_flag=0, g_wave_show_flag=0 ;
long	g_wave_data_size ;

short	*g_waveForm_pcm ;
long	g_waveForm_pcm_len=0 ;

double	g_curFreq=0, g_showFreq=0 ;

long	g_test=0 ;
char	g_cur_file[500] ;

long	g_xx_div ;
long	g_pcm_data_max, g_pcm_data_pos ;
long	g_pcm_data_is, g_pcm_data_ie ;
long	g_pcm_data_cn ;
LPBYTE	g_pcm_data ;
POINT	*g_xy_data ;


ZMZ_DATA	*g_zmz_data ;
long		g_zmz_nn=0, g_zmz_freq, g_zmz_change, g_zmz_0_pass ;
double	*g_pcm_check_data ;


MM_DATA *g_mm_data ;
long	g_mm_data_len ;

FREQ_DATA	*g_freq_data ;
long		g_freq_data_nn=0, g_freq_data_len ;
double		g_freq_dif_tot, g_each_dif_tot ;
sort_Data	*g_sort_data ;




char	g_from_proc[200] ;
char	g_testFile[200] ;
long	g_testFile_pos, g_testFile_len, g_testFile_tot ;


TEST_DATA	g_test_data ;

TJ_DATA		*g_tj_data ;
long		g_tj_data_nn, g_tj_data_ff=0 ;

char		g_note_show_fmt[30] ;

long		g_note_pcm_nn, g_note_pcm_do, g_note_pcm_ok1, g_note_pcm_ok2, g_note_pcm_ok3 ;


FREQ_TJ_DATA	*g_freq_tj ;
long			g_freq_tj_nn, g_freq_tj_max=500 ;
double			g_freq_tot, g_freq_avg, g_freq_dif, g_freq_vv ;
long			g_freq_nn ;

POINT			*g_waveForm_data ;
long			g_waveForm_data_aa=0, g_waveForm_data_nn=0 ;
long			g_print_flag=1 ;

long			g_waveForm_save=0 ;
long			g_note_nn=0, g_note_flag=1 ;
char			g_note_file[500] ;


Note_Show		g_freq_show ;

long			g_waveForm_xy_x1, g_waveForm_xy_x2 ;

long			g_waveForm_xy_a1, g_waveForm_xy_a2 ;
long			g_waveForm_xy_i1, g_waveForm_xy_i2 ;
long			g_waveForm_xy_i3, g_waveForm_xy_i4 ;

long			g_waveForm_redraw=0, g_dataInfo_redraw=0 ;

long			g_waveForm_show_nn=20 ;


LPBYTE			g_all_pcm ;
long			g_all_pcm_max=0, g_all_pcm_pos=0 ;


double			g_freq_avg_vv ;

long		g_test_data_flag=0 ;
char		g_test_pcm_file[500] ;
char		g_test_pcm_name[100] ;

//////////////////////////////////////////////////////////////////////
void show_mm_data_proc( MM_DATA *mm_data, long mm_si, long mm_ei, long xx_len, long mm_color ) ;

AudioProc	g_Audio ;
double		g_Samples ;
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
CRITICAL_SECTION	cs_WaveData={0} ;
long	flag_WaveData=0 ;
//////////////////////////////////////////////////////////////////////
long	do_WaveData( long flag, short *wave_buf )
{

	long	ret_val=0 ;

	if ( flag_WaveData==0 )
	{
		InitializeCriticalSection(&cs_WaveData);
		flag_WaveData = 1 ;
	}
	EnterCriticalSection(&cs_WaveData) ;
	__try
	{
		switch ( flag )
		{
			case	101	: 
							memcpy( g_wave_data_buf, wave_buf, g_wave_data_size ) ;
							g_wave_data_flag = 1 ;
							break ;
			case	102	: 
							if ( g_wave_data_flag>0 )
							{
								memcpy( g_wave_show_buf, g_wave_data_buf, g_wave_data_size ) ;
								g_wave_data_flag = 0 ;
								g_wave_show_flag = 1 ;
								ret_val = 1 ;
							}
							break ;
		}
	}
	__finally
	{
		LeaveCriticalSection(&cs_WaveData);
	}

	return ( ret_val ) ;

}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
CRITICAL_SECTION	cs_Task={0} ;
long	flag_Task=0 ;
//////////////////////////////////////////////////////////////////////
long	do_task( long flag, char *dataVars )
{

	long	ret_val ;

	if ( flag_Task==0 )
	{
		InitializeCriticalSection(&cs_Task);
		flag_Task = 1 ;
	}
	EnterCriticalSection(&cs_Task) ;
	__try
	{
		ret_val = 0 ;
		switch ( flag )
		{
			case	101	: 
							strcpy( g_waveForm_rect_vars, dataVars ) ;
							break ;
			case	201	: 
							if ( g_waveForm_rect_vars[0] !='\0' )
								ret_val = 201 ;
							break ;
			case	102	: 
							strcpy( g_dataInfo_rect_vars, dataVars ) ;
							break ;
			case	202	: 
							if ( g_dataInfo_rect_vars[0] !='\0' )
								ret_val = 202 ;
							break ;
			case	103	: 
							strcpy( g_new_rect_vars, dataVars ) ;
							break ;
			case	203	: 
							if ( g_new_rect_vars[0] !='\0' )
								ret_val = 203 ;
							break ;


			case	105	: 
							sscanf( dataVars, "%lu", &g_dataInfo_refresh ) ;
							break ;
			case	205	: 
							ret_val = g_dataInfo_refresh ;
							break ;

			case	106	: 
							sscanf( dataVars, "%lu", &g_waveForm_refresh ) ;
							break ;
			case	206	: 
							ret_val = g_waveForm_refresh ;
							break ;


			case	110	: 
							g_waveForm_refresh = 1 ;
							break ;

			case	111	: 
							g_waveForm_redraw = 1 ;
							break ;


//			case	201	:
//							sscanf( dataVars, "%lf", &g_curFreq ) ;
//							g_dataInfo_refresh = 1 ;

//							break ;

			case	501	:
							if ( dataVars[0] !='\0' )
								sscanf( dataVars, "%ld", &g_ExitFlag ) ;
							break ;
			case	502	:
							ret_val = g_ExitFlag ;
							break ;


			case	1001	: 
							do_vars_proc() ;
							break ;
			case	1002	: 
							waveForm_reshow( 0 ) ;
							break ;
			case	1003	: 
							do_showFreq_proc() ;
							break ;
		}
	}
	__finally
	{
		LeaveCriticalSection(&cs_Task);
	}

	return ( ret_val ) ;

}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
static	long tick_ThreadProc()
{

	HANDLE	tick_event ;

	tick_event = CreateEvent( NULL, FALSE, FALSE, NULL ) ; 
	if ( tick_event==NULL )
		return ( 0 ) ;

	ResetEvent(tick_event) ;

	PopEvent( 2001, 1, "tick_ThreadProc()" ) ; // tick_ThreadProc () 

	long	flag ;

	for ( ;; )
	{

		flag = do_task( 502, "" ) ;
		if ( flag>=1000 )
			break ;

		WaitForSingleObject( tick_event, 20 ) ; 

//		do_wave_data_proc( 0 ) ;
//		do_wave_data_proc_2() ;
		
//		do_task( 1001, "" ) ; // do_vars_proc() ;
//		do_task( 1002, "" ) ; // waveForm_reshow() ;
//		do_task( 1003, "" ) ; // do_showFreq_proc() ;

	}

	do_task( 501, "2000" ) ;

	PopEvent( 2008, 1, "tick_ThreadProc()" ) ; // tick_ThreadProc () 


	return ( 0 ) ;
}
//////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
long	quick_sort_data( sort_Data *s_data, long iLo, long iHi )
{
	long		iSize, Lo, Hi, Mid ;
	sort_Data	a_data ;

	iSize = sizeof( sort_Data ) ;

	Lo = iLo;
	Hi = iHi;
  
	Mid = (s_data[Lo].vv+s_data[Hi].vv) / 2;

	while ( true )
	{
		while ( s_data[Lo].vv < Mid )
			Lo++ ;
		while ( s_data[Hi].vv > Mid )
			Hi-- ;

		if ( Lo <= Hi )
		{
//			memcpy( &a_data, &s_data[Lo], iSize ) ;
//			memcpy( &s_data[Lo], &s_data[Hi], iSize ) ;
//			memcpy( &s_data[Hi], &a_data, iSize ) ;

			a_data.ii = s_data[Lo].ii ;
			a_data.vv = s_data[Lo].vv ;

			s_data[Lo].ii = s_data[Hi].ii ;
			s_data[Lo].vv = s_data[Hi].vv ;

			s_data[Hi].ii = a_data.ii ;
			s_data[Hi].vv = a_data.vv ;

			Lo++;
			Hi--;

		}
		if ( Lo>Hi )
			break ;
    }

	if ( Hi > iLo )
		quick_sort_data( s_data, iLo, Hi);
	if ( Lo < iHi )
		quick_sort_data( s_data, Lo, iHi);
  
	return ( 0 ) ;
}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
long waveIn_Data_Proc( LPVOID dataBuf, long dataSize, LPVOID lpparam )
{
	if ( g_test_data_flag>0 )
		return ( 0 ) ;

	if ( g_waveForm_save>0 )
		AppendBufToFile( "c:\\hhs.wav", dataBuf, dataSize ) ;

//	do_WaveData( 101, (short*)dataBuf ) ;

	do_pcmData_in_new( (short*)dataBuf, dataSize/2 ) ;

	return ( 0 ) ;
}
//////////////////////////////////////////////////////////////////////
void	audio_close_proc()
{
	g_Audio.Audio_Stop() ;
	g_Audio.Audio_Close() ;
//	FreeBuf( g_pcm_data ) ;
//	FreeBuf( g_int_data ) ;
//	FreeBuf( g_mm_data ) ;
//	FreeBuf( g_freq_data ) ;
}
//////////////////////////////////////////////////////////////////////
void	open_all_pcm_data( long max_sec, long buf_size )
{

	init_fbp() ;

	g_all_pcm_max = max_sec*buf_size*MAX_PCM_DIV ;

//	g_all_pcm_max = buf_size*20 ;
//	g_all_pcm_max = max_sec*buf_size*MAX_PCM_DIV ;

	g_all_pcm = (LPBYTE)GlobalAlloc( GPTR, (g_all_pcm_max)*sizeof(BYTE) ) ;
	if ( g_all_pcm==NULL )
		g_all_pcm_max = 0 ;
	g_all_pcm_pos = 0 ;
/**
	g_all_pcm_max = max_sec*MAX_PCM_DIV ;
	g_all_pcm = (All_Pcm*)GlobalAlloc( GPTR, (g_all_pcm_max)*sizeof(All_Pcm*) ) ;
	g_all_pcm_nn = 0 ;
**/
}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
long	audio_open_proc( char *DataValue )
{
	long	samples, bits, channels, bufs, buf_size, data_tot, freq_tot, max_sec ;

	sscanf( DataValue, "%ld %ld %ld %ld %ld %ld %ld", &samples, &bits, &channels, &bufs, &data_tot, &freq_tot, &max_sec ) ;

	buf_size = (bits/8)*samples / MAX_PCM_DIV ;

	g_wave_data_size = buf_size ;

	g_Audio.m_Caller = (LPVOID)NULL ;
	g_Audio.m_WaveIn_Data_Proc = waveIn_Data_Proc ;

	if ( g_Audio.Audio_Open( 0, samples, bits, channels, bufs, buf_size )>=0 )
	{

		g_Samples = samples ;

/**
		g_wave_data_buf = (short*)GlobalAlloc( GPTR, g_wave_data_size ) ;
		g_wave_show_buf = (short*)GlobalAlloc( GPTR, g_wave_data_size ) ;

		g_waveForm_xy_len = g_wave_data_size/2 ;
		g_waveForm_xy = (POINT*)GlobalAlloc( GPTR, g_waveForm_xy_len*sizeof(POINT) ) ;

		g_waveForm_pcm = (short*)GlobalAlloc( GPTR, g_wave_data_size ) ;

		// open all bufs here
		g_pcm_data_max = data_tot ;
		g_pcm_data_max *= buf_size ;
		g_pcm_data_pos = 0 ;

		g_pcm_data =  (BYTE*)GlobalAlloc( GPTR, g_pcm_data_max ) ;
		g_int_data =  (short*)GlobalAlloc( GPTR, g_pcm_data_max ) ;

//		g_xy_data =  (POINT*)GlobalAlloc( GPTR, 4096*sizeof(POINT) ) ;
		g_xy_data =  (POINT*)GlobalAlloc( GPTR, g_pcm_data_max*sizeof(POINT) ) ;

		g_waveForm_data =  (POINT*)GlobalAlloc( GPTR, g_pcm_data_max*sizeof(POINT) ) ;
		

		g_mm_data_len = g_pcm_data_max/5 ;
		g_mm_data = (MM_DATA*)GlobalAlloc( GPTR, (g_mm_data_len)*sizeof(MM_DATA) ) ;

		g_zmz_data = (ZMZ_DATA*)GlobalAlloc( GPTR, (g_pcm_data_max*max_sec)*sizeof(ZMZ_DATA) ) ;

		g_pcm_check_data = (double*)GlobalAlloc( GPTR, (g_pcm_data_max/10+10)*sizeof(double) ) ;
		

		g_freq_data_len = freq_tot ;
		g_freq_data = (FREQ_DATA*)GlobalAlloc( GPTR, (g_freq_data_len)*sizeof(FREQ_DATA) ) ;
		g_sort_data = (sort_Data*)GlobalAlloc( GPTR, (g_freq_data_len)*sizeof(sort_Data) ) ;

		g_tj_data = (TJ_DATA*)GlobalAlloc( GPTR, (g_freq_data_len)*sizeof(TJ_DATA) ) ;
		g_freq_tj = (FREQ_TJ_DATA*)GlobalAlloc( GPTR, (g_freq_tj_max)*sizeof(FREQ_TJ_DATA) ) ;


		// 打开所有的采样数据的结构

		open_all_pcm_data( max_sec, buf_size ) ;
**/
		init_fbp() ;
		g_Audio.Audio_Start() ;

		return ( 1 ) ;
	}
	else
	{
		return ( 0 ) ;
	}
}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
long	Open_Tick_Thread()
{
	DWORD	dwID ;
	HANDLE	tick_thread ;

	tick_thread = CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)(tick_ThreadProc), NULL, 0, &dwID ) ;
	if ( tick_thread==NULL )
		return ( -100 ) ;

	return ( 0 ) ;
}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void close_proc()
{
	HANDLE exit_event = CreateEvent( NULL, FALSE, FALSE, NULL ) ; 
	if ( exit_event==NULL )
		return ;

	ResetEvent(exit_event) ;

	do_task( 501, "1000" ) ;

	long	flag ;
	for ( ;; )
	{
		flag = do_task( 502, "" ) ;
		if ( flag>=2000 )
			break ;
		WaitForSingleObject( exit_event, 50 ) ; 
	}

	do_task( 501, "3000" ) ;


	audio_close_proc() ;
	
	PopEvent( 2009, 1, "close_proc() OK" ) ;

}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
long	TAF_DATA_VALUE( char *DataName, char *DataValue )
{
	strupr( DataName ) ;
	if ( DataName[0] == '\0' )
		return ( 0 ) ;
	else if ( memcmp( DataName, "COM_", 4 )==0 )
		return ( Set_Com_Data( DataName, DataValue ) ) ;
/**
	else if ( memcmp( DataName, "MDD_", 4 )==0 )
		return ( Set_Mdd_Data( DataName, DataValue ) ) ;
	else if ( memcmp( DataName, "HTTP_", 5 )==0 )
		return ( Set_Http_Data( DataName, DataValue ) ) ;
**/
	return ( 0 ) ;

}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void	set_note_name( short i, short di )
{
	short	dm, dv ;
	char	cn, ci, ca ;
	
	dm = di ;
	dv = dm % 12 ;
	dm = dm / 12 ;

	switch( dv )
	{
		case 0 : cn = 'C' ; ca = ' ' ; break ;
		case 1 : cn = 'C' ; ca = '#' ; break ;
		case 2 : cn = 'D' ; ca = ' ' ; break ;
		case 3 : cn = 'E' ; ca = 'b' ; break ;
		case 4 : cn = 'E' ; ca = ' ' ; break ;
		case 5 : cn = 'F' ; ca = ' ' ; break ;
		case 6 : cn = 'F' ; ca = '#' ; break ;
		case 7 : cn = 'G' ; ca = ' ' ; break ;
		case 8 : cn = 'G' ; ca = '#' ; break ;
		case 9 : cn = 'A' ; ca = ' ' ; break ;
		case 10 : cn = 'B' ; ca = 'b' ; break ;
		case 11 : cn = 'B' ; ca = ' ' ; break ;
	}

	ci = dm + 48 ;

	sprintf( g_note_info[i].note_name, "%c%c%c", cn, ca, ci ) ;

}
//////////////////////////////////////////////////////////////////////
void	make_pcm_freq( short i )
{

	short	di ;
	double	dfi ;
	double	df ; // 标准音a=440
	double	vf ; // 指定音的频率

	df = 440 ;	// 标准音

	di = i-g_pcm_data_half_ii ; // 标准音的起始位置

	dfi = di ;
	vf = df * pow( 2, dfi/12 ) ; // 得到指定音的频率

	g_note_info[i].note_freq = vf ;

	set_note_name( i, di+(9+4*12) ) ; // 从标准音到C1的差是9+4*12

}
//////////////////////////////////////////////////////////////////////
void	make_note_pcm_data()
{
	short	i ;
	for ( i=0; i<NOTE_INFO_MAX; i++ )
	{
		make_pcm_freq( i ) ;
	}
}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void do_set_dataInfo_cx_proc()
{
	long	cx, ta, tb ;
	char	ss[30] ;

	cx = g_NoteName_xx ;
	strcpy( ss, "A" ) ;
	SetFont( g_dataInfo_DC, g_NoteName_fontHeight, FW_BOLD, g_dataInfo_fontName ) ;
	Get_Text_AB( g_dataInfo_DC, ss, &ta, &tb ) ;
	cx += ta ;

	strcpy( ss, "#" ) ;
	SetFont( g_dataInfo_DC, g_NoteAux_fontHeight, FW_BOLD, g_dataInfo_fontName ) ;
	Get_Text_AB( g_dataInfo_DC, ss, &ta, &tb ) ;
	cx += ta + ta / 2 ;

	strcpy( ss, "99999.99Hz" ) ;
	SetFont( g_dataInfo_DC, g_Freq_fontHeight, FW_BOLD, g_dataInfo_fontName ) ;
	Get_Text_AB( g_dataInfo_DC, ss, &ta, &tb ) ;
	cx += ta ;

	g_dataInfo_cx = cx+20 ;

}
//////////////////////////////////////////////////////////////////////
void draw_noteName( short ni, double note_freq )
{
	char	show_ss[20], fmt_ss[20] ;
	long	tx, ty, ta, tb, fx, fy ;
	
	clear_freq_area() ;

	show_ss[1] = '\0' ;

	// show noteName
	show_ss[0] = g_note_info[ni].note_name[0] ;
	SetFont( g_dataInfo_DC, g_NoteName_fontHeight, FW_BOLD, g_dataInfo_fontName ) ;
	DrawText( g_dataInfo_DC, show_ss, g_NoteName_xx, g_NoteName_yy, g_NoteName_fontColor, g_dataInfo_backColor, 1, 1 ) ;


	// show noteAux
	Get_Text_AB( g_dataInfo_DC, show_ss, &ta, &tb ) ;
	tx = g_NoteName_xx + ta ;
	ty = g_NoteName_yy + tb / 20 ;
	fx = tx ;
	fy = ty ;

	show_ss[0] = g_note_info[ni].note_name[1] ;
	SetFont( g_dataInfo_DC, g_NoteAux_fontHeight, FW_BOLD, g_dataInfo_fontName ) ;
	DrawText( g_dataInfo_DC, show_ss, tx - ta / 20, ty, g_NoteName_fontColor, g_dataInfo_backColor, 1, 1 ) ;


	// show noteIdx
	show_ss[0] = g_note_info[ni].note_name[2] ;
	SetFont( g_dataInfo_DC, g_NoteIdx_fontHeight, FW_BOLD, g_dataInfo_fontName ) ;

	ty = g_NoteName_yy + tb + 2 ;
	Get_Text_AB( g_dataInfo_DC, show_ss, &ta, &tb ) ;
	ty = ty - tb - tb / 3 ;
	DrawText( g_dataInfo_DC, show_ss, tx-ta/20, ty, g_NoteName_fontColor, g_dataInfo_backColor, 1, 1 ) ;





	SetFont( g_dataInfo_DC, g_Freq_fontHeight, FW_BOLD, g_dataInfo_fontName ) ;
	Get_Text_AB( g_dataInfo_DC, show_ss, &ta, &tb ) ;

	// show standard_freq
	fx = g_dataInfo_cx - ta/2 ;
	fy = g_NoteName_yy + tb / 4 + 6 ;



	strcpy( fmt_ss, "%-" ) ;
	strcat( fmt_ss, g_note_show_fmt ) ;
	strcat( fmt_ss, "Hz" ) ;

	sprintf( show_ss, fmt_ss, g_note_info[ni].note_freq ) ;
	DrawText( g_dataInfo_DC, show_ss, fx, fy, g_Freq_fontColor, g_dataInfo_backColor, -1, 1 ) ;

	// show note_freq
	fy += (tb - tb/10);

	sprintf( show_ss, fmt_ss, note_freq ) ;
	SetFont( g_dataInfo_DC, g_Freq_fontHeight, FW_BOLD, g_dataInfo_fontName ) ;
	DrawText( g_dataInfo_DC, show_ss, fx, fy, g_Freq_fontColor, g_dataInfo_backColor, -1, 1 ) ;


	// show diff_freq
	short	df ;
	fy += (tb - tb/10);
	sprintf( show_ss, fmt_ss, note_freq-g_note_info[ni].note_freq ) ;

	if ( strstr( show_ss, "0.00" )!=NULL )
	{
		sprintf( show_ss, "0.00Hz" ) ;
		df = 0 ;
	}
	else if ( show_ss[0] == '-' ) 
		df = -1 ;
	else 
	{
		char	ss[30] ;
		sprintf( ss, "+%s", show_ss ) ;
		strcpy( show_ss, ss ) ;
		df = 1 ;
	}

	SetFont( g_dataInfo_DC, g_Freq_fontHeight, FW_BOLD, g_dataInfo_fontName ) ;
	DrawText( g_dataInfo_DC, show_ss, fx, fy, g_Diff_fontColor, g_dataInfo_backColor, -1, 1 ) ;


	// show diff_cents
	double	dv ;
	fy += (tb - tb/10);

	strcpy( fmt_ss, "%-" ) ;
	strcat( fmt_ss, g_note_show_fmt ) ;
	strcat( fmt_ss, "Cs" ) ;

	if ( df==0 )
	{
		sprintf( show_ss, fmt_ss, 0 ) ;
	}
	else
	{
		if ( df<0 )
			dv = (g_note_info[ni].note_freq-g_note_info[ni-1].note_freq)/100 ;
		else
			dv = (g_note_info[ni+1].note_freq-g_note_info[ni].note_freq)/100 ;

		dv = (note_freq-g_note_info[ni].note_freq)/dv ;
		
		sprintf( show_ss, fmt_ss, dv ) ;

		if ( df>0 ) 
		{
			char	ss[20] ;
			sprintf( ss, "+%s", show_ss ) ;
			strcpy( show_ss, ss ) ;
		}

	}

	SetFont( g_dataInfo_DC, g_Freq_fontHeight, FW_BOLD, g_dataInfo_fontName ) ;
	DrawText( g_dataInfo_DC, show_ss, fx, fy, g_Cents_fontColor, g_dataInfo_backColor, -1, 1 ) ;

	Get_Text_AB( g_dataInfo_DC, show_ss, &ta, &tb ) ;
	DrawBar( g_dataInfo_DC, fx-ta, fy, fx-1, fy+tb, RGB(100,100,100), PS_SOLID, R2_XORPEN ) ;

}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
double get_freq_by_note( char *music_note )
{
	long	i ;
	for ( i=0; i<NOTE_INFO_MAX-1; i++ )
	{
		if ( strcmp( music_note, g_note_info[i].note_name )==0 )
			return ( g_note_info[i].note_freq ) ;
	}
	return ( -1 ) ;
}
//////////////////////////////////////////////////////////////////////
long get_idx_by_freq( double note_freq )
{
	short	i, ni ;

	ni = -1 ;
	for ( i=0; i<NOTE_INFO_MAX-1; i++ )
	{
		if ( note_freq>=g_note_info[i].note_freq && note_freq<g_note_info[i+1].note_freq ) 
		{
			if ( fabs(note_freq-g_note_info[i].note_freq)<fabs(note_freq-g_note_info[i+1].note_freq) )
				ni = i ;
			else
				ni = i+1 ;
			break ;
		}
	}
	return ( ni ) ;
}
//////////////////////////////////////////////////////////////////////
long noteInfo_proc( double note_freq, long draw_flag )
{

	if ( note_freq<0.0001 )
		return( -1 ) ;

	long ni ;
	
	ni = get_idx_by_freq( note_freq ) ;
	if ( ni<0 )
		return( -1 ) ;

	if ( draw_flag>0 )
		draw_noteName_new( (short)ni, note_freq ) ;

	g_showFreq = note_freq ;

	return ( ni ) ;

}
//////////////////////////////////////////////////////////////////////
void do_showFreq_proc()
{

	if ( fabs(g_curFreq-g_showFreq)>=0.01 )
	{
		g_showFreq = g_curFreq ;
		noteInfo_proc( g_showFreq, 1 ) ;
	}
/**
	else if ( g_dataInfo_refresh>0 )
	{
		noteInfo_proc( g_showFreq, 1 ) ;
		g_dataInfo_refresh = 0 ;
	}
**/
}
//////////////////////////////////////////////////////////////////////
void dataInfo_refresh()
{
//	g_dataInfo_refresh = 1 ;
//	noteShow_proc( 0 ) ;
	do_task( 105, "1" ) ;
}
//////////////////////////////////////////////////////////////////////
long open_proc( char *DataValue )
{
	unsigned long	form_wnd, wf_wnd, di_wnd ;

	sscanf( DataValue, "%lu %lu %lu %s", &form_wnd, &wf_wnd, &di_wnd, g_note_show_fmt ) ;
	strcat( g_note_show_fmt, "lf" ) ;

	g_CallBack_Handle = (HWND)form_wnd ;

	g_waveForm_wnd = (HWND)wf_wnd ;
	g_waveForm_DC = GetDC( g_waveForm_wnd ) ;
//	g_waveForm_DC = (HDC)wf_wnd ;

	g_dataInfo_wnd = (HWND)di_wnd ;
	g_dataInfo_DC = GetDC( g_dataInfo_wnd ) ;

	make_note_pcm_data() ;

	Open_Tick_Thread() ;

	strcpy( g_dataInfo_rect_vars, "" ) ;
	strcpy( g_waveForm_rect_vars, "" ) ;

	strcpy( g_new_rect_vars, "" ) ;

	strcpy( g_logFile, "..\\testData\\hhs.log" ) ;
	strcpy( g_logFile2, "..\\testData\\hhs2.log" ) ;
	strcpy( g_tjFile, "..\\testData\\tj.log" ) ;
	

	DeleteFile( "..\\hhs.wav" ) ;

	PopEvent( 1001, form_wnd, "open_proc_ok" ) ;


	return ( 0 ) ;

}
//////////////////////////////////////////////////////////////////////
void waveForm_vars( char *DataValue )
{
	sscanf( DataValue, "%lx %s", &g_waveForm_backColor, g_waveForm_fontName ) ;
}
//////////////////////////////////////////////////////////////////////
void dataInfo_vars( char *DataValue )
{
	sscanf( DataValue, "%lx %s", &g_dataInfo_backColor, g_dataInfo_fontName ) ;
}
//////////////////////////////////////////////////////////////////////
void noteInfo_vars( char *DataValue )
{
	sscanf( DataValue, "%lu %lu %lx %lx %lx %lx", &g_NoteName_xx, &g_NoteName_yy, &g_NoteName_fontColor, &g_Freq_fontColor, &g_Diff_fontColor, &g_Cents_fontColor ) ;
}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void waveForm_rect_vars( char *DataValue )
{
	sscanf( DataValue, "%lu %lu", &g_waveForm_aa, &g_waveForm_bb ) ;
//	show_waveForm() ;
}
//////////////////////////////////////////////////////////////////////
void dataInfo_rect_vars( char *DataValue )
{
	sscanf( DataValue, "%lu %lu", &g_dataInfo_aa, &g_dataInfo_bb ) ;

	g_NoteName_fontHeight = 7*g_dataInfo_bb / 8 ;
	g_NoteAux_fontHeight = g_NoteName_fontHeight / 2 ;
	g_NoteIdx_fontHeight = g_NoteName_fontHeight / 3 ;
	g_Freq_fontHeight = g_NoteName_fontHeight / 3 - g_NoteName_fontHeight/8 ;

	do_set_dataInfo_cx_proc() ;

}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void do_vars_proc()
{
	if ( g_waveForm_rect_vars[0] != '\0' )
	{
		waveForm_rect_vars( g_waveForm_rect_vars ) ;
		strcpy( g_waveForm_rect_vars, "" ) ;
		g_waveForm_redraw = 1 ;
	}

	if ( g_dataInfo_rect_vars[0] != '\0' )
	{
		dataInfo_rect_vars( g_dataInfo_rect_vars ) ;
		strcpy( g_dataInfo_rect_vars, "" ) ;
		g_dataInfo_redraw = 1 ;
//		g_dataInfo_refresh = 1 ;
	}
}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void show_shortData_proc( short *data, long data_len, long data_color, long x_start, long x_end, long y_off, double y_amp )
{
	long	i, ii, y0 ;
	double	xx, yy, yb, dx_inc ;

	y0 = g_waveForm_bb / 2 ;
	yb = y0 ;

	dx_inc = g_waveForm_aa ;
	dx_inc /= data_len ;

	ii = 0 ;
	xx = 0 ;
	for ( i=x_start; i<=x_end; i++ )
	{
		xx = i ;
		xx *= dx_inc ;
		g_waveForm_xy[ii].x = (long)floor(xx) ;

		yy = data[i] ;
		yy *= y_amp ;
		yy += y_off ;

		if ( yy>32700 )
			yy = 32700 ;
		if ( yy<-32700 )
			yy = -32700 ;

		yy *= yb ;
		yy /= 32768 ;

		yy += y0 ;
		yy = yb*2-yy ;

		g_waveForm_xy[ii].y = (long)floor(yy) ;

		ii++ ;

	}

	PolyLine( g_waveForm_DC, g_waveForm_xy, ii, 1, data_color, PS_SOLID, R2_COPYPEN ) ;
	for ( i=0; i<ii; i++ )
	{
		DrawLine( g_waveForm_DC, g_waveForm_xy[i].x, g_waveForm_xy[i].y, g_waveForm_xy[i].x, g_waveForm_xy[i].y, 1, RGB(0,250,200), PS_SOLID, R2_COPYPEN ) ;
	}

	if ( y_off!=0 )
	{
		y0 += (short)((y_off*yb)/32768) ;
		DrawLine( g_waveForm_DC, 0, y0, g_waveForm_aa-1, y0, 1, RGB(240,0,0), PS_SOLID, R2_COPYPEN ) ;
	}

}
//////////////////////////////////////////////////////////////////////
void show_waveForm_proc_ex()
{

	// clear waveForm show area
	DrawBar( g_waveForm_DC, 0, 0, g_waveForm_aa-1, g_waveForm_bb-1, g_waveForm_backColor, PS_SOLID, R2_COPYPEN ) ;

	show_shortData_proc( g_wave_show_buf, g_waveForm_xy_len, RGB(0,120,0), 0, g_waveForm_xy_len-1, 0, 1 ) ; // 显示波形

}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void print_freq_data( FREQ_DATA *freq_data, long da_len, char *file_name )
{
	long	i ;

	log_prt( g_logFile, "---------------------------------- tot=%ld %s\r\n", da_len, file_name ) ;
	for ( i=0; i<da_len; i++ )
	{
		log_prt( g_logFile, "i=%-5ld ff=%-5ld is=%-5ld ie=%-5ld nn=%-5ld vv=%-10.2lf avg=%-8.2lf dif=%-8.2lf vvv=%-8.2lf dif=%-8.2lf\r\n",
						i, 
						freq_data[i].freq_ff,
						freq_data[i].freq_is,
						freq_data[i].freq_ie,
						freq_data[i].freq_nn,
						freq_data[i].freq_vv,
						freq_data[i].freq_avg,
						freq_data[i].freq_dif,
						freq_data[i].each_vvv,
						freq_data[i].each_dif
						) ;
	}
}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void show_waveForm_proc()
{
	show_waveForm_proc_ex() ;
	show_freq_from_pcm_data( g_wave_show_buf, g_waveForm_xy_len ) ;

	long y0 = g_waveForm_bb / 2 ;
	DrawLine( g_waveForm_DC, 0, y0, g_waveForm_aa-1, y0, 1, RGB(255,0,0), PS_SOLID, R2_COPYPEN ) ;

}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void save_to_file( long file_count, char *sFile )
{
	char	saveFile[200] ;

	if ( file_count>0 )
	{
		char	ss[200], sExt[50] ;
		long	i, n;

		strcpy( ss, sFile ) ;
		sExt[0] = '\0' ;
		n = strlen(sFile) ;
		for ( i=n-1; i>=0; i-- )
		{
			if ( sFile[i]=='.' )
			{
				strcpy( sExt, sFile+i ) ;
				memcpy( ss, sFile, i ) ;
				ss[i] = '\0' ;
				break ;
			}
		}
		sprintf( saveFile, "%s-%05ld", ss, file_count ) ;
		strcat( saveFile, sExt ) ;
		DeleteFile( saveFile ) ;
	}
	else
		strcpy( saveFile, sFile ) ;

	AppendBufToFile( saveFile, g_wave_show_buf, g_wave_data_size ) ;
	strcpy( g_cur_file, saveFile ) ;

}
//////////////////////////////////////////////////////////////////////
bool check_wave_data_ok( double vv_min )
{
	long	i ;
	double	vv ;

	vv = 0 ;
	for ( i=0; i<g_waveForm_xy_len; i++ )
	{
		vv += g_wave_show_buf[i] ;
	}
	vv /= g_waveForm_xy_len ;
	if ( vv<vv_min )
		return ( false ) ;
	else
		return ( true ) ;
}
//////////////////////////////////////////////////////////////////////
void do_wave_data_proc( long waveFileNo )
{

	do_WaveData( 102, NULL ) ;
	if ( g_wave_show_flag>0 ) 
	{
		if ( check_wave_data_ok(10) )
		{
			if ( waveFileNo>0 )
				save_to_file( waveFileNo, "c:\\hhs.wav" ) ;
			show_waveForm_proc() ;
		}
		g_wave_show_flag = 0 ;
	}
}
//////////////////////////////////////////////////////////////////////
void do_wave_data_from_file( char *sFile )
{

	long nn = ReadBufFromFile( sFile, g_wave_show_buf, 44, g_wave_data_size ) ;
//	long nn = ReadBufFromFile( sFile, g_wave_show_buf, 0, g_wave_data_size ) ;
	if ( nn<=0 )
		return ;

//	WriteBufToFile( "d:\\hhs-1.wav", g_wave_show_buf, g_wave_data_size ) ;

	strcpy( g_cur_file, sFile ) ;

	log_prt( g_logFile, "================================================ %s\r\n", g_cur_file ) ;

	show_waveForm_proc() ;

	g_wave_show_flag = 0 ;
}
//////////////////////////////////////////////////////////////////////
void do_wave_data_from_file2( char *sFile )
{
	
	strcpy( g_testFile, sFile ) ;

	g_testFile_len = Get_FileSize( g_testFile ) ;
	g_testFile_pos = 44 ;

	g_pcm_data_pos = 0 ;
	g_pcm_data_is = 0 ;
	g_pcm_data_ie = 0 ;
	g_pcm_data_cn = 0 ;

	clear_wave_area() ;
	clear_freq_area() ;

}
//////////////////////////////////////////////////////////////////////
long do_testFile_data_proc()
{

	if ( g_testFile_pos>=g_testFile_len )
		return ( 0 ) ;

	long nRead, nSize ;
	
	nSize = g_wave_data_size ;
	if ( g_testFile_pos+nSize>g_testFile_len )
		return ( 0 ) ;
//		nSize = g_testFile_len-g_testFile_pos ;

	nRead = ReadBufFromFile( g_testFile, g_wave_show_buf, g_testFile_pos, g_wave_data_size ) ;
	if ( nRead!=nSize )
		return ( 0 ) ;

	g_testFile_pos += nRead ;

	g_note_pcm_nn++ ;

	push_pcm_proc_new( (BYTE*)g_wave_show_buf, g_wave_data_size ) ;

	return ( 1 ) ;


}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
long	Set_Com_Data( char *DataName, char *DataValue )
{

	if ( strcmp( DataName, "COM_OPEN" )==0 )
		open_proc( DataValue ) ;
	else if ( strcmp( DataName, "COM_CLOSE" )==0 )
		close_proc() ;

	else if ( strcmp( DataName, "COM_AUDIO_OPEN" )==0 )
		return ( audio_open_proc( DataValue ) ) ;

	else if ( strcmp( DataName, "COM_WAVEFORN_VARS" )==0 )
		waveForm_vars( DataValue ) ;

	else if ( strcmp( DataName, "COM_DATAINFO_VARS" )==0 )
		dataInfo_vars( DataValue ) ;

	else if ( strcmp( DataName, "COM_NOTE_INFO_VARS" )==0 )
		noteInfo_vars( DataValue ) ;

	else if ( strcmp( DataName, "COM_WAVEFORM_RECT_VARS" )==0 )
		do_task( 101, DataValue ) ;

	else if ( strcmp( DataName, "COM_DATAINFO_RECT_VARS" )==0 )
		do_task( 102, DataValue ) ;


//	else if ( strcmp( DataName, "COM_NEW_RECT_VARS" )==0 )
//		do_task( 103, DataValue ) ;


	else if ( strcmp( DataName, "COM_DATAINFO_REFRESH" )==0 )
		dataInfo_refresh() ;

	else if ( strcmp( DataName, "COM_WAVEFORM_REFRESH" )==0 )
		waveForm_refresh() ;

	else if ( strcmp( DataName, "COM_TEST" )==0 )
		test_proc( DataValue ) ;

	else if ( strcmp( DataName, "COM_TEST2" )==0 )
		test2_proc( DataValue ) ;

	else if ( strcmp( DataName, "COM_TEST3" )==0 )
		test3_proc( DataValue ) ;

	else if ( strcmp( DataName, "COM_TEST4" )==0 )
		test4_proc( DataValue ) ;

	else if ( strcmp( DataName, "COM_TEST5" )==0 )
		test5_proc( DataValue ) ;

	else if ( strcmp( DataName, "COM_TEST6" )==0 )
		test6_proc( DataValue ) ;

	return ( 0 ) ;

}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void print_test_data()
{
	log_prt( g_logFile, "test_data==================*****wrong_note=%s wrong_nn=%-6ld freq_difs=%1.2lf\r\n", 
										g_test_data.music_note,
										g_test_data.wrong_note_nn,
										g_test_data.note_freq_dif
							) ;
}
//////////////////////////////////////////////////////////////////////
void print_test_data2()
{
	log_prt( g_logFile, "test_data2 ==================*****tot_wrong=%-8ld tot_difs=%-8.2lf\r\n", 
									g_test_data.wrong_note_nn_tot, 
									g_test_data.note_freq_dif_tot
									) ;
}
//////////////////////////////////////////////////////////////////////
void print_tj_data2()
{
	log_prt( g_tjFile, "tj_data2 ==================*****change_vv=%-8.2lf change_nn=%-5ld  \r\n", 
									g_test_data.freq_dif_change_vv_tot,
									g_test_data.freq_dif_change_nn_tot
									) ;
}
//////////////////////////////////////////////////////////////////////
void set_music_note_read( char *waveFile )
{
	long	i, n, ff ;

	n = strlen( waveFile ) ;
	ff = 0 ;
	for ( i=n-1; i>=0; i--) 
	{
		if ( ff==0 )
		{
			if ( waveFile[i]=='.' )
			{
				ff = 1 ;
			}
		}
		else if ( ff==1 )
		{
			if ( waveFile[i]=='-' )
			{
				break ;
			}
		}
	}
	
	g_test_data.music_note[0] = waveFile[i+1] ;
	g_test_data.music_note[1] = waveFile[i+2] ;
	g_test_data.music_note[2] = waveFile[i+3] ;
	g_test_data.music_note[3] = '\0' ;

	g_test_data.music_note_freq = get_freq_by_note( g_test_data.music_note ) ;

}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
long test2_proc( char *DataValue )
{
	return ( 0 ) ;
/*
	char	ss[200] ;
	sprintf( ss, "D:\\Dev\\Tuner\\testData\\wav\\%s.wav", DataValue ) ;

//	do_wave_data_from_file( ss ) ;
	do_wave_data_from_file2( ss ) ;

	return ( 0 ) ;
*/
}
//////////////////////////////////////////////////////////////////////
void test3_proc( char *DataValue )
{

//	do_testFile_data_proc() ;
//	return ;

	g_test_data_flag = 1 ;

	char	sFile[500] ;
	long	nSize, nOff ;

//	strcpy( sFile, "D:\\Dev\\Tuner\\testData\\tmp\\tst-A 3-00014.wav" ) ;

	strcpy( sFile, DataValue ) ;

//	strcpy( sFile, "D:\\Dev\\Tuner\\testData\\hhs1.wav" ) ;
//	strcpy( sFile, "D:\\Dev\\Tuner\\testData\\hhs2.wav" ) ;

	nSize = Get_FileSize( sFile ) ;

	set_music_note_read( sFile ) ;

	log_prt( g_logFile, "====================================================================== test_file=%s\r\n", sFile ) ;


	nOff = 0 ;
	ReadBufFromFile( sFile, g_pcm_data, nOff, nSize ) ;

//	pcm_data_filter_1( (short*)g_pcm_data, nSize/2, 100 ) ;

	long	ff ;
	ff = make_freq_from_pcmData( 1, 1, (short*)g_pcm_data, nSize/2, "test3_proc" ) ;
	if ( ff<=0 )
	{
		// to do list
		// re do all pcm_data
	}

	g_test_data_flag = 0 ;

	return ;

}
//////////////////////////////////////////////////////////////////////
void clear_tj_data()
{
	g_freq_tj_nn = 0 ;
	g_freq_tot = 0 ;
}
//////////////////////////////////////////////////////////////////////
void test4_proc( char *DataValue )
{

	g_tj_data_nn = 0 ;

	g_tj_data_ff = 1 ;


	do_wave_data_from_file2( DataValue ) ;

	set_music_note_read( DataValue ) ;

	g_test_data.wrong_note_nn=0 ;
	g_test_data.note_freq_dif=0 ;


	g_test_data.filter_problem_0 = 0 ;
	g_test_data.filter_problem_1 = 0 ;
	g_test_data.filter_problem_2 = 0 ;

	g_note_pcm_nn = 0 ;
	g_note_pcm_do = 0 ;
	g_note_pcm_ok1 = 0 ;
	g_note_pcm_ok2 = 0 ;
	g_note_pcm_ok3 = 0 ;



	clear_tj_data() ;

	g_all_pcm_pos = 0 ;
	for ( ;; )
	{
		if ( do_testFile_data_proc()<=0 )
			break ;
//		Sleep( 10 ) ;
	}

	g_test_data.note_freq_dif_tot += g_test_data.note_freq_dif ;
	g_test_data.wrong_note_nn_tot += g_test_data.wrong_note_nn ;

	print_test_data() ;

	print_tj_data() ;

	print_ok_data() ;
	print_filter_data() ;

	print_freq_tj_data() ;

	g_tj_data_ff = 0 ;

}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void test5_proc( char *DataValue )
{
	g_test_data.wrong_note_nn_tot=0 ;
	g_test_data.note_freq_dif_tot=0 ;
	g_test_data_flag = 1 ;
}
//////////////////////////////////////////////////////////////////////
void test6_proc( char *DataValue )
{
	print_test_data2() ;
	print_tj_data2() ;
	print_filter_data2() ;
	g_test_data_flag = 0 ;
}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void create_integral_data( short *pcm_data, long pcm_len, short *int_data, short max_value )
{
	long	i ;
	double	dv, max_dv, min_dv, dr ;

	min_dv = 10000000000 ;
	max_dv = -10000000000 ;
	dv = 0 ;
	for ( i=0; i<pcm_len; i++ )
	{
		dv += pcm_data[i] ;
		if ( max_dv<dv )
			max_dv = dv ;
		if ( min_dv>dv )
			min_dv = dv ;
	}

	dr = (double)max_value/(max_dv-min_dv) ;

	dv = 0 ;
	for ( i=0; i<pcm_len; i++ )
	{
		dv += pcm_data[i] ;
		int_data[i] = (short)floor(dv*dr) ;
	}

}
//////////////////////////////////////////////////////////////////////
long create_mm_data( short *pcm_data, long pcm_len, MM_DATA *mm_data )
{
	long	i, ii, ff ;
	short	vv ;

	ff = 0 ;
	ii = 0 ;
	vv = pcm_data[ii] ;
	mm_data[ii].x = (short)ii ;
	mm_data[ii].y = vv ;
	mm_data[ii].f = 0 ;
	ii ++ ;
	for ( i=1; i<pcm_len; i++ )
	{
		if ( pcm_data[i]>vv )
		{
			if ( ff<0 ) // 此时为最小值
			{
				mm_data[ii].x = i-1 ;
				mm_data[ii].y = vv ;
				mm_data[ii].f = -1 ;
				ii++ ;
			}
			ff = 1 ;
		}
		else if ( pcm_data[i]<vv ) // 此时为最大值
		{
			if ( ff>0 )
			{
				mm_data[ii].x = i-1 ;
				mm_data[ii].y = vv ;
				mm_data[ii].f = 1 ;
				ii++ ;
			}
			ff = -1 ;
		}
		vv = pcm_data[i] ;
	}
	mm_data[ii].x = i-1 ;
	mm_data[ii].y = vv ;
	mm_data[ii].f = (short)ff ;
	ii++ ;
	return ( ii ) ;
}
//////////////////////////////////////////////////////////////////////
// 去除那些相邻的最大最小值过小的那些值，即明显小于指定ratio的（通常为10%的）
long remove_too_small_mm_data( MM_DATA *mm_data, long mm_len, short max_value, double ratio )
{
	long	i, i0, ii ;
	long	vv, min_vv, max_vv ;
	double	dv, dr ;

	min_vv = max_value ;
	max_vv = -max_value;
	for ( i=0; i<mm_len; i++ )
	{
		vv = mm_data[i].y ;
		if ( mm_data[i].f<0 )
		{
			if ( min_vv>vv )
				min_vv = vv ;
		}
		else if ( mm_data[i].f>0 )
		{
			if ( max_vv<vv )
				max_vv = vv ;
		}
	}
	dr = ratio * (max_vv-min_vv) ; // 计算比例


	// 1. 首先找到最大值与最小值之差的最大之差
	max_vv = -max_value;
	for ( i=1; i<mm_len; i++ )
	{
		vv = abs(mm_data[i].y-mm_data[i-1].y) ;
		if ( max_vv<vv )
			max_vv = vv ;
	}

	dr = ratio * max_vv ; // 计算比例

	// 2. 然后去掉那些小于dr的最大最小值
	i0 = 0 ;
	for ( i=1; i<mm_len; i++ )
	{
		dv = fabs(mm_data[i].y-mm_data[i0].y) ;
		if ( dv<dr )
		{
			if ( i<mm_len-1 )
			{
				if ( fabs(mm_data[i+1].y-mm_data[i].y)<dr )
				{
					mm_data[i].f = 1000 ;
				}
			}
			if ( mm_data[i].f!=1000 )
			{
				if ( i0>0 && i<mm_len-1 )
				{
					if ( (mm_data[i0].y-mm_data[i0-1].y)>dr && (mm_data[i+1].y-mm_data[i].y)>dr ||
						 (mm_data[i0].y-mm_data[i0-1].y)<dr && (mm_data[i+1].y-mm_data[i].y)<dr )
					{
						mm_data[i].f = 1000 ;
					}
				}
			}
		}
		if ( mm_data[i].f!=1000 )
			i0 = i ;
	}
	ii = 0 ;
	for ( i=0; i<mm_len; i++ )
	{
		if ( mm_data[i].f==1000 )
			continue ;
		memcpy( &mm_data[ii], &mm_data[i], sizeof(MM_DATA) ) ;
		ii ++ ;
	}
	return ( ii ) ;
}
//////////////////////////////////////////////////////////////////////
// 去除那些同极性（同为极大或同为极小值的）点
long remove_same_direction_mm_data( MM_DATA *mm_data, long mm_len )
{
	long	i, ii, i0 ;

	i0 = 0 ;
	for ( i=1; i<mm_len-1; i++ )
	{
		if ( mm_data[i].f==mm_data[i-1].f || mm_data[i].f==mm_data[i+1].f )
			mm_data[i].f = 1000 ;
		else
			i0 = i ;
	}
	ii = 0 ;
	for ( i=0; i<mm_len-1; i++ )
	{
		if ( mm_data[i].f==1000 )
			continue ;
		memcpy( &mm_data[ii], &mm_data[i], sizeof(MM_DATA) ) ;
		ii ++ ;
	}
	return ( ii ) ;
}
//////////////////////////////////////////////////////////////////////
// 建立相邻两个极值非常接近的“频率关联点”，这个相邻的关联点之间的距离可以作为频率Freq计算的参考值
// 算法思路：
//			1. 首先找到极大与极小之间的平均差
//			2. 然后按照ratio给定的比例值查找相邻两个极值之差是否小于这个比例，如果是则为相邻的两个极值点，即频率关联点
void create_freq_connected_mm_data( MM_DATA *mm_data, long mm_len, double ratio )
{
	double	dv, dv_ratio ;
	long	i, nn, i0, ff ;
	short	freq_ff ;

	// 1. freq_ff 清零
	for ( i=0; i<mm_len; i++ )
	{
		mm_data[i].freq_ff = 0 ;
		mm_data[i].freq_done = 0 ;
	}

	// 2. 获取平均差值
	dv_ratio = 0 ;
	nn = 0 ;
	for ( i=1; i<mm_len; i++ )
	{
		if ( mm_data[i].f>0 && mm_data[i-1].f<0 )
		{
			dv_ratio += mm_data[i].y-mm_data[i-1].y ;
			nn ++ ;
		}
	}
	dv_ratio /= nn ;
	dv_ratio *= ratio ;

	// 3. 找最小值点的距离
	freq_ff = -1 ;
	for ( ;; )
	{
		ff = 0 ;
		i0 = 0 ;
		for ( i=0; i<mm_len; i++ )
		{
			if ( mm_data[i].freq_ff != 0 || mm_data[i].f == 0 )
				continue ;
			if ( mm_data[i].f<0 )
			{
				if ( mm_data[i].freq_ff==0 )
				{
					if ( ff==0 )
					{
						mm_data[i].freq_ff = freq_ff ;
						mm_data[i].freq_ii = -1 ;
						i0 = i ;
						ff = 1 ;
					}
					else
					{
						dv = fabs(mm_data[i].y-mm_data[i0].y) ;
						if ( dv<dv_ratio )
						{
							mm_data[i].freq_ff = freq_ff ;
							mm_data[i].freq_ii = (short)i0 ;
							i0 = i ;
						}
					}
				}
			}
		}
		if ( ff==0 )
			break ;
		else
			freq_ff -- ;
	}

	// 4. 找最大值点的距离
	freq_ff = 1 ;
	for ( ;; )
	{
		ff = 0 ;
		for ( i=0; i<mm_len; i++ )
		{
			if ( mm_data[i].freq_ff != 0 || mm_data[i].f == 0 )
				continue ;
			if ( mm_data[i].f>0 )
			{
				if ( mm_data[i].freq_ff==0 )
				{
					if ( ff==0 )
					{
						mm_data[i].freq_ff = freq_ff ;
						mm_data[i].freq_ii = -1 ;
						i0 = i ;
						ff = 1 ;
					}
					else
					{
						dv = fabs(mm_data[i].y-mm_data[i0].y) ;
						if ( dv<dv_ratio )
						{
							mm_data[i].freq_ff = freq_ff ;
							mm_data[i].freq_ii = (short)i0 ;
							i0 = i ;
						}
					}
				}
			}
		}
		if ( ff==0 )
			break ;
		else
			freq_ff ++ ;
	}
}
//////////////////////////////////////////////////////////////////////
// 去掉频率相关值中那些其离散值较大的那些极值点
void remove_some_mm_data( MM_DATA *mm_data, long mm_len, double ratio )
{

	double	vv, vv_avg, vv_ratio ;
	long	i, k, i0, ff, nn ;

	for ( i=0; i<mm_len; i++ )
		mm_data[i].freq_done = 0 ;

	for ( i=0; i<mm_len; i++ )
	{
		if ( mm_data[i].freq_ff!=0 && mm_data[i].freq_done==0 )
		{
			ff = mm_data[i].freq_ff ;
			nn = 0 ;
			vv = 0 ;
			for ( k=i+1; k<mm_len; k++ )
			{
				if ( mm_data[k].freq_ff == ff )
				{
					i0 = mm_data[k].freq_ii ;
					vv += mm_data[k].x-mm_data[i0].x ;
					nn ++ ;
					mm_data[k].freq_done = 1 ;
				}
			}
			if ( nn>0 )
			{
				vv_avg = vv/nn ;
				vv_ratio = vv_avg*ratio ;
				for ( k=i+1; k<mm_len; k++ )
				{
					if ( mm_data[k].freq_ff == ff )
					{
						i0 = mm_data[k].freq_ii ;
						vv = mm_data[k].x-mm_data[i0].x ;

						if ( fabs(vv-vv_avg)>=vv_ratio ) // 如果极值点的值与平均值超过指定的百分比后，则去掉它的极值标志，但仍保留它的其他数据
							mm_data[k].freq_ff = 0 ;

					}
				}
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void show_mm_data_diff_value_proc( MM_DATA *mm_data, long mm_si, long mm_ei, long xy_len, long fontColor )
{

	SetFont( g_waveForm_DC, 10, FW_NORMAL, "Arial" ) ;
	
	char	show_ss[50] ;
	long	i, i0, y0, x, y, fx, fy, x_diff, ff ;
	double	xx, yy, yb, dx_inc ;

	y0 = g_waveForm_bb / 2 ;
	yb = y0 ;

	dx_inc = g_waveForm_aa ;
	dx_inc /= xy_len ;

	fx = 0 ;
	for ( i=mm_si; i<=mm_ei; i++ )
	{
		if ( mm_data[i].freq_ff==0 || mm_data[i].freq_ii<0 || mm_data[i].f==0 )
			continue ;

		// 获得点坐标
		xx = mm_data[i].x ;
		xx *= dx_inc ;

		x = (long)floor(xx) ;

		yy = mm_data[i].y ;

		if ( yy>32000 )
			yy = 32000 ;
		if ( yy<-32000 )
			yy = -32000 ;

		yy *= yb ;
		yy /= 32768 ;

		yy += y0 ;
		yy = yb*2-yy ;

		y = (long)floor(yy) ;

		if ( mm_data[i].freq_ff<0 )
			fy = 5 ;
		else if ( mm_data[i].freq_ff>0 )
			fy = -5 ;

		i0 = mm_data[i].freq_ii ;
		x_diff = mm_data[i].x-mm_data[i0].x ;
		ff = mm_data[i].freq_ff ;

		sprintf( show_ss, "%ld-%ld", ff, x_diff ) ;
		DrawText( g_waveForm_DC, show_ss, x, y, fontColor, g_waveForm_backColor, fx, fy ) ;

	}

}
//////////////////////////////////////////////////////////////////////
// 从连续的xy数据中得到同一个极值下的间距数据，以便计算频率
// 极值可能会有多个，如果极值过于离散，说明这次频率判断无法做出
// -------------------------------------------------------------------
double make_freq_from_mm_data( MM_DATA *mm_data, long mm_len, double ratio )
{

	double	vv, vv_avg, vv_ratio ;
	long	i, k, i0, ii, ff, is, ie, nn, tot_nn, tot_ff ;
	long	min_xx, max_xx, xx, tot_mm ;

	// 1. 首先建立频率的基本统计数据，即对每一个相同极值的间距和个数进行计算。
	ii = 0 ;
	tot_nn = 0 ;
	tot_ff = 0 ;
	vv_avg = 0 ;
	for ( i=0; i<mm_len; i++ )
	{
		if ( mm_data[i].freq_ff!=0 && mm_data[i].freq_done==0 )
		{
			ff = mm_data[i].freq_ff ;
			is = mm_data[i].x ;
			nn = 0 ;
			i0 = i ;
			vv = 0 ;
			min_xx = 100000000 ;
			max_xx = -100000000 ;
			for ( k=i; k<mm_len; k++ )
			{
				if ( mm_data[k].freq_ff == ff && mm_data[k].freq_ii>=0 )
				{
					i0 = mm_data[k].freq_ii ;
					xx = mm_data[k].x-mm_data[i0].x ;
					if ( min_xx>xx )
						min_xx = xx ;
					if ( max_xx<xx )
						max_xx = xx ;
					vv += xx ;
					nn ++ ;
					ie = mm_data[k].x ;
					i0 = k ;
					mm_data[k].freq_done = 1 ;
				}
			}
			if ( nn>0 )
			{
				g_freq_data[ii].freq_ff = (short)ff ;
				g_freq_data[ii].freq_is = (short)is ;
				g_freq_data[ii].freq_ie = (short)ie ;

				g_freq_data[ii].freq_vv = vv ;
				g_freq_data[ii].freq_min = min_xx ;
				g_freq_data[ii].freq_max = max_xx ;

				g_freq_data[ii].freq_avg = vv/nn ;

				g_freq_data[ii].freq_nn = nn ;

				vv_avg += vv ;
				tot_nn += nn ; // 统计总个数
				tot_ff ++ ;

				ii ++ ;
			}
		}
	}
	nn = ii ;
	vv_avg /= tot_nn ;
	print_freq_data( g_freq_data, nn, g_cur_file ) ;


	// 2. 然后去掉极值点个数太少的那些数据
/**
	tot_mm = 0 ;
	for ( i=0; i<nn; i++ )
	{
		tot_mm += g_freq_data[i].freq_nn ;
	}
	tot_mm /= nn ;
**/

	long	mm, min_mm, max_mm ;
	min_mm = 100000 ;
	max_mm = -100000 ;
	for ( i=0; i<nn; i++ )
	{
		mm = g_freq_data[i].freq_nn ;
		if ( min_mm>mm )
			min_mm = mm ;
		if ( max_mm<mm )
			max_mm = mm ;
	}
	tot_mm = (max_mm-min_mm)/2 ;

	for ( i=0; i<nn; i++ )
	{
		if ( g_freq_data[i].freq_nn<tot_mm ) // 如果这组极值点的个数小于平均数的话，则直接去掉
			g_freq_data[i].freq_ff = 0 ;
	}

	ii = 0 ;
	for ( i=0; i<nn; i++ )
	{
		if ( g_freq_data[i].freq_ff==0 )
			continue ;
		memcpy( &g_freq_data[ii], &g_freq_data[i], sizeof(FREQ_DATA) ) ;
		ii++ ;
	}
	nn = ii ;
	print_freq_data( g_freq_data, nn, g_cur_file ) ;

/**

	// 3. 对统计出的极值数据按同一极值下出现的个数进行排序
	FREQ_DATA	fq_data ;
	for ( i=0; i<nn; i++ )
	{
		for ( k=i+1; k<nn; k++ )
		{
			if ( g_freq_data[k].freq_nn>g_freq_data[i].freq_nn )
			{
				memcpy( &fq_data, &g_freq_data[k], sizeof(FREQ_DATA) ) ;
				memcpy( &g_freq_data[k], &g_freq_data[i], sizeof(FREQ_DATA) ) ;
				memcpy( &g_freq_data[i], &fq_data, sizeof(FREQ_DATA) ) ;
			}
		}
	}
	print_freq_data( g_freq_data, ii, g_cur_file ) ;
**/

	// 计算每个极值的平均值与总平均值之差
	vv_avg = 0 ;
	for ( i=0; i<nn; i++ )
		vv_avg += fabs( g_freq_data[i].freq_avg ) ;
	vv_avg /= nn ;

	for ( i=0; i<nn; i++ )
		g_freq_data[i].freq_dif = fabs( g_freq_data[i].freq_avg-vv_avg ) ;

	// 计算差值的总平均值
	double vv_dif ;
	vv_dif = 0 ;
	for ( i=0; i<nn; i++ )
		vv_dif += fabs( g_freq_data[i].freq_dif ) ;
	vv_dif /= nn ;

	if ( vv_dif>1 )
	{
		// 去掉差值太大的
		vv_ratio = vv_dif*ratio ;
		for ( i=0; i<nn; i++ )
		{
			if ( fabs(g_freq_data[i].freq_dif-vv_dif)>vv_ratio ) // 如果这组极值点的平均极值与总的极值平均值差的太多
				g_freq_data[i].freq_ff = 0 ;
		}
		print_freq_data( g_freq_data, nn, g_cur_file ) ;
	}
/**
	// 4. 去掉差值小于ratio比率的那些数据
	vv_ratio = vv_avg*ratio ;
	for ( i=0; i<nn; i++ )
	{
		if ( fabs(g_freq_data[i].freq_avg-vv_avg)>vv_ratio ) // 如果这组极值点的平均极值与总的极值平均值差的太多
			g_freq_data[i].freq_ff = 0 ;
	}
	print_freq_data( g_freq_data, nn, g_cur_file ) ;
**/
	// 5. 计算出剩下的所有极值点的平均值
	vv = 0 ;
	ii = 0 ;
	for ( i=0; i<nn; i++ )
	{
		if ( g_freq_data[i].freq_ff!=0 )
		{
			vv += g_freq_data[i].freq_vv ;
			ii += g_freq_data[i].freq_nn ;
		}
	}
	if ( ii>0 )
		vv /= ii ;


	return ( vv ) ;

}
//////////////////////////////////////////////////////////////////////
void avg_pcm_data( short *pcm_data, long pcm_len )
{
	long	i, ii, nn, f ;
	short	vv ;

	f = 0 ;
	ii = 0 ;
	for ( i=1; i<pcm_len; i++ )
	{
		if ( pcm_data[i]-pcm_data[i-1]<0 )
		{
			if ( f>0 )
			{
				g_xy_data[ii].x = i-1 ;
				g_xy_data[ii].y = pcm_data[i-1] ;
				ii++ ;

			}
			f = -1 ;
		}
		else if ( pcm_data[i]-pcm_data[i-1]>0 )
		{
			if ( f<0 )
			{
				g_xy_data[ii].x = i-1 ;
				g_xy_data[ii].y = pcm_data[i-1] ;
				ii++ ;

			}
			f = -1 ;
		}
	}
	nn = ii ;
	for ( ii=0; ii<nn-1; ii++ )
	{
		vv = (g_xy_data[ii+1].y-g_xy_data[ii].y)/2 ;
		for ( i=g_xy_data[ii].x; i<g_xy_data[ii+1].x; i++ )
			pcm_data[i] += vv ;
	}

}
//////////////////////////////////////////////////////////////////////
void show_freq_from_pcm_data( short *pcm_data, long pcm_len )
{

	long	n, m ;
	short	max_value=28000 ;

	char	ss[100] ;
	sprintf( ss, "-------------------show_freq_from_pcm_data\r\n" ) ;

//	memcpy( g_int_data, pcm_data, pcm_len*sizeof(short) ) ;
	create_integral_data( pcm_data, pcm_len, g_int_data, max_value ) ;

	show_WaveForm_proc( g_int_data, pcm_len, RGB(240,120,0) ) ; // 显示波形

//	avg_pcm_data( g_int_data, pcm_len ) ;
//	show_WaveForm_proc( g_int_data, pcm_len, RGB(250,250,250) ) ; // 显示波形

//	return ;

	MM_DATA *mm_data ;

	mm_data = g_mm_data ;

	n = create_mm_data( g_int_data, pcm_len, mm_data ) ;
//	show_mm_data_proc( mm_data, 0, n-1, pcm_len, RGB(0,120,125) ) ;

	m = remove_too_small_mm_data( mm_data, n, max_value, 0.2 ) ;
	m = remove_same_direction_mm_data( mm_data, m ) ;
	show_mm_data_proc( mm_data, 0, m-1, pcm_len, RGB(255,250,0) ) ;

	show_mm_data_diff_value_proc( mm_data, 0, m-1, pcm_len, RGB(255,0,0) ) ;

	if ( m<=10 )
		return ;

//	create_freq_connected_mm_data( mm_data, m, 0.1 ) ;
	create_freq_connected_mm_data( mm_data, m, 0.03 ) ;
	show_mm_data_diff_value_proc( mm_data, 0, m-1, pcm_len, RGB(255,0,0) ) ;

//	remove_some_mm_data( mm_data, m, 0.3 ) ;
	remove_some_mm_data( mm_data, m, 0.1 ) ;

	show_mm_data_diff_value_proc( mm_data, 0, m-1, pcm_len, RGB(255,250,0) ) ;

//	show_mm_data_proc( mm_data, 0, m-1, pcm_len, RGB(255,250,250), 0, 0, 1.0 ) ;


	double	freq, freq_nn ;

//	freq_nn = make_freq_from_mm_data( mm_data, m, 0.1 ) ;
	freq_nn = make_freq_from_mm_data( mm_data, m, 0.5 ) ;

	freq = g_Samples ;
	freq /= freq_nn ;

	log_prt( g_logFile, "freq_nn=%3.2lf freq=%3.2lf\r\n", freq_nn, freq ) ;

	sprintf( ss, "%0.2lf", freq ) ;
//	do_task( 201, ss ) ;


//	GlobalFree( mm_data ) ;

}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void show_mm_data_proc( MM_DATA *mm_data, long mm_si, long mm_ei, long xx_len, long mm_color )
{
	long	i, ii, y0 ;
	double	xx, yy, yb, dx ;

	y0 = g_waveForm_bb / 2 ;
	yb = y0 ;

	dx = g_waveForm_aa ;
	dx /= xx_len ;

	ii = 0 ;
	xx = 0 ;
	for ( i=mm_si; i<=mm_ei; i++ )
	{
		xx = mm_data[i].x ;
		xx *= dx ;
		g_xy_data[ii].x = (long)floor(xx) ;

		yy = mm_data[i].y ;
		yy *= yb ;
		yy /= 32768 ;

		yy += y0 ;
		yy = yb*2-yy ;

		g_xy_data[ii].y = (long)floor(yy) ;
		ii++ ;
	}

	PolyLine( g_waveForm_DC, g_xy_data, ii, 1, mm_color, PS_SOLID, R2_COPYPEN ) ;

}
//////////////////////////////////////////////////////////////////////
void show_WaveForm_proc( short *pcm_data, long pcm_len, long data_color )
{
	long	i, ii, x, y, x0, y0, min_y, max_y ;
	double	xx, dx ;

	y0 = g_waveForm_bb / 2 ;

	dx = g_waveForm_aa ;
	dx /= pcm_len ;

	if ( dx<1 )
	{
		min_y = 1000000 ;
		max_y = -1000000 ;
	}

	ii = 0 ;
	x0 = -1 ;
	xx = 0 ;
	g_xy_data[ii].x = 0 ;
	for ( i=0; i<pcm_len; i++ )
	{
		y = pcm_data[i] ;

		if ( dx<1 )
		{
			if ( min_y>y )
				min_y = y ;
			if ( max_y<y )
				max_y = y ;

			xx += dx ;
			x = (long)floor(xx) ;
			if ( x>x0 )
			{
				g_xy_data[ii].x = x0 ;
				g_xy_data[ii].y = get_waveForm_y( min_y, y0 ) ;
				ii++ ;

				g_xy_data[ii].x = x0 ;
				g_xy_data[ii].y = get_waveForm_y( max_y, y0 ) ;
				ii++ ;

				min_y = 1000000 ;
				max_y = -1000000 ;

				x0 = x ;

			}
		}
		else
		{
			g_xy_data[ii].x = x ;
			g_xy_data[ii].y = get_waveForm_y( y, y0 ) ;
			ii ++ ;
			xx += dx ;
			x = (long)floor(xx) ;
		}
	}
	if ( dx<1 )
	{
		if ( min_y == 1000000 )
			min_y = 0 ;
		if ( max_y == -1000000 )
			max_y = 0 ;

		g_xy_data[ii].x = x0 ;
		g_xy_data[ii].y = get_waveForm_y( min_y, y0 ) ;
		ii++ ;
		g_xy_data[ii].x = x0 ;
		g_xy_data[ii].y = get_waveForm_y( max_y, y0 ) ;
		ii++ ;
	}


	PolyLine( g_waveForm_DC, g_xy_data, ii, 1, data_color, PS_SOLID, R2_COPYPEN ) ;

	DrawLine( g_waveForm_DC, 0, y0, g_waveForm_aa-1, y0, 1, RGB(125,0,0), PS_SOLID, R2_COPYPEN ) ;


}
//////////////////////////////////////////////////////////////////////
bool check_pcm_data_too_small( short *pcm_buf, long pcm_len, double min_vv )
{
	if ( pcm_len<=0 )
		return ( true ) ;

	long	i ;
	double	vv ;

	vv = 0 ;
	for ( i=0; i<pcm_len; i++ )
		vv += fabs(pcm_buf[i]) ;
	vv /= pcm_len ;
	if ( vv<min_vv )
		return ( true ) ;
	else
		return ( false ) ;
}
//////////////////////////////////////////////////////////////////////
void clear_wave_area()
{
	DrawBar( g_waveForm_DC, 0, 0, g_waveForm_aa-0, g_waveForm_bb-0, g_waveForm_backColor, PS_SOLID, R2_COPYPEN ) ;
}
//////////////////////////////////////////////////////////////////////
void clear_freq_area()
{
//	DrawBar( g_dataInfo_DC, 0, 0, g_dataInfo_cx-1, g_dataInfo_bb-2, g_dataInfo_backColor, PS_SOLID, R2_COPYPEN ) ;
	DrawBar( g_dataInfo_DC, 0, 0, g_waveForm_aa-1, g_dataInfo_bb-2, g_dataInfo_backColor, PS_SOLID, R2_COPYPEN ) ;
}
//////////////////////////////////////////////////////////////////////
void push_pcm_proc( BYTE *pcm_buf, long pcm_len )
{

	if ( check_pcm_data_too_small( (short*)pcm_buf, pcm_len/2, 1000 ) )
	{

		return ;
	}

//	pcm_data_filter_1( (short*)pcm_buf, pcm_len/2, 100 ) ;
/**
	if ( g_pcm_data_pos>=g_pcm_data_max )
	{
		memmove( g_pcm_data, g_pcm_data+pcm_len, g_pcm_data_max-pcm_len ) ;
		memcpy( g_pcm_data+g_pcm_data_max-pcm_len, pcm_buf, pcm_len ) ;
		g_pcm_data_is = 0 ;
		g_pcm_data_ie = 0 ;
	}
	else
	{
		memcpy( g_pcm_data+g_pcm_data_pos, pcm_buf, pcm_len ) ;
		g_pcm_data_pos += pcm_len ;
		g_pcm_data_ie = g_pcm_data_is ;
	}

**/

	g_testFile_tot ++ ;

	sprintf( g_test_pcm_name, "tst-%s-%05ld", g_test_data.music_note, g_pcm_data_cn++ ) ;
	sprintf( g_test_pcm_file, "z:\\d5\\Tuner\\testData\\tmp\\%s.wav", g_test_pcm_name ) ;

//	WriteBufToFile( g_test_pcm_file, g_pcm_data, g_pcm_data_pos ) ;
	WriteBufToFile( g_test_pcm_file, pcm_buf, pcm_len ) ;

	log_prt( g_logFile, "start======================================================================test_tot=%ld data_cn=%ld tst_file=%s\r\n", g_testFile_tot, g_pcm_data_cn, g_test_pcm_file ) ;

//	if ( g_testFile_tot==1236 )
//		g_testFile_tot = g_testFile_tot ;

//	if ( strcmp(g_test_data.music_note, "G 7" )==0 && g_pcm_data_cn==15 )
//		g_testFile_tot = g_testFile_tot ;

	g_note_pcm_do++ ;

//	make_freq_from_pcmData( 1, (short*)g_pcm_data, g_pcm_data_pos/2 ) ;

	make_freq_from_pcmData( 1, 1, (short*)pcm_buf, pcm_len/2, "push_pcm_proc" ) ;

	DoEventProc() ;

}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void push_pcm_proc_new( BYTE *pcm_buf, long pcm_len )
{
/**
	if ( check_pcm_data_too_small( (short*)pcm_buf, pcm_len/2, 1000 ) )
	{
		return ;
	}
**/
//	pcm_data_filter_1( (short*)pcm_buf, pcm_len/2, 100 ) ;

/**
	if ( g_pcm_data_pos>=g_pcm_data_max )
	{
		memmove( g_pcm_data, g_pcm_data+pcm_len, g_pcm_data_max-pcm_len ) ;
		memcpy( g_pcm_data+g_pcm_data_max-pcm_len, pcm_buf, pcm_len ) ;
		g_pcm_data_is = 0 ;
		g_pcm_data_ie = 0 ;
	}
	else
	{
		memcpy( g_pcm_data+g_pcm_data_pos, pcm_buf, pcm_len ) ;
		g_pcm_data_pos += pcm_len ;
		g_pcm_data_ie = g_pcm_data_is ;
	}
**/

	g_testFile_tot ++ ;

	sprintf( g_test_pcm_name, "tst-%s-%05ld", g_test_data.music_note, g_pcm_data_cn++ ) ;
	sprintf( g_test_pcm_file, "z:\\d5\\Tuner\\testData\\tmp\\%s.wav", g_test_pcm_name ) ;
//	WriteBufToFile( g_test_pcm_file, g_pcm_data, g_pcm_data_pos ) ;
	WriteBufToFile( g_test_pcm_file, pcm_buf, pcm_len ) ;

	log_prt( g_logFile, "start======================================================================test_tot=%ld data_cn=%ld tst_file=%s\r\n", g_testFile_tot, g_pcm_data_cn, g_test_pcm_file ) ;

//	if ( g_testFile_tot==1236 )
//		g_testFile_tot = g_testFile_tot ;

//	if ( strcmp(g_test_data.music_note, "G 7" )==0 && g_pcm_data_cn==15 )
//		g_testFile_tot = g_testFile_tot ;

	g_note_pcm_do++ ;

	long ff = make_freq_from_pcmData( 1, 1, (short*)pcm_buf, pcm_len/2, g_test_pcm_name ) ;
	if ( ff==-1 )
	{
		if ( g_all_pcm_pos>0 ) 
		{
			char	allFile[500] ;
			sprintf( allFile, "z:\\d5\\Tuner\\testData\\tmp\\tst-%s-all.wav", g_test_data.music_note ) ;
			WriteBufToFile( allFile, g_all_pcm, g_all_pcm_pos ) ;
log_prt( g_logFile, "start======================================================================tst_file=%s\r\n", allFile ) ;
			make_freq_from_pcmData( 1, 0, (short*)g_all_pcm, g_all_pcm_pos/2, "all" ) ;
			g_all_pcm_pos = 0 ;
		}
	}

//	DoEventProc() ;

}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void show_zmz_data_proc( ZMZ_DATA *zmz_data, long zmz_nn, long xx_len, long zmz_color, long tot_color )
{
	long	i, ii, y0, x, y ;
	double	xx, dx, vv, max_vv, vv_ratio ;

	y0 = g_waveForm_bb / 2 ;

	dx = g_waveForm_aa ;
	dx /= xx_len ;

	max_vv = -1000000000 ;
	ii = 0 ;
	xx = 0 ;
	for ( i=0; i<zmz_nn; i++ )
	{
		// 左过零点
		xx = zmz_data[i].x1 ;
		xx *= dx ;
		g_xy_data[ii].x = (long)floor(xx) ;
		g_xy_data[ii].y = get_waveForm_y( 0, y0 ) ;
		ii++ ;


		// 极值点
		xx = zmz_data[i].max_ii ;
		xx *= dx ;
		g_xy_data[ii].x = (long)floor(xx) ;
		g_xy_data[ii].y = get_waveForm_y( (long)floor(zmz_data[i].max_vv), y0 ) ;
		ii++ ;

		// 右过零点
		xx = zmz_data[i].x2 ;
		xx *= dx ;
		g_xy_data[ii].x = (long)floor(xx) ;
		g_xy_data[ii].y = get_waveForm_y( 0, y0 ) ;
		ii++ ;

		vv = fabs(zmz_data[i].tot_vv) ;
		if ( max_vv<vv )
			max_vv = vv ;

	}
	PolyLine( g_waveForm_DC, g_xy_data, ii, 1, zmz_color, PS_SOLID, R2_COPYPEN ) ;
	
	vv_ratio = 20000/max_vv ;

	long	fx, fy ;
	char	show_ss[100] ;

	SetFont( g_waveForm_DC, 10, FW_NORMAL, "Arial" ) ;

	fx = 0 ;
	for ( i=0; i<zmz_nn; i++ )
	{
		xx = zmz_data[i].tot_xx ;
		xx *= dx ;
		x = (long)floor(xx) ;
		vv = zmz_data[i].tot_vv ;
		vv *= vv_ratio ;
		y = get_waveForm_y( (long)floor(vv), y0 ) ;

		DrawLine( g_waveForm_DC, x, y0, x, y, 1, tot_color, PS_SOLID, R2_COPYPEN ) ;

		if ( zmz_data[i].tot_vv<0 )
			fy = 1 ;
		else
			fy = -1 ;

		sprintf( show_ss, "%ld-%ld", zmz_data[i].first_ff, zmz_data[i].first_freq_nn ) ;
		DrawText( g_waveForm_DC, show_ss, x, y, tot_color, g_waveForm_backColor, fx, fy ) ;

	}
}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// 获取两个点直线与x轴相交的点
double get_x0_from_xy1_and_xy2( long x1, long y1, long x2, long y2 )
{
	double	k, x ;

	if ( fabs(y2-y1)<0.0001 ) // 如果两个y值太小，则直接返回x1
		x = (x2+x1)/2 ;
	else
	{
		k = y2-y1 ;
		k /= x2-x1 ;
		x = -y1/k + x1 ;
	}
	return( x ) ;
}
//////////////////////////////////////////////////////////////////////
long set_zmz_data( long ii, short *pcm_data )
{
	if ( g_zmz_data[ii].i2-g_zmz_data[ii].i1<=1 )
		return ( ii ) ;

	long	i, max_ii, fff, ff, ff_nn, xx, vv, max_vv ;
	double	tot_vv ;

	tot_vv = 0 ;
	max_ii = -1 ;
	max_vv = -10000000 ;

	ff_nn = 0 ;
	ff = g_zmz_data[ii].ff ;
	fff = ff ;
	for ( i=g_zmz_data[ii].i1+1; i<g_zmz_data[ii].i2; i++ )
	{
		vv = abs(pcm_data[i]) ;
		if ( max_vv<vv )
		{
			max_vv = vv ;
			max_ii = i ;
		}
		tot_vv += vv ;

		if ( ff>=0 )
		{
			if ( pcm_data[i]>=pcm_data[i-1] )
				continue ;
			xx = i ;
			ff = -1 ;
			ff_nn++ ;
		}
		else if ( ff<0 )
		{
			if ( pcm_data[i]<pcm_data[i-1] )
				continue ;
			xx = i ;
			ff = 1 ;
			ff_nn++ ;
		}
	}
	if ( ff_nn==0 )
		return ( ii ) ;

	tot_vv /= (g_zmz_data[ii].i2-g_zmz_data[ii].i1) ;

	max_vv = fff*max_vv ;
	tot_vv = fff*tot_vv ;

	g_zmz_data[ii].max_ii = max_ii ;
	g_zmz_data[ii].max_vv = max_vv ;
	g_zmz_data[ii].tot_vv = max_vv ; //tot_vv ;
	g_zmz_data[ii].tot_xx = (g_zmz_data[ii].i2+g_zmz_data[ii].i1)/2 ;
	g_zmz_data[ii].pole_xx = max_ii-g_zmz_data[ii].tot_xx ; //极值点与中心点之差

	g_zmz_data[ii].ff_nn = ff_nn ;
	g_zmz_data[ii].ff_tot_vv = tot_vv * ff_nn ;

	g_zmz_data[ii].xx = xx ;
	g_zmz_data[ii].yy = pcm_data[xx] ;

	ii ++ ;

	return ( ii ) ;
}
//////////////////////////////////////////////////////////////////////
void clear_zmz_data()
{
	long	i ;
	for ( i=0; i<g_zmz_nn; i++ )
		memset( &g_zmz_data[i], 0, sizeof(ZMZ_DATA) ) ;
	g_zmz_nn = 0 ;
}
//////////////////////////////////////////////////////////////////////
long make_zmz_data( short *pcm_data, long pcm_len )
{
	long	i, f, ii, ff ;
	double	x ;

	ff = 0 ;
	ii = 0 ;
	f = pcm_data[0] ;
	for ( i=1; i<pcm_len; i++ )
	{
		if ( pcm_data[i]<0 ) // 如果当前值小于0
		{
			if ( f>=0 ) // 并且前一个值是大于等于0的
			{
				x = get_x0_from_xy1_and_xy2( i-1, f, i, pcm_data[i] ) ; // 计算过零点

				if ( ff==0 )
				{
					g_zmz_data[ii].i1 = i ;
					g_zmz_data[ii].x1 = x ;	// 如果是起始值，则只设置左边界
				}
				else if ( ff==1 )			// 如果前边有一个最大值的数据，则设置右边界，同时设置下一个的左边界
				{
					g_zmz_data[ii].i2 = i ;
					g_zmz_data[ii].x2 = x ;
					g_zmz_data[ii].ff = 1 ;

					ii = set_zmz_data( ii, pcm_data ) ;

					g_zmz_data[ii].i1 = i ;
					g_zmz_data[ii].x1 = x ;

				}
				ff = -1 ;
			}
		}
		else if ( pcm_data[i]>=0 ) // 如果当前是大于等于0的
		{
			if ( f<0 ) // 如果前一个值是小于0的
			{
				x = get_x0_from_xy1_and_xy2( i-1, f, i, pcm_data[i] ) ; // 计算过零点
				if ( ff==0 )
				{
					g_zmz_data[ii].i1 = i ;
					g_zmz_data[ii].x1 = x ; // 如果是起始值，则只设置左边界
				}
				else if ( ff==-1 )			// 如果前边有一个最小值的数据，则设置右边界，同时设置下一个的左边界
				{
					g_zmz_data[ii].i2 = i ;
					g_zmz_data[ii].x2 = x ;
					g_zmz_data[ii].ff = -1 ;

					ii = set_zmz_data( ii, pcm_data ) ;

					g_zmz_data[ii].i1 = i ;
					g_zmz_data[ii].x1 = x ;

				}
				ff = 1 ;
			}
		}
		f = pcm_data[i] ;
	}
	return ( ii ) ;
}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void print_linked_zmz_data( ZMZ_DATA *zmz_data, long zmz_nn, char *label_ss )
{
	long	i ;

	log_prt( g_logFile, "------------------------------%s\r\n", label_ss ) ;
	for ( i=0; i<zmz_nn; i++ )
	{
		if ( zmz_data[i].first_ff==0 )
			continue ;

		log_prt( g_logFile, "i=%-5ld ff=%-3ld vv=%-9.1lf prev_ii=%-5ld freq_nn=%-5ld ff_nn=%-3ld pole_xx=%-6ld ff_tot_vv=%-8.1lf\r\n", 
						i, 
						zmz_data[i].first_ff,
						zmz_data[i].first_vv,
						zmz_data[i].first_prev_ii,
						zmz_data[i].first_freq_nn,
						zmz_data[i].ff_nn,
						zmz_data[i].pole_xx,
						zmz_data[i].ff_tot_vv
				) ;
	}
}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// 建立相邻两个非常接近的“频率关联点”，这个相邻的关联点之间的距离可以作为频率Freq计算的参考值
// 算法思路：
//			1. 首先找到zmz积分值的平均差
//			2. 然后按照vv_step从最大峰值往下降一条平行直线，如果有相交的点，则计算它们之间的距离值，将其作为计算频率的freq_nn
void create_freq_linked_zmz_data_1( ZMZ_DATA *zmz_data, long zmz_nn, long vv_step )
{
	double	dv, vv, min_vv, max_vv ;
	long	i, k, k0, ff, first_ff ;
	
	// 1. first_ff 清零
	for ( i=0; i<zmz_nn; i++ )
	{
		zmz_data[i].first_ff = 0 ;
		zmz_data[i].first_vv = 0 ;
		zmz_data[i].freq_done = 0 ;
	}

	// 2. 获取最大最小值
	min_vv = 1000000000 ;
	max_vv = -1000000000 ;
	for ( i=0; i<zmz_nn; i++ )
	{
		vv = zmz_data[i].tot_vv ;
		if ( max_vv<vv )
			max_vv = vv ;
		if ( min_vv>vv )
			min_vv = vv ;
	}

	// 3. 找负峰值的点
	first_ff = -1 ;
	dv = min_vv / vv_step ;
	for ( vv=min_vv-dv; vv<0; vv-=dv )
	{
		ff = 0 ;
		for ( i=0; i<zmz_nn; i++ )
		{
			if ( zmz_data[i].ff>0 )
				continue ;
			if ( zmz_data[i].first_ff!=0 )
				continue ;
			if ( zmz_data[i].tot_vv>vv )
				continue ;
			zmz_data[i].first_ff = first_ff ;
			zmz_data[i].first_vv = vv ;
			ff ++ ;
		}
		if ( ff>0 )
			first_ff -- ;
	}

	// 4. 找正峰值的点，做出标识
	first_ff = 1 ;
	dv = max_vv / vv_step ;
	for ( vv=max_vv-dv; vv>0; vv-=dv )
	{
		ff = 0 ;
		for ( i=0; i<zmz_nn; i++ )
		{
			if ( zmz_data[i].ff<0 )
				continue ;
			if ( zmz_data[i].first_ff!=0 )
				continue ;
			if ( zmz_data[i].tot_vv<vv )
				continue ;
			zmz_data[i].first_ff = first_ff ;
			zmz_data[i].first_vv = vv ;
			ff ++ ;
		}
		if ( ff>0 )
			first_ff ++ ;
	}

	// 5. 计算相同first_ff的点的距离
	for ( i=0; i<zmz_nn; i++ )
	{
		first_ff = zmz_data[i].first_ff ;
		k0 = i ;
		for ( k=i+1; k<zmz_nn; k++ )
		{
			if ( zmz_data[k].first_ff==first_ff )
			{
				zmz_data[k].first_prev_ii = k0 ;
				zmz_data[k].first_freq_nn = zmz_data[k].tot_xx-zmz_data[k0].tot_xx ;
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////
// 建立相邻两个非常接近的“频率关联点”，这个相邻的关联点之间的距离可以作为频率Freq计算的参考值
// 算法思路：
//			1. 首先找到zmz积分值的平均差
//			2. 然后按照vv_step从最大峰值往下降一条平行直线，如果有相交的点，则计算它们之间的距离值，将其作为计算频率的freq_nn
void create_freq_linked_zmz_data_2( ZMZ_DATA *zmz_data, long zmz_nn, long vv_step )
{
	double	dv, vv, min_vv, max_vv ;
	long	i, k, k0, ff, first_ff ;
	
	// 1. first_ff 清零
	for ( i=0; i<zmz_nn; i++ )
	{
		zmz_data[i].first_ff = 0 ;
		zmz_data[i].first_vv = 0 ;
		zmz_data[i].freq_done = 0 ;
	}

	// 2. 获取最大最小值
	min_vv = 1000000000 ;
	max_vv = -1000000000 ;
	for ( i=0; i<zmz_nn; i++ )
	{
		vv = zmz_data[i].ff_tot_vv ;
		if ( max_vv<vv )
			max_vv = vv ;
		if ( min_vv>vv )
			min_vv = vv ;
	}

	// 3. 找负峰值的点
	first_ff = -1 ;
	dv = min_vv / vv_step ;
	for ( vv=min_vv-dv; vv<0; vv-=dv )
	{
		ff = 0 ;
		for ( i=0; i<zmz_nn; i++ )
		{
			if ( zmz_data[i].ff>0 )
				continue ;
			if ( zmz_data[i].first_ff!=0 )
				continue ;
			if ( zmz_data[i].ff_tot_vv>vv )
				continue ;
			zmz_data[i].first_ff = first_ff ;
			zmz_data[i].first_vv = vv ;
			ff ++ ;
		}
		if ( ff>0 )
			first_ff -- ;
	}

	// 4. 找正峰值的点，做出标识
	first_ff = 1 ;
	dv = max_vv / vv_step ;
	for ( vv=max_vv-dv; vv>0; vv-=dv )
	{
		ff = 0 ;
		for ( i=0; i<zmz_nn; i++ )
		{
			if ( zmz_data[i].ff<0 )
				continue ;
			if ( zmz_data[i].first_ff!=0 )
				continue ;
			if ( zmz_data[i].ff_tot_vv<vv )
				continue ;
			zmz_data[i].first_ff = first_ff ;
			zmz_data[i].first_vv = vv ;
			ff ++ ;
		}
		if ( ff>0 )
			first_ff ++ ;
	}

	// 5. 计算相同first_ff的点的距离
	for ( i=0; i<zmz_nn; i++ )
	{
		first_ff = zmz_data[i].first_ff ;
		k0 = i ;
		for ( k=i+1; k<zmz_nn; k++ )
		{
			if ( zmz_data[k].first_ff==first_ff )
			{
				zmz_data[k].first_prev_ii = k0 ;
				zmz_data[k].first_freq_nn = zmz_data[k].tot_xx-zmz_data[k0].tot_xx ;
				k0 = k ;
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////
void create_freq_linked_zmz_data_3( ZMZ_DATA *zmz_data, long zmz_nn, long vv_step, long poly_dx )
{
	double	vv, min_vv, max_vv, vv_ratio, tot_vv ;
	long	i, k, k0, first_ff ;

	long	ff_nn, pole_xx ;


	
	// 1. first_ff 清零
	for ( i=0; i<zmz_nn; i++ )
	{
		zmz_data[i].first_ff = 0 ;
		zmz_data[i].first_vv = 0 ;
		zmz_data[i].freq_done = 0 ;
	}
	// 2. 获取最大最小值
	min_vv = 1000000000 ;
	max_vv = -1000000000 ;
	for ( i=0; i<zmz_nn; i++ )
	{
		vv = fabs(zmz_data[i].tot_vv) ;
		if ( max_vv<vv )
			max_vv = vv ;
		if ( min_vv>vv )
			min_vv = vv ;
	}
//	vv_ratio = (max_vv-min_vv) / vv_step ;
	vv_ratio = max_vv / vv_step ;

	first_ff = 1 ;
	for ( i=0; i<zmz_nn; i++ )
	{
		if ( zmz_data[i].first_ff!=0 )
			continue ;

		ff_nn = zmz_data[i].ff_nn ;
		pole_xx = zmz_data[i].pole_xx ;
		tot_vv = zmz_data[i].tot_vv ;
		
		for ( k=i; k<zmz_nn; k++ )
		{
			if ( zmz_data[k].first_ff!=0 )
				continue ;
			if ( zmz_data[k].ff_nn!=ff_nn )
				continue ;
			if ( abs(zmz_data[k].pole_xx-pole_xx)>poly_dx )
				continue ;
			if ( fabs(zmz_data[k].tot_vv-tot_vv)>vv_ratio )
				continue ;
			zmz_data[k].first_ff = first_ff ;
			zmz_data[k].first_vv = tot_vv ;
		}
		first_ff ++ ;
	}

	// 5. 计算相同first_ff的点的距离
	for ( i=0; i<zmz_nn; i++ )
	{
		first_ff = zmz_data[i].first_ff ;
		k0 = i ;
		for ( k=i+1; k<zmz_nn; k++ )
		{
			if ( zmz_data[k].first_ff==first_ff )
			{
				zmz_data[k].first_prev_ii = k0 ;
				zmz_data[k].first_freq_nn = zmz_data[k].tot_xx-zmz_data[k0].tot_xx ;
				k0 = k ;
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////
long get_freq_data_from_first_ff( ZMZ_DATA *zmz_data, long zmz_nn )
{

	double	vv, vv_avg ;
	long	i, k, ii, ff, is, ie, nn, tot_nn, tot_ff ;

	// 1. 首先建立频率的基本统计数据，即对每一个相同峰值的个数进行计算。
	ii = 0 ;
	tot_nn = 0 ;
	tot_ff = 0 ;
	vv_avg = 0 ;
	for ( i=0; i<zmz_nn; i++ )
	{
		if ( zmz_data[i].first_ff!=0 && zmz_data[i].freq_done==0 )
		{
			zmz_data[i].freq_done = 1 ;

			ff = zmz_data[i].first_ff ;
			is = zmz_data[i].tot_xx ;
			nn = 0 ;
			vv = 0 ;
			for ( k=i+1; k<zmz_nn; k++ )
			{
				if ( zmz_data[k].first_ff==ff )
				{
					nn ++ ;
					ie = zmz_data[k].tot_xx ;
					vv += zmz_data[k].first_freq_nn ;
					zmz_data[k].freq_done = 1 ;
				}
			}
			if ( nn>0 )
			{
				g_freq_data[ii].freq_ff = (short)ff ;
				g_freq_data[ii].freq_is = (short)is ;
				g_freq_data[ii].freq_ie = (short)ie ;

				g_freq_data[ii].freq_vv = vv ;
				g_freq_data[ii].freq_min = 0 ;
				g_freq_data[ii].freq_max = 0 ;

				g_freq_data[ii].freq_avg = vv/nn ;

				g_freq_data[ii].freq_nn = nn ;

				vv_avg += vv ;
				tot_nn += nn ; // 统计总个数
				tot_ff ++ ;

				ii ++ ;
			}
		}
	}
	return ( ii ) ;
}
//////////////////////////////////////////////////////////////////////
void clear_freq_data()
{
	long	i ;
	for ( i=0; i<g_freq_data_nn; i++ )
		memset( &g_freq_data[i], 0, sizeof( FREQ_DATA ) ) ;
	g_freq_data_nn = 0 ;
}
//////////////////////////////////////////////////////////////////////
long get_freq_data_from_first_ff_2( ZMZ_DATA *zmz_data, long zmz_nn )
{

	double	vv ;
	long	i, k, ii, ff, is, ie, nn, freq_nn ;

	// 1. 首先建立频率的基本统计数据，即对每一个相同峰值的个数进行计算。
	ii = 0 ;
	ff = 1 ;
	for ( i=0; i<zmz_nn; i++ )
	{
		if ( zmz_data[i].first_freq_nn==0 || zmz_data[i].freq_done!=0 )
			continue ;
		
		freq_nn = zmz_data[i].first_freq_nn ;
		is = i ;
		nn = 0 ;
		vv = 0 ;
		for ( k=i+1; k<zmz_nn; k++ )
		{
			if ( zmz_data[k].first_freq_nn==freq_nn )
			{
				zmz_data[k].freq_done = 1 ;
				ie = k ;
				vv += zmz_data[k].first_freq_nn ;
				nn ++ ;
			}
		}

		if ( nn>0 )
		{
			g_freq_data[ii].freq_ff = (short)ff ;
			g_freq_data[ii].freq_is = (short)is ;
			g_freq_data[ii].freq_ie = (short)ie ;

			g_freq_data[ii].freq_vv = vv ;
			g_freq_data[ii].freq_min = 0 ;
			g_freq_data[ii].freq_max = 0 ;

			g_freq_data[ii].freq_avg = vv/nn ;
			g_freq_data[ii].freq_nn = nn ;


			g_freq_data[ii].each_vvv = 0 ;
			g_freq_data[ii].each_dif = 0 ;

			ii ++ ;
			ff++ ;
		}
	}
	print_freq_data( g_freq_data, ii, g_cur_file ) ;

	return ( ii ) ;
}
//////////////////////////////////////////////////////////////////////
void make_freq_data_freq_diff_proc( FREQ_DATA *freq_data, long freq_nn, double ratio )
{

	double	vv, vv_avg, vv_ratio ;
	long	i ;

//  下面这个方法不是很好

	// 计算每个平均值与总平均值之差
	vv_avg = 0 ;
	for ( i=0; i<freq_nn; i++ )
		vv_avg += fabs( freq_data[i].freq_avg ) ;
	vv_avg /= freq_nn ;

	for ( i=0; i<freq_nn; i++ )
		freq_data[i].freq_dif = fabs( freq_data[i].freq_avg-vv_avg ) ;

	// 计算差值的总平均值
	double vv_dif ;
	vv_dif = 0 ;
	for ( i=0; i<freq_nn; i++ )
		vv_dif += fabs( freq_data[i].freq_dif ) ;
	vv_dif /= freq_nn ;

	if ( vv_dif>1 )
	{
		// 去掉差值太大的
		vv_ratio = vv_dif*ratio ;
		for ( i=0; i<freq_nn; i++ )
		{
			vv = fabs(freq_data[i].freq_dif-vv_dif) ;
			if ( vv>vv_ratio ) // 如果这组极值点的平均极值与总的极值平均值差的太多
				freq_data[i].freq_ff = 0 ;
		}
		print_freq_data( freq_data, freq_nn, g_cur_file ) ;
	}
}
//////////////////////////////////////////////////////////////////////
long move_freq_data( FREQ_DATA *freq_data, long freq_nn )
{
	long	i, ii ;

	ii = 0 ;
	for ( i=0; i<freq_nn; i++ )
	{
		if ( freq_data[i].freq_ff==0 )
			continue ;
		memcpy( &freq_data[ii], &freq_data[i], sizeof(FREQ_DATA) ) ;
		ii++ ;
	}
	return ( ii ) ;
}
//////////////////////////////////////////////////////////////////////
long make_freq_data_each_diff_proc( FREQ_DATA *freq_data, long freq_nn, double ratio )
{
	double	vv, vv_avg, vv_tot ;
	double vv_dif, vv_ratio ;

	long	i, k ;

	// 1. 计算积分值的均值
	vv_tot = 0 ;
	for ( i=0; i<freq_nn; i++ )
	{
		vv_tot += fabs( freq_data[i].freq_avg ) ;
		freq_data[i].each_vvv = 0 ;
		freq_data[i].each_dif = 0 ;
	}
	vv_avg = vv_tot / freq_nn ;

	vv_ratio = vv_avg*ratio ;

	// 2. 设置每个积分值与均值的差，同时计算它们的离散差之和
	g_freq_dif_tot = 0 ;
	for ( i=0; i<freq_nn; i++ )
	{
		vv = fabs( freq_data[i].freq_avg-vv_avg) ;
		freq_data[i].freq_dif = vv ;
		g_freq_dif_tot += vv ;
	}

	vv_dif = g_freq_dif_tot ;
	if ( vv_dif>2 )
	{
		for ( i=0; i<freq_nn; i++ )
		{
			vv = fabs(freq_data[i].freq_dif) ;
			if ( vv>vv_ratio )
				freq_data[i].freq_ff = 0 ;
		}
		freq_nn = move_freq_data( freq_data, freq_nn ) ;
		if ( freq_nn==0 )
		{
			log_prt( g_logFile, "****** freq_dif_too_big=%1.2lf %s\r\n", g_freq_dif_tot, g_testFile ) ;
			return ( freq_nn ) ;
		}
	}

	// 3. 计算两两积分值的加权值
	for ( i=0; i<freq_nn; i++ )
	{
		for ( k=0; k<freq_nn; k++ )
		{
			if ( k==i )
				continue ;
			vv = fabs(freq_data[i].freq_avg-freq_data[k].freq_avg) ;
			if ( vv>0.001 )
				freq_data[i].each_vvv += (vv_tot/vv)*(freq_data[i].freq_nn+freq_data[k].freq_nn) ;
		}
	}

	vv_avg = 0 ;
	for ( i=0; i<freq_nn; i++ )
		vv_avg += fabs( freq_data[i].each_vvv ) ;

	vv_avg /= freq_nn ;

	g_each_dif_tot = 0 ;
	for ( i=0; i<freq_nn; i++ )
	{
		vv = fabs( freq_data[i].each_vvv-vv_avg ) ;
		freq_data[i].each_dif = vv ;
		g_each_dif_tot += vv ;
	}

	print_freq_data( freq_data, freq_nn, g_cur_file ) ;

	vv_dif = g_each_dif_tot ;
	if ( vv_dif>2 )
	{
		// 去掉差值太大的，同时计算有效的那些数据的均方差
		vv_ratio = vv_dif*ratio ;
		for ( i=0; i<freq_nn; i++ )
		{
			vv = fabs(freq_data[i].each_dif-vv_dif) ;
			if ( vv>vv_ratio ) // 如果这组极值点的平均极值与总的极值平均值差的太多
				freq_data[i].freq_ff = 0 ;
		}

		freq_nn = move_freq_data( freq_data, freq_nn ) ;

		if ( freq_nn==0 )
		{
			log_prt( g_logFile, "****** each_dif_too_big=%1.2lf %s\r\n", g_each_dif_tot, g_testFile ) ;
			return ( freq_nn ) ;
		}

	}

	return ( freq_nn ) ;

}
//////////////////////////////////////////////////////////////////////
double freq_data_proc( FREQ_DATA *freq_data, long freq_nn, double ratio )
{

	double	vv ;
	long	i, ii ;

	// 1. 去掉极值点个数太少的那些数据
	long	mm, min_mm, max_mm, tot_mm ;
	min_mm = 100000 ;
	max_mm = -100000 ;
	for ( i=0; i<freq_nn; i++ )
	{
		mm = freq_data[i].freq_nn ;
		if ( min_mm>mm )
			min_mm = mm ;
		if ( max_mm<mm )
			max_mm = mm ;
	}
	tot_mm = (max_mm-min_mm)/2 ;

	for ( i=0; i<freq_nn; i++ )
	{
		if ( freq_data[i].freq_nn<tot_mm ) // 如果这组极值点的个数小于平均数的话，则直接去掉
			freq_data[i].freq_ff = 0 ;
	}

	ii = 0 ;
	for ( i=0; i<freq_nn; i++ )
	{
		if ( freq_data[i].freq_ff==0 )
			continue ;
		memcpy( &freq_data[ii], &freq_data[i], sizeof(FREQ_DATA) ) ;
		ii++ ;
	}
	freq_nn = ii ;
	print_freq_data( freq_data, freq_nn, g_cur_file ) ;

//	make_freq_data_freq_diff_proc( freq_data, freq_nn, ratio ) ;
	freq_nn = make_freq_data_each_diff_proc( freq_data, freq_nn, ratio ) ;

	if ( freq_nn==0 )
		return ( 0 ) ;

	print_freq_data( freq_data, freq_nn, g_cur_file ) ;


	// 5. 计算出剩下的所有极值点的平均值
	vv = 0 ;
	ii = 0 ;
	for ( i=0; i<freq_nn; i++ )
	{
		if ( freq_data[i].freq_ff!=0 )
		{
			vv += freq_data[i].freq_vv ;
			ii += freq_data[i].freq_nn ;
		}
	}
	if ( ii>0 )
	{
		vv /= ii ;
		return ( vv ) ;
	}
	else
		return ( 0 ) ;

}
/////////////////////////////////////////////////////////////////////////////
void do_quickSort_freq_data_by_freq_vv( FREQ_DATA *freq_data, long iLo, long iHi )
{
	long		iSize, Lo, Hi ;
	double		Mid ;
	FREQ_DATA	a_data ;

	iSize = sizeof( FREQ_DATA ) ;

	Lo = iLo;
	Hi = iHi;
  
	Mid = (freq_data[Lo].freq_vv+freq_data[Hi].freq_vv) / 2;

	while ( true )
	{
		while ( freq_data[Lo].freq_vv < Mid )
			Lo++ ;
		while ( freq_data[Hi].freq_vv > Mid )
			Hi-- ;

		if ( Lo <= Hi )
		{
			memcpy( &a_data, &freq_data[Lo], iSize ) ;
			memcpy( &freq_data[Lo], &freq_data[Hi], iSize ) ;
			memcpy( &freq_data[Hi], &a_data, iSize ) ;
			Lo++;
			Hi--;

		}
		if ( Lo>Hi )
			break ;
    }

	if ( Hi > iLo )
		do_quickSort_freq_data_by_freq_vv( freq_data, iLo, Hi);
	if ( Lo < iHi )
		do_quickSort_freq_data_by_freq_vv( freq_data, Lo, iHi);
 
}
/////////////////////////////////////////////////////////////////////////////
void do_quickSort_freq_data_by_freq_nn( FREQ_DATA *freq_data, long iLo, long iHi )
{
	long		iSize, Lo, Hi ;
	long		Mid ;
	FREQ_DATA	a_data ;

	iSize = sizeof( FREQ_DATA ) ;

	Lo = iLo;
	Hi = iHi;
  
	Mid = (freq_data[Lo].freq_nn+freq_data[Hi].freq_nn) / 2;

	while ( true )
	{
		while ( freq_data[Lo].freq_nn < Mid )
			Lo++ ;
		while ( freq_data[Hi].freq_nn > Mid )
			Hi-- ;

		if ( Lo <= Hi )
		{
			memcpy( &a_data, &freq_data[Lo], iSize ) ;
			memcpy( &freq_data[Lo], &freq_data[Hi], iSize ) ;
			memcpy( &freq_data[Hi], &a_data, iSize ) ;
			Lo++;
			Hi--;

		}
		if ( Lo>Hi )
			break ;
    }

	if ( Hi > iLo )
		do_quickSort_freq_data_by_freq_nn( freq_data, iLo, Hi);
	if ( Lo < iHi )
		do_quickSort_freq_data_by_freq_nn( freq_data, Lo, iHi);
 
}
/////////////////////////////////////////////////////////////////////////////
void do_quickSort_freq_data_by_freq_avg( FREQ_DATA *freq_data, long iLo, long iHi )
{
	long		iSize, Lo, Hi ;
	double		Mid ;
	FREQ_DATA	a_data ;

	iSize = sizeof( FREQ_DATA ) ;

	Lo = iLo;
	Hi = iHi;
  
	Mid = (freq_data[Lo].freq_avg+freq_data[Hi].freq_avg) / 2;

	while ( true )
	{
		while ( freq_data[Lo].freq_avg < Mid )
			Lo++ ;
		while ( freq_data[Hi].freq_avg > Mid )
			Hi-- ;

		if ( Lo <= Hi )
		{
			memcpy( &a_data, &freq_data[Lo], iSize ) ;
			memcpy( &freq_data[Lo], &freq_data[Hi], iSize ) ;
			memcpy( &freq_data[Hi], &a_data, iSize ) ;
			Lo++;
			Hi--;

		}
		if ( Lo>Hi )
			break ;
    }

	if ( Hi > iLo )
		do_quickSort_freq_data_by_freq_nn( freq_data, iLo, Hi);
	if ( Lo < iHi )
		do_quickSort_freq_data_by_freq_nn( freq_data, Lo, iHi);
 
}
/////////////////////////////////////////////////////////////////////////////
void do_reverse_freq_data( FREQ_DATA *freq_data, long Lo, long Hi )
{
	long		iSize ;
	FREQ_DATA	a_data ;

	iSize = sizeof(FREQ_DATA) ;
	for ( ;; )
	{
		memcpy( &a_data, &freq_data[Lo], iSize ) ;
		memcpy( &freq_data[Lo], &freq_data[Hi], iSize ) ;
		memcpy( &freq_data[Hi], &a_data, iSize ) ;
		Lo ++ ;
		Hi -- ;
		if ( Lo>=Hi )
			break ;
	}
}
//////////////////////////////////////////////////////////////////////
long freq_data_proc_by_freq_vv( FREQ_DATA *freq_data, long freq_nn )
{

	log_prt( g_logFile, "==================freq_data_proc_by_freq_vv\r\n" ) ;

	do_quickSort_freq_data_by_freq_vv( freq_data, 0, freq_nn-1 ) ;
	do_reverse_freq_data( freq_data, 0, freq_nn-1 ) ;

	print_freq_data( freq_data, freq_nn, g_cur_file ) ;

	long	i, ii ;
	double	vv ;

	ii = 1 ;
	for ( i=1; i<freq_nn; i++ )
	{
		vv = fabs(freq_data[i].freq_vv-freq_data[i-1].freq_vv) ;
		if ( vv>freq_data[i-1].freq_vv/2 )
			break ;
		ii++ ;
	}
	freq_nn = ii ;
	print_freq_data( freq_data, freq_nn, g_cur_file ) ;

	return ( freq_nn ) ;

}
//////////////////////////////////////////////////////////////////////
long freq_data_proc_by_freq_nn( FREQ_DATA *freq_data, long freq_nn )
{

	log_prt( g_logFile, "==================freq_data_proc_by_freq_nn\r\n" ) ;

	do_quickSort_freq_data_by_freq_nn( freq_data, 0, freq_nn-1 ) ;
	do_reverse_freq_data( freq_data, 0, freq_nn-1 ) ;

	print_freq_data( freq_data, freq_nn, g_cur_file ) ;

	long	i, ii, nn ;

	ii = 1 ;
	for ( i=1; i<freq_nn; i++ )
	{
		nn = abs(freq_data[i].freq_nn-freq_data[i-1].freq_nn) ;
		if ( nn>freq_data[i-1].freq_nn/2 )
			break ;
		ii++ ;
	}
	freq_nn = ii ;
	print_freq_data( freq_data, freq_nn, g_cur_file ) ;

	return ( freq_nn ) ;

}
//////////////////////////////////////////////////////////////////////
long freq_data_proc_by_freq_avg( FREQ_DATA *freq_data, long freq_nn )
{

	log_prt( g_logFile, "==================freq_data_proc_by_freq_avg\r\n" ) ;

	do_quickSort_freq_data_by_freq_avg( freq_data, 0, freq_nn-1 ) ;
	do_reverse_freq_data( freq_data, 0, freq_nn-1 ) ;

	print_freq_data( freq_data, freq_nn, g_cur_file ) ;

	long	i, ii ;
	double	vv ;

	ii = 1 ;
	for ( i=1; i<freq_nn; i++ )
	{
		vv = fabs(freq_data[i].freq_avg-freq_data[i-1].freq_avg) ;
		if ( vv>freq_data[i-1].freq_avg/2 )
			break ;
		ii++ ;
	}
	freq_nn = ii ;
	print_freq_data( freq_data, freq_nn, g_cur_file ) ;

	return ( freq_nn ) ;

}
//////////////////////////////////////////////////////////////////////
long freq_data_proc_by_freq_nn_avg( FREQ_DATA *freq_data, long freq_nn )
{

	log_prt( g_logFile, "==================freq_data_proc_by_freq_nn_avg\r\n" ) ;

	long	i, ii, nn_avg ;

	nn_avg = 0 ;
	for ( i=0; i<freq_nn; i++ )
		nn_avg += freq_data[i].freq_nn ;

	nn_avg /= freq_nn ;

	ii = 0 ;
	for ( i=0; i<freq_nn; i++ )
	{
		if ( freq_data[i].freq_nn<nn_avg )
			continue ;
		if ( i>ii )
			memcpy( &freq_data[ii], &freq_data[i], sizeof(FREQ_DATA) ) ;
		ii ++ ;
	}
	freq_nn = ii ;

	print_freq_data( freq_data, freq_nn, g_cur_file ) ;

	return ( freq_nn ) ;

}
//////////////////////////////////////////////////////////////////////
long freq_data_proc_by_freq_nn_avg2( FREQ_DATA *freq_data, long freq_nn )
{

	log_prt( g_logFile, "==================freq_data_proc_by_freq_nn_avg\r\n" ) ;

	long	i, ii, nn, nn_max ;

	nn_max = -100000 ;
	for ( i=0; i<freq_nn; i++ )
	{
		nn = freq_data[i].freq_nn ;
		if ( nn_max<nn )
			nn_max = nn ;
	}

	ii = 0 ;
	for ( i=0; i<freq_nn; i++ )
	{
		if ( freq_data[i].freq_nn<nn_max/2 )
			continue ;
		if ( i>ii )
			memcpy( &freq_data[ii], &freq_data[i], sizeof(FREQ_DATA) ) ;
		ii ++ ;
	}
	freq_nn = ii ;

	print_freq_data( freq_data, freq_nn, g_cur_file ) ;

	return ( freq_nn ) ;

}
//////////////////////////////////////////////////////////////////////
long freq_data_proc_by_freq_vv_avg( FREQ_DATA *freq_data, long freq_nn )
{

	log_prt( g_logFile, "==================freq_data_proc_by_freq_vv_avg\r\n" ) ;

	long	i, ii ;
	double	vv_avg ;

	vv_avg = 0 ;
	for ( i=0; i<freq_nn; i++ )
		vv_avg += freq_data[i].freq_vv ;

	vv_avg /= freq_nn ;

	ii = 0 ;
	for ( i=0; i<freq_nn; i++ )
	{
		if ( freq_data[i].freq_vv<vv_avg )
			continue ;
		if ( i>ii )
			memcpy( &freq_data[ii], &freq_data[i], sizeof(FREQ_DATA) ) ;
		ii ++ ;
	}
	freq_nn = ii ;

	print_freq_data( freq_data, freq_nn, g_cur_file ) ;

	return ( freq_nn ) ;

}
//////////////////////////////////////////////////////////////////////
long freq_data_proc_by_freq_avg_avg( FREQ_DATA *freq_data, long freq_nn )
{

	log_prt( g_logFile, "==================freq_data_proc_by_freq_vv_avg_avg\r\n" ) ;

	long	i, ii ;
	double	vv_avg ;

	vv_avg = 0 ;
	for ( i=0; i<freq_nn; i++ )
		vv_avg += freq_data[i].freq_avg ;

	vv_avg /= freq_nn ;

	ii = 0 ;
	for ( i=0; i<freq_nn; i++ )
	{
		if ( freq_data[i].freq_avg<vv_avg )
			continue ;
		if ( i>ii )
			memcpy( &freq_data[ii], &freq_data[i], sizeof(FREQ_DATA) ) ;
		ii ++ ;
	}
	freq_nn = ii ;

	print_freq_data( freq_data, freq_nn, g_cur_file ) ;

	return ( freq_nn ) ;

}
//////////////////////////////////////////////////////////////////////
double freq_data_proc2( FREQ_DATA *freq_data, long freq_nn, double ratio )
{

	if ( freq_nn>1 )
//		freq_nn = freq_data_proc_by_freq_nn( freq_data, freq_nn ) ;
		freq_nn = freq_data_proc_by_freq_nn_avg2( freq_data, freq_nn ) ;

	if ( freq_nn>1 )
//		freq_nn = freq_data_proc_by_freq_vv_avg( freq_data, freq_nn ) ;
		freq_nn = freq_data_proc_by_freq_vv( freq_data, freq_nn ) ;

	if ( freq_nn>1 )
//		freq_nn = freq_data_proc_by_freq_avg( freq_data, freq_nn ) ;
		freq_nn = freq_data_proc_by_freq_avg_avg( freq_data, freq_nn ) ;

	if ( freq_nn<=0 )
		return ( 0 ) ;

	long	i ;
	double	vv ;

	vv = 0 ;
	for ( i=0; i<freq_nn; i++ )
		vv += freq_data[i].freq_avg ;
	vv /= freq_nn ;
	return ( vv ) ;
}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void make_freq_from_pcm_data( short *pcm_data, long pcm_len )
{
	log_prt( g_logFile, "-------------------make_freq_from_pcm_data\r\n" ) ;

	clear_zmz_data() ;
	clear_freq_data() ;

	g_zmz_nn = make_zmz_data( pcm_data, pcm_len ) ;

//	create_freq_linked_zmz_data_1( g_zmz_data, g_zmz_nn, 10 ) ;
//	create_freq_linked_zmz_data_1( g_zmz_data, g_zmz_nn, 20 ) ;
//	create_freq_linked_zmz_data_2( g_zmz_data, g_zmz_nn, 20 ) ;

	create_freq_linked_zmz_data_3( g_zmz_data, g_zmz_nn, 10, 3 ) ;

	print_linked_zmz_data( g_zmz_data, g_zmz_nn, "lined_1" ) ;

	show_zmz_data_proc( g_zmz_data, g_zmz_nn, pcm_len, RGB(122,0,120), RGB(220,220,220) ) ;

	long nn ;
	nn = get_freq_data_from_first_ff_2( g_zmz_data, g_zmz_nn ) ;
	if ( nn==0 ) 
		return ;
	g_freq_data_nn = nn ;




	double freq, freq_nn ;

	freq_nn = freq_data_proc2( g_freq_data, nn, 0.328 ) ;


	if ( freq_nn==0 )
		return ;

	freq = g_Samples ;
	freq /= freq_nn ;

	char	ss[100] ;

	sprintf( ss, "%0.2lf", freq ) ;
/**
	sscanf( ss, "%lf", &freq ) ;

	long ni = noteInfo_proc( freq, 1 ) ;

	char	freq_ss[10] ;

	freq_ss[0] = g_note_info[ni].note_name[0] ;
	freq_ss[1] = g_note_info[ni].note_name[1] ;
	freq_ss[2] = g_note_info[ni].note_name[2] ;
	freq_ss[3] = '\0' ;

	difs = 0 ;
	if ( strcmp( freq_ss, g_test_data.music_note )!=0 )
	{
		log_prt( g_logFile, "\r\n************************************* wrong_note=%s music_note=%s freq=%1.2lf note_freq=%1.2lf\r\n\r\n", freq_ss, g_test_data.music_note, freq, g_test_data.music_note_freq ) ;

//		PopEvent( 5001, 1, "make_freq_from_pcm_data()" ) ;

		g_test_data.wrong_note_nn ++ ;
	}
	else
	{
		difs = freq-g_note_info[ni].note_freq ;
		g_test_data.note_freq_dif += fabs(difs) ;

		log_prt( g_logFile, "\r\n------------------------------------- freq_nn=%-5.0lf freq=%1.2lf-%1.2lf(%1.2lf) tot_dif=%2.2lf\r\n\r\n", freq_nn, freq, g_note_info[ni].note_freq, difs, g_test_data.note_freq_dif ) ;

	}
**/
//	do_task( 201, ss ) ;

}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void show_zmz_data_proc_2( ZMZ_DATA *zmz_data, long zmz_nn, long xx_len, long zmz_color, long tot_color )
{
	long	i, ii, y0, x, y ;
	double	xx, dx, vv, max_vv, vv_ratio ;

	y0 = g_waveForm_bb / 2 ;

	dx = g_waveForm_aa ;
	dx /= xx_len ;

	max_vv = -100000000 ;
	ii = 0 ;
	xx = 0 ;
	for ( i=0; i<zmz_nn; i++ )
	{
		// 左过零点
		xx = zmz_data[i].x1 ;
		xx *= dx ;
		g_xy_data[ii].x = (long)floor(xx) ;
		g_xy_data[ii].y = get_waveForm_y( 0, y0 ) ;
		ii++ ;


		// 极值点
		xx = zmz_data[i].max_xx ;
		xx *= dx ;
		g_xy_data[ii].x = (long)floor(xx) ;
		g_xy_data[ii].y = get_waveForm_y( (long)floor(zmz_data[i].max_vv), y0 ) ;
		ii++ ;

		// 右过零点
		xx = zmz_data[i].x2 ;
		xx *= dx ;
		g_xy_data[ii].x = (long)floor(xx) ;
		g_xy_data[ii].y = get_waveForm_y( 0, y0 ) ;
		ii++ ;

		vv = abs(zmz_data[i].max_vv) ;
		if ( max_vv<vv )
			max_vv = vv ;

	}
	PolyLine( g_waveForm_DC, g_xy_data, ii, 1, zmz_color, PS_SOLID, R2_COPYPEN ) ;
	
	vv_ratio = 30000/max_vv ;

	long	fx, fy ;
	char	show_ss[100] ;

	SetFont( g_waveForm_DC, 10, FW_NORMAL, "Arial" ) ;

	fx = 0 ;
	for ( i=0; i<zmz_nn; i++ )
	{
		xx = zmz_data[i].i0 ;
		xx *= dx ;
		x = (long)floor(xx) ;

		vv = zmz_data[i].max_vv ;
		vv *= vv_ratio ;
		y = get_waveForm_y( (long)floor(vv), y0 ) ;

		DrawLine( g_waveForm_DC, x, y0, x, y, 1, tot_color, PS_SOLID, R2_COPYPEN ) ;

		if ( zmz_data[i].max_vv<0 )
			fy = 1 ;
		else
			fy = -1 ;

		sprintf( show_ss, "%ld-%ld-%ld", zmz_data[i].freq_ff, zmz_data[i].freq_nn, zmz_data[i].ff_nn ) ;

		DrawText( g_waveForm_DC, show_ss, x, y, tot_color, g_waveForm_backColor, fx, fy ) ;

	}
}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
long set_zmz_data_2_old( long ii, short *pcm_data )
{
	if ( g_zmz_data[ii].i2-g_zmz_data[ii].i1<=1 )
		return ( ii ) ;

	long	i, max_ii, fff, ff, ff_nn, vv, max_vv, v1, v2 ;
	double	tot_vv ;

	tot_vv = 0 ;
	max_ii = -1 ;
	max_vv = -10000000 ;

	ff = g_zmz_data[ii].ff ;

	ff_nn = 0 ;
	fff = ff ;
	for ( i=g_zmz_data[ii].i1+1; i<g_zmz_data[ii].i2; i++ )
	{
		vv = abs(pcm_data[i]) ;
		if ( max_vv<vv )
		{
			max_vv = vv ;
			max_ii = i ;
		}
		v1 = pcm_data[i] ;
		v2 = pcm_data[i-1] ;
		if ( ff>=0 )
		{
			if ( v1>=v2 )
				continue ;
			ff = -1 ;
			ff_nn++ ;
		}
		else if ( ff<0 )
		{
			if ( v1<=v2 )
				continue ;
			ff = 1 ;
			ff_nn++ ;
		}
	}
	if ( ff_nn==0 )
		return ( ii ) ;

	max_vv = fff*max_vv ;

	g_zmz_data[ii].max_ii = max_ii-g_zmz_data[ii].i0 ;
	g_zmz_data[ii].max_vv = max_vv ;
	g_zmz_data[ii].i0 = (g_zmz_data[ii].i2+g_zmz_data[ii].i1)/2 ;
//	g_zmz_data[ii].im = max_ii-g_zmz_data[ii].i0 ;
	g_zmz_data[ii].ff_nn = ff_nn ;

	g_zmz_data[ii].xx = (g_zmz_data[ii].x2+g_zmz_data[ii].x1)/2 ;

	ii ++ ;

	return ( ii ) ;
}
//////////////////////////////////////////////////////////////////////
long set_zmz_data_2_ex( long ii, short *pcm_data, long pcm_nn )
{
	if ( g_zmz_data[ii].i2-g_zmz_data[ii].i1<=1 )
		return ( ii ) ;

	long	i, max_ii, fff, ff, ff_nn, vv, max_vv, v1, v2 ;
	double	tot_vv ;

	tot_vv = 0 ;
	max_ii = -1 ;
	max_vv = -10000000 ;

	ff = g_zmz_data[ii].ff ;

	ff_nn = 0 ;
	fff = ff ;
	for ( i=0; i<pcm_nn; i++ )
	{
		vv = abs(pcm_data[i]) ;
		if ( max_vv<vv )
		{
			max_vv = vv ;
			max_ii = i ;
		}
		v1 = pcm_data[i] ;
		v2 = pcm_data[i-1] ;
		if ( ff>=0 )
		{
			if ( v1>=v2 )
				continue ;
			ff = -1 ;
			ff_nn++ ;
		}
		else if ( ff<0 )
		{
			if ( v1<=v2 )
				continue ;
			ff = 1 ;
			ff_nn++ ;
		}
	}
	if ( ff_nn==0 )
		return ( ii ) ;

//	if ( max_vv<3000 )
//		return ( ii ) ;

	max_vv = fff*max_vv ;

	g_zmz_data[ii].max_ii = max_ii ;
	g_zmz_data[ii].max_xx = g_zmz_data[ii].i1 + max_ii*g_zmz_data[ii].xx_div ;
	g_zmz_data[ii].max_vv = max_vv ;
	g_zmz_data[ii].i0 = (g_zmz_data[ii].i2+g_zmz_data[ii].i1)/2 ;
	g_zmz_data[ii].ff_nn = ff_nn ;

	g_zmz_change += ff_nn ;

	g_zmz_data[ii].xx = (g_zmz_data[ii].x2+g_zmz_data[ii].x1)/2 ;

	ii ++ ;

	return ( ii ) ;
}
//////////////////////////////////////////////////////////////////////
long set_zmz_data_2( long ii, short *pcm_data )
{
	long	i, k, xx_div, xx_nn, dn ;
	double	vv ;

	dn = 30 ;
	xx_nn = g_zmz_data[ii].i2-g_zmz_data[ii].i1 ;
	xx_div = xx_nn/dn ;

	if ( (xx_nn % dn )>0 )
		xx_div ++ ;
	if ( xx_div==0 )
		xx_div = 1 ;

	k = 0 ;
	vv = 0 ;
	for ( i=g_zmz_data[ii].i1; i<g_zmz_data[ii].i2; i++ )
	{
		vv += pcm_data[i] ;
		if ( (i % xx_div)==0 )
		{
			g_int_data[k] = (short)floor(vv/xx_div) ;
			k++ ;
			vv = 0 ;
		}
	}
	if ( (i % xx_div) !=0 )
	{
		g_int_data[k] = (short)floor(vv/xx_div) ;
		k++ ;
	}
	g_zmz_data[ii].xx_div = xx_div ;

	return ( set_zmz_data_2_ex( ii, g_int_data, k ) ) ;
}
//////////////////////////////////////////////////////////////////////
long make_zmz_data_2( short *pcm_data, long pcm_len )
{
	long	i, f, ii, ff ;
	double	x ;

	g_zmz_change = 0 ;
	g_zmz_0_pass = 0 ;
	ff = 0 ;
	ii = 0 ;
	f = pcm_data[0] ;
	for ( i=1; i<pcm_len; i++ )
	{
		if ( pcm_data[i]<0 ) // 如果当前值小于0
		{
			if ( f>=0 ) // 并且前一个值是大于等于0的
			{
				x = get_x0_from_xy1_and_xy2( i-1, f, i, pcm_data[i] ) ; // 计算过零点

				if ( ff==0 )
				{
					g_zmz_data[ii].i1 = i ;
					g_zmz_data[ii].x1 = x ;	// 如果是起始值，则只设置左边界
				}
				else if ( ff==1 )			// 如果前边有一个最大值的数据，则设置右边界，同时设置下一个的左边界
				{
					g_zmz_data[ii].i2 = i ;
					g_zmz_data[ii].x2 = x ;
					g_zmz_data[ii].ff = 1 ;

					ii = set_zmz_data_2( ii, pcm_data ) ;

					g_zmz_data[ii].i1 = i ;
					g_zmz_data[ii].x1 = x ;

					g_zmz_0_pass ++ ;

				}
				ff = -1 ;
			}
		}
		else if ( pcm_data[i]>=0 ) // 如果当前是大于等于0的
		{
			if ( f<0 ) // 如果前一个值是小于0的
			{
				x = get_x0_from_xy1_and_xy2( i-1, f, i, pcm_data[i] ) ; // 计算过零点
				if ( ff==0 )
				{
					g_zmz_data[ii].i1 = i ;
					g_zmz_data[ii].x1 = x ; // 如果是起始值，则只设置左边界
				}
				else if ( ff==-1 )			// 如果前边有一个最小值的数据，则设置右边界，同时设置下一个的左边界
				{
					g_zmz_data[ii].i2 = i ;
					g_zmz_data[ii].x2 = x ;
					g_zmz_data[ii].ff = -1 ;

					ii = set_zmz_data_2( ii, pcm_data ) ;

					g_zmz_data[ii].i1 = i ;
					g_zmz_data[ii].x1 = x ;

					g_zmz_0_pass ++ ;

				}
				ff = 1 ;
			}
		}
		f = pcm_data[i] ;
	}
	return ( ii ) ;
}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void create_freq_zmz_data( ZMZ_DATA *zmz_data, long zmz_nn, long vv_step, long pole_im )
{
	long	i, k, k0 ;

	long	ff, freq_ff, ff_nn, max_ii, max_vv, vv, vv_ratio ;


	
	// 1. freq_ff 清零
	for ( i=0; i<zmz_nn; i++ )
	{
		zmz_data[i].freq_ff = 0 ;
		zmz_data[i].freq_vv = 0 ;
		zmz_data[i].freq_nn = 0 ;
		zmz_data[i].freq_done = 0 ;
	}

	// 2. 获取绝对峰值的最大值
	max_vv = -10000000 ;
	for ( i=0; i<zmz_nn; i++ )
	{
		vv = abs(zmz_data[i].max_vv) ;
		if ( max_vv<vv )
			max_vv = vv ;
	}
	vv_ratio = max_vv/vv_step ;

	freq_ff = 1 ;
	for ( i=0; i<zmz_nn; i++ )
	{
		if ( zmz_data[i].freq_ff!=0 )
			continue ;

		ff = zmz_data[i].ff ;
		ff_nn = zmz_data[i].ff_nn ;
//		im = zmz_data[i].im ;
		max_ii = zmz_data[i].max_ii ;
		max_vv = zmz_data[i].max_vv ;
		
		for ( k=i; k<zmz_nn; k++ )
		{

			if ( zmz_data[k].freq_ff!=0 ) // 如果已经赋值了，则不再处理
				continue ;
			if ( zmz_data[k].ff!=ff ) // 如果极值正负值方向不同，则不做
				continue ;
			if ( zmz_data[k].ff_nn!=ff_nn ) // 如果极值变化个数不同，则不做
				continue ;
//			if ( abs(zmz_data[k].im-im)>pole_im ) // 如果极值偏移量之差过大，则不做
			if ( abs(zmz_data[k].max_ii-max_ii)>pole_im ) // 如果极值偏移量之差过大，则不做
				continue ;
			if ( fabs(zmz_data[k].max_vv-max_vv)>vv_ratio ) // 如果极值之差过大，则不做
				continue ;

			zmz_data[k].freq_ff = freq_ff ;
			zmz_data[k].freq_vv = max_vv ;

		}
		freq_ff ++ ;
	}

	// 计算相同freq_ff的点的距离
	for ( i=0; i<zmz_nn; i++ )
	{
		freq_ff = zmz_data[i].freq_ff ;
		k0 = i ;
		for ( k=i+1; k<zmz_nn; k++ )
		{
			if ( zmz_data[k].freq_ff==freq_ff )
			{
				zmz_data[k].freq_nn = zmz_data[k].i0-zmz_data[k0].i0 ;
//				zmz_data[k].ii = k0 ;
				zmz_data[k].freq_xx = zmz_data[k].xx-zmz_data[k0].xx ;
				k0 = k ;
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////
void print_freq_zmz_data( ZMZ_DATA *zmz_data, long zmz_nn, char *label_ss )
{
	long	i ;

	log_prt( g_logFile, "print_freq_zmz_data------------------------------%s\r\n", label_ss ) ;
	for ( i=0; i<zmz_nn; i++ )
	{
		if ( zmz_data[i].freq_ff==0 )
			continue ;

		log_prt( g_logFile, "i=%-5ld ff=%-3ld freq_vv=%-9ld max_vv=%-9ld freq_nn=%-5ld ff_nn=%-4ld i0=%-6ld i1=%-6ld i2=%-6ld xx_dif=%-6ld\r\n", 
						i, 
						zmz_data[i].freq_ff,
						zmz_data[i].freq_vv,
						zmz_data[i].max_vv,
						zmz_data[i].freq_nn,
						zmz_data[i].ff_nn,
						zmz_data[i].i0,
						zmz_data[i].i1,
						zmz_data[i].i2,
						zmz_data[i].i2-zmz_data[i].i1
				) ;
	}
}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void print_freq_data_2( FREQ_DATA *freq_data, long da_len, char *prt_title )
{
	long	i ;

	log_prt( g_logFile, "print_freq_data_2---------------------------------- tot=%ld title=%s\r\n", da_len, prt_title ) ;
	for ( i=0; i<da_len; i++ )
	{
		log_prt( g_logFile, "i=%-5ld ff=%-5ld nn=%-5ld vv=%-10.2lf avg=%-8.2lf flag=%-3ld\r\n",
						i, 
						freq_data[i].freq_ff,
						freq_data[i].freq_nn,
						freq_data[i].freq_vv,
						freq_data[i].freq_avg,
						freq_data[i].freq_flag
						) ;
	}
}
//////////////////////////////////////////////////////////////////////
long get_freq_data_from_zmz_data( ZMZ_DATA *zmz_data, long zmz_nn, long freq_nn_dif )
{

	long	i, k, ii, ff, is, ie, nn, vv, freq_nn ;

	// 1. 首先建立频率的基本统计数据，即对每一个相同峰值的个数进行计算。
	ii = 0 ;
	ff = 1 ;
	for ( i=0; i<zmz_nn; i++ )
	{
		if ( zmz_data[i].freq_nn==0 || zmz_data[i].freq_done!=0 )
			continue ;
		
		freq_nn = zmz_data[i].freq_nn ;
		is = i ;
		nn = 0 ;
		vv = 0 ;
		for ( k=i+1; k<zmz_nn; k++ )
		{
			if ( fabs(zmz_data[k].freq_nn-freq_nn)<=freq_nn_dif )
			{
				zmz_data[k].freq_done = 1 ;
				ie = k ;
				vv += zmz_data[k].freq_nn ;
				nn ++ ;
			}
		}

		if ( nn>0 )
		{
			g_freq_data[ii].freq_ff = (short)ff ;
			g_freq_data[ii].freq_vv = vv ;
			g_freq_data[ii].freq_nn = nn ;
			g_freq_data[ii].freq_avg = g_freq_data[ii].freq_vv/nn ;
			ii ++ ;
			ff++ ;
		}
	}
	print_freq_data_2( g_freq_data, ii, "filter0" ) ;

	return ( ii ) ;
}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
long filter_freq_data_by_freq_vv_old( FREQ_DATA *freq_data, long freq_nn )
{

	do_quickSort_freq_data_by_freq_vv( freq_data, 0, freq_nn-1 ) ;
	do_reverse_freq_data( freq_data, 0, freq_nn-1 ) ;

	print_freq_data_2( freq_data, freq_nn, "filter2-0" ) ;

	long	i, ii ;
	double	vv ;

	ii = 1 ;
	for ( i=1; i<freq_nn; i++ )
	{
		vv = fabs(freq_data[i].freq_vv-freq_data[i-1].freq_vv) ;
		if ( vv>freq_data[i-1].freq_vv/2 )
			break ;
		ii++ ;
	}
	freq_nn = ii ;
	print_freq_data_2( freq_data, freq_nn, "filter2-1" ) ;

	return ( freq_nn ) ;

}
//////////////////////////////////////////////////////////////////////
long filter_freq_data_by_freq_vv( FREQ_DATA *freq_data, long freq_nn, double ratio )
{

	long	i, ii ;
	double	vv_avg, vv_ratio ;

	vv_avg = 0 ;
	for ( i=0; i<freq_nn; i++ )
		vv_avg += freq_data[i].freq_vv ;

	vv_avg /= freq_nn ;
	vv_ratio = vv_avg*ratio ;

	ii = 0 ;
	for ( i=0; i<freq_nn; i++ )
	{
		if ( freq_data[i].freq_vv<vv_ratio )
			continue ;
		if ( i>ii )
			memcpy( &freq_data[ii], &freq_data[i], sizeof(FREQ_DATA) ) ;
		ii ++ ;
	}
	freq_nn = ii ;

	print_freq_data_2( freq_data, freq_nn, "filter3" ) ;

	return ( freq_nn ) ;

}
//////////////////////////////////////////////////////////////////////
long filter_freq_data_by_freq_avg( FREQ_DATA *freq_data, long freq_nn, double ratio )
{

	long	i, ii ;
	double	vv_avg, vv_ratio ;

	vv_avg = 0 ;
	for ( i=0; i<freq_nn; i++ )
		vv_avg += freq_data[i].freq_avg ;

	vv_avg /= freq_nn ;
	vv_ratio = vv_avg * ratio ;

	ii = 0 ;
	for ( i=0; i<freq_nn; i++ )
	{
		if ( freq_data[i].freq_avg<vv_ratio )
			continue ;
		if ( i>ii )
			memcpy( &freq_data[ii], &freq_data[i], sizeof(FREQ_DATA) ) ;
		ii ++ ;
	}
	freq_nn = ii ;

	print_freq_data_2( freq_data, freq_nn, "filter4" ) ;

	return ( freq_nn ) ;

}
//////////////////////////////////////////////////////////////////////
long filter_freq_data_by_near_same_freq_avg( FREQ_DATA *freq_data, long freq_nn, double min_dif )
{

	long	i, k, ii ;

	for ( i=0; i<freq_nn; i++ )
		freq_data[i].near_same = 0 ;

	ii = 0 ;
	for ( i=0; i<freq_nn; i++ )
	{
		for ( k=i+1; k<freq_nn; k++ )
		{
			if ( fabs(freq_data[i].freq_avg-freq_data[k].freq_avg)<=min_dif )
			{
				if ( freq_data[i].near_same==0 )
					freq_data[i].near_same = 1 ;
				if ( freq_data[k].near_same==0 )
					freq_data[k].near_same = 1 ;
				ii++ ;
			}
		}
	}

	if ( ii>0 )
	{
		ii = 0 ;
		for ( i=0; i<freq_nn; i++ )
		{
			if ( freq_data[i].near_same==0 )
				continue ;
			if ( i>ii )
				memcpy( &freq_data[ii], &freq_data[i], sizeof(FREQ_DATA) ) ;
			ii ++ ;
		}
		freq_nn = ii ;
		print_freq_data_2( freq_data, freq_nn, "filter5" ) ;
		return ( freq_nn ) ;
	}
	else
		return ( 0 ) ;

}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
long filter_freq_data_by_freq_nn_avg( FREQ_DATA *freq_data, long freq_nn, double ratio )
{

	long	i, ii, nn, nn_max ;

	nn_max = -100000 ;
	for ( i=0; i<freq_nn; i++ )
	{
		nn = freq_data[i].freq_nn ;
		if ( nn_max<nn )
			nn_max = nn ;
	}
	ratio *= nn_max ;

	nn_max = (long)floor(ratio) ;

	ii = 0 ;
	for ( i=0; i<freq_nn; i++ )
	{
		if ( freq_data[i].freq_nn<nn_max )
			continue ;
		if ( i>ii )
			memcpy( &freq_data[ii], &freq_data[i], sizeof(FREQ_DATA) ) ;
		ii ++ ;
	}
	freq_nn = ii ;

	print_freq_data_2( freq_data, freq_nn, "filter1" ) ;

	return ( freq_nn ) ;

}
//////////////////////////////////////////////////////////////////////
double get_freq_nn_from_freq_data( FREQ_DATA *freq_data, long freq_nn, double ratio, long over_nn )
{

//	if ( freq_nn>1 )
//		freq_nn = filter_freq_data_by_freq_nn_avg( freq_data, freq_nn, ratio*2 ) ;

	if ( freq_nn>1 )
		freq_nn = filter_freq_data_by_freq_nn_avg( freq_data, freq_nn, ratio*2 ) ;
	if ( freq_nn>1 )
		freq_nn = filter_freq_data_by_freq_vv( freq_data, freq_nn, ratio ) ;
	if ( freq_nn>1 )
		freq_nn = filter_freq_data_by_freq_avg( freq_data, freq_nn, ratio ) ;

	if ( freq_nn<=0 )
	{
		g_test_data.filter_problem_1 ++ ;
		g_test_data.tot_filter_problem_1 ++ ;
		log_prt( g_logFile, "********* filter_problem_1=%ld %s\r\n", g_test_data.filter_problem_1, g_test_data.music_note ) ;
		return ( 0 ) ;
	}

	g_note_pcm_ok2++ ;

	if ( freq_nn>over_nn )
	{
		freq_nn = filter_freq_data_by_near_same_freq_avg( freq_data, freq_nn, 2.0 ) ;
		if ( freq_nn==0 )
		{
			g_test_data.filter_problem_2 ++ ;
			g_test_data.tot_filter_problem_2 ++ ;
			log_prt( g_logFile, "********* filter_problem_2=%ld %s\r\n", g_test_data.filter_problem_2, g_test_data.music_note ) ;
			return ( 0 ) ;
		}
	}

	g_note_pcm_ok3++ ;

	long	i ;

/**
	long	max_nn ;
	double	freq_avg ;

	max_nn = -10000000 ;
	for ( i=0; i<freq_nn; i++ )
	{
		if ( max_nn<freq_data[i].freq_nn )
		{
			max_nn = freq_data[i].freq_nn ;
			freq_avg = freq_data[i].freq_avg ;
		}
	}
	return ( freq_avg ) ;
**/

	double	vv ;
	vv = 0 ;
	for ( i=0; i<freq_nn; i++ )
		vv += freq_data[i].freq_avg ;
	vv /= freq_nn ;
	return ( vv ) ;


}
//////////////////////////////////////////////////////////////////////
bool check_freq_data_mean( FREQ_DATA *freq_data, long freq_data_nn, double min_diff )
{
	bool	bf ;
	long	i, mean_nn ;
	double	mean_vv ;

	mean_vv = 0 ;
	mean_nn = 0 ;
	for ( i=0; i<freq_data_nn; i++ )
	{
		mean_vv += fabs(freq_data[i].freq_avg-g_freq_avg_vv)*freq_data[i].freq_nn ; 
		mean_nn += freq_data[i].freq_nn ;
	}
	mean_vv /= mean_nn ;

log_prt( g_logFile, "check_freq_data_mean() ********************** mean_vv=%-8.2lf %ld*min_diff=%-8.2lf mean_nn=%-7ld", mean_vv, MIN_AMP_MEAN, min_diff*MIN_AMP_MEAN, mean_nn ) ;

	if ( mean_vv>MIN_AMP_MEAN*min_diff )
		bf = false ;
	else
		bf = true ;
if ( bf==false )
	log_prt( g_logFile, " ################# false\r\n" ) ;
else
	log_prt( g_logFile, "\r\n" ) ;
	return ( bf ) ;
}
//////////////////////////////////////////////////////////////////////
bool check_freq_data_flag( FREQ_DATA *freq_data, long freq_data_nn )
{
	bool	bf ;
	long	i ;
	double	nn1, nn0, vv ;

	nn1 = 0 ;
	nn0 = 0 ;
	for ( i=0; i<freq_data_nn; i++ )
	{
		if ( freq_data[i].freq_flag>0 )
			nn1 += freq_data[i].freq_nn ;
		else
			nn0 += freq_data[i].freq_nn ;
	}
	if ( nn0>0 )
		vv = nn1/nn0 ;
	else
		vv = 100 ;
	if ( vv>1 )
		bf = true ;
	else
		bf = false ;

log_prt( g_logFile, "check_freq_data_flag() ********************** nn1=%-6.0lf nn0=%-6.0lf", nn1, nn0 ) ;

if ( bf==false )
	log_prt( g_logFile, " ################# false\r\n" ) ;
else
	log_prt( g_logFile, "\r\n" ) ;

	return ( bf ) ;
}
//////////////////////////////////////////////////////////////////////
double get_freq_nn_from_freq_data_new( FREQ_DATA *freq_data, long freq_data_nn, double min_ratio )
{
	bool	bf1, bf2 ;
	long	i, nn1, nn0 ;
	double	min_diff, vv1 ;

	min_diff = min_ratio * g_freq_avg_vv ;

	bf1 = bf2 = true ;

	bf1 = check_freq_data_mean( freq_data, freq_data_nn, min_diff ) ;
	bf2 = check_freq_data_flag( freq_data, freq_data_nn ) ;

	if ( bf1==false || bf2==false )
	{
log_prt( g_logFile, "#################***FALSE\r\n" ) ;
		return ( 0 ) ;
	}

	vv1 = 0 ;
	nn1 = 0 ;
	nn0 = 0 ;
	for ( i=0; i<freq_data_nn; i++ )
	{
		if ( freq_data[i].freq_flag>0 )
		{
			vv1 += freq_data[i].freq_avg*freq_data[i].freq_nn ;
			nn1 += freq_data[i].freq_nn ;
		}
		else
			nn0 += freq_data[i].freq_nn ;
	}

	if ( nn0>0 )
	{
		if ( nn0/nn1>0.5 )
			return ( 0 ) ;
	}

	if ( nn1==0 ) 
		return ( 0 ) ;

	vv1 /= nn1 ;
	return ( vv1 ) ;
}
//////////////////////////////////////////////////////////////////////
void adjust_freq_data_proc_ex( FREQ_DATA *freq_data, double freq_avg_vv, double min_diff, long mm )
{
	for ( ;; )
	{
		if ( mm<2 )
			break ;
		if ( fabs(freq_data->freq_avg/mm-freq_avg_vv)<=min_diff )
		{
			freq_data->freq_avg /= mm ;
			freq_data->freq_nn *= mm ;
		}
		mm -- ;
	}

}
//////////////////////////////////////////////////////////////////////
void adjust_freq_data_proc_1( FREQ_DATA *freq_data, long freq_data_nn, double min_rate )
{
	long	i, ii, freq_nn_max, freq_avg_nn ;
	double	min_diff, freq_avg_vv ;

	freq_nn_max = 0 ;
	ii = 0 ;
	for ( i=0; i<freq_data_nn; i++ )
	{
		freq_data[i].near_same = 0 ;
		if ( freq_nn_max<freq_data[i].freq_nn )
		{
			freq_nn_max = freq_data[i].freq_nn ;
			ii = i ;
		}
	}

	min_diff = min_rate * freq_data[ii].freq_avg ;

	freq_avg_vv = 0 ;
	freq_avg_nn = 0 ;
	for ( i=0; i<freq_data_nn; i++ )
	{
		if ( fabs(freq_data[i].freq_avg-freq_data[ii].freq_avg)<=min_diff )
		{
			freq_avg_vv += freq_data[i].freq_avg * freq_data[i].freq_nn ;
			freq_avg_nn += freq_data[i].freq_nn ; 
		}
	}
	freq_avg_vv /= freq_avg_nn ;

	for ( i=0; i<freq_data_nn; i++ )
	{
		if ( fabs(freq_data[i].freq_avg-freq_avg_vv)<=min_diff )
			continue ;

		adjust_freq_data_proc_ex( &freq_data[i], freq_avg_vv, min_diff, 3 ) ;

	}

	freq_avg_vv = 0 ;
	freq_avg_nn = 0 ;
	for ( i=0; i<freq_data_nn; i++ )
	{
		if ( fabs(freq_data[i].freq_avg-freq_data[ii].freq_avg)<=min_diff )
		{
			freq_avg_vv += freq_data[i].freq_avg * freq_data[i].freq_nn ;
			freq_avg_nn += freq_data[i].freq_nn ; 
			freq_data[i].freq_flag = 1 ; 
		}
		else
			freq_data[i].freq_flag = 0 ; 
	}
	freq_avg_vv /= freq_avg_nn ;

	g_freq_avg_vv = freq_avg_vv ;

	print_freq_data_2( freq_data, freq_data_nn, "filter00" ) ;


}
//////////////////////////////////////////////////////////////////////
void print_tj_data()
{
	log_prt( g_tjFile, "print_tj_data==================*****=tj_data_nn=%-6ld freq_difs=%1.2lf %s\r\n", 
										g_tj_data_nn,
										g_test_data.note_freq_dif,
										g_test_data.music_note
							) ;

	long	i, dif_nn ;
	double	dif_avg, dif_dif, dif_vv, dif_tot, vv ;

	dif_avg = 0 ;
	dif_dif = 0 ;
	dif_tot = 0 ;
	dif_nn = 0 ;

	if ( g_tj_data_nn>0 )
	{
		// 计算平均值
		for ( i=0; i<g_tj_data_nn; i++ )
			dif_avg += fabs(g_tj_data[i].tj_freq_dif) ;
		dif_avg /= g_tj_data_nn ;

		// 计算均方差，即设法得到离散性
		for ( i=0; i<g_tj_data_nn; i++ )
		{

			if ( i==0 )
				dif_vv = 0 ;
			else 
				dif_vv = fabs(g_tj_data[i].tj_freq_dif-g_tj_data[i-1].tj_freq_dif) ;

			if ( dif_vv>0.00001 )
				dif_nn ++ ;

			dif_tot += dif_vv ;

			vv = fabs(g_tj_data[i].tj_freq_dif-dif_avg) ;
			dif_dif += vv ;

			log_prt( g_tjFile, "ii=%-5ld dif=%-8.2lf dif_dif=%-8.2lf dif_vv=%-8.2lf\r\n", 
								i,
								g_tj_data[i].tj_freq_dif,
								vv, 
								dif_vv ) ;
		}
	}
	log_prt( g_tjFile, "dif_avg=%-8.2lf dif_dif=%-8.2lf dif_tot=%-8.2lf dif_nn=%-5ld\r\n\r\n", 
										dif_avg,
										dif_dif, 
										dif_tot,
										dif_nn
							) ;

	g_test_data.freq_dif_change_nn_tot += dif_nn ;
	g_test_data.freq_dif_change_vv_tot += dif_tot ;

}
//////////////////////////////////////////////////////////////////////
double freq_tj_data_proc()
{
	long	i ;

	double	vv ;


	g_freq_dif = 0 ;
	for ( i=0; i<g_freq_tj_nn; i++ )
	{
		vv = g_freq_tj[i].freq_vv-g_freq_avg ;
		g_freq_tj[i].freq_dif = vv ;
		g_freq_dif += fabs(vv) ;
	}
	g_freq_dif /= g_freq_tj_nn ;
	for ( i=0; i<g_freq_tj_nn; i++ )
	{
		if ( fabs(g_freq_tj[i].freq_dif)>=g_freq_dif )
			g_freq_tj[i].ok_flag = 0 ;
		else
			g_freq_tj[i].ok_flag = 1 ;
	}
	g_freq_vv = 0 ;
	g_freq_nn = 0 ;
	for ( i=0; i<g_freq_tj_nn; i++ )
	{
		if ( g_freq_tj[i].ok_flag==1 )
		{
			g_freq_vv += g_freq_tj[i].freq_vv ;
			g_freq_nn ++ ;
		}
	}
	if ( g_freq_nn==0 )
		return ( 0 ) ;
	g_freq_vv /= g_freq_nn ;
	return ( g_freq_vv ) ;
}
//////////////////////////////////////////////////////////////////////
void print_freq_tj_data()
{
	long	i ;
	char	ss[100] ;

	freq_tj_data_proc() ;

	sprintf( ss, "%ld-%ld", g_freq_tj_nn, g_pcm_data_cn ) ;
	log_prt( g_logFile, "print_freq_tj_data================== freq=%s tj_nn=%-20s nn=%-5ld avg=%-8.2lf dif=%-8.2lf vv=%-8.2lf nn=%-5ld\r\n", 
											g_test_data.music_note, ss,
											g_freq_tj_nn, g_freq_avg, g_freq_dif, g_freq_vv, g_freq_nn ) ;
	for ( i=0; i<g_freq_tj_nn; i++ )
	{
		log_prt( g_logFile, "freq_nn=%-8.2lf freq_vv=%-10.2lf dif=%-8.2lf ok_flag=%-5ld\r\n", g_freq_tj[i].freq_nn, g_freq_tj[i].freq_vv, g_freq_tj[i].freq_dif, g_freq_tj[i].ok_flag ) ;
	}

	log_prt( g_logFile, "\r\n\r\n" ) ;
}
//////////////////////////////////////////////////////////////////////
void print_ok_data()
{
	log_prt( g_logFile, "print_ok_data==================*****=pcm_nn=%-5ld pcm_do=%-5ld pcm_ok1=%-5ld pcm_ok2=%-5ld pcm_ok3=%-5ld %s\r\n", 
									g_note_pcm_nn, 
									g_note_pcm_do, 
									g_note_pcm_ok1, 
									g_note_pcm_ok2, 
									g_note_pcm_ok3,
									g_test_data.music_note
							) ;
}
//////////////////////////////////////////////////////////////////////
void print_filter_data()
{
	log_prt( g_logFile, "print_filter_data==================*****=filter_problem_0=%-5ld problem_1=%-5ld problem_2=%-5ld %s\r\n", 
									g_test_data.filter_problem_0, 
									g_test_data.filter_problem_1, 
									g_test_data.filter_problem_2, 
									g_test_data.music_note
							) ;
}
//////////////////////////////////////////////////////////////////////
void print_filter_data2()
{
	log_prt( g_logFile, "print_filter_data2==================*****=filter_problem_0=%-5ld problem_1=%-5ld problem_2=%-5ld\r\n", 
									g_test_data.tot_filter_problem_0, 
									g_test_data.tot_filter_problem_1, 
									g_test_data.tot_filter_problem_2 
							) ;
}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
double redo_freq_nn_from_zmz_data( ZMZ_DATA *zmz_data, long zmz_nn, double freq_nn, double ratio_nn )
{

	long	i, nn ;
	double	freq_vv, vv ;

	freq_vv = 0 ;
	nn = 0 ;
	for ( i=0; i<zmz_nn; i++ )
	{
		vv = fabs( freq_nn-zmz_data[i].freq_nn ) ;
		if ( vv<=ratio_nn )
		{
			freq_vv += zmz_data[i].freq_nn ;
			nn ++ ;
		}
	}
	if ( nn>0 )
		freq_vv /= nn ;
	return ( freq_vv ) ;
}
//////////////////////////////////////////////////////////////////////
double redo_freq_nn_from_zmz_data2( ZMZ_DATA *zmz_data, long zmz_nn, double freq_nn, double ratio_nn )
{

	long	i, nn ;
	double	freq_vv, vv ;

	freq_vv = 0 ;
	nn = 0 ;
	for ( i=0; i<zmz_nn; i++ )
	{
		vv = fabs( freq_nn-zmz_data[i].freq_nn ) ;
		if ( vv<=ratio_nn )
		{
			freq_vv += zmz_data[i].freq_xx ;
			nn ++ ;
		}
	}
	g_zmz_freq = nn ;
	if ( nn>0 )
		freq_vv /= nn ;
	return ( freq_vv ) ;
}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
bool check_pcm_data_ok( short *pcm_data, long pcm_nn, long mm )
{
	long	i, ii, nn ;
	double	vv, vv_tot, vv_avg, vv1, vv2 ;

	nn = pcm_nn / mm ;

	vv_avg = 0 ;

	ii = 0 ;
	vv_tot = 0 ;
	for ( i=0; i<pcm_nn; i++ )
	{
		vv = pcm_data[i] ;
		vv_tot += fabs(vv) ;
		if ( i>0 && i%nn==0 )
		{
			vv_avg += fabs(vv_tot) ;
			g_pcm_check_data[ii] = vv_tot ;
			ii ++ ;
			vv_tot = 0 ;
		}
	}
	nn = ii ;
	vv_avg /= nn ;
	vv1 = 0 ;
	for ( i=0; i<nn/2; i++ )
	{
		vv1 += fabs(g_pcm_check_data[i]-vv_avg) ;
	}
	vv1 /= nn/2 ;

	vv2 = 0 ;
	for ( i=nn/2; i<nn; i++ )
	{
		vv2 += fabs(g_pcm_check_data[i]-vv_avg) ;
	}
	vv2 /= nn/2 ;

	if ( fabs(vv1-vv2)>vv_avg/2 )
		return ( false ) ;
	else
		return ( true ) ;
}
//////////////////////////////////////////////////////////////////////
void show_WaveForm_proc_2_ex( short *pcm_data, long i1, long i2, long xxx, long aa, long data_color )
{
	long	i, ii, x, y, x0, y0, min_y, max_y ;
	double	xx, dx ;

	y0 = g_waveForm_bb / 2 ;

	dx = aa ;
	dx /= (i2-i1+1) ;

	if ( dx<1 )
	{
		min_y = 1000000 ;
		max_y = -1000000 ;
	}

	ii = 0 ;
	x0 = 0 ;
	xx = 0 ;
	x = 0 ;
	g_xy_data[ii].x = 0 ;

	for ( i=i1; i<=i2; i++ )
	{
		y = pcm_data[i] ;

		if ( dx<1 )
		{
			if ( min_y>y )
				min_y = y ;
			if ( max_y<y )
				max_y = y ;

			xx += dx ;
			x = (long)floor(xx) ;
			if ( x>x0 )
			{
				g_xy_data[ii].x = x0 + xxx ;
				g_xy_data[ii].y = get_waveForm_y( min_y, y0 ) ;
				ii++ ;

				g_xy_data[ii].x = x0 + xxx ;
				g_xy_data[ii].y = get_waveForm_y( max_y, y0 ) ;
				ii++ ;

				min_y = 1000000 ;
				max_y = -1000000 ;

				x0 = x ;

			}
		}
		else
		{
			g_xy_data[ii].x = x + xxx ;
			g_xy_data[ii].y = get_waveForm_y( y, y0 ) ;
			ii ++ ;
			xx += dx ;
			x = (long)floor(xx) ;
		}
	}
	if ( dx<1 )
	{
		if ( min_y == 1000000 )
			min_y = 0 ;
		if ( max_y == -1000000 )
			max_y = 0 ;

		g_xy_data[ii].x = x0 + xxx ;
		g_xy_data[ii].y = get_waveForm_y( min_y, y0 ) ;
		ii++ ;
		g_xy_data[ii].x = x0 + xxx ;
		g_xy_data[ii].y = get_waveForm_y( max_y, y0 ) ;
		ii++ ;
	}


	PolyLine( g_waveForm_DC, g_xy_data, ii, 1, data_color, PS_SOLID, R2_COPYPEN ) ;

	DrawLine( g_waveForm_DC, 0, y0, g_waveForm_aa-1, y0, 1, RGB(125,0,0), PS_SOLID, R2_COPYPEN ) ;


}
//////////////////////////////////////////////////////////////////////
long get_pcm_show_i1_i2_proc( long ff, long xx, long aa, long *ii1, long *ii2 )
{
	long	i, i1, i2, x1, x2 ;

	i1 = 0 ;
	i2 = 0 ;

	if ( ff==1 ) // 正向查找
	{
		// 查找起始波形
		for ( i=0; i<g_zmz_nn; i++ )
		{

			// 小于起始点，则继续
			if ( g_zmz_data[i].x1<xx )
				continue ;

			// 判断是否为正半弦
			if ( g_zmz_data[i].freq_vv>0 )
			{
				i1 = g_zmz_data[i].i1 ;
				x1 = (long)floor(g_zmz_data[i].x1) ;
				break ;
			}
		}
		// 查找结束波形
		for ( ; i<g_zmz_nn; i++ )
		{
			// 小于结束点，则继续
			if ( g_zmz_data[i].x1-x1<aa )
				continue ;
			// 判断是否为正半弦
			if ( g_zmz_data[i].freq_vv>0 )
			{
				i2 = g_zmz_data[i-1].i2-1 ; // 正半弦向后退一个点，正好是一个负半弦
				break ;
			}
		}
		if ( i==g_zmz_nn )
			i2 = g_zmz_data[i-1].i2-1 ;
	}
	else if ( ff==-1 ) // 反向查找
	{
		// 从后向前查找一个负半弦（结束波形）
		for ( i=g_zmz_nn-1; i>=0; i-- )
		{
			if ( g_zmz_data[i].x2>xx+aa )
				continue ;
			if ( g_zmz_data[i].freq_vv<0 )
			{
				i2 = g_zmz_data[i].i2-1 ;
				x2 = (long)floor(g_zmz_data[i].x2) ;
				break ;
			}
		}
		// 继续从后向前查找到起始波形（起始波形）
		for ( ; i>=0; i-- )
		{
			// 大于起始点，则继续
			if ( g_zmz_data[i].x2>xx )
				continue ;

			// 判断是否为负半弦
			if ( g_zmz_data[i].freq_vv<0 )
			{
				i1 = g_zmz_data[i+1].i1 ;
				break ;
			}
		}
		if ( i==0 )
			i1 = 0 ;
	}
	*ii1 = i1 ;
	*ii2 = i2 ;
	aa = i2-i1 ;
	return ( aa ) ;
}
//////////////////////////////////////////////////////////////////////
void show_ellipsis( long x1, long x2, long dot_color, long bg_color )
{

	long	y0, dx, dy ;
	y0 = g_waveForm_bb / 2 ;

	dx = 20 ;
	dy = 10 ;
	if ( bg_color!=0 )
		DrawBar( g_waveForm_DC, x1, y0-dy, x2, y0+dy, bg_color, PS_SOLID, R2_XORPEN ) ;

	DrawLine( g_waveForm_DC, x1+dx, y0, x2-dx, y0, 5, dot_color, PS_DOT, R2_COPYPEN ) ;

}
//////////////////////////////////////////////////////////////////////
long make_WaveForm_data( short *pcm_data, long i0, long i1, long i2, long xxx, long aa )
{
	long	i, ii, x, y, x0, y0, min_y, max_y ;
	double	xx, dx ;

	y0 = g_waveForm_bb / 2 ;

	dx = aa ;
	dx /= (i2-i1+1) ;

	if ( dx<1 )
	{
		min_y = 1000000 ;
		max_y = -1000000 ;
	}

	ii = i0 ;
	x0 = 0 ;
	xx = 0 ;
	x = 0 ;
	g_xy_data[ii].x = 0 ;

	for ( i=i1; i<=i2; i++ )
	{
		if ( i==i1 )
			y = 0 ;
		else
			y = pcm_data[i] ;

		if ( dx<1 )
		{
			if ( min_y>y )
				min_y = y ;
			if ( max_y<y )
				max_y = y ;

			xx += dx ;
			x = (long)floor(xx) ;
			if ( x>x0 )
			{
				g_waveForm_data[ii].x = x0 + xxx ;
				g_waveForm_data[ii].y = get_waveForm_y( min_y, y0 ) ;
				ii++ ;

				g_waveForm_data[ii].x = x0 + xxx ;
				g_waveForm_data[ii].y = get_waveForm_y( max_y, y0 ) ;
				ii++ ;

				min_y = 1000000 ;
				max_y = -1000000 ;

				x0 = x ;

			}
		}
		else
		{
			g_waveForm_data[ii].x = x + xxx ;
			g_waveForm_data[ii].y = get_waveForm_y( y, y0 ) ;
			ii ++ ;
			xx += dx ;
			x = (long)floor(xx) ;
		}
	}
	if ( dx<1 )
	{
		if ( min_y == 1000000 )
			min_y = 0 ;
		if ( max_y == -1000000 )
			max_y = 0 ;

		g_waveForm_data[ii].x = x0 + xxx ;
		g_waveForm_data[ii].y = get_waveForm_y( min_y, y0 ) ;
		ii++ ;
		g_waveForm_data[ii].x = x0 + xxx ;
		g_waveForm_data[ii].y = get_waveForm_y( max_y, y0 ) ;
		ii++ ;
	}

	return ( ii ) ;

}
//////////////////////////////////////////////////////////////////////
void waveForm_reshow( long dot_show_flag )
{

	if ( g_waveForm_redraw>0 )
	{
		make_waveForm() ;
		g_waveForm_redraw = 0 ;
		g_waveForm_refresh = 1 ;
	}
	if ( g_waveForm_refresh==0 )
		return ;

	clear_wave_area() ;
	g_waveForm_refresh = 0 ;

	if ( g_waveForm_xy_nn>0 )
	{
		show_waveForm_xy( dot_show_flag, g_waveForm_xy, g_waveForm_xy_nn, 1, RGB(0,120,0), RGB(120,0,0) ) ; 

		if ( g_waveForm_xy_ca>0 )
			show_ellipsis( g_waveForm_xy_ca, g_waveForm_aa-g_waveForm_xy_ca, RGB(0,220,0), RGB(80,80,80) ) ;

	}
}
//////////////////////////////////////////////////////////////////////
void waveForm_refresh()
{
	do_task( 106, "1" ) ;
}
//////////////////////////////////////////////////////////////////////
long make_WaveForm_xy( short *pcm_data, long i0, long i1, long i2, long xxx, long aaa )
{
	long	i, ii, x, y, x0, y0, min_y, max_y, a_max ;
	double	xx, dx ;

	y0 = g_waveForm_bb / 2 ;

	dx = aaa ;
	dx /= (i2-i1+1) ;

	a_max = 100000000 ;

	if ( dx<1 )
	{
		min_y = a_max ;
		max_y = -a_max ;
	}

	ii = i0 ;
	x0 = 0 ;
	xx = 0 ;
	x = 0 ;
	g_xy_data[ii].x = 0 ;

	for ( i=i1; i<=i2; i++ )
	{
		y = pcm_data[i] ;

		if ( dx<1 )
		{
			if ( min_y>y )
				min_y = y ;
			if ( max_y<y )
				max_y = y ;

			xx = (i-i1)*dx ;
			x = (long)floor(xx) ;

			if ( x>x0 )
			{
				if ( ii==i0 )
				{
					g_waveForm_xy[ii].x = x0 + xxx ;
					g_waveForm_xy[ii].y = get_waveForm_y( 0, y0 ) ;
					ii++ ;
					x0 ++ ;
					g_waveForm_xy[ii].x = x0 + xxx ;
					g_waveForm_xy[ii].y = get_waveForm_y( y, y0 ) ;
					ii++ ;
				}
				else
				{
					if ( min_y>0 && max_y>0 )
					{
						g_waveForm_xy[ii].x = x0 + xxx ;
						g_waveForm_xy[ii].y = get_waveForm_y( max_y, y0 ) ;
						ii++ ;
					}
					else if ( min_y<0 && max_y<0 )
					{
						g_waveForm_xy[ii].x = x0 + xxx ;
						g_waveForm_xy[ii].y = get_waveForm_y( min_y, y0 ) ;
						ii++ ;
					}
					else
					{
						g_waveForm_xy[ii].x = x0 + xxx ;
						g_waveForm_xy[ii].y = get_waveForm_y( min_y, y0 ) ;
						ii++ ;

						g_waveForm_xy[ii].x = x0 + xxx ;
						g_waveForm_xy[ii].y = get_waveForm_y( max_y, y0 ) ;
						ii++ ;
					}
				}
				min_y = a_max ;
				max_y = -a_max ;

				x0 = x ;

			}
		}
		else
		{
			if ( ii==i0 )
			{
				g_waveForm_xy[ii].x = x0 + xxx ;
				g_waveForm_xy[ii].y = get_waveForm_y( y, y0 ) ;
			}
			else
			{
				g_waveForm_xy[ii].x = x0 + xxx ;
				g_waveForm_xy[ii].y = get_waveForm_y( y, y0 ) ;
			}
			ii ++ ;
			xx =(i-i0)*dx ;
			x0 = (long)floor(xx) ;
		}
	}
	if ( dx<1 )
	{
		if ( min_y != a_max && max_y != -a_max )
		{
			g_waveForm_xy[ii].x = x0 + xxx ;
			g_waveForm_xy[ii].y = get_waveForm_y( min_y, y0 ) ;
			ii++ ;
			g_waveForm_xy[ii].x = x0 + xxx ;
			g_waveForm_xy[ii].y = get_waveForm_y( max_y, y0 ) ;
			ii++ ;
		}
	}
	g_waveForm_xy[ii].x = x0 + xxx ;
	g_waveForm_xy[ii].y = get_waveForm_y( 0, y0 ) ; ;
	ii++ ;

	return ( ii ) ;

}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
double get_freq_avg( double freq_nn )
{
	double	freq ;

	freq = g_Samples /freq_nn ;

	g_freq_tj[g_freq_tj_nn].freq_nn = freq_nn ;
	g_freq_tj[g_freq_tj_nn].freq_vv = freq ;
	g_freq_tj_nn ++ ;

	g_freq_tot += freq ;

	g_freq_avg = g_freq_tot / g_freq_tj_nn ;

	if ( g_freq_tj_nn>3 )
		freq = freq_tj_data_proc() ;
	else
		freq = g_freq_avg ;


	char	ss[100] ;
	sprintf( ss, "%0.2lf", freq ) ;
	sscanf( ss, "%lf", &freq ) ;

	return ( freq ) ;

}
//////////////////////////////////////////////////////////////////////
void push_pcm_into_all_pcm( short *pcm_data, long pcm_bytes )
{
	if ( g_all_pcm_max>0 )
	{
		// 如果新来的PCM数据超出缓冲区大小，则将缓冲区向前移动出pcm_bytes的空间
		if ( g_all_pcm_pos>=g_all_pcm_max )
		{
			g_all_pcm_pos -= pcm_bytes ;
			memmove( g_all_pcm, g_all_pcm+pcm_bytes, g_all_pcm_pos ) ; 
		}
		// 将新的pcm数据拷贝到缓冲区的尾部
		memcpy( g_all_pcm+g_all_pcm_pos, pcm_data, pcm_bytes ) ;
		g_all_pcm_pos += pcm_bytes ;
	}
}
//////////////////////////////////////////////////////////////////////
void set_waveForm_pcm( short *pcm_data, long pcm_len )
{
	memcpy( g_waveForm_pcm, pcm_data, pcm_len*2 ) ;
	g_waveForm_pcm_len = pcm_len ;
	g_waveForm_redraw = 1 ;

	push_pcm_into_all_pcm( pcm_data, pcm_len*2 ) ;

}
//////////////////////////////////////////////////////////////////////
void make_WaveForm_draw_data( short *pcm_data, long pcm_len )
{
	if ( pcm_len<g_waveForm_aa )
	{
		g_waveForm_data_nn = make_WaveForm_xy( pcm_data, 0, 0, pcm_len-1, 0, g_waveForm_aa ) ;
		g_waveForm_xy_x1 = g_waveForm_xy_x2 = 0 ;
	}
	else
	{

		long	aa, aaa, i1, i2, da, xy_nn ;

		aaa = g_waveForm_aa / 2 ; // 从波形显示的中间位置确定

		da = 80 ; // 显示省略符的宽度

		// 计算左边显示的数据（0-起始位置，aaa-da=右边界位置）
		aa = get_pcm_show_i1_i2_proc( 1, 0, aaa-da, &i1, &i2 ) ;
		xy_nn = make_WaveForm_xy( pcm_data, 0, i1, i2, 0, aa ) ;

log_prt( g_logFile, "1-------------------i1=%ld i2=%ld aa=%ld xy_nn=%ld g_zmz_nn=%ld\r\n", i1, i2, aa, xy_nn, g_zmz_nn ) ;

		g_waveForm_xy_x1 = aa ;

		// 计算右边显示的数据（aaa+da=起始位置，g_waveForm_aa=右边界位置）
		aa = get_pcm_show_i1_i2_proc( -1, g_waveForm_aa-aa, aa, &i1, &i2 ) ;
		xy_nn = make_WaveForm_xy( pcm_data, xy_nn, i1, i2, g_waveForm_aa-aa, aa ) ;

log_prt( g_logFile, "2-------------------i1=%ld i2=%ld aa=%ld xy_nn=%ld g_zmz_nn=%ld\r\n", i1, i2, aa, xy_nn, g_zmz_nn ) ;

		g_waveForm_xy_nn = xy_nn ;
//		g_waveForm_xy_ca = aa ;

		g_waveForm_xy_x2 = g_waveForm_aa-aa ;


	}

}
//////////////////////////////////////////////////////////////////////
void make_waveForm()
{
	if ( g_waveForm_pcm_len>0 )
		make_WaveForm_draw_data( g_waveForm_pcm, g_waveForm_pcm_len ) ; // 建立显示波形
}
//////////////////////////////////////////////////////////////////////
void create_freq_zmz_data_2( ZMZ_DATA *zmz_data, long zmz_nn, long vv_step, long pole_im )
{
	long	i, k, k0 ;

	long	ff, freq_ff, ff_nn, max_ii, max_vv, vv, vv_ratio ;


	
	// 1. freq_f2 清零
	for ( i=0; i<zmz_nn; i++ )
	{
		zmz_data[i].freq_f2 = 0 ;
		zmz_data[i].freq_v2 = 0 ;
		zmz_data[i].freq_n2 = 0 ;
		zmz_data[i].freq_d2 = 0 ;
	}

	// 2. 获取绝对峰值的最大值
	max_vv = -10000000 ;
	for ( i=0; i<zmz_nn; i++ )
	{
		vv = abs(zmz_data[i].max_vv) ;
		if ( max_vv<vv )
			max_vv = vv ;
	}
	vv_ratio = max_vv/vv_step ;

	freq_ff = 1 ;
	for ( i=0; i<zmz_nn; i++ )
	{
		if ( zmz_data[i].freq_ff!=0 )
			continue ;

		ff = zmz_data[i].ff ;
		ff_nn = zmz_data[i].ff_nn ;
		max_ii = zmz_data[i].max_ii ;
		max_vv = zmz_data[i].max_vv ;
		
		for ( k=i; k<zmz_nn; k++ )
		{

			if ( zmz_data[k].freq_ff!=0 ) // 如果已经赋值了，则不再处理
				continue ;
			if ( zmz_data[k].ff!=ff ) // 如果极值正负值方向不同，则不做
				continue ;
			if ( zmz_data[k].ff_nn!=ff_nn ) // 如果极值变化个数不同，则不做
				continue ;
//			if ( abs(zmz_data[k].im-im)>pole_im ) // 如果极值偏移量之差过大，则不做
			if ( abs(zmz_data[k].max_ii-max_ii)>pole_im ) // 如果极值偏移量之差过大，则不做
				continue ;
			if ( fabs(zmz_data[k].max_vv-max_vv)>vv_ratio ) // 如果极值之差过大，则不做
				continue ;

			zmz_data[k].freq_ff = freq_ff ;
			zmz_data[k].freq_vv = max_vv ;

		}
		freq_ff ++ ;
	}

	// 计算相同freq_ff的点的距离
	for ( i=0; i<zmz_nn; i++ )
	{
		freq_ff = zmz_data[i].freq_ff ;
		k0 = i ;
		for ( k=i+1; k<zmz_nn; k++ )
		{
			if ( zmz_data[k].freq_ff==freq_ff )
			{
				zmz_data[k].freq_nn = zmz_data[k].i0-zmz_data[k0].i0 ;
//				zmz_data[k].ii = k0 ;
				zmz_data[k].freq_xx = zmz_data[k].xx-zmz_data[k0].xx ;
				k0 = k ;
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////
long make_freq_from_pcmData( long print_flag, long pcm_save_flag, short *pcm_data, long pcm_len, char *from_proc )
{

	if ( pcm_save_flag>0 )
	{
		// 如果进来的音频数据的幅度过小，则认为是杂音
		if ( check_pcm_data_too_small( pcm_data, pcm_len, 1000 ) )
			return ( -1 ) ;

		set_waveForm_pcm( pcm_data, pcm_len ) ;
		waveForm_show_proc( "make_freq_from_pcmData" ) ;

	}

	g_print_flag = print_flag ;

	strcpy( g_from_proc, from_proc ) ;

if ( g_print_flag>0 )
	log_prt( g_logFile, "-------------------make_freq_from_pcmData\r\n" ) ;

	clear_zmz_data() ;
	clear_freq_data() ;

	g_zmz_nn = make_zmz_data_2( pcm_data, pcm_len ) ;

	if ( g_zmz_nn<5 )
	{
if ( g_print_flag>0 )
	log_prt( g_logFile, "zmz_nn<5**************** %-5ld %s\r\n", g_zmz_nn, g_test_pcm_name ) ;
		return ( -2 ) ;
	}

	create_freq_zmz_data( g_zmz_data, g_zmz_nn, 10, 3 ) ;
//	create_freq_zmz_data_2( g_zmz_data, g_zmz_nn, 10, 3 ) ;

//g_print_flag = 1 ;
if ( g_print_flag>0 )
	print_freq_zmz_data( g_zmz_data, g_zmz_nn, "lined_1" ) ;
//g_print_flag = print_flag ;

	long nn ;

	nn = get_freq_data_from_zmz_data( g_zmz_data, g_zmz_nn, 0 ) ;
	if ( nn<=0 )
	{
if ( g_print_flag>0 )
	log_prt( g_logFile, "get_freq_data_from_zmz_data() nn<=0********%-5ld %s\r\n", nn, g_test_pcm_name ) ;
		return ( -3 ) ;
	}
	adjust_freq_data_proc_1( g_freq_data, nn, 0.10 ) ;

/**
	if ( g_print_flag>0 )
	{
		if ( nn<=2 )
		{
			g_test_data.filter_problem_0 ++ ;
			g_test_data.tot_filter_problem_0 ++ ;
			log_prt( g_logFile, "********* filter_problem_0=%ld %s\r\n", g_test_data.filter_problem_0, g_test_data.music_note ) ;
			return ( 0 ) ;
		}
	}
**/

	g_freq_data_nn = nn ;

	g_note_pcm_ok1++ ;


	double freq, freq_nn, freq_nn2 ;

//	freq_nn = get_freq_nn_from_freq_data( g_freq_data, nn, 0.382, 2 ) ;
	freq_nn = get_freq_nn_from_freq_data_new( g_freq_data, nn, 0.10 ) ;

	if ( freq_nn==0 )
	{
if ( g_print_flag>0 )
	log_prt( g_logFile, "get_freq_nn_from_freq_data_new() freq_nn=0******** %-5.1lf %s\r\n", freq_nn, g_test_pcm_name ) ;
		return ( -4 ) ;
	}

	freq_nn2 = freq_nn ;

//	freq_nn2 = redo_freq_nn_from_zmz_data2( g_zmz_data, g_zmz_nn, freq_nn2, 1.0 ) ;
	if ( g_print_flag>0 )
	{
		log_prt( g_logFile, "freq_nn=%-8.2lf freq_nn2=%-8.2lf\t\tg_note_flag=%ld\r\n", freq_nn, freq_nn2, g_note_flag ) ;
//		char	sFile[200] ;
//		sprintf( sFile, "%s-%05ld.wav", g_logFile, g_note_flag ) ;
//		WriteBufToFile( sFile, pcm_data, pcm_len*2 ) ;
	}

	if ( freq_nn2==0 )
	{
		return ( -5 ) ;
	}

//	set_waveForm_pcm( pcm_data, pcm_len ) ;

//	show_waveForm() ;
//show_WaveForm_proc_2( pcm_data, pcm_len, RGB(0,120,0) ) ; // 显示波形
//show_zmz_data_proc_2( g_zmz_data, g_zmz_nn, pcm_len, RGB(122,0,120), RGB(120,120,0) ) ;


	freq = get_freq_avg( freq_nn2 ) ;

	if ( g_tj_data_ff>0 )
	{
		long ni = noteInfo_proc( freq, 0 ) ;
		do_freq_compare_proc( ni, freq, freq_nn2 ) ;
	}
//	g_showFreq = freq ;
//	do_refresh_proc(); 

//	make_WaveForm_draw_data( g_waveForm_pcm, g_waveForm_pcm_len ) ; // 建立显示波形

	noteInfo_proc( freq, 1 ) ;

//	waveForm_show_proc() ;

//	make_WaveForm_draw_data( pcm_data, pcm_len ) ;

//	do_new_show_vars_proc() ;

//	char	ss[100] ;
//	sprintf( ss, "%1.2lf", freq ) ;

//	sscanf( dataVars, "%lf", &g_curFreq ) ;

//	g_curFreq = freq ;

//	do_task( 201, ss ) ;

//	do_showFreq_proc() ;

	return ( 1 ) ;
}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void do_freq_compare_proc( long ni, double freq, double freq_nn )
{
	char	freq_ss[10] ;
	double	difs ;

	freq_ss[0] = g_note_info[ni].note_name[0] ;
	freq_ss[1] = g_note_info[ni].note_name[1] ;
	freq_ss[2] = g_note_info[ni].note_name[2] ;
	freq_ss[3] = '\0' ;

	difs = 0 ;
	if ( strcmp( freq_ss, g_test_data.music_note )!=0 )
	{
		log_prt( g_logFile, "\r\n************************************* wrong_note=%s music_note=%s freq=%1.2lf note_freq=%1.2lf\r\n\r\n", freq_ss, g_test_data.music_note, freq, g_test_data.music_note_freq ) ;
		g_test_data.wrong_note_nn ++ ;
	}
	else
	{
		difs = freq-g_note_info[ni].note_freq ;
		g_test_data.note_freq_dif += fabs(difs) ;
		log_prt( g_logFile, "\r\n-------------------------------------from_proc=%-20s freq_nn=%-5.0lf freq=%1.2lf-%1.2lf(%1.2lf) tot_dif=%2.2lf\r\n\r\n", 
																			g_from_proc,
																			freq_nn, freq, g_note_info[ni].note_freq, difs, g_test_data.note_freq_dif ) ;
	}

	g_tj_data[g_tj_data_nn].tj_freq_dif = difs ;
	g_tj_data_nn++ ;

}
//////////////////////////////////////////////////////////////////////
void pcm_data_filter_1( short *pcm_data, long pcm_len, long yy_div )
{
	long	i, vv ;
	for ( i=0; i<pcm_len; i++ )
	{
		vv = pcm_data[i] ;
		vv /= yy_div ;
		vv *= yy_div ;
		pcm_data[i] = (short)vv ;
	}
}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void do_pcmData_in( short *pcm_data, long pcm_len )
{
	// 如果进来的音频数据的幅度过小，则认为是杂音
	if ( check_pcm_data_too_small( pcm_data, pcm_len, 1000 ) )
	{
		if ( g_note_flag>0 )
		{
			clear_tj_data() ;
			g_note_nn ++ ;
			if ( g_waveForm_save>0 )
			{
				sprintf( g_note_file, "z:\\hhs-%05ld.wav", g_note_nn ) ;
				WriteBufToFile( g_note_file, pcm_data, 0 ) ;
			}
		}
		g_note_flag = 0 ;
		do_refresh_proc() ;
	}
	else
	{
		if ( g_waveForm_save>0 )
			AppendBufToFile( g_note_file, pcm_data, pcm_len*2 ) ;

		if ( g_note_flag==0 )
			g_note_flag ++ ;

		make_freq_from_pcmData( 0, 1, pcm_data, pcm_len, "do_pcmData_in" ) ;

	}
}
//////////////////////////////////////////////////////////////////////
void do_pcmData_in_new( short *pcm_data, long pcm_len )
{
/**
	// 如果进来的音频数据的幅度过小，则认为是杂音
	if ( check_pcm_data_too_small( pcm_data, pcm_len, 1000 ) )
	{
		if ( g_note_flag>0 )
		{
			clear_tj_data() ;
			g_note_nn ++ ;
			if ( g_waveForm_save>0 )
			{
				sprintf( g_note_file, "d:\\hhs-%05ld.wav", g_note_nn ) ;
				WriteBufToFile( g_note_file, pcm_data, 0 ) ;
			}
		}
		g_note_flag = 0 ;
		do_refresh_proc() ;
	}
	else
	{
		if ( g_waveForm_save>0 )
			AppendBufToFile( g_note_file, pcm_data, pcm_len*2 ) ;

		if ( g_note_flag==0 )
			g_note_flag ++ ;

		make_freq_from_pcmData( 0, pcm_data, pcm_len ) ;

	}
**/




	if ( g_waveForm_save>0 )
		AppendBufToFile( g_note_file, pcm_data, pcm_len*2 ) ;

	if ( g_note_flag==0 )
		g_note_flag ++ ;


	long	ff ;
	ff = make_freq_from_pcmData( 0, 1, pcm_data, pcm_len, "do_pcmData_in_new" ) ;
	if ( ff<=0 )
	{
		if ( g_note_flag>0 )
		{
			clear_tj_data() ;
			g_note_nn ++ ;
			if ( g_waveForm_save>0 )
			{
				sprintf( g_note_file, "z:\\hhs-%05ld.wav", g_note_nn ) ;
				WriteBufToFile( g_note_file, pcm_data, 0 ) ;
			}
		}
		g_note_flag = 0 ;
//		do_refresh_proc() ;
	}
}
//////////////////////////////////////////////////////////////////////
void do_wave_data_proc_2()
{

	do_WaveData( 102, NULL ) ;
	if ( g_wave_show_flag>0 ) 
	{
		do_pcmData_in( g_wave_show_buf, g_waveForm_xy_len ) ;
		g_wave_show_flag = 0 ;
	}
}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void set_show_data( Show_Info *show_data, char *show_ss, long x, long y, long fx, long fy, long font_color, long back_color, long font_height, long font_weight )
{

	strcpy( show_data->show_name, show_ss ) ;

	show_data->show_x = x ;
	show_data->show_y = y ;
	show_data->show_fx = fx ;
	show_data->show_fy = fy ;

	show_data->font_color = font_color ;
	show_data->back_color = back_color ;
	show_data->font_height = font_height ;
	show_data->font_weight = font_weight ;

}
//////////////////////////////////////////////////////////////////////
void make_noteShow_data( short ni, double note_freq )
{
	char	show_ss[20], fmt_ss[20] ;
	long	tx, ty, ta, tb, fx, fy ;
	
	show_ss[1] = '\0' ;

	// noteName
	show_ss[0] = g_note_info[ni].note_name[0] ;
	SetFont( g_dataInfo_DC, g_NoteName_fontHeight, FW_BOLD, g_dataInfo_fontName ) ;
	set_show_data( &g_freq_show.note, show_ss, g_NoteName_xx, g_NoteName_yy, 1, 1, g_NoteName_fontColor, g_dataInfo_backColor, g_NoteName_fontHeight, FW_BOLD ) ;

	// noteAux
	Get_Text_AB( g_dataInfo_DC, show_ss, &ta, &tb ) ;
	tx = g_NoteName_xx + ta ;
	ty = g_NoteName_yy + tb / 20 ;
	fx = tx ;
	fy = ty ;

	show_ss[0] = g_note_info[ni].note_name[1] ;
	SetFont( g_dataInfo_DC, g_NoteAux_fontHeight, FW_BOLD, g_dataInfo_fontName ) ;

	set_show_data( &g_freq_show.note_aux, show_ss, tx-ta/20, ty, 1, 1, g_NoteName_fontColor, g_dataInfo_backColor, g_NoteAux_fontHeight, FW_BOLD ) ;

	// noteIdx
	show_ss[0] = g_note_info[ni].note_name[2] ;
	SetFont( g_dataInfo_DC, g_NoteIdx_fontHeight, FW_BOLD, g_dataInfo_fontName ) ;

	ty = g_NoteName_yy + tb + 2 ;
	Get_Text_AB( g_dataInfo_DC, show_ss, &ta, &tb ) ;
	ty = ty - tb - tb / 3 ;

	set_show_data( &g_freq_show.note_idx, show_ss, tx-ta/20, ty, 1, 1, g_NoteName_fontColor, g_dataInfo_backColor, g_NoteIdx_fontHeight, FW_BOLD ) ;


	SetFont( g_dataInfo_DC, g_Freq_fontHeight, FW_BOLD, g_dataInfo_fontName ) ;
	Get_Text_AB( g_dataInfo_DC, show_ss, &ta, &tb ) ;

	// standard_freq
	fx = g_dataInfo_cx - ta/2 ;
	fy = g_NoteName_yy + tb / 4 + 6 ;

	strcpy( fmt_ss, "%-" ) ;
	strcat( fmt_ss, g_note_show_fmt ) ;
	strcat( fmt_ss, "Hz" ) ;

	sprintf( show_ss, fmt_ss, g_note_info[ni].note_freq ) ;

	set_show_data( &g_freq_show.freq_std, show_ss, fx, fy, -1, 1, g_Freq_fontColor, g_dataInfo_backColor, g_Freq_fontHeight, FW_BOLD ) ;


	// note_freq
	fy += (tb - tb/10);

	sprintf( show_ss, fmt_ss, note_freq ) ;
	SetFont( g_dataInfo_DC, g_Freq_fontHeight, FW_BOLD, g_dataInfo_fontName ) ;

	set_show_data( &g_freq_show.freq_cur, show_ss, fx, fy, -1, 1, g_Freq_fontColor, g_dataInfo_backColor, g_Freq_fontHeight, FW_BOLD ) ;


	// diff_freq
	short	df ;
	fy += (tb - tb/10);
	sprintf( show_ss, fmt_ss, note_freq-g_note_info[ni].note_freq ) ;

	if ( strstr( show_ss, "0.00" )!=NULL )
	{
		sprintf( show_ss, "0.00Hz" ) ;
		df = 0 ;
	}
	else if ( show_ss[0] == '-' ) 
		df = -1 ;
	else 
	{
		char	ss[30] ;
		sprintf( ss, "+%s", show_ss ) ;
		strcpy( show_ss, ss ) ;
		df = 1 ;
	}

	SetFont( g_dataInfo_DC, g_Freq_fontHeight, FW_BOLD, g_dataInfo_fontName ) ;
	set_show_data( &g_freq_show.freq_dif, show_ss, fx, fy, -1, 1, g_Diff_fontColor, g_dataInfo_backColor, g_Freq_fontHeight, FW_BOLD ) ;


	// diff_cents
	double	dv ;
	fy += (tb - tb/10);

	strcpy( fmt_ss, "%-" ) ;
	strcat( fmt_ss, g_note_show_fmt ) ;
	strcat( fmt_ss, "Cs" ) ;

	if ( df==0 )
	{
		sprintf( show_ss, fmt_ss, 0 ) ;
	}
	else
	{
		if ( df<0 )
			dv = (g_note_info[ni].note_freq-g_note_info[ni-1].note_freq)/100 ;
		else
			dv = (g_note_info[ni+1].note_freq-g_note_info[ni].note_freq)/100 ;

		dv = (note_freq-g_note_info[ni].note_freq)/dv ;
		
		sprintf( show_ss, fmt_ss, dv ) ;

		if ( df>0 ) 
		{
			char	ss[20] ;
			sprintf( ss, "+%s", show_ss ) ;
			strcpy( show_ss, ss ) ;
		}

	}

	SetFont( g_dataInfo_DC, g_Freq_fontHeight, FW_BOLD, g_dataInfo_fontName ) ;
	set_show_data( &g_freq_show.freq_cns, show_ss, fx, fy, -1, 1, g_Cents_fontColor, RGB(100,100,100), g_Freq_fontHeight, FW_BOLD ) ;
	
}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void display_show_data( Show_Info show_data )
{
	SetFont( g_dataInfo_DC, show_data.font_height, show_data.font_weight, g_dataInfo_fontName ) ;
	DrawText( g_dataInfo_DC, show_data.show_name, show_data.show_x, show_data.show_y, show_data.font_color, show_data.back_color, show_data.show_fx, show_data.show_fy ) ;
}
//////////////////////////////////////////////////////////////////////
void noteShow_proc( long clear_flag )
{

	if ( clear_flag>0 )
		clear_freq_area() ;

	display_show_data( g_freq_show.note ) ;
	display_show_data( g_freq_show.note_aux ) ;
	display_show_data( g_freq_show.note_idx ) ;
	display_show_data( g_freq_show.freq_std ) ;
	display_show_data( g_freq_show.freq_cur ) ;
	display_show_data( g_freq_show.freq_dif ) ;
	display_show_data( g_freq_show.freq_cns ) ;
	DoEventProc() ;

}
//////////////////////////////////////////////////////////////////////
void draw_noteName_new( short ni, double note_freq )
{
	do_new_show_vars_proc() ;
	make_noteShow_data( ni, note_freq ) ;
	noteShow_proc( 1 ) ;
	do_task( 105, "0" ) ;
}
//////////////////////////////////////////////////////////////////////
void make_WaveForm_pcm_data( short *pcm_data, long pcm_len )
{
	if ( pcm_len<g_waveForm_aa )
	{
		g_waveForm_xy_nn = make_WaveForm_xy( pcm_data, 0, 0, pcm_len-1, 0, g_waveForm_aa ) ;
		g_waveForm_xy_x1 = g_waveForm_xy_x2 = 0 ;
	}
	else
	{

		long	aa, aaa, i1, i2, da; //, xy_nn ;

		aaa = g_waveForm_aa / 2 ; // 从波形显示的中间位置确定

		da = 80 ; // 显示省略符的宽度

		// 计算左边显示的数据（0-起始位置，aaa-da=右边界位置）
		aa = get_pcm_show_i1_i2_proc( 1, 0, aaa-da, &i1, &i2 ) ;
		g_waveForm_xy_i1 = i1 ;
		g_waveForm_xy_i2 = i2 ;
		g_waveForm_xy_a1 = aa ;

//		g_waveForm_xy_x1 = aa ;
//		xy_nn = make_WaveForm_xy( pcm_data, 0, i1, i2, 0, aa ) ;
//log_prt( g_logFile, "1-------------------i1=%ld i2=%ld aa=%ld xy_nn=%ld g_zmz_nn=%ld\r\n", i1, i2, aa, xy_nn, g_zmz_nn ) ;


		// 计算右边显示的数据（aaa+da=起始位置，g_waveForm_aa=右边界位置）
		aa = get_pcm_show_i1_i2_proc( -1, g_waveForm_aa-aa, aa, &i1, &i2 ) ;
		g_waveForm_xy_i3 = i1 ;
		g_waveForm_xy_i4 = i2 ;
		g_waveForm_xy_a2 = aa ;

//		xy_nn = make_WaveForm_xy( pcm_data, xy_nn, i1, i2, g_waveForm_aa-aa, aa ) ;
//log_prt( g_logFile, "2-------------------i1=%ld i2=%ld aa=%ld xy_nn=%ld g_zmz_nn=%ld\r\n", i1, i2, aa, xy_nn, g_zmz_nn ) ;

//		g_waveForm_xy_nn = xy_nn ;
//		g_waveForm_xy_ca = aa ;
//		g_waveForm_xy_x2 = g_waveForm_aa-aa ;

	}

}
//////////////////////////////////////////////////////////////////////
long get_pcm_show_i1_i2_proc_new( long ff, long xx, long aa, long *ii1, long *ii2 )
{
	long	i, i1, i2, x1, x2 ;

	i1 = 0 ;
	i2 = 0 ;

	if ( ff==1 ) // 正向查找
	{
		// 查找起始波形
		for ( i=0; i<g_zmz_nn; i++ )
		{

			// 小于起始点，则继续
			if ( g_zmz_data[i].x1<xx )
				continue ;

			// 判断是否为正半弦
			if ( g_zmz_data[i].freq_vv>0 )
			{
				i1 = g_zmz_data[i].i1 ;
				x1 = (long)floor(g_zmz_data[i].x1) ;
				break ;
			}
		}
		// 查找结束波形
		for ( ; i<g_zmz_nn; i++ )
		{
			// 小于结束点，则继续
			if ( g_zmz_data[i].x1-x1<aa )
				continue ;
			// 判断是否为正半弦
			if ( g_zmz_data[i].freq_vv>0 )
			{
				i2 = g_zmz_data[i-1].i2-1 ; // 正半弦向后退一个点，正好是一个负半弦
				break ;
			}
		}
		if ( i==g_zmz_nn )
			i2 = g_zmz_data[i-1].i2-1 ;
	}
	else if ( ff==-1 ) // 反向查找
	{
		// 从后向前查找一个负半弦（结束波形）
		for ( i=g_zmz_nn-1; i>=0; i-- )
		{
			if ( g_zmz_data[i].x2>xx+aa )
				continue ;
			if ( g_zmz_data[i].freq_vv<0 )
			{
				i2 = g_zmz_data[i].i2-1 ;
				x2 = (long)floor(g_zmz_data[i].x2) ;
				break ;
			}
		}
		// 继续从后向前查找到起始波形（起始波形）
		for ( ; i>=0; i-- )
		{
			// 大于起始点，则继续
			if ( g_zmz_data[i].x2>xx )
				continue ;

			// 判断是否为负半弦
			if ( g_zmz_data[i].freq_vv<0 )
			{
				i1 = g_zmz_data[i+1].i1 ;
				break ;
			}
		}
		if ( i==0 )
			i1 = 0 ;
	}
	*ii1 = i1 ;
	*ii2 = i2 ;
	aa = i2-i1 ;
	return ( aa ) ;
}
//////////////////////////////////////////////////////////////////////
void make_WaveForm_draw_data_new( long flag, short *pcm_data, long pcm_len )
{
	if ( flag>0 || pcm_len<g_waveForm_aa )
	{
		g_waveForm_xy_nn = make_WaveForm_xy( pcm_data, 0, 0, pcm_len-1, 0, g_waveForm_aa ) ;
		g_waveForm_xy_x1 = g_waveForm_xy_x2 = 0 ;
	}
	else
	{
		if ( g_zmz_0_pass<=g_waveForm_show_nn )
		{
			g_waveForm_xy_nn = make_WaveForm_xy( pcm_data, 0, 0, pcm_len-1, 0, g_waveForm_aa ) ;
			g_waveForm_xy_x1 = g_waveForm_xy_x2 = 0 ;
		}
		else 
		{

			long	aa, aaa, i1, i2, da, xy_nn ;

			aaa = g_waveForm_aa / 2 ; // 从波形显示的中间位置确定

			da = 80 ; // 显示省略符的宽度

			// 计算左边显示的数据（0-起始位置，aaa-da=右边界位置）
			aa = get_pcm_show_i1_i2_proc_new( 1, 0, aaa-da, &i1, &i2 ) ;
			xy_nn = make_WaveForm_xy( pcm_data, 0, i1, i2, 0, aa ) ;

//log_prt( g_logFile, "1-------------------i1=%ld i2=%ld aa=%ld xy_nn=%ld g_zmz_nn=%ld\r\n", i1, i2, aa, xy_nn, g_zmz_nn ) ;

			g_waveForm_xy_x1 = aa ;

			// 计算右边显示的数据（aaa+da=起始位置，g_waveForm_aa=右边界位置）
			aa = get_pcm_show_i1_i2_proc_new( -1, g_waveForm_aa-aa, aa, &i1, &i2 ) ;
			xy_nn = make_WaveForm_xy( pcm_data, xy_nn, i1, i2, g_waveForm_aa-aa, aa ) ;

//log_prt( g_logFile, "2-------------------i1=%ld i2=%ld aa=%ld xy_nn=%ld g_zmz_nn=%ld\r\n", i1, i2, aa, xy_nn, g_zmz_nn ) ;

			g_waveForm_xy_nn = xy_nn ;

			g_waveForm_xy_x2 = g_waveForm_aa-aa ;
		}
	}

}
//////////////////////////////////////////////////////////////////////
void make_waveForm_show_data( char *from_proc )
{

from_proc_prt( g_logFile, "make_waveForm_show_data(0) --------------redraw=%-8ld pcm_len=%-8ld from_proc\r\n", g_waveForm_redraw, g_waveForm_pcm_len, from_proc ) ;

	if ( g_waveForm_redraw>0 )
	{
		if ( g_waveForm_pcm_len>0 )
		{
//			WriteBufToFile( "d:\\hhs.wav", g_waveForm_pcm, g_waveForm_pcm_len*2 ) ;
			if ( do_task( 201, "" )>0 )
			{
				sscanf( g_waveForm_rect_vars, "%lu %lu", &g_waveForm_aa, &g_waveForm_bb ) ;
				make_pan_dc_memory_buf( g_waveForm_DC, g_waveForm_aa, g_waveForm_bb ) ;
				strcpy( g_waveForm_rect_vars, "" ) ;
			}
			make_WaveForm_draw_data_new( 1,  g_waveForm_pcm, g_waveForm_pcm_len ) ; // 建立显示波形
			g_waveForm_refresh = 1 ;
from_proc_prt( g_logFile, "make_waveForm_show_data(1) --------------pcm_len=%-8ld xy_nn=%-8ld refresh=%-8ld\r\n", g_waveForm_pcm_len, g_waveForm_xy_nn, g_waveForm_refresh ) ;
		}
		g_waveForm_redraw = 0 ;
	}

}
//////////////////////////////////////////////////////////////////////
void show_omit( long x1, long x2, long dot_color, long bg_color )
{

	long	y0, dx, dy ;
	y0 = g_waveForm_bb / 2 ;

	dx = 20 ;
	dy = 10 ;
	if ( bg_color!=0 )
		DrawBar( g_waveForm_DC, x1, y0-dy, x2, y0+dy, bg_color, PS_SOLID, R2_XORPEN ) ;

	DrawLine( g_waveForm_DC, x1+dx, y0, x2-dx, y0, 5, dot_color, PS_DOT, R2_COPYPEN ) ;

}
//////////////////////////////////////////////////////////////////////
void draw_waveForm_show_data( long dot_show_flag, char *from_proc )
{

from_proc_prt( g_logFile, "draw_waveForm_show_data(1) refresh=%-8ld from_proc=%s\r\n", g_waveForm_refresh, from_proc ) ;

	if ( do_task( 206, "" )==0 )
		return ;
from_proc_prt( g_logFile, "draw_waveForm_show_data(2) refresh=%-8ld from_proc=%s\r\n", g_waveForm_refresh, from_proc ) ;
	clear_wave_area() ;
	do_task( 106, "0" ) ; // 清除waveForm的显示标识

from_proc_prt( g_logFile, "draw_waveForm_show_data(3) refresh=%-8ld xy_nn=%-8ld from_proc=%s\r\n", g_waveForm_refresh, g_waveForm_xy_nn, from_proc ) ;
	if ( g_waveForm_xy_nn>0 )
	{
		show_waveForm_xy( dot_show_flag, g_waveForm_xy, g_waveForm_xy_nn, 1, RGB(0,120,0), RGB(120,0,0) ) ; 
//		if ( g_waveForm_xy_x1>0 && g_waveForm_xy_x2>=g_waveForm_xy_x1 )
//			show_omit( g_waveForm_xy_x1, g_waveForm_xy_x2, RGB(0,220,0), RGB(80,80,80) ) ;
	}


}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void waveForm_show_proc( char *from_proc )
{
from_proc_prt( g_logFile, "waveForm_show_proc() from_proc=%s\r\n", from_proc ) ;
	make_waveForm_show_data( from_proc ) ;
	draw_waveForm_show_data( 0, from_proc ) ;
}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void do_refresh_proc()
{
	if ( do_task( 205, "" )>0 )
	{
		noteInfo_proc( g_showFreq, 1 ) ;
	}
	waveForm_show_proc( "do_refresh_proc" ) ;
}
//////////////////////////////////////////////////////////////////////
void do_new_show_vars_proc()
{

	if ( do_task( 202, "" ) >0 ) 
	{

//		sscanf( g_new_rect_vars, "%lu %lu %lu %lu", &g_waveForm_aa, &g_waveForm_bb, &g_dataInfo_aa, &g_dataInfo_bb ) ;
//		strcpy( g_new_rect_vars, "" ) ;

		sscanf( g_dataInfo_rect_vars, "%lu %lu", &g_dataInfo_aa, &g_dataInfo_bb ) ;
		strcpy( g_dataInfo_rect_vars, "" ) ;

		g_NoteName_fontHeight = 7*g_dataInfo_bb / 8 ;
		g_NoteAux_fontHeight = g_NoteName_fontHeight / 2 ;
		g_NoteIdx_fontHeight = g_NoteName_fontHeight / 3 ;
		g_Freq_fontHeight = g_NoteName_fontHeight / 3 - g_NoteName_fontHeight/8 ;

		g_waveForm_show_nn = g_waveForm_aa/60 ;

		do_set_dataInfo_cx_proc() ;


		g_waveForm_redraw = 1 ;

	}
}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
TFreqByPcm	*g_fbp ;
TWavePeriod		*g_wp ;
TWavePeriod2	*g_wp2 ;
//////////////////////////////////////////////////////////////////////
void init_fbp()
{
	g_fbp = new TFreqByPcm ;
	g_wp = new TWavePeriod ;
	g_wp2 = new TWavePeriod2 ;
}
//////////////////////////////////////////////////////////////////////
long do_test_wave_proc( char *wav_file )
{
	long	nRead, nBytes, nSize, nOff, nCount ;
	long	show_flag, print_flag ;
	double	period_f1, period_f2 ;
	short	*pcm_data ;

	nOff = 44 ;
	nSize = Get_FileSize( wav_file ) ;
	strcpy( g_wavFile, wav_file ) ;

	nSize -= nOff ;

	nBytes = 8820 ;

	pcm_data = (short*)GlobalAlloc( GPTR, nBytes ) ;
	strcpy( g_wp->m_WaveFile, wav_file ) ;
	g_wp->clear_period_data() ;
	g_wp2->clear_period_data() ;
	__try
	{
		nCount = 0 ;
		show_flag = 0 ;
		print_flag = -1 ;
		while ( nSize>0 )
		{
			nRead = ReadBufFromFile( wav_file, pcm_data, nOff, nBytes ) ;
			if ( nRead>0 ) 
			{
				period_f1 = g_wp->make_period_data( (short*)pcm_data, nRead/2, show_flag, print_flag, nCount ) ;
				if ( period_f1<=0 )
					period_f1 = 0 ;
				period_f2 = g_wp2->make_period_data( (short*)pcm_data, nRead/2, show_flag, print_flag, nCount ) ;
				if ( period_f2<=0 )
					period_f2 = 0 ;

log_prt( g_logFile, "=======================================================period_f1=%-8.3lf period_f2=%-8.3lf nCount=%-8ld\r\n", period_f1, period_f2, nCount ) ;

			}
			nOff += nRead ;
			nSize -= nRead ;
			nCount ++ ;
//			if ( nCount>3 )
//				break ;
		}
		if ( show_flag==0 )
		{
			clear_waveForm_area() ;
			g_wp->show_period_data() ;
			SaveToBmpFile( wav_file, nCount ) ;
		}
	}
	__finally
	{
		FreeBuf( pcm_data ) ;
	}
	return ( 0 ) ;
}
//////////////////////////////////////////////////////////////////////
long test_proc( char *DataValue )
{

	if ( do_task( 201, "" )>0 )
	{
		sscanf( g_waveForm_rect_vars, "%lu %lu", &g_waveForm_aa, &g_waveForm_bb ) ;
		do_task( 101, "" ) ;
		make_pan_dc_memory_buf( g_waveForm_DC, g_waveForm_aa, g_waveForm_bb ) ;
		g_fbp->make_xy_buf( g_waveForm_aa ) ;
	}

	do_test_wave_proc( DataValue ) ;

	return ( 0 ) ;
}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
