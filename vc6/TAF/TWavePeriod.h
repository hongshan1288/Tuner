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
	long	next_ii ;
	long	next_ff ;
	long	next_nn ;
} ;
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
struct	TJDa_Type
{
	double	zq_dx ;
	double	zq_val ;
	long	zq_tot ;
	double	zq_sum ;
} ;
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
class TWavePeriod  
{

private:


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



	// PeriodDa
	PeriodDa_Type	*m_PeriodDa ;
	long		m_PeriodDa_m, m_PeriodDa_n ;
	long		m_PeriodDa_si ;

	void	set_PeriodDa_Len( long nn ) ;
	long	push_PeriodDa( short ff, long ix, double xx ) ;

	void clear_next_data() ;
	
	long get_sign( long yy ) ;
	double get_xx( long i, long y0, long yy ) ;
	short get_yy( double xx, short *pcm_data ) ;

	void make_PeriodData() ;
	void remove_ShortPeriodData( double min_dx ) ;
	void show_PeriodData( long di, long y_off, long show_color, long yy_color, long next_color ) ;



	// PeriodYY
	PeriodYY_Type	*m_PeriodYY ;
	long			m_PeriodYY_m, m_PeriodYY_n ;
	void	set_PeriodYY_Len( long nn ) ;

	long push_PeriodYY( long xx, short yy ) ;
	void make_PeriodYY() ;
	void show_PeriodYY( long i, long show_color ) ;



	// nextData
	long	m_next_si, m_next_ei, m_next_ff ;
	void make_next_data( long comp_nn, double max_dx, long max_dyTot ) ;
	long set_NextPeriodData( long i, long comp_nn, double max_dx, long max_dyTot ) ;
	void push_NextPeriodData( long i, long next_ii ) ;
	double like_NextPeriodData( long i1, long i2, long comp_nn ) ;
	long get_DyTotFromPeriodData( long i1, long i2, long comp_nn ) ;

	void push_next_ff( long si, long next_ii ) ;
	void make_next_ff() ;
	void set_all_next_ff() ;
	void set_all_next_ff_ex( long next_ff, long val ) ;



	// TJData
	double	m_zq_val, m_zq_dx, m_zq_len ;

	long			m_TJDa_m, m_TJDa_n ;
	TJDa_Type		*m_TJDa ;

	void clear_TJDa() ;
	void set_TJDa_len( long nn ) ;

	void quick_sort_tj_data( TJDa_Type *tj_data, long iLo, long iHi ) ;
	void reverse_tj_data() ;

	void push_tj_data( double zq_dx, double zq_val ) ;
	void avg_tj_data() ;

	void remove_tj_data( long min_tot ) ;
	void remove_some_tj_data() ;
	void group_tj_data() ;
	void shrink_tj_data() ;
	double get_max_zq_val() ;

	void sort_tj_data() ;

	void clear_next_nn( long next_ff ) ;
	long make_TJDataEx( long nn ) ;
	void make_TJData() ;

	void clear_tj_data() ;
	void make_tj_data() ;
	void print_tj_data( char print_name[] ) ;


	// make_next_data_by_zq
	long	m_next_nn ;
	long make_next_data_by_zq( double zq_len, double max_dx, long max_dyTot ) ;


	long get_DyTot_by_zq( long i0, long i1, long i2 ) ;
	void push_NextData_by_zq( long i, long next_ii ) ;
	long get_Idx_by_zq( long i, double zq_val, double max_dx ) ;
	long get_PreIdx_by_zq( long i0, double zq_len, double max_dx ) ;

	
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
