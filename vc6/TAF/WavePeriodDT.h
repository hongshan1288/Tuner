
#if !defined(WavePeriodDT__INCLUDED_)
#define WavePeriodDT__INCLUDED_
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
	short	ff, vv ;
	long	ix ;
	long	PeriodYY_si ;
	long	PeriodYY_nn ;
	long	next_ii ;
	long	next_ff ;
	long	next_nn ;
} ;
//////////////////////////////////////////////////////////////////////
struct	TJDa_Type
{
	double	zq_dx ;
	double	zq_val ;
	long	zq_tot ;
	double	zq_sum ;
} ;
//////////////////////////////////////////////////////////////////////

#endif // !defined(AWavePeriodDT__INCLUDED_)
