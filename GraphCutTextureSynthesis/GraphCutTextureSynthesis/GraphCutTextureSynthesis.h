
// GraphCutTextureSynthesis.h : GraphCutTextureSynthesis アプリケーションのメイン ヘッダー ファイル
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH に対してこのファイルをインクルードする前に 'stdafx.h' をインクルードしてください"
#endif

#include "resource.h"       // メイン シンボル


// CGraphCutTextureSynthesisApp:
// このクラスの実装については、GraphCutTextureSynthesis.cpp を参照してください。
//

class CGraphCutTextureSynthesisApp : public CWinApp
{
public:
	CGraphCutTextureSynthesisApp();


// オーバーライド
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// 実装
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CGraphCutTextureSynthesisApp theApp;
