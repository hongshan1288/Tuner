// FreqByPcm.h: interface for the TFreqByPcm class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TFREQBYPCM_H__A204FA9D_85D5_487F_BE67_8D11C486EECF__INCLUDED_)
#define AFX_TFREQBYPCM_H__A204FA9D_85D5_487F_BE67_8D11C486EECF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

struct	PeriodData_Type
{
	long	ix, in ;
	short	ff ; 
	long	dy_sum ;
} ;

struct	SameData_Type
{
	long	cur_ii, next_ii, pd_nn ;
	long	in_tot, dy_sum, dy_tot ;
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

	char				*m_dy_data ;
	void make_dy_data( short *pcm_data, long pcm_len ) ;
	void show_dy_data( long show_color ) ;

	// flat data
	void	make_flat_data( short *pcm_data, long pcm_len ) ;
	void	show_PcmData( short *pcm_data, long pcm_len, long show_color, long dot_color ) ;

	PeriodData_Type	*m_PeriodData ;
	long			m_PeriodData_m, m_PeriodData_n ;
	void	Set_PeriodData_Len( long nn ) ;
	long	push_PeriodData( long ix, short ff ) ;
	void	make_PeriodData_dySum() ;

	long			m_Period_is, m_Period_ie, m_Period_dn ;
	void	get_PeriodData_vars( short *pcm_data, long pcm_len ) ;
	void	make_PeriodData( short *pcm_data, long pcm_len ) ;
	long	get_PeriodData_yy( short *pcm_data, long ix, long n ) ;

	void show_PeriodData( long show_color ) ;
	void draw_VerticalLine( long ii, long fx, long show_color, char *sa ) ;

	SameData_Type	*m_SameData ;
	long			m_SameData_m, m_SameData_n ;
	void	set_SameData_Len( long nn ) ;
	long	push_SameData( long cur_ii, long next_ii, long pd_nn, long dy_tot ) ;

	long		m_Max_In, m_Max_DySum, m_Max_DyTot ;
	long	get_DyTot( long i1, long i2, long pd_nn ) ;
	bool	like_SameData( long i, long ii, long pd_nn ) ;
	long	get_SameData( long cur_ii, long pd_nn ) ;
	void	make_SameData() ;

	void show_SameData( long show_color ) ;

public:

	double	mFreq ;

	TFreqByPcm();
	virtual ~TFreqByPcm();

	long GetFreqFromPcm( short *pcm_data, long pcm_len, char *from_proc ) ;

};

#endif // !defined(AFX_TFREQBYPCM_H__A204FA9D_85D5_487F_BE67_8D11C486EECF__INCLUDED_)
