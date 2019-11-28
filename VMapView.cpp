// VMapView.cpp : implementation of the CVMapView class
//

#include "stdafx.h"
#include "VMap.h"

#include "VMapDoc.h"
#include "VMapView.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVMapView

IMPLEMENT_DYNCREATE(CVMapView, CView)

BEGIN_MESSAGE_MAP(CVMapView, CView)
	//{{AFX_MSG_MAP(CVMapView)
	ON_COMMAND(ID_MOVE, OnMove)
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_COMMAND(ID_WHOLEMAP, OnWholemap)
	ON_COMMAND(ID_ZOOMIN, OnZoomin)
	ON_COMMAND(ID_ADDPOINT, OnAddpoint)
	ON_COMMAND(ID_ADDLINE, OnAddline)
	ON_COMMAND(ID_ADDPOLY, OnAddpoly)
	ON_WM_LBUTTONDBLCLK()
	ON_UPDATE_COMMAND_UI(ID_ADDLINE, OnUpdateAddline)
	ON_UPDATE_COMMAND_UI(ID_ADDPOINT, OnUpdateAddpoint)
	ON_UPDATE_COMMAND_UI(ID_ADDPOLY, OnUpdateAddpoly)
	ON_UPDATE_COMMAND_UI(ID_ZOOMIN, OnUpdateZoomin)
	ON_UPDATE_COMMAND_UI(ID_MOVE, OnUpdateMove)
	ON_COMMAND(ID_TEST, OnTest)
	ON_COMMAND(ID_GROUP, OnGroup)
	ON_COMMAND(ID_centrepoint, Oncentrepoint)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVMapView construction/destruction

CVMapView::CVMapView()
{
	// TODO: add construction code here
	m_iBmpX=0;
	m_iBmpY=0;
	m_iFuncCurrent = 0;
	m_PtArr.RemoveAll();
	m_bLButtonDown = FALSE;

	m_polyCount=0;//多边形目标数量
	m_pPolyPtNum=NULL; //每个多边形包含的点数
	m_pPolyPt=NULL ; //每个多边形的点坐标
	m_pPolycrPt=NULL;//每个多边形中心点坐标--自己加的
	m_pPoly1=NULL;
	m_pPoly2=NULL;
	m_pPoly3=NULL;//zijijiade
	tritag=NULL;
	m_pDTPointArrayPtr=NULL;//存储三角网模型的顶点信息
	m_pDTTriArrayPtr=NULL;//存储三角网的三角形信息
}

CVMapView::~CVMapView()
{
}

BOOL CVMapView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CVMapView drawing

void CVMapView::OnDraw(CDC* pDC)
{
	CVMapDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	DrawVMap();

	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CVMapView printing

BOOL CVMapView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CVMapView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CVMapView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CVMapView diagnostics

#ifdef _DEBUG
void CVMapView::AssertValid() const
{
	CView::AssertValid();
}

void CVMapView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CVMapDoc* CVMapView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CVMapDoc)));
	return (CVMapDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CVMapView message handlers

void CVMapView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo) 
{
	// TODO: Add your specialized code here and/or call the base class
	CView::OnPrepareDC(pDC, pInfo);
}

void CVMapView::OnInitialUpdate() 
{
	CView::OnInitialUpdate();
	
	


	// TODO: Add your specialized code here and/or call the base class
	CVMapDoc* pDoc = GetDocument();
	GetClientRect(&m_ScreenRect);


	//计算比例尺
    m_fBlc=(pDoc->m_fMaxX-pDoc->m_fMinX)/(m_ScreenRect.right-m_ScreenRect.left-20);//x轴比例尺
	m_fBll=(pDoc->m_fMaxY-pDoc->m_fMinY)/(m_ScreenRect.bottom-m_ScreenRect.top-20);//y轴比例尺
	if(m_fBll>m_fBlc) 
		m_fBlc=m_fBll;
	m_fBl=m_fBlc;
	//计算屏幕的高和宽
	m_iScreenHigh=m_ScreenRect.bottom-m_ScreenRect.top;
	m_iScreenWide=m_ScreenRect.right-m_ScreenRect.left;


	////屏幕转换参数m_fStartX、m_fStartY、m_fBl
    //以图廓中心对应屏幕中心
	m_fStartX=(float)((pDoc->m_fMaxX+pDoc->m_fMinX-m_iScreenWide*m_fBlc)/2);
	m_fStartY=(float)((pDoc->m_fMaxY+pDoc->m_fMinY-m_iScreenHigh*m_fBlc)/2);
	m_fBl;
}


void CVMapView::RealPtoDP(float RealX,float RealY,int& DpX,int& DpY)
{
	
	//实际坐标转换成屏幕坐标
	DpX=(int)((RealX-m_fStartX)/m_fBlc);
	DpY=(int)(m_iScreenHigh-(RealY-m_fStartY)/m_fBlc);
}


void CVMapView::DPtoRealP(int DpX,int DpY,float& RealX,float& RealY)
{
	RealX = RealY = -1;
	RealX = DpX*m_fBlc+m_fStartX;
	RealY = m_iScreenHigh*m_fBlc +m_fStartY-DpY*m_fBlc;

}

void CVMapView::DrawVMap()	
{
	
	POSITION pos=NULL;
	CVMapDoc* pDoc = GetDocument();
	CPointList  *pPointList;	
	CLineList   *pLineList;
	CAreaList   *pAreaList;
	int iScreenX,iScreenY;		//屏幕坐标

	//先给位图贴上白背景
	CBrush backgroundBrush((COLORREF) ::GetSysColor(COLOR_WINDOW));
	CBrush* pOldBrush=m_MemDC.SelectObject(&backgroundBrush);
    m_MemDC.PatBlt(0,0,m_iBmpWidth,m_iBmpHigh,PATCOPY);
	m_MemDC.SelectObject(pOldBrush);

	
	

	COLORREF brcolor = RGB(100,100,100);
	COLORREF pencolor = RGB(0,0,255);

	CBrush  br(brcolor);
	CPen  pen(PS_SOLID,0,pencolor);
	for(pos=pDoc->m_IndexList->m_AreaList.GetHeadPosition();pos!=NULL;)
	{//画面要素
		int   ii=0;
		float fTempX;
		float fTempY;
		pAreaList=pDoc->m_IndexList->m_AreaList.GetNext(pos);
		if(pAreaList->bFlagIsDeleted==FALSE)
		{//如果面的边线没有被删除才画
			m_MemDC.SelectObject(br);
			m_MemDC.SelectObject(pen);
			POINT* dPts= new POINT [pAreaList->lPointNumbers];
			for (ii=0;ii<pAreaList->lPointNumbers;ii++)
			{
				fTempX=pAreaList->PointArrayX[ii];
				fTempY=pAreaList->PointArrayY[ii];
				iScreenX=(int)((fTempX-m_fStartX)/m_fBlc);
				iScreenY=(int)(m_iScreenHigh-(fTempY-m_fStartY)/m_fBlc);
				dPts[ii].x = iScreenX;
				dPts[ii].y = iScreenY;
				//m_MemDC.LineTo(iScreenX,iScreenY);
			}
			m_MemDC.Polygon(dPts,pAreaList->lPointNumbers);
			delete [] dPts;
		}
	}

	CPen NewPen_Black(PS_SOLID,1,RGB(255,0,0));
	CPen		*pOldPen;
	for(pos=pDoc->m_IndexList->m_LineList.GetHeadPosition();pos!=NULL;)
	{//画线要素
		int   ii=0;
		float fTempX;
		float fTempY;
		pLineList=pDoc->m_IndexList->m_LineList.GetNext(pos);
		if(pLineList->bFlagIsDeleted==FALSE)
		{//如果线没有被删除才画
			pOldPen=m_MemDC.SelectObject(&NewPen_Black);//
			fTempX=pLineList->PointArrayX[0];
			fTempY=pLineList->PointArrayY[0];
			//实际坐标转换成屏幕坐标
			iScreenX=(int)((fTempX-m_fStartX)/m_fBlc);
			iScreenY=(int)(m_iScreenHigh-(fTempY-m_fStartY)/m_fBlc);
			m_MemDC.MoveTo(iScreenX,iScreenY);
			for (ii=0;ii<pLineList->lPointNumbers;ii++)
			{
				fTempX=pLineList->PointArrayX[ii];
				fTempY=pLineList->PointArrayY[ii];
				iScreenX=(int)((fTempX-m_fStartX)/m_fBlc);
				iScreenY=(int)(m_iScreenHigh-(fTempY-m_fStartY)/m_fBlc);
				m_MemDC.LineTo(iScreenX,iScreenY);
			}
			
		}
	}	



	CPen  pen1(PS_SOLID,0,RGB(0,255,0));
	for(pos=pDoc->m_IndexList->m_PointList.GetHeadPosition();pos!=NULL;)
	{//画点要素
		int   ii=0;			//循环变量
		float fTempX,fTempY;//临时变量
		pPointList=pDoc->m_IndexList->m_PointList.GetNext(pos);
		if(pPointList->bFlagIsDeleted==FALSE)
		{//如果点没有被删除才画
			m_MemDC.SelectObject(pen1);
			fTempX=pPointList->PointArrayX[0];
			fTempY=pPointList->PointArrayY[0];
			//实际坐标转换成屏幕坐标
			iScreenX=(int)((fTempX-m_fStartX)/m_fBlc);
			iScreenY=(int)(m_iScreenHigh-(fTempY-m_fStartY)/m_fBlc);
			m_MemDC.MoveTo(iScreenX-4,iScreenY);
			m_MemDC.LineTo(iScreenX+4,iScreenY);
			m_MemDC.MoveTo(iScreenX,iScreenY-4);
			m_MemDC.LineTo(iScreenX,iScreenY+4);
			m_MemDC.MoveTo(iScreenX,iScreenY);
			for (ii=0;ii<pPointList->lPointNumbers;ii++)
			{
				fTempX=pPointList->PointArrayX[ii];
				fTempY=pPointList->PointArrayY[ii];
				iScreenX=(int)((fTempX-m_fStartX)/m_fBlc);
				iScreenY=(int)(m_iScreenHigh-(fTempY-m_fStartY)/m_fBlc);
				m_MemDC.LineTo(iScreenX,iScreenY);
			}
			
			}
		
	}

//
//	m_MemDC.SelectObject(pOldPen);	

	//把位图贴在屏幕上
	CRect Rect;
	GetClientRect(Rect);
	CDC* dc = GetDC();
	dc->BitBlt(0,0,Rect.Width(),Rect.Height(),&m_MemDC,m_iBmpX,m_iBmpY,SRCCOPY);
}



void CVMapView::CreateTIN()
{
	////////////////////////////////////////////////////////////////////////////////////
	//Step1: 初始化组件接口
	//由于此处三角网创建函数由一个com组件库提供，需要com组件相关操作。但是大家不必深究。
	////////////////////////////////////////////////////////////////////////////////////
	IAGeneralizationPtr _iGenCore = Initialization_module();



	////////////////////////////////////////////////////////////////////////////	
	//Step2: 构建多边形群Delaunay三角网模型
	////////////////////////////////////////////////////////////////////////////

	//Step2-1:将多边形目标组织为点群结构作为建网输入数据）
	m_pInMultiPt.CreateInstance(_uuidof(DoMultipoint));//首先，创建一个点串对象，装载构成多边形的点群数据
	
	//根据多边形中心生成三角网//
	////////////////////////////////
	/*int i,j;
	for (i=0;i<m_polyCount;i++) 
	{
		int count_pt=0;
		for (j=0;j<m_polyCount;j++)//注意，多边形首尾点相同，预处理时去除末尾点(避免重复点)
		{
			IDoPointPtr pDoPt;		       
			pDoPt.CreateInstance(_uuidof(doPoint));
			pDoPt->put_RealX(m_pPolycrPt[j].x);
			pDoPt->put_RealY(m_pPolycrPt[j].y);
	 		m_pInMultiPt->AddPoint(pDoPt);
			m_PointTo.Add(i);
			count_pt++;
		}
		m_Polypointnumarray.Add(count_pt); //记录每个多边形的点数
	}*/

	int i,j;
	for (i=0;i<m_polyCount;i++) 
	{
		int count_pt=0;
		for (j=0;j<m_pPolyPtNum[i]-1;j++)//注意，多边形首尾点相同，预处理时去除末尾点(避免重复点)
		{
			IDoPointPtr pDoPt;		       
			pDoPt.CreateInstance(_uuidof(doPoint));
			pDoPt->put_RealX(m_pPolyPt[i][j].x);
			pDoPt->put_RealY(m_pPolyPt[i][j].y);
	 		m_pInMultiPt->AddPoint(pDoPt);
			m_PointTo.Add(i);
			count_pt++;
		}
		m_Polypointnumarray.Add(count_pt); //记录每个多边形的点数
	}



	
	//Step2-2: 实施建网

	//初始化三角网顶点串接口对象(pOutDTPointArrayPtr)，用于记录建网后输出的三角网顶点信息
	m_pDTPointArrayPtr.CreateInstance(_uuidof(DTpointArray)); 

	//初始化三角网三角形串接口对象(pOutDTTriArrayPtr)，装载建网后输出的三角网三角形信息
	m_pDTTriArrayPtr.CreateInstance(_uuidof(DTTriArray));  

	//获得Delaunay三角网模型对象接口(pIDelaunayTIN)，由改对象接口提供三角网建立函数（直接调用，不用关心内部机制）
   	IAGeneralizationPtr _iGenProcHandler = (IAGeneralizationPtr)_iGenCore; 
	IDelaunayTINPtr pIDelaunayTIN;
	HRESULT res =  _iGenProcHandler->get_DelaunayTIN(&pIDelaunayTIN); 

	//构建三角网,pInMultiPt—>输入的参与建网点串信息，pOut_DTPointArrayPtr—>输出的三角网顶点信息，pOut_DTTriArrayPtr—>输出的三角网三角形信息
	HRESULT	hr = pIDelaunayTIN->CreatTIN(m_pInMultiPt,&m_pDTPointArrayPtr,&m_pDTTriArrayPtr);

}
//Denaulay三角网组件初始化函数
IAGeneralizationPtr CVMapView::Initialization_module()
{
	CoInitialize(NULL);   
	IGeneralizerCorePtr _iGenCore;	
	_iGenCore.CreateInstance(_uuidof(GeneralizerCore));
	int nSourceScale =2000;
	int nTargetScale = 10000;
	_iGenCore->InitGenEvironment(nSourceScale,nTargetScale,1,domapUnitsMeter);		
	CString strstr;
	char    szModuleName[MAX_PATH];
	GetModuleFileName(AfxGetApp()->m_hInstance,(LPSTR)szModuleName,sizeof(szModuleName));	
	strstr = szModuleName;
	strstr= strstr.Left(strstr.GetLength()-8);
	strstr +=_T("\\rule.inf"); 
	_iGenCore->InitSysParaTable(_bstr_t(strstr));
	
	return _iGenCore;
}

//绘制一个三角网三角形(index_DTtri—三角形下标，pencolor—画笔颜色，width—画笔宽度)
void CVMapView::DrawOneTri(int index_DTtri, COLORREF pencolor, int width)
{
	if (index_DTtri<0)
	{
		AfxMessageBox("绘制的三角网三角形下标错误!");
		return;
	}

	IDTTriPtr oneDTTri;
	m_pDTTriArrayPtr->get_element(index_DTtri,&oneDTTri); //获得下标为i的三角形

	long no1,no2,no3;
	oneDTTri->get_PointNo1(&no1); //三角形三个顶点的编号
	oneDTTri->get_PointNo2(&no2);
	oneDTTri->get_PointNo3(&no3);
	
	IDTPointPtr DTPoint1,DTPoint2,DTPoint3;
	m_pDTPointArrayPtr->get_element(no1,&DTPoint1); //根据编号在输出顶点串中（pOut_DTPointArrayPt）得到顶点对象
	m_pDTPointArrayPtr->get_element(no2,&DTPoint2);
	m_pDTPointArrayPtr->get_element(no3,&DTPoint3);
	
	
	FPOINT* ptList = NULL;
	ptList = new FPOINT[4];
	
	double xx,yy;
	DTPoint1->get_coorX(&xx);//获得每个三角形顶点的坐标
	DTPoint1->get_coorY(&yy);
	ptList[0].x = xx;
	ptList[0].y = yy;
	
	DTPoint2->get_coorX(&xx);
	DTPoint2->get_coorY(&yy);
	ptList[1].x = xx;
	ptList[1].y = yy;
	
	DTPoint3->get_coorX(&xx);
	DTPoint3->get_coorY(&yy);
	ptList[2].x = xx;
	ptList[2].y = yy;
	
	ptList[3].x = ptList[0].x;
	ptList[3].y = ptList[0].y;
	//自己加的
	/*int d1,d2,d3,dmax;
	dmax=0;
	d1=(ptList[0].x-ptList[1].x)*(ptList[0].x-ptList[1].x)+(ptList[0].y-ptList[1].y)*(ptList[0].y-ptList[1].y);
	if(d1>dmax) dmax=d1;
	d2=(ptList[1].x-ptList[2].x)*(ptList[1].x-ptList[2].x)+(ptList[1].y-ptList[2].y)*(ptList[1].y-ptList[2].y);
	if(d2>dmax) dmax=d2;
	d3=(ptList[2].x-ptList[3].x)*(ptList[2].x-ptList[3].x)+(ptList[2].y-ptList[3].y)*(ptList[2].y-ptList[3].y);
	if(d3>dmax) dmax=d3;
	
	if(dmax<4000)*/
	DrawPolyline(ptList,3,pencolor, width);	//绘制三角形边信息
	if (ptList)
	{
		delete[] ptList;
		ptList = NULL;
	}	

}


//绘制一个三角网三角形(index_DTtri—三角形下标，pencolor—画笔颜色， brcolor—填充颜色，width—画笔宽度)
void CVMapView::DrawOneTri(int index_DTtri,COLORREF pencolor, COLORREF brcolor,int width)
{
	if (index_DTtri<0)
	{
		AfxMessageBox("绘制的三角网三角形下标错误!");
		return;
	}
	
	IDTTriPtr oneDTTri;
	m_pDTTriArrayPtr->get_element(index_DTtri,&oneDTTri); //获得下标为i的三角形
	
	long no1,no2,no3;
	oneDTTri->get_PointNo1(&no1); //三角形三个顶点的编号
	oneDTTri->get_PointNo2(&no2);
	oneDTTri->get_PointNo3(&no3);
	
	IDTPointPtr DTPoint1,DTPoint2,DTPoint3;
	m_pDTPointArrayPtr->get_element(no1,&DTPoint1); //根据编号在输出顶点串中（pOut_DTPointArrayPt）得到顶点对象
	m_pDTPointArrayPtr->get_element(no2,&DTPoint2);
	m_pDTPointArrayPtr->get_element(no3,&DTPoint3);
	
	
	FPOINT* ptList = NULL;
	ptList = new FPOINT[4];
	
	double xx,yy;
	DTPoint1->get_coorX(&xx);//获得每个三角形顶点的坐标
	DTPoint1->get_coorY(&yy);
	ptList[0].x = xx;
	ptList[0].y = yy;
	
	DTPoint2->get_coorX(&xx);
	DTPoint2->get_coorY(&yy);
	ptList[1].x = xx;
	ptList[1].y = yy;
	
	DTPoint3->get_coorX(&xx);
	DTPoint3->get_coorY(&yy);
	ptList[2].x = xx;
	ptList[2].y = yy;
	
	ptList[3].x = ptList[0].x;
	ptList[3].y = ptList[0].y;

	DrawPolygon(ptList,4,pencolor,brcolor,width);
	if (ptList)
	{
		delete[] ptList;
		ptList = NULL;
	}	

}

//绘制一个三角网顶点(index_DTPoint—顶点下标，pencolor—画笔颜色，width—画笔宽度)
void CVMapView::DrawOneDTPoint(int index_DTPoint,COLORREF pencolor, int width)
{
	IDTPointPtr oneDTPoint;
	m_pDTPointArrayPtr->get_element(index_DTPoint,&oneDTPoint);//获得下标为i的三角网顶点对象
	double xx,yy;
	oneDTPoint->get_coorX(&xx);//获得顶点的坐标
	oneDTPoint->get_coorY(&yy);
	FPOINT onePt;
	onePt.x = xx;
	onePt.y = yy;
	DrawPoint(onePt,pencolor,width);//绘制点信息

}

//点绘制函数
void CVMapView::DrawPoint(FPOINT retpt,COLORREF pencolor, int width)
{ 
	CDC* dc = GetDC();
 	CPen pen(PS_SOLID,2,pencolor);
 	CPen* old = dc->SelectObject(&pen);
 	CBrush  br(pencolor);
 	CBrush* poldBr = dc->SelectObject(&br);
 	int a=width;
	int x1,y1;
	RealPtoDP(retpt.x,retpt.y,x1,y1);
	dc->Ellipse(x1-width,y1-width,x1+width,y1+width);
 	dc->SelectObject(poldBr);
 	dc->SelectObject(old);
	ReleaseDC(dc);
	return;
}

//线绘制函数
void CVMapView::DrawPolyline(FPOINT* retpt, int retptNum, COLORREF pencolor, int width)
{   

	CDC* dc = GetDC();
	POINT* newPt=  new POINT [retptNum];
	int tmpX,tmpY;
	tmpX = tmpY = -1;
	for (int i=0;i<retptNum;i++)
	{	
		RealPtoDP(retpt[i].x,retpt[i].y,tmpX,tmpY);
		newPt[i].x = tmpX;
		newPt[i].y = tmpY;
	}

	if(width>0)
	{ 
		CPen pen(PS_SOLID,width,pencolor);
		CPen* old = dc->SelectObject(&pen);

		dc->Polyline(newPt,retptNum);
		
		dc->SelectObject(old);
		ReleaseDC(dc);
		
	}
	else
	{ 
		CPen pen(PS_DOT,1 ,pencolor);
		CPen* old = dc->SelectObject(&pen);
		if(width<-100 )  dc->SetROP2(R2_XORPEN);
		//dc->Polyline(newPt,retptNum);
		dc->SelectObject(old);
		ReleaseDC(dc);
	}
	delete [] newPt;
	return;
}

//将屏幕绘图区域刷白，清除之前的图形痕迹
void CVMapView::DrawRefreshStudyArea()
{	
	CDC* dc = GetDC();
	CPen pen(PS_SOLID,1,RGB(240,240,240));
	CPen* old = dc->SelectObject(&pen);
	CBrush  br(RGB(255,255,255));
	CBrush* poldBr = dc->SelectObject(&br);
	dc->Rectangle(-5000,-5000,5000,5000);
	dc->SelectObject(poldBr);
    dc->SelectObject(old);
	ReleaseDC(dc);
	return;
}


//查询与某个三角网顶点相连接的三角网三角形
//index_DTPoint—输入的顶点编号，indexList_adjacentDTTris—输出的连接三角形下标数组
void CVMapView::GetTris_adjacentToOnePoint(int index_DTPoint,LONGARRARY& indexList_adjacentDTTris)
{
	IDTPointPtr oneDTPoint;
	m_pDTPointArrayPtr->get_element(index_DTPoint,&oneDTPoint);//获得下标为i的三角网顶点对象

	if (oneDTPoint)
	{
		long nCount_adj;
	//	nCount_adj = oneDTPoint->Get_nCount_RelateTri();
		oneDTPoint->get_nCount_RelateTri(&nCount_adj);
		for (int i=0;i<nCount_adj;i++)
		{
			long index_adj;
			oneDTPoint->get_TriNo_relate(i, &index_adj);
		
			indexList_adjacentDTTris.Add(index_adj);
		}
	}

	
}


//查询与某个三角网三角形相邻近的其它三角网三角形
//index_DTPoint—输入的三角形编号，indexList_adjacentDTTris—输出的邻近三角形下标数组
void CVMapView::GetTris_adjacentToOneTri(int index_DTTri,INTARRARY& indexList_adjacentDTTris)
{
	IDTTriPtr oneDTTri;
	m_pDTTriArrayPtr->get_element(index_DTTri,&oneDTTri);
	
	long ajTri_index1, ajTri_index2, ajTri_index3;
	oneDTTri->get_AdjaceTriNo1(&ajTri_index1);
	oneDTTri->get_AdjaceTriNo2(&ajTri_index2); 
	oneDTTri->get_AdjaceTriNo3(&ajTri_index3);  //与三条边分别邻接的三角形（三角形串的位置下标）

	if (ajTri_index1>-1)
	{
		indexList_adjacentDTTris.Add(ajTri_index1);
	}

	if (ajTri_index2>-1)
	{
		indexList_adjacentDTTris.Add(ajTri_index2);
	}

	if (ajTri_index3>-1)
	{
		indexList_adjacentDTTris.Add(ajTri_index3);
	}

}

//多边形绘制函数
void CVMapView::DrawPolygon(FPOINT* retpt, int retptNum, 
							COLORREF pencolor,COLORREF brcolor, int width)
{   
    CDC* dc = GetDC();
	POINT* newPt = new POINT [retptNum];
	int tmpX,tmpY;
	tmpX = tmpY = -1;
	for (int i=0;i<retptNum;i++)
	{	
		RealPtoDP(retpt[i].x,retpt[i].y,tmpX,tmpY);
		newPt[i].x = tmpX;
		newPt[i].y = tmpY;
	}

	if(width>0)
	{ CPen pen(PS_SOLID,width,pencolor);
 	  CPen* old = dc->SelectObject(&pen);
	  CBrush  br(brcolor);
	  CBrush* poldBr = dc->SelectObject(&br);
	  dc->Polygon(newPt,retptNum);
	  dc->SelectObject(poldBr);
      dc->SelectObject(old);
	  ReleaseDC(dc);
    }
	else
	{ 
		CPen pen(PS_DOT,10,pencolor);
		CPen* old = dc->SelectObject(&pen);
		if(width<-100)  dc->SetROP2(R2_XORPEN);
		CBrush  br(brcolor);
		CBrush* poldBr = dc->SelectObject(&br);
		dc->Polygon(newPt,retptNum);
		dc->SelectObject(poldBr);
		dc->SelectObject(old);
		ReleaseDC(dc);	  
    }
	delete [] newPt;
    return;
}

BOOL CVMapView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	// TODO: Add your specialized code here and/or call the base class
	BOOL bFlag=CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
	//初始化位图
	CDC *pDC=GetDC();
   // m_pDC=new CClientDC(this);
	m_MemDC.CreateCompatibleDC(pDC);
	m_iBmpWidth=::GetSystemMetrics(SM_CXSCREEN);
	m_iBmpHigh=::GetSystemMetrics(SM_CYSCREEN);
	m_MemBmp.CreateCompatibleBitmap(pDC,m_iBmpWidth,m_iBmpHigh);
	m_MemDC.SelectObject(&m_MemBmp);
	//先给位图贴上白背景
	CBrush backgroundBrush((COLORREF) ::GetSysColor(COLOR_WINDOW));
	CBrush* pOldBrush=m_MemDC.SelectObject(&backgroundBrush);
    m_MemDC.PatBlt(0,0,m_iBmpWidth,m_iBmpHigh,PATCOPY);
	m_MemDC.SelectObject(pOldBrush);
	return bFlag;
	return CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}

void CVMapView::OnMove() 
{
	// TODO: Add your command handler code here
	m_iFuncCurrent=FUN_MOVE;
}

BOOL CVMapView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	// TODO: Add your message handler code here and/or call default
	switch(m_iFuncCurrent)
	{
		case FUN_MOVE:
			SetCursor(AfxGetApp()->LoadCursor(IDC_MOVE));
			break;
		case FUN_ZOOMIN:
			SetCursor(AfxGetApp()->LoadCursor(IDC_MAGNIFY));
			break;
		case FUN_ADDPOINT:
		case FUN_ADDLINE:
		case FUN_ADDPOLY:
			SetCursor(AfxGetApp()->LoadCursor(IDC_ADDOBJ));
			break;
		default:
			SetCursor(AfxGetApp()->LoadCursor(IDC_ARROW));
			break;


	}
	return true;
//	return CView::OnSetCursor(pWnd, nHitTest, message);
}

void CVMapView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if(m_iFuncCurrent==FUN_MOVE)
	{//如果是漫游
		m_PointOrigin=point;
		m_PointOld=point;
		SetCapture();
	}
	else if(m_iFuncCurrent==FUN_ZOOMIN) 
	{//如果是放大
		//pDoc->m_iPushNumb++;
		m_PointOrigin=point;
		m_PointOld=point;
		//pDoc->m_bFlagMove=FALSE;
		SetCapture();
	}
	else if (m_iFuncCurrent==FUN_ADDPOINT)
	{
		CVMapDoc* pDoc = GetDocument();
		float RealX,RealY;
		DPtoRealP(point.x,point.y,RealX,RealY);
		CPointList *pTempPointList;
		pTempPointList=new CPointList;
		pTempPointList->lPointNumbers = 1;
		pTempPointList->bFlagIsDeleted=FALSE;
		for(int li=0;li<pTempPointList->lPointNumbers;li++)
		{
			//给点赋坐标值				
			pTempPointList->PointArrayX.Add(RealX);
			pTempPointList->PointArrayY.Add(RealY);				
		}
		pDoc->m_IndexList->m_PointList.AddTail(pTempPointList);//把该结点挂到m_IndexList上
		Invalidate(FALSE);
	}
	else if (m_iFuncCurrent==FUN_ADDLINE || m_iFuncCurrent==FUN_ADDPOLY)
	{
		//m_PtArr.Add(point);
		//AfxMessageBox("lb");
		m_bLButtonDown=TRUE;
		int Num = m_PtArr.GetSize();
		if(Num == 0)
		{ 
			m_PreMousePoint = point;
			m_PostMousePoint = point;
			m_PtArr.Add(point);
			SetCapture();
		}
		else
		{
			int nsize = m_PtArr.GetSize();
			POINT lastPt =  m_PtArr.GetAt(nsize-1);
			if ((point.x != lastPt.x)&& (point.y != lastPt.y))
			{
				//GetUserFunc()->m_ArcShapePtArray.Add(point);
				m_PreMousePoint = point;
				m_PostMousePoint = point;
				m_PtArr.Add(point);
			}
			
		}

	}
	
	CView::OnLButtonDown(nFlags, point);
}

void CVMapView::OnLButtonUp(UINT nFlags, CPoint point) 
{

	int x1,y1,x2,y2;
	float bl1,bl2;
	float k;
	// TODO: Add your message handler code here and/or call default
	if(m_iFuncCurrent==FUN_MOVE)
	{//如果是漫游
		m_fStartX=m_fStartX-(point.x-m_PointOrigin.x)*m_fBlc;
		m_fStartY=m_fStartY+(point.y-m_PointOrigin.y)*m_fBlc;
		m_iBmpX=0;
		m_iBmpY=0;
		//DrawMapZBA();
	}
	else if(m_iFuncCurrent==FUN_ZOOMIN)
	{// 如果是放大且鼠标左键已按下
	//	pDoc->m_iPushNumb=0;
		x1=min(m_PointOrigin.x,point.x);
		y1=max(m_PointOrigin.y,point.y);
		x2=max(m_PointOrigin.x,point.x);
		y2=min(m_PointOrigin.y,point.y);
		m_iScreenHigh=m_ScreenRect.bottom-m_ScreenRect.top;
		m_iScreenWide=m_ScreenRect.right-m_ScreenRect.left;
		if(x2-x1>3&&y1-y2>3)//够拉框范围，进行拉框放大
		{
			bl1=(float)m_iScreenWide/(float)(x2-x1);
			bl2=(float)m_iScreenHigh/(float)(y1-y2);
			if(bl2<bl1)
				bl1=bl2;
			if(m_fBl/m_fBlc<15)
			{
				k=1/bl1; //以矩形的中心点为屏幕中心确定始点
				m_fStartX=(float)(m_fStartX+(point.x-0.8*k*m_iScreenWide)*m_fBlc);
				m_fStartY=(float)(m_fStartY+(m_iScreenHigh*(1-0.1*k)-point.y)*m_fBlc);
				m_fBlc=k*m_fBlc;
			}
		}
		else//不够拉框范围，进行点击放大
		{
			k=(float)0.5;
			if(m_fBl/m_fBlc<15)	
			{
				m_fStartX=(float)(m_fStartX+(point.x-0.5*k*m_iScreenWide)*m_fBlc);
				m_fStartY=(float)(m_fStartY+(m_iScreenHigh*(1-0.5*k)-point.y)*m_fBlc);
				m_fBlc=(float)(k*m_fBlc);
			}
		}

		Invalidate(FALSE);
	}

	ReleaseCapture();
	CView::OnLButtonUp(nFlags, point);
}

void CVMapView::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if(m_iFuncCurrent==FUN_MOVE)
	{
		if(nFlags==MK_LBUTTON)
		{//如果是漫游
			int dx=m_PointOld.x-point.x;
			int dy=m_PointOld.y-point.y;
			ScrollWindow(-dx,-dy,NULL,NULL);
			m_PointOld=point;
			m_iBmpX+=dx;
			m_iBmpY+=dy;
		}
	}
	else 	if(m_iFuncCurrent == FUN_ZOOMIN && nFlags==MK_LBUTTON)
	{//如果是放大，且鼠标左键已按下一次   
		if(point!=m_PointOld)
		{//拉框
			//pDoc->m_bFlagMove=TRUE;
			CDC *pDC=GetDC();
			pDC->SetROP2(R2_NOT);
				
			CPen Pen(PS_DOT,1,RGB(192,192,192)),*pOldPen;
 			pOldPen=pDC->SelectObject(&Pen);
 			
 			pDC->SelectStockObject(NULL_BRUSH);
 			pDC->Rectangle(m_PointOrigin.x,m_PointOrigin.y,m_PointOld.x,m_PointOld.y);
 			pDC->Rectangle(m_PointOrigin.x,m_PointOrigin.y,point.x,point.y);
 			m_PointOld=point;
 			pDC->SelectObject(pOldPen);
			ReleaseDC(pDC);
		}			  
	}
	else if (m_iFuncCurrent == FUN_ADDLINE || m_iFuncCurrent == FUN_ADDPOLY)
	{
		//if(!m_bLButtonDown) 
		//	break;
		if (m_PtArr.GetSize()>=1 && m_bLButtonDown == TRUE)
		{
			CDC* pDC = GetDC();
			pDC->SetROP2(R2_NOT);
			CPen Pen(PS_DOT,1,RGB(192,192,192)),*pOldPen;
			pOldPen=pDC->SelectObject(&Pen);
			pDC->MoveTo(m_PreMousePoint.x, m_PreMousePoint.y);
			pDC->LineTo(m_PostMousePoint.x, m_PostMousePoint.y);
			pDC->MoveTo(m_PreMousePoint.x, m_PreMousePoint.y);
			pDC->LineTo(point.x, point.y);
			//Line(m_pDC->m_hDC,m_PreMousePoint.x, m_PreMousePoint.y, m_PostMousePoint.x, m_PostMousePoint.y);
			//Line(m_pDC->m_hDC,m_PreMousePoint.x, m_PreMousePoint.y, point.x,point.y);
			pDC->SelectObject(pOldPen);
			ReleaseDC(pDC);
			m_PostMousePoint = point;
		}
		
	}

	SetStatusBar(point);
	CView::OnMouseMove(nFlags, point);
}

void CVMapView::OnWholemap() 
{
	// TODO: Add your command handler code here
	//全图的响应函数
	m_iBmpX=0;
	m_iBmpY=0;
	CVMapDoc* pDoc = GetDocument();
	GetClientRect(&m_ScreenRect);
	//计算比例尺
    m_fBlc=(pDoc->m_fMaxX-pDoc->m_fMinX)/(m_ScreenRect.right-m_ScreenRect.left-20);//x轴比例尺
	m_fBll=(pDoc->m_fMaxY-pDoc->m_fMinY)/(m_ScreenRect.bottom-m_ScreenRect.top-20);//y轴比例尺
	if(m_fBll>m_fBlc) 
		m_fBlc=m_fBll;
	m_fBl=m_fBlc;
	//计算屏幕的高和宽
	m_iScreenHigh=m_ScreenRect.bottom-m_ScreenRect.top;
	m_iScreenWide=m_ScreenRect.right-m_ScreenRect.left;
    //以图廓中心对应屏幕中心
	m_fStartX=(float)((pDoc->m_fMaxX+pDoc->m_fMinX-m_iScreenWide*m_fBlc)/2);
	m_fStartY=(float)((pDoc->m_fMaxY+pDoc->m_fMinY-m_iScreenHigh*m_fBlc)/2);
	//图
	Invalidate(FALSE);
}

void CVMapView::OnZoomin() 
{
	// TODO: Add your command handler code here
	m_iFuncCurrent=FUN_ZOOMIN;
}

void CVMapView::OnAddpoint() 
{
	// TODO: Add your command handler code here
	m_iFuncCurrent= FUN_ADDPOINT;
}

void CVMapView::OnAddline() 
{
	// TODO: Add your command handler code here
	m_iFuncCurrent= FUN_ADDLINE;
	m_PtArr.RemoveAll();
}

void CVMapView::OnAddpoly() 
{
	// TODO: Add your command handler code here
	m_iFuncCurrent= FUN_ADDPOLY;	
	m_PtArr.RemoveAll();
}

void CVMapView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	m_bLButtonDown=FALSE;
	if(m_iFuncCurrent==FUN_ADDLINE)
	{
		ReleaseCapture();
		CVMapDoc* pDoc = GetDocument();
		//////加入线数据...
		//如果是线则开辟线空间且赋属性值   
		int nSize = m_PtArr.GetSize();
		FPOINT* RealPt = new FPOINT [nSize];
		for (int i=0;i<nSize;i++)
		{
			POINT DpXY = m_PtArr.GetAt(i);
			float RealX,RealY;
			DPtoRealP(DpXY.x,DpXY.y,RealX,RealY);
			RealPt[i].x = RealX;
			RealPt[i].y = RealY;
		}
		CLineList* pTempLineList;
		pTempLineList=new CLineList;
		pTempLineList->nCode = 100;
		pTempLineList->lPointNumbers=nSize;
		pTempLineList->bFlagIsDeleted=FALSE;
		for(int li=0;li<nSize;li++)
		{	//给线赋坐标值
			pTempLineList->PointArrayX.Add(RealPt[li].x);
			pTempLineList->PointArrayY.Add(RealPt[li].y);
		}			
		pDoc->m_IndexList->m_LineList.AddTail(pTempLineList);
		m_PtArr.RemoveAll();
		delete [] RealPt;
		Invalidate(FALSE);
	
	}
	else if(m_iFuncCurrent==FUN_ADDPOLY)
	{
		ReleaseCapture();
		CVMapDoc* pDoc = GetDocument();
		//////加入线数据...
		//如果是线则开辟线空间且赋属性值   
		int nSize = m_PtArr.GetSize();
		FPOINT* RealPt = new FPOINT [nSize];
		for (int i=0;i<nSize;i++)
		{
			POINT DpXY = m_PtArr.GetAt(i);
			float RealX,RealY;
			DPtoRealP(DpXY.x,DpXY.y,RealX,RealY);
			RealPt[i].x = RealX;
			RealPt[i].y = RealY;
		}
		CAreaList* pTempAreaList;
		pTempAreaList=new CAreaList;
		pTempAreaList->nCode = 200;
		pTempAreaList->lPointNumbers=nSize;
		pTempAreaList->bFlagIsDeleted=FALSE;
		for(int li=0;li<nSize;li++)
		{	//给线赋坐标值
			pTempAreaList->PointArrayX.Add(RealPt[li].x);
			pTempAreaList->PointArrayY.Add(RealPt[li].y);
		}			
		pDoc->m_IndexList->m_AreaList.AddTail(pTempAreaList);
		m_PtArr.RemoveAll();
		delete [] RealPt;
		Invalidate(FALSE);

	}

	CView::OnLButtonDblClk(nFlags, point);
}

void CVMapView::OnUpdateAddline(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	bool bCurFun;
	if(m_iFuncCurrent== FUN_ADDLINE)
		bCurFun = TRUE;
	else 
		bCurFun = FALSE;
	pCmdUI->SetCheck(bCurFun);
}

void CVMapView::OnUpdateAddpoint(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	bool bCurFun;
	if(m_iFuncCurrent== FUN_ADDPOINT)
		bCurFun = TRUE;
	else 
		bCurFun = FALSE;
	pCmdUI->SetCheck(bCurFun);
}

void CVMapView::OnUpdateAddpoly(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	bool bCurFun;
	if(m_iFuncCurrent== FUN_ADDPOLY)
		bCurFun = TRUE;
	else 
		bCurFun = FALSE;
	pCmdUI->SetCheck(bCurFun);
}

void CVMapView::OnUpdateZoomin(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	bool bCurFun;
	if(m_iFuncCurrent== FUN_ZOOMIN)
		bCurFun = TRUE;
	else 
		bCurFun = FALSE;
	pCmdUI->SetCheck(bCurFun);
}

void CVMapView::OnUpdateMove(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	bool bCurFun;
	if(m_iFuncCurrent== FUN_MOVE)
		bCurFun = TRUE;
	else 
		bCurFun = FALSE;
	pCmdUI->SetCheck(bCurFun);
}

void CVMapView::OnTest() 
{
	AfxMessageBox("多边形目标构建三角网模型实例");

	DrawRefreshStudyArea();//刷白屏幕
	
	int i,j;	
	CWaitCursor wait;
	



	////////////////////////////////////////////////////////////////////////////
	//Step1: 获取并组织实验多边形数据
	////////////////////////////////////////////////////////////////////////////

	m_polyCount = 0;   //多边形数量
	m_pPolyPtNum = NULL; //每个多边形包含的点数
	m_pPolyPt = NULL; //每个多边形的点坐标
	//自己加的--每个多边形中心点坐标
	m_pPolycrPt = NULL;
	m_pPoly1=NULL;


	CVMapDoc* pDoc = GetDocument();
	m_polyCount = pDoc->m_IndexList->m_AreaList.GetCount();
	m_pPolyPtNum = new int[m_polyCount];
	m_pPolyPt = new FPOINT*[m_polyCount];
	m_pPolycrPt=new FPOINT[m_polyCount];
	POSITION pos =NULL;
	int index = 0;
	CAreaList *pAreaList = NULL;
	for(pos=pDoc->m_IndexList->m_AreaList.GetHeadPosition();pos!=NULL;)
	{
		pAreaList=pDoc->m_IndexList->m_AreaList.GetNext(pos);
		if(pAreaList->bFlagIsDeleted==FALSE)
		{
			int ptNum = pAreaList->lPointNumbers;
			m_pPolyPtNum[index] = ptNum;
			m_pPolyPt[index] = new FPOINT[ptNum];
			float m_nPolyPtsumx=0;
			float m_nPolyPtsumy=0;
			for (int ii=0;ii<pAreaList->lPointNumbers;ii++)
			{

				m_pPolyPt[index][ii].x = pAreaList->PointArrayX[ii];
				m_pPolyPt[index][ii].y = pAreaList->PointArrayY[ii];
				m_nPolyPtsumx+=m_pPolyPt[index][ii].x;
				m_nPolyPtsumy+=m_pPolyPt[index][ii].y;
			}
			m_pPolycrPt[index].x=m_nPolyPtsumx/m_pPolyPtNum[index];
			m_pPolycrPt[index].y=m_nPolyPtsumy/m_pPolyPtNum[index];

			index++;
		}

	}

	//将获取的多边形目标在屏幕上进行绘制
	for (i=0;i<m_polyCount;i++)
	{	
		
		DrawPolygon(m_pPolyPt[i],m_pPolyPtNum[i],RGB(180,180,180),RGB(220,220,220),1);
		//DrawPolygon(m_pPolycrPt,13,RGB(180,180,180),RGB(220,220,220),5);
		//DrawPolygon(m_pPolycrPt,5,RGB(180,180,180),RGB(220,220,220),5);
		DrawPoint(m_pPolycrPt[i],RGB(0,0,255),3);
		
		//AfxMessageBox("多边形的中心点!");

	}	

	AfxMessageBox("读取的多边形目标成功!");


	
	////////////////////////////////////////////////////////////////////////////	
	//Step2: 对多边形目标构建Delaunay三角网。执行CreateTIN函数后，多边形目标构建的
	//三角网信息可从变量m_pDTTriArrayPtr，m_pDTPointArrayPtr获得。
	////////////////////////////////////////////////////////////////////////////

    CreateTIN();

	

	////////////////////////////////////////////////////////////////////////////
	//Step3: 解析建网后的结构化信息查询
	////////////////////////////////////////////////////////////////////////////

	//_1: 绘制三角形信息
	long nCount_DTTri;
	m_pDTTriArrayPtr->get_nCount(&nCount_DTTri);//获得三角形数量

	for (i=0;i<nCount_DTTri;i++) 
	{
		DrawOneTri(i,RGB(150,150,150),2);

	}
	
//	2: 绘制顶点信息
	long nCount_DTPoint;
	//m_pDTPointArrayPtr->get_nCount(&nCount_DTPoint); //获得三角网顶点数量
	for (i=0;i<nCount_DTPoint;i++)
	{
		DrawOneDTPoint(i,RGB(0,0,0),3);	
	}

	AfxMessageBox("绘制三角网顶点及三角形边!");





	//_3: 查询并绘制某个顶点相连接的三角形
	/*int index_DTPoint = 15; //赋予需要查询的顶点下标
	DrawOneDTPoint(index_DTPoint,RGB(255,0,0),5);
	char str[200];
	sprintf(str,"下标为%d的三角网顶点",index_DTPoint);
	AfxMessageBox(str);

	LONGARRARY indexList_adjTris;//定义记录相邻的三角形下标(即在数组m_pDTTriArrayPtr的下标)的数组
	GetTris_adjacentToOnePoint(index_DTPoint,indexList_adjTris);//查询函数

	for (i=0;i<indexList_adjTris.GetSize();i++)
	{
		int index = indexList_adjTris[i];
		DrawOneTri(index,RGB(0,0,255),3);
	}

	DrawOneDTPoint(index_DTPoint,RGB(255,0,0),5);
	char str1[200];
	sprintf(str1,"与下标为%d的顶点相邻的三角形",index_DTPoint);
	if(indexList_adjTris.GetSize()>0){indexList_adjTris.RemoveAll();}
	AfxMessageBox(str1);







	//_4: 查询并绘制某个三角形相邻近的其它三角形
	int index_DTTri = 50;//赋予需要查询的三角形下标
	DrawOneTri(index_DTTri,RGB(180,0,0),RGB(180,0,0),1);
	char str3[200];
	sprintf(str3,"下标为%d的三角网三角形",index_DTTri);
	AfxMessageBox(str3);

	INTARRARY indexList_adjTris0;//记录相邻的三角形下标(即在数组m_pDTTriArrayPtr的下标)
	GetTris_adjacentToOneTri(index_DTTri,indexList_adjTris0);//查询函数
	for (i=0;i<indexList_adjTris0.GetSize();i++)
	{
		int index = indexList_adjTris0[i];
		DrawOneTri(index,RGB(0,0,200),RGB(0,0,200),1);
	}
	char str4[200];
	sprintf(str4,"与下标为%d的三角形相邻的其它三角形",index_DTTri);
	if(indexList_adjTris0.GetSize()>0){indexList_adjTris0.RemoveAll();}
	AfxMessageBox(str4);


	




	//_5: 查询与某个多边形相关联的所有三角形
	int index_Poly = 10; //赋予需要查询的多边形目标下标
	DrawPolygon(m_pPolyPt[index_Poly],m_pPolyPtNum[index_Poly],RGB(255,0,0),RGB(220,220,220),5);
	char str5[200];
	sprintf(str5,"下标为%d的多边形目标",index_Poly);
	AfxMessageBox(str5);

	LONGARRARY IndexList_PolyrelateTri;
	for (i=0;i<m_PointTo.GetSize();i++)
	{
		if (index_Poly == m_PointTo[i])
		{
			LONGARRARY indexList_adjTris;//记录与该顶点关联的三角形下标(即在数组m_pDTTriArrayPtr的下标)
	        GetTris_adjacentToOnePoint(i,indexList_adjTris);//获取与下标为index_DTPoint顶点的相邻三角形
			for (int j=0;j<indexList_adjTris.GetSize();j++)
			{
				IndexList_PolyrelateTri.Add(indexList_adjTris[j]);
			}	
			indexList_adjTris.RemoveAll();
		}
	}
	
	for (i=0;i<IndexList_PolyrelateTri.GetSize();i++)
	{
		long index_temp = IndexList_PolyrelateTri[i];
		DrawOneTri(index_temp,RGB(0,0,200),3);
	}

	char str6[200];
	sprintf(str6,"与下标为%d的多边形目标相连接的三角形",index_Poly);

	AfxMessageBox(str6);



	//进一步地，获取与该多边形1阶邻近的其它多边形
	LONGARRARY Ad_polylist;
	for (i=0;i<IndexList_PolyrelateTri.GetSize();i++)
	{
       long index_temp = IndexList_PolyrelateTri[i];
	   IDTTriPtr oneDTTri;
	   m_pDTTriArrayPtr->get_element(index_temp,&oneDTTri); //获得下标为i的三角形
	   
	   long no1,no2,no3;
	   oneDTTri->get_PointNo1(&no1); //三角形三个顶点的编号
	   oneDTTri->get_PointNo2(&no2);
       oneDTTri->get_PointNo3(&no3);
	   if (index_Poly != m_PointTo[no1])
	   {
         Ad_polylist.Add(m_PointTo[no1]);
	   }
	   if (index_Poly != m_PointTo[no2])
	   {
		 Ad_polylist.Add(m_PointTo[no2]);
	   }
	   if (index_Poly != m_PointTo[no3])
	   {
		   Ad_polylist.Add(m_PointTo[no3]);
	   }
	}

	DrawPolygon(m_pPolyPt[index_Poly],m_pPolyPtNum[index_Poly],RGB(255,0,0),RGB(220,220,220),5);
	for (i=0;i<Ad_polylist.GetSize();i++)
	{
		DrawPolygon(m_pPolyPt[Ad_polylist[i]],m_pPolyPtNum[Ad_polylist[i]],RGB(0,255,0),RGB(220,220,220),5);
	}
	char str7[200];
	sprintf(str7,"与下标为%d的多边形目标1阶邻接的其它多边形",index_Poly);
	AfxMessageBox(str7);

	if(IndexList_PolyrelateTri.GetSize()>0){IndexList_PolyrelateTri.RemoveAll();}*/






	////////////////////////////////////////////////////////////////////////////
	//Step5: 释放内存
	////////////////////////////////////////////////////////////////////////////
	FreeBuffer();	 
	
	wait.Restore();	   		
}


void CVMapView::SetStatusBar(CPoint pt)
{
	CString str;
	float RealX,RealY;
	RealX= RealY =-1;
	DPtoRealP(pt.x,pt.y,RealX,RealY);
	str.Format("%.2f  %.2f",RealX,RealY);
 	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
 	if(pMainFrame == NULL)
		return;
 	pMainFrame->SetStatusPane(0,str);
	
}

void CVMapView::OnGroup() 
{
	AfxMessageBox("分组实验!");
	int i,j;	
	CWaitCursor wait;

	////////////////////////////////             ////////////////////////////////////////////
	//Step1: 获取并组织实验数据
	////////////////////////////////////////////////////////////////////////////
	
	m_polyCount = 0;   //多边形数量
	m_pPolyPtNum = NULL; //每个多边形包含的点数
	m_pPolyPt = NULL; //每个多边形的点坐标
	tritag=NULL;
	CVMapDoc* pDoc = GetDocument();
	m_polyCount = pDoc->m_IndexList->m_AreaList.GetCount();
	m_pPolyPtNum = new int[m_polyCount];
	m_pPolyPt = new FPOINT*[m_polyCount];

	POSITION pos =NULL;
	int index = 0;
	CAreaList *pAreaList = NULL;
	for(pos=pDoc->m_IndexList->m_AreaList.GetHeadPosition();pos!=NULL;)
	{
		pAreaList=pDoc->m_IndexList->m_AreaList.GetNext(pos);
		if(pAreaList->bFlagIsDeleted==FALSE)
		{
			int ptNum = pAreaList->lPointNumbers;
			m_pPolyPtNum[index] = ptNum;
			m_pPolyPt[index] = new FPOINT[ptNum];
			for (int ii=0;ii<pAreaList->lPointNumbers;ii++)
			{
				m_pPolyPt[index][ii].x = pAreaList->PointArrayX[ii];
				m_pPolyPt[index][ii].y = pAreaList->PointArrayY[ii];
			}
			index++;
		}
	}


	
	
	////////////////////////////////////////////////////////////////////////////	
	//Step2: 对多边形目标构建Delaunay三角网。执行CreateTIN函数后，多边形目标构建的
	//三角网信息可从变量m_pDTTriArrayPtr，m_pDTPointArrayPtr获得。
	////////////////////////////////////////////////////////////////////////////

    CreateTIN();
	


	////////////////////////////////////////////////////////////////////////////
	//Step3: Insert your programmes;
	////////////////////////////////////////////////////////////////////////////
	
	long nCount_DTTri;
	m_pDTTriArrayPtr->get_nCount(&nCount_DTTri);//获得三角形数量
	tritag=new int[nCount_DTTri];
	for (i=0;i<nCount_DTTri;i++) 
	{

		DrawOneTrinew(i,RGB(150,150,150),1);
		INTARRARY indexList_adjTris0;//记录相邻的三角形下标(即在数组m_pDTTriArrayPtr的下标)
		GetTris_adjacentToOneTri(i,indexList_adjTris0);//查询函数
		
		for (int j=0;j<indexList_adjTris0.GetSize();j++)
		{
			int index = indexList_adjTris0[j];
			if (tritag[index]==0)
			DrawOneTrinew(i,RGB(255,0,0),3);
		}
		if(tritag[i]==0)
		DrawOneTri(i,RGB(255,0,0), 3);
	}
	
	

	////////////////////////////////////////////////////////////////////////////
	//Step4: 释放内存
	////////////////////////////////////////////////////////////////////////////
    FreeBuffer();
	wait.Restore();	   	
	
}


void CVMapView::FreeBuffer()
{
	int i;
	if (m_pPolyPtNum) 
	{
		delete[] m_pPolyPtNum;
		m_pPolyPtNum = NULL;
	}
	if (m_pPolyPt) 
	{
		for (i=0;i<m_polyCount;i++) 
		{
			if (m_pPolyPt[i]) 
			{
				delete[] m_pPolyPt[i];
				m_pPolyPt[i] = NULL;			   
			}
		}
		delete[] m_pPolyPt;
		m_pPolyPt = NULL;
	}

	if (m_PointTo.GetSize()>0)
	{
		m_PointTo.RemoveAll();
	}

	if (m_Polypointnumarray.GetSize()>0)
	{
		m_Polypointnumarray.RemoveAll();
	}
	
}

void CVMapView::Oncentrepoint() 
{
	// TODO: Add your command handler code here
	AfxMessageBox("生成多边形重心的三角网!");

	DrawRefreshStudyArea();//刷白屏幕
	
	int i,j;	
	CWaitCursor wait;
	



	////////////////////////////////////////////////////////////////////////////
	//Step1: 获取并组织实验多边形数据
	////////////////////////////////////////////////////////////////////////////

	m_polyCount = 0;   //多边形数量
	m_pPolyPtNum = NULL; //每个多边形包含的点数
	m_pPolyPt = NULL; //每个多边形的点坐标
	//自己加的--每个多边形中心点坐标
	m_pPolycrPt = NULL;
	m_pPoly1=NULL;


	CVMapDoc* pDoc = GetDocument();
	m_polyCount = pDoc->m_IndexList->m_AreaList.GetCount();
	m_pPolyPtNum = new int[m_polyCount];
	m_pPolyPt = new FPOINT*[m_polyCount];
	m_pPolycrPt=new FPOINT[m_polyCount];
	POSITION pos =NULL;
	int index = 0;
	CAreaList *pAreaList = NULL;
	for(pos=pDoc->m_IndexList->m_AreaList.GetHeadPosition();pos!=NULL;)
	{
		pAreaList=pDoc->m_IndexList->m_AreaList.GetNext(pos);
		if(pAreaList->bFlagIsDeleted==FALSE)
		{
			int ptNum = pAreaList->lPointNumbers;
			m_pPolyPtNum[index] = ptNum;
			m_pPolyPt[index] = new FPOINT[ptNum];
			float m_nPolyPtsumx=0;
			float m_nPolyPtsumy=0;
			for (int ii=0;ii<pAreaList->lPointNumbers;ii++)
			{

				m_pPolyPt[index][ii].x = pAreaList->PointArrayX[ii];
				m_pPolyPt[index][ii].y = pAreaList->PointArrayY[ii];
				m_nPolyPtsumx+=m_pPolyPt[index][ii].x;
				m_nPolyPtsumy+=m_pPolyPt[index][ii].y;
			}
			m_pPolycrPt[index].x=m_nPolyPtsumx/m_pPolyPtNum[index];
			m_pPolycrPt[index].y=m_nPolyPtsumy/m_pPolyPtNum[index];

			index++;
		}

	}

	//将获取的多边形目标在屏幕上进行绘制
	for (i=0;i<m_polyCount;i++)
	{	
		
		DrawPolygon(m_pPolyPt[i],m_pPolyPtNum[i],RGB(180,180,180),RGB(220,220,220),1);
		//DrawPolygon(m_pPolycrPt,13,RGB(180,180,180),RGB(220,220,220),5);
		//DrawPolygon(m_pPolycrPt,5,RGB(180,180,180),RGB(220,220,220),5);
		DrawPoint(m_pPolycrPt[i],RGB(0,0,255),3);
		
		//AfxMessageBox("多边形的中心点!");

	}	

	AfxMessageBox("读取的多边形目标成功!");


	
	////////////////////////////////////////////////////////////////////////////	
	//Step2: 对多边形目标构建Delaunay三角网。执行CreateTIN函数后，多边形目标构建的
	//三角网信息可从变量m_pDTTriArrayPtr，m_pDTPointArrayPtr获得。
	////////////////////////////////////////////////////////////////////////////

    CreateTINnew();

	

	////////////////////////////////////////////////////////////////////////////
	//Step3: 解析建网后的结构化信息查询
	////////////////////////////////////////////////////////////////////////////

	//_1: 绘制三角形信息
	long nCount_DTTri;
	m_pDTTriArrayPtr->get_nCount(&nCount_DTTri);//获得三角形数量

	for (i=0;i<nCount_DTTri;i++) 
	{
		DrawOneTri(i,RGB(150,150,150),2);

	}
	
//	2: 绘制顶点信息
	long nCount_DTPoint;
	//m_pDTPointArrayPtr->get_nCount(&nCount_DTPoint); //获得三角网顶点数量
	for (i=0;i<nCount_DTPoint;i++)
	{
		DrawOneDTPoint(i,RGB(0,0,0),3);	
	}

	AfxMessageBox("绘制三角网顶点及三角形边!");
	FreeBuffer();	 
	
	wait.Restore();	
}



void CVMapView::CreateTINnew()
{
	////////////////////////////////////////////////////////////////////////////////////
	//Step1: 初始化组件接口
	//由于此处三角网创建函数由一个com组件库提供，需要com组件相关操作。但是大家不必深究。
	////////////////////////////////////////////////////////////////////////////////////
	IAGeneralizationPtr _iGenCore = Initialization_module();



	////////////////////////////////////////////////////////////////////////////	
	//Step2: 构建多边形群Delaunay三角网模型
	////////////////////////////////////////////////////////////////////////////

	//Step2-1:将多边形目标组织为点群结构作为建网输入数据）
	m_pInMultiPt.CreateInstance(_uuidof(DoMultipoint));//首先，创建一个点串对象，装载构成多边形的点群数据
	
	//根据多边形中心生成三角网//
	////////////////////////////////
	int i,j;
	for (i=0;i<m_polyCount;i++) 
	{
		int count_pt=0;
		for (j=0;j<m_polyCount;j++)//注意，多边形首尾点相同，预处理时去除末尾点(避免重复点)
		{
			IDoPointPtr pDoPt;		       
			pDoPt.CreateInstance(_uuidof(doPoint));
			pDoPt->put_RealX(m_pPolycrPt[j].x);
			pDoPt->put_RealY(m_pPolycrPt[j].y);
	 		m_pInMultiPt->AddPoint(pDoPt);
			m_PointTo.Add(i);
			count_pt++;
		}
		m_Polypointnumarray.Add(count_pt); //记录每个多边形的点数
	}

	/*int i,j;
	for (i=0;i<m_polyCount;i++) 
	{
		int count_pt=0;
		for (j=0;j<m_pPolyPtNum[i]-1;j++)//注意，多边形首尾点相同，预处理时去除末尾点(避免重复点)
		{
			IDoPointPtr pDoPt;		       
			pDoPt.CreateInstance(_uuidof(doPoint));
			pDoPt->put_RealX(m_pPolyPt[i][j].x);
			pDoPt->put_RealY(m_pPolyPt[i][j].y);
	 		m_pInMultiPt->AddPoint(pDoPt);
			m_PointTo.Add(i);
			count_pt++;
		}
		m_Polypointnumarray.Add(count_pt); //记录每个多边形的点数
	}*/



	
	//Step2-2: 实施建网

	//初始化三角网顶点串接口对象(pOutDTPointArrayPtr)，用于记录建网后输出的三角网顶点信息
	m_pDTPointArrayPtr.CreateInstance(_uuidof(DTpointArray)); 

	//初始化三角网三角形串接口对象(pOutDTTriArrayPtr)，装载建网后输出的三角网三角形信息
	m_pDTTriArrayPtr.CreateInstance(_uuidof(DTTriArray));  

	//获得Delaunay三角网模型对象接口(pIDelaunayTIN)，由改对象接口提供三角网建立函数（直接调用，不用关心内部机制）
   	IAGeneralizationPtr _iGenProcHandler = (IAGeneralizationPtr)_iGenCore; 
	IDelaunayTINPtr pIDelaunayTIN;
	HRESULT res =  _iGenProcHandler->get_DelaunayTIN(&pIDelaunayTIN); 

	//构建三角网,pInMultiPt—>输入的参与建网点串信息，pOut_DTPointArrayPtr—>输出的三角网顶点信息，pOut_DTTriArrayPtr—>输出的三角网三角形信息
	HRESULT	hr = pIDelaunayTIN->CreatTIN(m_pInMultiPt,&m_pDTPointArrayPtr,&m_pDTTriArrayPtr);
}

void CVMapView::DrawOneTrinew(int index_DTtri, COLORREF pencolor, int width)
{
	if (index_DTtri<0)
	{
		AfxMessageBox("绘制的三角网三角形下标错误!");
		return;
	}

	IDTTriPtr oneDTTri;
	m_pDTTriArrayPtr->get_element(index_DTtri,&oneDTTri); //获得下标为i的三角形

	long no1,no2,no3;
	oneDTTri->get_PointNo1(&no1); //三角形三个顶点的编号
	oneDTTri->get_PointNo2(&no2);
	oneDTTri->get_PointNo3(&no3);
	
	IDTPointPtr DTPoint1,DTPoint2,DTPoint3;
	m_pDTPointArrayPtr->get_element(no1,&DTPoint1); //根据编号在输出顶点串中（pOut_DTPointArrayPt）得到顶点对象
	m_pDTPointArrayPtr->get_element(no2,&DTPoint2);
	m_pDTPointArrayPtr->get_element(no3,&DTPoint3);
	
	
	FPOINT* ptList = NULL;
	ptList = new FPOINT[4];
	
	double xx,yy;
	DTPoint1->get_coorX(&xx);//获得每个三角形顶点的坐标
	DTPoint1->get_coorY(&yy);
	ptList[0].x = xx;
	ptList[0].y = yy;
	
	DTPoint2->get_coorX(&xx);
	DTPoint2->get_coorY(&yy);
	ptList[1].x = xx;
	ptList[1].y = yy;
	
	DTPoint3->get_coorX(&xx);
	DTPoint3->get_coorY(&yy);
	ptList[2].x = xx;
	ptList[2].y = yy;
	
	ptList[3].x = ptList[0].x;
	ptList[3].y = ptList[0].y;
	//自己加的
	int d1,d2,d3,dmax;
	dmax=0;
	d1=(ptList[0].x-ptList[1].x)*(ptList[0].x-ptList[1].x)+(ptList[0].y-ptList[1].y)*(ptList[0].y-ptList[1].y);
	if(d1>dmax) dmax=d1;
	d2=(ptList[1].x-ptList[2].x)*(ptList[1].x-ptList[2].x)+(ptList[1].y-ptList[2].y)*(ptList[1].y-ptList[2].y);
	if(d2>dmax) dmax=d2;
	d3=(ptList[2].x-ptList[3].x)*(ptList[2].x-ptList[3].x)+(ptList[2].y-ptList[3].y)*(ptList[2].y-ptList[3].y);
	if(d3>dmax) dmax=d3;
	
	if(dmax<5000)
	{
		DrawPolyline(ptList,3,pencolor, width);	//绘制三角形边信息
		tritag[index_DTtri]=1;
	}

	//if(tritag[index_DTtri]==1)
		//DrawPolyline(ptList,3,RGB(255,0,0), 3);
	if (ptList)
	{
		delete[] ptList;
		ptList = NULL;
	}	

}
