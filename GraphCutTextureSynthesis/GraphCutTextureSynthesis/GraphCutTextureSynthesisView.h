
// GraphCutTextureSynthesisView.h : CGraphCutTextureSynthesisView クラスのインターフェイス
//

#pragma once


class CGraphCutTextureSynthesisView : public CView
{
protected: // シリアル化からのみ作成します。
	CGraphCutTextureSynthesisView();
	DECLARE_DYNCREATE(CGraphCutTextureSynthesisView)

// 属性
public:
	CGraphCutTextureSynthesisDoc* GetDocument() const;

// 操作
public:

// オーバーライド
public:
	virtual void OnDraw(CDC* pDC);  // このビューを描画するためにオーバーライドされます。
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 実装
public:
	virtual ~CGraphCutTextureSynthesisView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成された、メッセージ割り当て関数
protected:
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // GraphCutTextureSynthesisView.cpp のデバッグ バージョン
inline CGraphCutTextureSynthesisDoc* CGraphCutTextureSynthesisView::GetDocument() const
   { return reinterpret_cast<CGraphCutTextureSynthesisDoc*>(m_pDocument); }
#endif

