// VMap.h : main header file for the VMAP application
//

#if !defined(AFX_VMAP_H__8A8C5504_9D4B_4301_85FE_31FE0AEAD5E9__INCLUDED_)
#define AFX_VMAP_H__8A8C5504_9D4B_4301_85FE_31FE0AEAD5E9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CVMapApp:
// See VMap.cpp for the implementation of this class
//

class CVMapApp : public CWinApp
{
public:
	CVMapApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVMapApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CVMapApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VMAP_H__8A8C5504_9D4B_4301_85FE_31FE0AEAD5E9__INCLUDED_)
