// LongLongMS.h: interface for the LongLongMS class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LONGLONGMS_H__0CACC0F2_9557_4EC1_AB06_82A22F47AF63__INCLUDED_)
#define AFX_LONGLONGMS_H__0CACC0F2_9557_4EC1_AB06_82A22F47AF63__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class LongLongMS  
{
public:
	LongLongMS();
	virtual ~LongLongMS();

	LONGLONG GetTicks() ; // cpu计数

	LONGLONG GetMS0() ; // 毫秒
	LONGLONG GetUS0() ; // 微秒
	LONGLONG GetNS0() ; // 纳秒
	LONGLONG GetPS0() ; // 皮秒

	// plus 起始时间 m_StartMS
	LONGLONG GetMS() ; // 毫秒
	LONGLONG GetUS() ; // 微秒
	LONGLONG GetNS() ; // 纳秒
	LONGLONG GetPS() ; // 皮秒

private:

	LARGE_INTEGER	m_NS_Start ; // 开始的计数器
	LONGLONG		m_Freq ; // cpu的频率
	LONGLONG		m_StartMS ; // 开始的毫秒数（from year 1900）

	void Set_StartMS() ; // 计算开始时的值

};

#endif // !defined(AFX_LONGLONGMS_H__0CACC0F2_9557_4EC1_AB06_82A22F47AF63__INCLUDED_)
