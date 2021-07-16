// TWavePeriod.h: interface for the TWavePeriod class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TWAVEPERIOD_H__C0A74FFF_1619_48BA_8B20_E8FFD46349C9__INCLUDED_)
#define AFX_TWAVEPERIOD_H__C0A74FFF_1619_48BA_8B20_E8FFD46349C9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////
struct	PeriodYY_Type
{
	long	xx ;
	short	yy ;
} ;
//////////////////////////////////////////////////////////////////////
struct	PeriodDa_Type
{
	double	xx ;
	short	ff ;
	long	ix ;
	long	PeriodYY_si ;
	long	PeriodYY_nn ;
	long	next_ii[2] ;
	long	next_ff[2] ;
	long	next_nn[2] ;
} ;
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
class TWavePeriod  
{

private:

	PeriodYY_Type	*m_PeriodYY ;
	long			m_PeriodYY_m, m_PeriodYY_n ;
	void	set_PeriodYY_Len( long nn ) ;

	long push_PeriodYY( long xx, short yy ) ;
	void make_PeriodYY() ;
	void show_PeriodYY( long i, long show_color ) ;

	PeriodDa_Type	*m_PeriodData ;
	long		m_PeriodData_m, m_PeriodData_n ;
	long		m_PeriodData_si ;

	void	set_PeriodData_Len( long nn ) ;
	long	push_PeriodData( short ff, long ix, double xx ) ;

	
	long get_sign( long yy ) ;
	double get_xx( long i, long y0, long yy ) ;
	short get_yy( double xx, short *pcm_data ) ;

	void make_PeriodData() ;
	void remove_TooShort_PeriodData( double min_dx ) ;
	void show_PeriodData( long di, long y_off, long show_color, long yy_color, long next_color ) ;



	// pcm data
	double	m_MinVV ;
	long	m_FlatVV ;
	long	m_DC_line_color ;

	short	*m_PcmData ;
	long	m_PcmData_si, m_PcmData_mm, m_PcmData_nn ;
	void make_flat_data( short *pcm_data, long pcm_len ) ;
	void push_PcmData(short *pcm_data, long pcm_len) ;

	long draw_VLine( double xx, long y1, long y2, long xoff, long fx, long show_color, char *sa ) ;
	void draw_WaveForm_data( long dot_show_flag, POINT *xy_data, long xy_nn, long line_width, long waveForm_color, long dot_color ) ;
	long make_WaveFormData( short *pcm_data, long pcm_len, long xxx, long aaa, POINT *xy_data ) ;


public:

	TWavePeriod();
	virtual ~TWavePeriod();

	void clear_period_data() ;
	double make_period_data(short *pcm_data, long pcm_len) ;

	void show_pcm_data( short *pcm_data, long pcm_len, long show_color, long dot_color ) ;

};
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_TWAVEPERIOD_H__C0A74FFF_1619_48BA_8B20_E8FFD46349C9__INCLUDED_)
