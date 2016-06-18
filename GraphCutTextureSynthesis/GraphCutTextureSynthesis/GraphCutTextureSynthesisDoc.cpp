
// GraphCutTextureSynthesisDoc.cpp : CGraphCutTextureSynthesisDoc クラスの実装
//

#include "stdafx.h"
// SHARED_HANDLERS は、プレビュー、縮小版、および検索フィルター ハンドラーを実装している ATL プロジェクトで定義でき、
// そのプロジェクトとのドキュメント コードの共有を可能にします。
#ifndef SHARED_HANDLERS
#include "GraphCutTextureSynthesis.h"
#endif

#include "GraphCutTextureSynthesisDoc.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CGraphCutTextureSynthesisDoc

IMPLEMENT_DYNCREATE(CGraphCutTextureSynthesisDoc, CDocument)

BEGIN_MESSAGE_MAP(CGraphCutTextureSynthesisDoc, CDocument)
END_MESSAGE_MAP()


// CGraphCutTextureSynthesisDoc コンストラクション/デストラクション

CGraphCutTextureSynthesisDoc::CGraphCutTextureSynthesisDoc()
{
	// TODO: この位置に 1 度だけ呼ばれる構築用のコードを追加してください。

}

CGraphCutTextureSynthesisDoc::~CGraphCutTextureSynthesisDoc()
{
}

BOOL CGraphCutTextureSynthesisDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: この位置に再初期化処理を追加してください。
	// (SDI ドキュメントはこのドキュメントを再利用します。

	return TRUE;
}




// CGraphCutTextureSynthesisDoc シリアル化

void CGraphCutTextureSynthesisDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 格納するコードをここに追加してください。
	}
	else
	{
		// TODO: 読み込むコードをここに追加してください。
	}
}

#ifdef SHARED_HANDLERS

//縮小版のサポート
void CGraphCutTextureSynthesisDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// このコードを変更してドキュメントのデータを描画します
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// 検索ハンドラーのサポート
void CGraphCutTextureSynthesisDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// ドキュメントのデータから検索コンテンツを設定します。
	// コンテンツの各部分は ";" で区切る必要があります

	// 例:      strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CGraphCutTextureSynthesisDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CGraphCutTextureSynthesisDoc 診断

#ifdef _DEBUG
void CGraphCutTextureSynthesisDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CGraphCutTextureSynthesisDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CGraphCutTextureSynthesisDoc コマンド
