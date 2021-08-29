// LongLongMS.cpp: implementation of the LongLongMS class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LongLongMS.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
LongLongMS::LongLongMS()
{
	m_StartMS = 0 ;
	Set_StartMS() ;

	// Initialize timer variables
	LARGE_INTEGER	lv ;
	QueryPerformanceCounter( &m_NS_Start ); // get start count
	QueryPerformanceFrequency( &lv ) ; // get freq of cpu
	m_Freq = lv.QuadPart ;
}
//////////////////////////////////////////////////////////////////////
LongLongMS::~LongLongMS()
{

}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void LongLongMS::Set_StartMS()
{
//	long	md[12] =  { 31, 28, 31,  30,  31,  30,  31,  31,  30,  31,  30,  31 } ;
	long	mdd[12] = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 } ;

//	long	daysDelta = 693594 ; // Days between 1/1/0001 and 12/31/1899
//	long	daysDelta = 730485 ; // Days from 2000.1.1
	long	dd, mm, yy, date, ms ;

	SYSTEMTIME	SysTime ;

	GetLocalTime( &SysTime ) ;	// 得到系统时间

	// 以下为计算64位的毫秒数
	yy = SysTime.wYear ;
	mm = SysTime.wMonth ;
	dd = SysTime.wDay ;

	dd += mdd[mm-1] ; // 首先加上前几个月的天数
	if ( mm>2 ) // 如果大于2月份且又是闰年则再加一天
	{
		if ( ( yy % 4 == 0) && (( yy % 100 != 0) || ( yy % 400 == 0)) ) // 如果今年是闰年
			dd ++ ;
	}

	//先计算以前年的天数
	yy -= 1 ;			
    date = yy * 365 ;
	date += yy / 4 - yy / 100 + yy / 400 ; // 加上以前那些年的闰天数

	date += dd ; // 加上今年的天数
    date -= 693594 ; // 再减去 1900 年以前的天数，我们只从 1900 年以后开始算时间，这个是为了与Delphi兼容

	ms = SysTime.wHour * 3600 + SysTime.wMinute * 60 + SysTime.wSecond ; // 今天当前时间的秒数
	ms *= 1000 ; // 转为毫秒
	ms += SysTime.wMilliseconds ; // 再加上当前的毫秒数

	// 开始计算天数的毫秒数
	m_StartMS = date ;	
	m_StartMS *= 86400 ; // 先转换为秒
	m_StartMS *= 1000 ;	// 在转换为毫秒
	m_StartMS += ms ;	// 最后加上当前的毫秒数

}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
LONGLONG LongLongMS::GetTicks()
{
	LARGE_INTEGER	lv ;
	QueryPerformanceCounter( &lv );
	return ( lv.QuadPart-m_NS_Start.QuadPart ) ;
}
//////////////////////////////////////////////////////////////////////
LONGLONG LongLongMS::GetMS0()
{
	LARGE_INTEGER	lv ;
	QueryPerformanceCounter( &lv );
	return ( 1000*(lv.QuadPart-m_NS_Start.QuadPart)/m_Freq ) ;
}
//////////////////////////////////////////////////////////////////////
LONGLONG LongLongMS::GetUS0()
{
	LARGE_INTEGER	lv ;
	QueryPerformanceCounter( &lv );
	return ( 1000000*(lv.QuadPart-m_NS_Start.QuadPart)/m_Freq ) ;
}
//////////////////////////////////////////////////////////////////////
LONGLONG LongLongMS::GetNS0()
{
	LARGE_INTEGER	lv ;
	QueryPerformanceCounter( &lv );
	return ( 1000000000*(lv.QuadPart-m_NS_Start.QuadPart)/m_Freq ) ;
}
//////////////////////////////////////////////////////////////////////
LONGLONG LongLongMS::GetPS0()
{
	LARGE_INTEGER	lv ;
	QueryPerformanceCounter( &lv );
	return ( 1000000000000*(lv.QuadPart-m_NS_Start.QuadPart)/m_Freq ) ;
}
//////////////////////////////////////////////////////////////////////
LONGLONG LongLongMS::GetMS()
{
	return ( m_StartMS + GetMS0()) ;
}
//////////////////////////////////////////////////////////////////////
LONGLONG LongLongMS::GetUS()
{
	return ( m_StartMS*1000 + GetUS0()) ;
}
//////////////////////////////////////////////////////////////////////
LONGLONG LongLongMS::GetNS()
{
	return ( m_StartMS*1000000 + GetNS0()) ;
}
//////////////////////////////////////////////////////////////////////
LONGLONG LongLongMS::GetPS()
{
	return ( m_StartMS*1000000000 + GetPS0()) ;
}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
