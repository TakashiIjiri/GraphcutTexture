
// GraphCutTextureSynthesisView.cpp : CGraphCutTextureSynthesisView �N���X�̎���
//

#include "stdafx.h"
// SHARED_HANDLERS �́A�v���r���[�A�k���ŁA����ь����t�B���^�[ �n���h���[���������Ă��� ATL �v���W�F�N�g�Œ�`�ł��A
// ���̃v���W�F�N�g�Ƃ̃h�L�������g �R�[�h�̋��L���\�ɂ��܂��B
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
	// �W������R�}���h
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
END_MESSAGE_MAP()

// CGraphCutTextureSynthesisView �R���X�g���N�V����/�f�X�g���N�V����

CGraphCutTextureSynthesisView::CGraphCutTextureSynthesisView()
{
	// TODO: �\�z�R�[�h�������ɒǉ����܂��B

}

CGraphCutTextureSynthesisView::~CGraphCutTextureSynthesisView()
{
}

BOOL CGraphCutTextureSynthesisView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: ���̈ʒu�� CREATESTRUCT cs ���C������ Window �N���X�܂��̓X�^�C����
	//  �C�����Ă��������B

	return CView::PreCreateWindow(cs);
}

// CGraphCutTextureSynthesisView �`��

void CGraphCutTextureSynthesisView::OnDraw(CDC* /*pDC*/)
{
	CGraphCutTextureSynthesisDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: ���̏ꏊ�Ƀl�C�e�B�u �f�[�^�p�̕`��R�[�h��ǉ����܂��B
}


// CGraphCutTextureSynthesisView ���

BOOL CGraphCutTextureSynthesisView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// ����̈������
	return DoPreparePrinting(pInfo);
}

void CGraphCutTextureSynthesisView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: ����O�̓��ʂȏ�����������ǉ����Ă��������B
}

void CGraphCutTextureSynthesisView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: �����̌㏈����ǉ����Ă��������B
}


// CGraphCutTextureSynthesisView �f�f

#ifdef _DEBUG
void CGraphCutTextureSynthesisView::AssertValid() const
{
	CView::AssertValid();
}

void CGraphCutTextureSynthesisView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CGraphCutTextureSynthesisDoc* CGraphCutTextureSynthesisView::GetDocument() const // �f�o�b�O�ȊO�̃o�[�W�����̓C�����C���ł��B
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CGraphCutTextureSynthesisDoc)));
	return (CGraphCutTextureSynthesisDoc*)m_pDocument;
}
#endif //_DEBUG


// CGraphCutTextureSynthesisView ���b�Z�[�W �n���h���[
