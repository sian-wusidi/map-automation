// VMapView.h : interface of the CVMapView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_VMAPVIEW_H__2F7CEE6C_FD26_43DA_A2B3_4BFFCE74F2F4__INCLUDED_)
#define AFX_VMAPVIEW_H__2F7CEE6C_FD26_43DA_A2B3_4BFFCE74F2F4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#define FUN_MOVE			10001	//平移
#define FUN_ZOOMIN			10002	//放大
#define FUN_ADDPOINT		10003	//加点
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
	RECT    m_ScreenRect;			//屏幕矩形
	float   m_fStartX,m_fStartY;	//屏幕左下角对应的实际坐标
	float   m_fBlc,m_fBll,m_fBl;	//及纵横比例尺，原比例尺
	int	    m_iScreenWide,m_iScreenHigh;//屏幕宽和高
	CDC     m_MemDC;				//位图CDC对象
	CBitmap m_MemBmp;				//选入内存描述表的位图指针
	long	m_iBmpX,m_iBmpY;		//位图左上角坐标
	long	m_iBmpWidth,m_iBmpHigh;	//位图的宽和高
	


	//////////////////////////////////////////////////////////////////////////
	CArray<POINT,POINT> m_PtArr;			///设备坐标....
	CPoint m_PreMousePoint;
	CPoint m_PostMousePoint;
	BOOL   m_bLButtonDown;


	/////功能...
	int     m_iFuncCurrent;			//标识当前功能
	HCURSOR m_hCursor;				//光标对象


	//记录坐标的变量
	CPoint  m_PointOrigin;			//鼠标第一次按下的点
	CPoint  m_PointOld;				//鼠标上一次按下的点
// Operations

private:
	int m_polyCount;//多边形目标数量
	int* m_pPolyPtNum; //每个多边形包含的点数
	FPOINT** m_pPolyPt ; //每个多边形的点坐标
    //自己加的--每个多边形的中心点坐标
	FPOINT* m_pPolycrPt ;
	FPOINT** m_pPoly1;
	FPOINT** m_pPoly2;
	FPOINT** m_pPoly3;
	int *tritag;//每个三角形的tag

	IDoMultipointPtr m_pInMultiPt;//输入建网的点群信息
	IDTTriArrayPtr m_pDTTriArrayPtr;//存储三角网的三角形信息
	IDTpointArrayPtr m_pDTPointArrayPtr;//存储三角网模型的顶点信息
	INTARRARY m_PointTo;    // 每个顶点所属的多边形下标
	INTARRARY m_Polypointnumarray;//每个多边形的点数（本次试验多边形目标不包括岛）

public:
	////实际坐标到设备坐标...
	void RealPtoDP(float RealX,float RealY,int& DpX,int& DpY);
	void DPtoRealP(int DpX,int DpY,float& RealX,float& RealY);
	void DrawVMap();
	void SetStatusBar(CPoint pt);


	//屏幕刷白
	void DrawRefreshStudyArea();
	//绘制一个点目标
	void DrawPoint(FPOINT retpt,COLORREF pencolor, int width);
	//绘制一条线目标
	void DrawPolyline(FPOINT* retpt, int retptNum, COLORREF pencolor, int width);
	//绘制一个多边形
	void DrawPolygon(FPOINT* retpt, int retptNum, COLORREF pencolor,COLORREF brcolor, int width);

	//绘制三角网三角形（只绘制三角形边，不填充）
	void DrawOneTri(int index_DTtri,COLORREF pencolor, int width);
	//绘制三角网三角形（填充）
	void DrawOneTri(int index_DTtri,COLORREF pencolor, COLORREF brcolor,int width);
	//绘制三角网顶点
	void DrawOneDTPoint(int index_DTPoint,COLORREF pencolor, int width);


	//创建三角网	
	void CreateTIN();
	IAGeneralizationPtr Initialization_module();

	//三角网顶点、三角形、目标间的空间关系查询函数

	//查询与某个三角网顶点关联的三角形
	void GetTris_adjacentToOnePoint(int index_DTPoint,LONGARRARY& indexList_adjacentDTTris);
	//获得与某个三角形邻接的其它三角形
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
