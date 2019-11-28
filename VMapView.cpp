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

	m_polyCount=0;//�����Ŀ������
	m_pPolyPtNum=NULL; //ÿ������ΰ����ĵ���
	m_pPolyPt=NULL ; //ÿ������εĵ�����
	m_pPolycrPt=NULL;//ÿ����������ĵ�����--�Լ��ӵ�
	m_pPoly1=NULL;
	m_pPoly2=NULL;
	m_pPoly3=NULL;//zijijiade
	tritag=NULL;
	m_pDTPointArrayPtr=NULL;//�洢������ģ�͵Ķ�����Ϣ
	m_pDTTriArrayPtr=NULL;//�洢����������������Ϣ
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


	//���������
    m_fBlc=(pDoc->m_fMaxX-pDoc->m_fMinX)/(m_ScreenRect.right-m_ScreenRect.left-20);//x�������
	m_fBll=(pDoc->m_fMaxY-pDoc->m_fMinY)/(m_ScreenRect.bottom-m_ScreenRect.top-20);//y�������
	if(m_fBll>m_fBlc) 
		m_fBlc=m_fBll;
	m_fBl=m_fBlc;
	//������Ļ�ĸߺͿ�
	m_iScreenHigh=m_ScreenRect.bottom-m_ScreenRect.top;
	m_iScreenWide=m_ScreenRect.right-m_ScreenRect.left;


	////��Ļת������m_fStartX��m_fStartY��m_fBl
    //��ͼ�����Ķ�Ӧ��Ļ����
	m_fStartX=(float)((pDoc->m_fMaxX+pDoc->m_fMinX-m_iScreenWide*m_fBlc)/2);
	m_fStartY=(float)((pDoc->m_fMaxY+pDoc->m_fMinY-m_iScreenHigh*m_fBlc)/2);
	m_fBl;
}


void CVMapView::RealPtoDP(float RealX,float RealY,int& DpX,int& DpY)
{
	
	//ʵ������ת������Ļ����
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
	int iScreenX,iScreenY;		//��Ļ����

	//�ȸ�λͼ���ϰױ���
	CBrush backgroundBrush((COLORREF) ::GetSysColor(COLOR_WINDOW));
	CBrush* pOldBrush=m_MemDC.SelectObject(&backgroundBrush);
    m_MemDC.PatBlt(0,0,m_iBmpWidth,m_iBmpHigh,PATCOPY);
	m_MemDC.SelectObject(pOldBrush);

	
	

	COLORREF brcolor = RGB(100,100,100);
	COLORREF pencolor = RGB(0,0,255);

	CBrush  br(brcolor);
	CPen  pen(PS_SOLID,0,pencolor);
	for(pos=pDoc->m_IndexList->m_AreaList.GetHeadPosition();pos!=NULL;)
	{//����Ҫ��
		int   ii=0;
		float fTempX;
		float fTempY;
		pAreaList=pDoc->m_IndexList->m_AreaList.GetNext(pos);
		if(pAreaList->bFlagIsDeleted==FALSE)
		{//�����ı���û�б�ɾ���Ż�
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
	{//����Ҫ��
		int   ii=0;
		float fTempX;
		float fTempY;
		pLineList=pDoc->m_IndexList->m_LineList.GetNext(pos);
		if(pLineList->bFlagIsDeleted==FALSE)
		{//�����û�б�ɾ���Ż�
			pOldPen=m_MemDC.SelectObject(&NewPen_Black);//
			fTempX=pLineList->PointArrayX[0];
			fTempY=pLineList->PointArrayY[0];
			//ʵ������ת������Ļ����
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
	{//����Ҫ��
		int   ii=0;			//ѭ������
		float fTempX,fTempY;//��ʱ����
		pPointList=pDoc->m_IndexList->m_PointList.GetNext(pos);
		if(pPointList->bFlagIsDeleted==FALSE)
		{//�����û�б�ɾ���Ż�
			m_MemDC.SelectObject(pen1);
			fTempX=pPointList->PointArrayX[0];
			fTempY=pPointList->PointArrayY[0];
			//ʵ������ת������Ļ����
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

	//��λͼ������Ļ��
	CRect Rect;
	GetClientRect(Rect);
	CDC* dc = GetDC();
	dc->BitBlt(0,0,Rect.Width(),Rect.Height(),&m_MemDC,m_iBmpX,m_iBmpY,SRCCOPY);
}



void CVMapView::CreateTIN()
{
	////////////////////////////////////////////////////////////////////////////////////
	//Step1: ��ʼ������ӿ�
	//���ڴ˴�����������������һ��com������ṩ����Ҫcom�����ز��������Ǵ�Ҳ������
	////////////////////////////////////////////////////////////////////////////////////
	IAGeneralizationPtr _iGenCore = Initialization_module();



	////////////////////////////////////////////////////////////////////////////	
	//Step2: ���������ȺDelaunay������ģ��
	////////////////////////////////////////////////////////////////////////////

	//Step2-1:�������Ŀ����֯Ϊ��Ⱥ�ṹ��Ϊ�����������ݣ�
	m_pInMultiPt.CreateInstance(_uuidof(DoMultipoint));//���ȣ�����һ���㴮����װ�ع��ɶ���εĵ�Ⱥ����
	
	//���ݶ������������������//
	////////////////////////////////
	/*int i,j;
	for (i=0;i<m_polyCount;i++) 
	{
		int count_pt=0;
		for (j=0;j<m_polyCount;j++)//ע�⣬�������β����ͬ��Ԥ����ʱȥ��ĩβ��(�����ظ���)
		{
			IDoPointPtr pDoPt;		       
			pDoPt.CreateInstance(_uuidof(doPoint));
			pDoPt->put_RealX(m_pPolycrPt[j].x);
			pDoPt->put_RealY(m_pPolycrPt[j].y);
	 		m_pInMultiPt->AddPoint(pDoPt);
			m_PointTo.Add(i);
			count_pt++;
		}
		m_Polypointnumarray.Add(count_pt); //��¼ÿ������εĵ���
	}*/

	int i,j;
	for (i=0;i<m_polyCount;i++) 
	{
		int count_pt=0;
		for (j=0;j<m_pPolyPtNum[i]-1;j++)//ע�⣬�������β����ͬ��Ԥ����ʱȥ��ĩβ��(�����ظ���)
		{
			IDoPointPtr pDoPt;		       
			pDoPt.CreateInstance(_uuidof(doPoint));
			pDoPt->put_RealX(m_pPolyPt[i][j].x);
			pDoPt->put_RealY(m_pPolyPt[i][j].y);
	 		m_pInMultiPt->AddPoint(pDoPt);
			m_PointTo.Add(i);
			count_pt++;
		}
		m_Polypointnumarray.Add(count_pt); //��¼ÿ������εĵ���
	}



	
	//Step2-2: ʵʩ����

	//��ʼ�����������㴮�ӿڶ���(pOutDTPointArrayPtr)�����ڼ�¼�����������������������Ϣ
	m_pDTPointArrayPtr.CreateInstance(_uuidof(DTpointArray)); 

	//��ʼ�������������δ��ӿڶ���(pOutDTTriArrayPtr)��װ�ؽ������������������������Ϣ
	m_pDTTriArrayPtr.CreateInstance(_uuidof(DTTriArray));  

	//���Delaunay������ģ�Ͷ���ӿ�(pIDelaunayTIN)���ɸĶ���ӿ��ṩ����������������ֱ�ӵ��ã����ù����ڲ����ƣ�
   	IAGeneralizationPtr _iGenProcHandler = (IAGeneralizationPtr)_iGenCore; 
	IDelaunayTINPtr pIDelaunayTIN;
	HRESULT res =  _iGenProcHandler->get_DelaunayTIN(&pIDelaunayTIN); 

	//����������,pInMultiPt��>����Ĳ��뽨���㴮��Ϣ��pOut_DTPointArrayPtr��>�����������������Ϣ��pOut_DTTriArrayPtr��>�������������������Ϣ
	HRESULT	hr = pIDelaunayTIN->CreatTIN(m_pInMultiPt,&m_pDTPointArrayPtr,&m_pDTTriArrayPtr);

}
//Denaulay�����������ʼ������
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

//����һ��������������(index_DTtri���������±꣬pencolor��������ɫ��width�����ʿ��)
void CVMapView::DrawOneTri(int index_DTtri, COLORREF pencolor, int width)
{
	if (index_DTtri<0)
	{
		AfxMessageBox("���Ƶ��������������±����!");
		return;
	}

	IDTTriPtr oneDTTri;
	m_pDTTriArrayPtr->get_element(index_DTtri,&oneDTTri); //����±�Ϊi��������

	long no1,no2,no3;
	oneDTTri->get_PointNo1(&no1); //��������������ı��
	oneDTTri->get_PointNo2(&no2);
	oneDTTri->get_PointNo3(&no3);
	
	IDTPointPtr DTPoint1,DTPoint2,DTPoint3;
	m_pDTPointArrayPtr->get_element(no1,&DTPoint1); //���ݱ����������㴮�У�pOut_DTPointArrayPt���õ��������
	m_pDTPointArrayPtr->get_element(no2,&DTPoint2);
	m_pDTPointArrayPtr->get_element(no3,&DTPoint3);
	
	
	FPOINT* ptList = NULL;
	ptList = new FPOINT[4];
	
	double xx,yy;
	DTPoint1->get_coorX(&xx);//���ÿ�������ζ��������
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
	//�Լ��ӵ�
	/*int d1,d2,d3,dmax;
	dmax=0;
	d1=(ptList[0].x-ptList[1].x)*(ptList[0].x-ptList[1].x)+(ptList[0].y-ptList[1].y)*(ptList[0].y-ptList[1].y);
	if(d1>dmax) dmax=d1;
	d2=(ptList[1].x-ptList[2].x)*(ptList[1].x-ptList[2].x)+(ptList[1].y-ptList[2].y)*(ptList[1].y-ptList[2].y);
	if(d2>dmax) dmax=d2;
	d3=(ptList[2].x-ptList[3].x)*(ptList[2].x-ptList[3].x)+(ptList[2].y-ptList[3].y)*(ptList[2].y-ptList[3].y);
	if(d3>dmax) dmax=d3;
	
	if(dmax<4000)*/
	DrawPolyline(ptList,3,pencolor, width);	//���������α���Ϣ
	if (ptList)
	{
		delete[] ptList;
		ptList = NULL;
	}	

}


//����һ��������������(index_DTtri���������±꣬pencolor��������ɫ�� brcolor�������ɫ��width�����ʿ��)
void CVMapView::DrawOneTri(int index_DTtri,COLORREF pencolor, COLORREF brcolor,int width)
{
	if (index_DTtri<0)
	{
		AfxMessageBox("���Ƶ��������������±����!");
		return;
	}
	
	IDTTriPtr oneDTTri;
	m_pDTTriArrayPtr->get_element(index_DTtri,&oneDTTri); //����±�Ϊi��������
	
	long no1,no2,no3;
	oneDTTri->get_PointNo1(&no1); //��������������ı��
	oneDTTri->get_PointNo2(&no2);
	oneDTTri->get_PointNo3(&no3);
	
	IDTPointPtr DTPoint1,DTPoint2,DTPoint3;
	m_pDTPointArrayPtr->get_element(no1,&DTPoint1); //���ݱ����������㴮�У�pOut_DTPointArrayPt���õ��������
	m_pDTPointArrayPtr->get_element(no2,&DTPoint2);
	m_pDTPointArrayPtr->get_element(no3,&DTPoint3);
	
	
	FPOINT* ptList = NULL;
	ptList = new FPOINT[4];
	
	double xx,yy;
	DTPoint1->get_coorX(&xx);//���ÿ�������ζ��������
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

//����һ������������(index_DTPoint�������±꣬pencolor��������ɫ��width�����ʿ��)
void CVMapView::DrawOneDTPoint(int index_DTPoint,COLORREF pencolor, int width)
{
	IDTPointPtr oneDTPoint;
	m_pDTPointArrayPtr->get_element(index_DTPoint,&oneDTPoint);//����±�Ϊi���������������
	double xx,yy;
	oneDTPoint->get_coorX(&xx);//��ö��������
	oneDTPoint->get_coorY(&yy);
	FPOINT onePt;
	onePt.x = xx;
	onePt.y = yy;
	DrawPoint(onePt,pencolor,width);//���Ƶ���Ϣ

}

//����ƺ���
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

//�߻��ƺ���
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

//����Ļ��ͼ����ˢ�ף����֮ǰ��ͼ�κۼ�
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


//��ѯ��ĳ�����������������ӵ�������������
//index_DTPoint������Ķ����ţ�indexList_adjacentDTTris������������������±�����
void CVMapView::GetTris_adjacentToOnePoint(int index_DTPoint,LONGARRARY& indexList_adjacentDTTris)
{
	IDTPointPtr oneDTPoint;
	m_pDTPointArrayPtr->get_element(index_DTPoint,&oneDTPoint);//����±�Ϊi���������������

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


//��ѯ��ĳ�����������������ڽ�������������������
//index_DTPoint������������α�ţ�indexList_adjacentDTTris��������ڽ��������±�����
void CVMapView::GetTris_adjacentToOneTri(int index_DTTri,INTARRARY& indexList_adjacentDTTris)
{
	IDTTriPtr oneDTTri;
	m_pDTTriArrayPtr->get_element(index_DTTri,&oneDTTri);
	
	long ajTri_index1, ajTri_index2, ajTri_index3;
	oneDTTri->get_AdjaceTriNo1(&ajTri_index1);
	oneDTTri->get_AdjaceTriNo2(&ajTri_index2); 
	oneDTTri->get_AdjaceTriNo3(&ajTri_index3);  //�������߷ֱ��ڽӵ������Σ������δ���λ���±꣩

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

//����λ��ƺ���
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
	//��ʼ��λͼ
	CDC *pDC=GetDC();
   // m_pDC=new CClientDC(this);
	m_MemDC.CreateCompatibleDC(pDC);
	m_iBmpWidth=::GetSystemMetrics(SM_CXSCREEN);
	m_iBmpHigh=::GetSystemMetrics(SM_CYSCREEN);
	m_MemBmp.CreateCompatibleBitmap(pDC,m_iBmpWidth,m_iBmpHigh);
	m_MemDC.SelectObject(&m_MemBmp);
	//�ȸ�λͼ���ϰױ���
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
	{//���������
		m_PointOrigin=point;
		m_PointOld=point;
		SetCapture();
	}
	else if(m_iFuncCurrent==FUN_ZOOMIN) 
	{//����ǷŴ�
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
			//���㸳����ֵ				
			pTempPointList->PointArrayX.Add(RealX);
			pTempPointList->PointArrayY.Add(RealY);				
		}
		pDoc->m_IndexList->m_PointList.AddTail(pTempPointList);//�Ѹý��ҵ�m_IndexList��
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
	{//���������
		m_fStartX=m_fStartX-(point.x-m_PointOrigin.x)*m_fBlc;
		m_fStartY=m_fStartY+(point.y-m_PointOrigin.y)*m_fBlc;
		m_iBmpX=0;
		m_iBmpY=0;
		//DrawMapZBA();
	}
	else if(m_iFuncCurrent==FUN_ZOOMIN)
	{// ����ǷŴ����������Ѱ���
	//	pDoc->m_iPushNumb=0;
		x1=min(m_PointOrigin.x,point.x);
		y1=max(m_PointOrigin.y,point.y);
		x2=max(m_PointOrigin.x,point.x);
		y2=min(m_PointOrigin.y,point.y);
		m_iScreenHigh=m_ScreenRect.bottom-m_ScreenRect.top;
		m_iScreenWide=m_ScreenRect.right-m_ScreenRect.left;
		if(x2-x1>3&&y1-y2>3)//������Χ����������Ŵ�
		{
			bl1=(float)m_iScreenWide/(float)(x2-x1);
			bl2=(float)m_iScreenHigh/(float)(y1-y2);
			if(bl2<bl1)
				bl1=bl2;
			if(m_fBl/m_fBlc<15)
			{
				k=1/bl1; //�Ծ��ε����ĵ�Ϊ��Ļ����ȷ��ʼ��
				m_fStartX=(float)(m_fStartX+(point.x-0.8*k*m_iScreenWide)*m_fBlc);
				m_fStartY=(float)(m_fStartY+(m_iScreenHigh*(1-0.1*k)-point.y)*m_fBlc);
				m_fBlc=k*m_fBlc;
			}
		}
		else//��������Χ�����е���Ŵ�
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
		{//���������
			int dx=m_PointOld.x-point.x;
			int dy=m_PointOld.y-point.y;
			ScrollWindow(-dx,-dy,NULL,NULL);
			m_PointOld=point;
			m_iBmpX+=dx;
			m_iBmpY+=dy;
		}
	}
	else 	if(m_iFuncCurrent == FUN_ZOOMIN && nFlags==MK_LBUTTON)
	{//����ǷŴ����������Ѱ���һ��   
		if(point!=m_PointOld)
		{//����
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
	//ȫͼ����Ӧ����
	m_iBmpX=0;
	m_iBmpY=0;
	CVMapDoc* pDoc = GetDocument();
	GetClientRect(&m_ScreenRect);
	//���������
    m_fBlc=(pDoc->m_fMaxX-pDoc->m_fMinX)/(m_ScreenRect.right-m_ScreenRect.left-20);//x�������
	m_fBll=(pDoc->m_fMaxY-pDoc->m_fMinY)/(m_ScreenRect.bottom-m_ScreenRect.top-20);//y�������
	if(m_fBll>m_fBlc) 
		m_fBlc=m_fBll;
	m_fBl=m_fBlc;
	//������Ļ�ĸߺͿ�
	m_iScreenHigh=m_ScreenRect.bottom-m_ScreenRect.top;
	m_iScreenWide=m_ScreenRect.right-m_ScreenRect.left;
    //��ͼ�����Ķ�Ӧ��Ļ����
	m_fStartX=(float)((pDoc->m_fMaxX+pDoc->m_fMinX-m_iScreenWide*m_fBlc)/2);
	m_fStartY=(float)((pDoc->m_fMaxY+pDoc->m_fMinY-m_iScreenHigh*m_fBlc)/2);
	//ͼ
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
		//////����������...
		//��������򿪱��߿ռ��Ҹ�����ֵ   
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
		{	//���߸�����ֵ
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
		//////����������...
		//��������򿪱��߿ռ��Ҹ�����ֵ   
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
		{	//���߸�����ֵ
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
	AfxMessageBox("�����Ŀ�깹��������ģ��ʵ��");

	DrawRefreshStudyArea();//ˢ����Ļ
	
	int i,j;	
	CWaitCursor wait;
	



	////////////////////////////////////////////////////////////////////////////
	//Step1: ��ȡ����֯ʵ����������
	////////////////////////////////////////////////////////////////////////////

	m_polyCount = 0;   //���������
	m_pPolyPtNum = NULL; //ÿ������ΰ����ĵ���
	m_pPolyPt = NULL; //ÿ������εĵ�����
	//�Լ��ӵ�--ÿ����������ĵ�����
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

	//����ȡ�Ķ����Ŀ������Ļ�Ͻ��л���
	for (i=0;i<m_polyCount;i++)
	{	
		
		DrawPolygon(m_pPolyPt[i],m_pPolyPtNum[i],RGB(180,180,180),RGB(220,220,220),1);
		//DrawPolygon(m_pPolycrPt,13,RGB(180,180,180),RGB(220,220,220),5);
		//DrawPolygon(m_pPolycrPt,5,RGB(180,180,180),RGB(220,220,220),5);
		DrawPoint(m_pPolycrPt[i],RGB(0,0,255),3);
		
		//AfxMessageBox("����ε����ĵ�!");

	}	

	AfxMessageBox("��ȡ�Ķ����Ŀ��ɹ�!");


	
	////////////////////////////////////////////////////////////////////////////	
	//Step2: �Զ����Ŀ�깹��Delaunay��������ִ��CreateTIN�����󣬶����Ŀ�깹����
	//��������Ϣ�ɴӱ���m_pDTTriArrayPtr��m_pDTPointArrayPtr��á�
	////////////////////////////////////////////////////////////////////////////

    CreateTIN();

	

	////////////////////////////////////////////////////////////////////////////
	//Step3: ����������Ľṹ����Ϣ��ѯ
	////////////////////////////////////////////////////////////////////////////

	//_1: ������������Ϣ
	long nCount_DTTri;
	m_pDTTriArrayPtr->get_nCount(&nCount_DTTri);//�������������

	for (i=0;i<nCount_DTTri;i++) 
	{
		DrawOneTri(i,RGB(150,150,150),2);

	}
	
//	2: ���ƶ�����Ϣ
	long nCount_DTPoint;
	//m_pDTPointArrayPtr->get_nCount(&nCount_DTPoint); //�����������������
	for (i=0;i<nCount_DTPoint;i++)
	{
		DrawOneDTPoint(i,RGB(0,0,0),3);	
	}

	AfxMessageBox("�������������㼰�����α�!");





	//_3: ��ѯ������ĳ�����������ӵ�������
	/*int index_DTPoint = 15; //������Ҫ��ѯ�Ķ����±�
	DrawOneDTPoint(index_DTPoint,RGB(255,0,0),5);
	char str[200];
	sprintf(str,"�±�Ϊ%d������������",index_DTPoint);
	AfxMessageBox(str);

	LONGARRARY indexList_adjTris;//�����¼���ڵ��������±�(��������m_pDTTriArrayPtr���±�)������
	GetTris_adjacentToOnePoint(index_DTPoint,indexList_adjTris);//��ѯ����

	for (i=0;i<indexList_adjTris.GetSize();i++)
	{
		int index = indexList_adjTris[i];
		DrawOneTri(index,RGB(0,0,255),3);
	}

	DrawOneDTPoint(index_DTPoint,RGB(255,0,0),5);
	char str1[200];
	sprintf(str1,"���±�Ϊ%d�Ķ������ڵ�������",index_DTPoint);
	if(indexList_adjTris.GetSize()>0){indexList_adjTris.RemoveAll();}
	AfxMessageBox(str1);







	//_4: ��ѯ������ĳ�����������ڽ�������������
	int index_DTTri = 50;//������Ҫ��ѯ���������±�
	DrawOneTri(index_DTTri,RGB(180,0,0),RGB(180,0,0),1);
	char str3[200];
	sprintf(str3,"�±�Ϊ%d��������������",index_DTTri);
	AfxMessageBox(str3);

	INTARRARY indexList_adjTris0;//��¼���ڵ��������±�(��������m_pDTTriArrayPtr���±�)
	GetTris_adjacentToOneTri(index_DTTri,indexList_adjTris0);//��ѯ����
	for (i=0;i<indexList_adjTris0.GetSize();i++)
	{
		int index = indexList_adjTris0[i];
		DrawOneTri(index,RGB(0,0,200),RGB(0,0,200),1);
	}
	char str4[200];
	sprintf(str4,"���±�Ϊ%d�����������ڵ�����������",index_DTTri);
	if(indexList_adjTris0.GetSize()>0){indexList_adjTris0.RemoveAll();}
	AfxMessageBox(str4);


	




	//_5: ��ѯ��ĳ������������������������
	int index_Poly = 10; //������Ҫ��ѯ�Ķ����Ŀ���±�
	DrawPolygon(m_pPolyPt[index_Poly],m_pPolyPtNum[index_Poly],RGB(255,0,0),RGB(220,220,220),5);
	char str5[200];
	sprintf(str5,"�±�Ϊ%d�Ķ����Ŀ��",index_Poly);
	AfxMessageBox(str5);

	LONGARRARY IndexList_PolyrelateTri;
	for (i=0;i<m_PointTo.GetSize();i++)
	{
		if (index_Poly == m_PointTo[i])
		{
			LONGARRARY indexList_adjTris;//��¼��ö���������������±�(��������m_pDTTriArrayPtr���±�)
	        GetTris_adjacentToOnePoint(i,indexList_adjTris);//��ȡ���±�Ϊindex_DTPoint���������������
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
	sprintf(str6,"���±�Ϊ%d�Ķ����Ŀ�������ӵ�������",index_Poly);

	AfxMessageBox(str6);



	//��һ���أ���ȡ��ö����1���ڽ������������
	LONGARRARY Ad_polylist;
	for (i=0;i<IndexList_PolyrelateTri.GetSize();i++)
	{
       long index_temp = IndexList_PolyrelateTri[i];
	   IDTTriPtr oneDTTri;
	   m_pDTTriArrayPtr->get_element(index_temp,&oneDTTri); //����±�Ϊi��������
	   
	   long no1,no2,no3;
	   oneDTTri->get_PointNo1(&no1); //��������������ı��
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
	sprintf(str7,"���±�Ϊ%d�Ķ����Ŀ��1���ڽӵ����������",index_Poly);
	AfxMessageBox(str7);

	if(IndexList_PolyrelateTri.GetSize()>0){IndexList_PolyrelateTri.RemoveAll();}*/






	////////////////////////////////////////////////////////////////////////////
	//Step5: �ͷ��ڴ�
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
	AfxMessageBox("����ʵ��!");
	int i,j;	
	CWaitCursor wait;

	////////////////////////////////             ////////////////////////////////////////////
	//Step1: ��ȡ����֯ʵ������
	////////////////////////////////////////////////////////////////////////////
	
	m_polyCount = 0;   //���������
	m_pPolyPtNum = NULL; //ÿ������ΰ����ĵ���
	m_pPolyPt = NULL; //ÿ������εĵ�����
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
	//Step2: �Զ����Ŀ�깹��Delaunay��������ִ��CreateTIN�����󣬶����Ŀ�깹����
	//��������Ϣ�ɴӱ���m_pDTTriArrayPtr��m_pDTPointArrayPtr��á�
	////////////////////////////////////////////////////////////////////////////

    CreateTIN();
	


	////////////////////////////////////////////////////////////////////////////
	//Step3: Insert your programmes;
	////////////////////////////////////////////////////////////////////////////
	
	long nCount_DTTri;
	m_pDTTriArrayPtr->get_nCount(&nCount_DTTri);//�������������
	tritag=new int[nCount_DTTri];
	for (i=0;i<nCount_DTTri;i++) 
	{

		DrawOneTrinew(i,RGB(150,150,150),1);
		INTARRARY indexList_adjTris0;//��¼���ڵ��������±�(��������m_pDTTriArrayPtr���±�)
		GetTris_adjacentToOneTri(i,indexList_adjTris0);//��ѯ����
		
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
	//Step4: �ͷ��ڴ�
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
	AfxMessageBox("���ɶ�������ĵ�������!");

	DrawRefreshStudyArea();//ˢ����Ļ
	
	int i,j;	
	CWaitCursor wait;
	



	////////////////////////////////////////////////////////////////////////////
	//Step1: ��ȡ����֯ʵ����������
	////////////////////////////////////////////////////////////////////////////

	m_polyCount = 0;   //���������
	m_pPolyPtNum = NULL; //ÿ������ΰ����ĵ���
	m_pPolyPt = NULL; //ÿ������εĵ�����
	//�Լ��ӵ�--ÿ����������ĵ�����
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

	//����ȡ�Ķ����Ŀ������Ļ�Ͻ��л���
	for (i=0;i<m_polyCount;i++)
	{	
		
		DrawPolygon(m_pPolyPt[i],m_pPolyPtNum[i],RGB(180,180,180),RGB(220,220,220),1);
		//DrawPolygon(m_pPolycrPt,13,RGB(180,180,180),RGB(220,220,220),5);
		//DrawPolygon(m_pPolycrPt,5,RGB(180,180,180),RGB(220,220,220),5);
		DrawPoint(m_pPolycrPt[i],RGB(0,0,255),3);
		
		//AfxMessageBox("����ε����ĵ�!");

	}	

	AfxMessageBox("��ȡ�Ķ����Ŀ��ɹ�!");


	
	////////////////////////////////////////////////////////////////////////////	
	//Step2: �Զ����Ŀ�깹��Delaunay��������ִ��CreateTIN�����󣬶����Ŀ�깹����
	//��������Ϣ�ɴӱ���m_pDTTriArrayPtr��m_pDTPointArrayPtr��á�
	////////////////////////////////////////////////////////////////////////////

    CreateTINnew();

	

	////////////////////////////////////////////////////////////////////////////
	//Step3: ����������Ľṹ����Ϣ��ѯ
	////////////////////////////////////////////////////////////////////////////

	//_1: ������������Ϣ
	long nCount_DTTri;
	m_pDTTriArrayPtr->get_nCount(&nCount_DTTri);//�������������

	for (i=0;i<nCount_DTTri;i++) 
	{
		DrawOneTri(i,RGB(150,150,150),2);

	}
	
//	2: ���ƶ�����Ϣ
	long nCount_DTPoint;
	//m_pDTPointArrayPtr->get_nCount(&nCount_DTPoint); //�����������������
	for (i=0;i<nCount_DTPoint;i++)
	{
		DrawOneDTPoint(i,RGB(0,0,0),3);	
	}

	AfxMessageBox("�������������㼰�����α�!");
	FreeBuffer();	 
	
	wait.Restore();	
}



void CVMapView::CreateTINnew()
{
	////////////////////////////////////////////////////////////////////////////////////
	//Step1: ��ʼ������ӿ�
	//���ڴ˴�����������������һ��com������ṩ����Ҫcom�����ز��������Ǵ�Ҳ������
	////////////////////////////////////////////////////////////////////////////////////
	IAGeneralizationPtr _iGenCore = Initialization_module();



	////////////////////////////////////////////////////////////////////////////	
	//Step2: ���������ȺDelaunay������ģ��
	////////////////////////////////////////////////////////////////////////////

	//Step2-1:�������Ŀ����֯Ϊ��Ⱥ�ṹ��Ϊ�����������ݣ�
	m_pInMultiPt.CreateInstance(_uuidof(DoMultipoint));//���ȣ�����һ���㴮����װ�ع��ɶ���εĵ�Ⱥ����
	
	//���ݶ������������������//
	////////////////////////////////
	int i,j;
	for (i=0;i<m_polyCount;i++) 
	{
		int count_pt=0;
		for (j=0;j<m_polyCount;j++)//ע�⣬�������β����ͬ��Ԥ����ʱȥ��ĩβ��(�����ظ���)
		{
			IDoPointPtr pDoPt;		       
			pDoPt.CreateInstance(_uuidof(doPoint));
			pDoPt->put_RealX(m_pPolycrPt[j].x);
			pDoPt->put_RealY(m_pPolycrPt[j].y);
	 		m_pInMultiPt->AddPoint(pDoPt);
			m_PointTo.Add(i);
			count_pt++;
		}
		m_Polypointnumarray.Add(count_pt); //��¼ÿ������εĵ���
	}

	/*int i,j;
	for (i=0;i<m_polyCount;i++) 
	{
		int count_pt=0;
		for (j=0;j<m_pPolyPtNum[i]-1;j++)//ע�⣬�������β����ͬ��Ԥ����ʱȥ��ĩβ��(�����ظ���)
		{
			IDoPointPtr pDoPt;		       
			pDoPt.CreateInstance(_uuidof(doPoint));
			pDoPt->put_RealX(m_pPolyPt[i][j].x);
			pDoPt->put_RealY(m_pPolyPt[i][j].y);
	 		m_pInMultiPt->AddPoint(pDoPt);
			m_PointTo.Add(i);
			count_pt++;
		}
		m_Polypointnumarray.Add(count_pt); //��¼ÿ������εĵ���
	}*/



	
	//Step2-2: ʵʩ����

	//��ʼ�����������㴮�ӿڶ���(pOutDTPointArrayPtr)�����ڼ�¼�����������������������Ϣ
	m_pDTPointArrayPtr.CreateInstance(_uuidof(DTpointArray)); 

	//��ʼ�������������δ��ӿڶ���(pOutDTTriArrayPtr)��װ�ؽ������������������������Ϣ
	m_pDTTriArrayPtr.CreateInstance(_uuidof(DTTriArray));  

	//���Delaunay������ģ�Ͷ���ӿ�(pIDelaunayTIN)���ɸĶ���ӿ��ṩ����������������ֱ�ӵ��ã����ù����ڲ����ƣ�
   	IAGeneralizationPtr _iGenProcHandler = (IAGeneralizationPtr)_iGenCore; 
	IDelaunayTINPtr pIDelaunayTIN;
	HRESULT res =  _iGenProcHandler->get_DelaunayTIN(&pIDelaunayTIN); 

	//����������,pInMultiPt��>����Ĳ��뽨���㴮��Ϣ��pOut_DTPointArrayPtr��>�����������������Ϣ��pOut_DTTriArrayPtr��>�������������������Ϣ
	HRESULT	hr = pIDelaunayTIN->CreatTIN(m_pInMultiPt,&m_pDTPointArrayPtr,&m_pDTTriArrayPtr);
}

void CVMapView::DrawOneTrinew(int index_DTtri, COLORREF pencolor, int width)
{
	if (index_DTtri<0)
	{
		AfxMessageBox("���Ƶ��������������±����!");
		return;
	}

	IDTTriPtr oneDTTri;
	m_pDTTriArrayPtr->get_element(index_DTtri,&oneDTTri); //����±�Ϊi��������

	long no1,no2,no3;
	oneDTTri->get_PointNo1(&no1); //��������������ı��
	oneDTTri->get_PointNo2(&no2);
	oneDTTri->get_PointNo3(&no3);
	
	IDTPointPtr DTPoint1,DTPoint2,DTPoint3;
	m_pDTPointArrayPtr->get_element(no1,&DTPoint1); //���ݱ����������㴮�У�pOut_DTPointArrayPt���õ��������
	m_pDTPointArrayPtr->get_element(no2,&DTPoint2);
	m_pDTPointArrayPtr->get_element(no3,&DTPoint3);
	
	
	FPOINT* ptList = NULL;
	ptList = new FPOINT[4];
	
	double xx,yy;
	DTPoint1->get_coorX(&xx);//���ÿ�������ζ��������
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
	//�Լ��ӵ�
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
		DrawPolyline(ptList,3,pencolor, width);	//���������α���Ϣ
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
