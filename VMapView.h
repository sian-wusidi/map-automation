// VMapView.h : interface of the CVMapView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_VMAPVIEW_H__2F7CEE6C_FD26_43DA_A2B3_4BFFCE74F2F4__INCLUDED_)
#define AFX_VMAPVIEW_H__2F7CEE6C_FD26_43DA_A2B3_4BFFCE74F2F4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#define FUN_MOVE			10001	//ƽ��
#define FUN_ZOOMIN			10002	//�Ŵ�
#define FUN_ADDPOINT		10003	//�ӵ�
#define FUN_ADDLINE         10004 //
#define FUN_ADDPOLY			10005

typedef CArray<int,int> INTARRARY;
typedef CArray<long,long> LONGARRARY;

class CVMapView : public CView
{
protected: // create from serialization only
	CVMapView();
	DECLARE_DYNCREATE(CVMapView)

// Attributes
public:
	CVMapDoc* GetDocument();
	RECT    m_ScreenRect;			//��Ļ����
	float   m_fStartX,m_fStartY;	//��Ļ���½Ƕ�Ӧ��ʵ������
	float   m_fBlc,m_fBll,m_fBl;	//���ݺ�����ߣ�ԭ������
	int	    m_iScreenWide,m_iScreenHigh;//��Ļ��͸�
	CDC     m_MemDC;				//λͼCDC����
	CBitmap m_MemBmp;				//ѡ���ڴ��������λͼָ��
	long	m_iBmpX,m_iBmpY;		//λͼ���Ͻ�����
	long	m_iBmpWidth,m_iBmpHigh;	//λͼ�Ŀ�͸�
	


	//////////////////////////////////////////////////////////////////////////
	CArray<POINT,POINT> m_PtArr;			///�豸����....
	CPoint m_PreMousePoint;
	CPoint m_PostMousePoint;
	BOOL   m_bLButtonDown;


	/////����...
	int     m_iFuncCurrent;			//��ʶ��ǰ����
	HCURSOR m_hCursor;				//������


	//��¼����ı���
	CPoint  m_PointOrigin;			//����һ�ΰ��µĵ�
	CPoint  m_PointOld;				//�����һ�ΰ��µĵ�
// Operations

private:
	int m_polyCount;//�����Ŀ������
	int* m_pPolyPtNum; //ÿ������ΰ����ĵ���
	FPOINT** m_pPolyPt ; //ÿ������εĵ�����
    //�Լ��ӵ�--ÿ������ε����ĵ�����
	FPOINT* m_pPolycrPt ;
	FPOINT** m_pPoly1;
	FPOINT** m_pPoly2;
	FPOINT** m_pPoly3;
	int *tritag;//ÿ�������ε�tag

	IDoMultipointPtr m_pInMultiPt;//���뽨���ĵ�Ⱥ��Ϣ
	IDTTriArrayPtr m_pDTTriArrayPtr;//�洢����������������Ϣ
	IDTpointArrayPtr m_pDTPointArrayPtr;//�洢������ģ�͵Ķ�����Ϣ
	INTARRARY m_PointTo;    // ÿ�����������Ķ�����±�
	INTARRARY m_Polypointnumarray;//ÿ������εĵ�����������������Ŀ�겻��������

public:
	////ʵ�����굽�豸����...
	void RealPtoDP(float RealX,float RealY,int& DpX,int& DpY);
	void DPtoRealP(int DpX,int DpY,float& RealX,float& RealY);
	void DrawVMap();
	void SetStatusBar(CPoint pt);


	//��Ļˢ��
	void DrawRefreshStudyArea();
	//����һ����Ŀ��
	void DrawPoint(FPOINT retpt,COLORREF pencolor, int width);
	//����һ����Ŀ��
	void DrawPolyline(FPOINT* retpt, int retptNum, COLORREF pencolor, int width);
	//����һ�������
	void DrawPolygon(FPOINT* retpt, int retptNum, COLORREF pencolor,COLORREF brcolor, int width);

	//���������������Σ�ֻ���������αߣ�����䣩
	void DrawOneTri(int index_DTtri,COLORREF pencolor, int width);
	//���������������Σ���䣩
	void DrawOneTri(int index_DTtri,COLORREF pencolor, COLORREF brcolor,int width);
	//��������������
	void DrawOneDTPoint(int index_DTPoint,COLORREF pencolor, int width);


	//����������	
	void CreateTIN();
	IAGeneralizationPtr Initialization_module();

	//���������㡢�����Ρ�Ŀ���Ŀռ��ϵ��ѯ����

	//��ѯ��ĳ�����������������������
	void GetTris_adjacentToOnePoint(int index_DTPoint,LONGARRARY& indexList_adjacentDTTris);
	//�����ĳ���������ڽӵ�����������
	void GetTris_adjacentToOneTri(int index_DTTri,INTARRARY& indexList_adjacentDTTris);


	void FreeBuffer();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVMapView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);
	virtual void OnInitialUpdate();
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	void DrawOneTrinew(int index_DTtri,COLORREF pencolor,int width);
	void CreateTINnew();
	virtual ~CVMapView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CVMapView)
	afx_msg void OnMove();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnWholemap();
	afx_msg void OnZoomin();
	afx_msg void OnAddpoint();
	afx_msg void OnAddline();
	afx_msg void OnAddpoly();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnUpdateAddline(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAddpoint(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAddpoly(CCmdUI* pCmdUI);
	afx_msg void OnUpdateZoomin(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMove(CCmdUI* pCmdUI);
	afx_msg void OnTest();
	afx_msg void OnGroup();
	afx_msg void Oncentrepoint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in VMapView.cpp
inline CVMapDoc* CVMapView::GetDocument()
   { return (CVMapDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VMAPVIEW_H__2F7CEE6C_FD26_43DA_A2B3_4BFFCE74F2F4__INCLUDED_)
