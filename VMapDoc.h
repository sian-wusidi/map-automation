// VMapDoc.h : interface of the CVMapDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_VMAPDOC_H__90790933_2CBB_4DA4_A3D8_882953812084__INCLUDED_)
#define AFX_VMAPDOC_H__90790933_2CBB_4DA4_A3D8_882953812084__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AllList.h"

class CVMapDoc : public CDocument
{
protected: // create from serialization only
	CVMapDoc();
	DECLARE_DYNCREATE(CVMapDoc)

// Attributes
public:
	FILE	 *m_fp;		  //文件指针
	CFile m_Shp;
	CAllList *m_IndexList;//索引链表根结点变量，在构造函数里开辟空间,在析构函数里释放空间。

	/////代表图形范围....
	float	m_fMaxX;	 
	float	m_fMinX;	 
	float	m_fMaxY; 	 
	float	m_fMinY;   

// Operations
public:
	void ReadDataFromZBA();
	void ReadDataFromShp(char* lpszFileName);
	void SetShpMapExt(char* lpszFileName);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVMapDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual void DeleteContents();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CVMapDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CVMapDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VMAPDOC_H__90790933_2CBB_4DA4_A3D8_882953812084__INCLUDED_)
