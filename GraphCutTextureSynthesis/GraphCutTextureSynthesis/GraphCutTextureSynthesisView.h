
// GraphCutTextureSynthesisView.h : CGraphCutTextureSynthesisView �N���X�̃C���^�[�t�F�C�X
//

#pragma once


class CGraphCutTextureSynthesisView : public CView
{
protected: // �V���A��������̂ݍ쐬���܂��B
	CGraphCutTextureSynthesisView();
	DECLARE_DYNCREATE(CGraphCutTextureSynthesisView)

// ����
public:
	CGraphCutTextureSynthesisDoc* GetDocument() const;

// ����
public:

// �I�[�o�[���C�h
public:
	virtual void OnDraw(CDC* pDC);  // ���̃r���[��`�悷�邽�߂ɃI�[�o�[���C�h����܂��B
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// ����
public:
	virtual ~CGraphCutTextureSynthesisView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// �������ꂽ�A���b�Z�[�W���蓖�Ċ֐�
protected:
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // GraphCutTextureSynthesisView.cpp �̃f�o�b�O �o�[�W����
inline CGraphCutTextureSynthesisDoc* CGraphCutTextureSynthesisView::GetDocument() const
   { return reinterpret_cast<CGraphCutTextureSynthesisDoc*>(m_pDocument); }
#endif

