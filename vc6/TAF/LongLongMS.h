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

	LONGLONG GetTicks() ; // cpu����

	LONGLONG GetMS0() ; // ����
	LONGLONG GetUS0() ; // ΢��
	LONGLONG GetNS0() ; // ����
	LONGLONG GetPS0() ; // Ƥ��

	// plus ��ʼʱ�� m_StartMS
	LONGLONG GetMS() ; // ����
	LONGLONG GetUS() ; // ΢��
	LONGLONG GetNS() ; // ����
	LONGLONG GetPS() ; // Ƥ��

private:

	LARGE_INTEGER	m_NS_Start ; // ��ʼ�ļ�����
	LONGLONG		m_Freq ; // cpu��Ƶ��
	LONGLONG		m_StartMS ; // ��ʼ�ĺ�������from year 1900��

	void Set_StartMS() ; // ���㿪ʼʱ��ֵ

};

#endif // !defined(AFX_LONGLONGMS_H__0CACC0F2_9557_4EC1_AB06_82A22F47AF63__INCLUDED_)
