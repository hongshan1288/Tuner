// FreqByPcm.h: interface for the TFreqByPcm class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TFREQBYPCM_H__A204FA9D_85D5_487F_BE67_8D11C486EECF__INCLUDED_)
#define AFX_TFREQBYPCM_H__A204FA9D_85D5_487F_BE67_8D11C486EECF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

struct	PcmModeData_Type
{
	long	idx_s, idx_e, idx_f ;
} ;

struct	PeakData_Type
{
	long	x ; 
	short	y, f ; // f=-1: ÏÂ½µ£»f=1£ºÉÏÉý
} ;

struct	PeriodData_Type
{
	long	ix, yy ; 
} ;

class TFreqByPcm  
{
private:

	double				m_dx ;

	long	m_FlatVV ;
	double	m_MinVV ;

	long	m_DC_line_color ;

	bool	m_DCFlag ;
	bool	m_MemFlag ;

	void do_DrawText( char *pText, long xx, long yy, long font_color, long bk_color, long text_align, long text_valign ) ;
	void do_DrawLine( long x1, long y1, long x2, long y2, long line_width, long line_color, long line_style, long line_mode ) ;
	void do_Polyline( POINT *xy, long xy_nn, long line_width, long line_color, long line_style, long line_mode  ) ;
	void do_DrawBar( long x1, long y1, long x2, long y2, long bar_color, long bar_style, long bar_mode ) ;

	void draw_WaveForm_data( long dot_show_flag, POINT *xy_data, long xy_nn, long line_width, long waveForm_color, long dot_color ) ;
	

	// check too small wav data as a noise data
	bool check_pcm_data_is_a_noise( short *pcm_data, long pcm_len ) ;


	// PcmData
	short				*m_pcm_data ;
	long				m_pcm_len ;

	// flat data
	void	make_flat_data( short *pcm_data, long pcm_len ) ;
	void	show_PcmData( short *pcm_data, long pcm_len, long show_color, long dot_color ) ;

	PeriodData_Type	*m_PeriodData ;
	long			m_PeriodData_m, m_PeriodData_n ;
	void	Set_PeriodData_Len( long nn ) ;
	long	push_PeriodData( long ix, short yy ) ;

	long			m_Period_is, m_Period_ie, m_Period_dn ;

	void	get_PeriodData_vars( short *pcm_data, long pcm_len ) ;

	void	make_PeriodData( short *pcm_data, long pcm_len ) ;
	long	get_PeriodData_yy( short *pcm_data, long ix, long n ) ;

	void show_PeriodData( long show_color ) ;
	void draw_VerticalLine( long ii, long fx, long show_color, char *sa ) ;


	// PeakData Min & Max Index
	long			m_Min_PeakData_idx, m_Max_PeakData_idx ;
	long			m_Min_PeakData_yy ;

	// PeakData start & end index ;
	long			m_PeakData_is, m_PeakData_ie ;
	void	get_PeakData_vars() ;
	void	show_PeakData_Vars( long i, long x, long y ) ;


	// PeakData
	PeakData_Type	*m_PeakData ;
	long			m_PeakData_m, m_PeakData_n ;
	void	SetPeakDataLen( long nn ) ;
	long	PushPeakData( long x, short y, short f ) ;

	void	make_PeakData( short *pcm_data, long pcm_len ) ;
	void	show_PeakData( long pcm_len, long show_color ) ;

	// ModeDat
	PcmModeData_Type	*m_PcmModeData ;
	long				m_PcmModeData_m, m_PcmModeData_n ;
	void	SetPcmModeDataLen( long nn ) ;
	void	PushPcmModeData( long idx, long idx_e, long idx_f ) ;

	// 
	void make_pmd_data( short *pcm_data, long pcm_len ) ;

	//


public:

	double	mFreq ;

	TFreqByPcm();
	virtual ~TFreqByPcm();

	long GetFreqFromPcm( short *pcm_data, long pcm_len, char *from_proc ) ;

};

#endif // !defined(AFX_TFREQBYPCM_H__A204FA9D_85D5_487F_BE67_8D11C486EECF__INCLUDED_)
