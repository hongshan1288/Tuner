// FreqByPcm.h: interface for the TFreqByPcm class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TFREQBYPCM_H__A204FA9D_85D5_487F_BE67_8D11C486EECF__INCLUDED_)
#define AFX_TFREQBYPCM_H__A204FA9D_85D5_487F_BE67_8D11C486EECF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

struct	SameData_Type
{
	long	cur_ii, next_ii, pd_nn ;
	long	in_tot, dy_sum, dy_tot ;
} ;

struct	NextData_Type
{
	long	next_ii ;
} ;

struct	PeriodData_Type
{
	long	ix, in ;
	short	ff ; 
	long	dy_sum ;
	long	next_ii[10] ;
} ;

class TFreqByPcm  
{
private:

	double				m_dx ;

	long	m_LineMode ;
	long	m_FlatVV ;
	double	m_MinVV ;

	long	m_DC_line_color ;

	bool	m_DCFlag ;
	bool	m_MemFlag ;

	void do_DrawText( char *pText, long xx, long yy, long font_color, long bk_color, long text_align, long text_valign ) ;
	void do_DrawLine( long x1, long y1, long x2, long y2, long line_width, long line_color, long line_style, long line_mode ) ;
	void do_Polyline( POINT *xy, long xy_nn, long line_width, long line_color, long line_style, long line_mode  ) ;
	void do_DrawBar( long x1, long y1, long x2, long y2, long bar_color, long bar_style, long bar_mode ) ;

	POINT	*m_waveForm_xy ;
	long	m_waveForm_xy_nn, m_waveForm_xy_size ;

	long make_WaveForm_Data( short *pcm_data, long pcm_len, long xxx, long aaa, POINT *xy_data ) ;
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
	void draw_VerticalLine( long ii, long xoff, long fx, long show_color, char *sa ) ;

	SameData_Type	*m_SameData ;
	long			m_SameData_m, m_SameData_n ;
	void	set_SameData_Len( long nn ) ;
	long	push_SameData( long cur_ii, long next_ii, long pd_nn, long dy_tot ) ;

	long		m_Max_In, m_Max_DySum, m_Max_DyTot ;

	void show_NextData( long idx, long y_off, long show_color, long line_mode ) ;
	void push_NextData( long i, long next_ii, long idx ) ;

	long	get_DyTot( long i1, long i2, long pd_nn ) ;
	bool	like_SameData( long i, long ii, long pd_nn ) ;
	long	get_SameData( long cur_ii, long pd_nn ) ;
	void	make_SameData(long n) ;
	void show_SameData( long show_color ) ;

	double	m_Max_DyTot2 ;

	long	push_SameData2( long cur_ii, long next_ii, long pd_nn, long dy_tot ) ;

	long	get_DyTot2( long i1, long i2, long pd_nn ) ;
	double	get_DyTot22( long i1, long i2, long pd_nn ) ;
	long	get_SameData2( long cur_ii, long pd_nn ) ;
	void	make_SameData2(long n) ;
	void show_SameData2( long show_color ) ;


public:

	double	mFreq ;

	TFreqByPcm();
	virtual ~TFreqByPcm();

	void	make_xy_buf( long aa ) ;
	long GetFreqFromPcm( short *pcm_data, long pcm_len, char *from_proc ) ;

};

#endif // !defined(AFX_TFREQBYPCM_H__A204FA9D_85D5_487F_BE67_8D11C486EECF__INCLUDED_)
