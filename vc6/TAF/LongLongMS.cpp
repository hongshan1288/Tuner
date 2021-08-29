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

	GetLocalTime( &SysTime ) ;	// �õ�ϵͳʱ��

	// ����Ϊ����64λ�ĺ�����
	yy = SysTime.wYear ;
	mm = SysTime.wMonth ;
	dd = SysTime.wDay ;

	dd += mdd[mm-1] ; // ���ȼ���ǰ�����µ�����
	if ( mm>2 ) // �������2�·��������������ټ�һ��
	{
		if ( ( yy % 4 == 0) && (( yy % 100 != 0) || ( yy % 400 == 0)) ) // �������������
			dd ++ ;
	}

	//�ȼ�����ǰ�������
	yy -= 1 ;			
    date = yy * 365 ;
	date += yy / 4 - yy / 100 + yy / 400 ; // ������ǰ��Щ���������

	date += dd ; // ���Ͻ��������
    date -= 693594 ; // �ټ�ȥ 1900 ����ǰ������������ֻ�� 1900 ���Ժ�ʼ��ʱ�䣬�����Ϊ����Delphi����

	ms = SysTime.wHour * 3600 + SysTime.wMinute * 60 + SysTime.wSecond ; // ���쵱ǰʱ�������
	ms *= 1000 ; // תΪ����
	ms += SysTime.wMilliseconds ; // �ټ��ϵ�ǰ�ĺ�����

	// ��ʼ���������ĺ�����
	m_StartMS = date ;	
	m_StartMS *= 86400 ; // ��ת��Ϊ��
	m_StartMS *= 1000 ;	// ��ת��Ϊ����
	m_StartMS += ms ;	// �����ϵ�ǰ�ĺ�����

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
