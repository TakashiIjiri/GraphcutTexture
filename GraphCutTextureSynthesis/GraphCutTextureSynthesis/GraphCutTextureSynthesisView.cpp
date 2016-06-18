
// GraphCutTextureSynthesisView.cpp : CGraphCutTextureSynthesisView クラスの実装
//

#include "stdafx.h"
// SHARED_HANDLERS は、プレビュー、縮小版、および検索フィルター ハンドラーを実装している ATL プロジェクトで定義でき、
// そのプロジェクトとのドキュメント コードの共有を可能にします。
#ifndef SHARED_HANDLERS
#include "GraphCutTextureSynthesis.h"
#endif

#include "GraphCutTextureSynthesisDoc.h"
#include "GraphCutTextureSynthesisView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CGraphCutTextureSynthesisView

IMPLEMENT_DYNCREATE(CGraphCutTextureSynthesisView, CView)

BEGIN_MESSAGE_MAP(CGraphCutTextureSynthesisView, CView)
	// 標準印刷コマンド
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
END_MESSAGE_MAP()

// CGraphCutTextureSynthesisView コンストラクション/デストラクション

CGraphCutTextureSynthesisView::CGraphCutTextureSynthesisView()
{
	// TODO: 構築コードをここに追加します。

}

CGraphCutTextureSynthesisView::~CGraphCutTextureSynthesisView()
{
}

BOOL CGraphCutTextureSynthesisView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: この位置で CREATESTRUCT cs を修正して Window クラスまたはスタイルを
	//  修正してください。

	return CView::PreCreateWindow(cs);
}

// CGraphCutTextureSynthesisView 描画

void CGraphCutTextureSynthesisView::OnDraw(CDC* /*pDC*/)
{
	CGraphCutTextureSynthesisDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: この場所にネイティブ データ用の描画コードを追加します。
}


// CGraphCutTextureSynthesisView 印刷

BOOL CGraphCutTextureSynthesisView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 既定の印刷準備
	return DoPreparePrinting(pInfo);
}

void CGraphCutTextureSynthesisView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 印刷前の特別な初期化処理を追加してください。
}

void CGraphCutTextureSynthesisView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 印刷後の後処理を追加してください。
}


// CGraphCutTextureSynthesisView 診断

#ifdef _DEBUG
void CGraphCutTextureSynthesisView::AssertValid() const
{
	CView::AssertValid();
}

void CGraphCutTextureSynthesisView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CGraphCutTextureSynthesisDoc* CGraphCutTextureSynthesisView::GetDocument() const // デバッグ以外のバージョンはインラインです。
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CGraphCutTextureSynthesisDoc)));
	return (CGraphCutTextureSynthesisDoc*)m_pDocument;
}
#endif //_DEBUG


// CGraphCutTextureSynthesisView メッセージ ハンドラー
