// GraphCutTexture.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "GraphCutTexture.h"

#include "atlimage.h"
#include "TMaxFlow_BK4.h"
#include <vector>



#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 唯一のアプリケーション オブジェクトです。

CWinApp theApp;

using namespace std;




class TImage 
{
public:
	int W,H;
	byte *rgba;
	
	TImage(){
		W = H = 0;
		rgba = 0;
	}

	bool isPixBlue(const int idx) const
	{
		return rgba[idx + 0] == 0 && 
			   rgba[idx + 1] == 0 && 
			   rgba[idx + 2] == 255;
	}



	void allocate(const int _W, const int _H){
		W = _W;
		H = _H;
		rgba = new byte[W * H * 4];
	}


	bool load(const char *fname )
	{
		//bmp jpeg png tiff画像の読み込み(bmp jpegのみ動作確認済み)
		CImage pic;
		if (!SUCCEEDED(pic.Load(_T(fname)))) return false;

		if( rgba ) delete[] rgba;
		rgba = 0;

		int pitch = pic.GetPitch();
		if (pitch < 0) pitch *= -1;

		W = pic.GetWidth();
		H = pic.GetHeight();
		rgba = new byte[ W * H * 4 ];

		//byte for 1 pixel
		int byteNum = pitch / W;

		if (byteNum == 3) 
		{
			for (int y = 0, i = 0; y < H; ++y)
			{
				for (int x = 0; x < W; ++x, ++i)
				{
					//RGB(24bit)に対するアドレスをbyteにキャストしてるので, R G B順の一番下位のBを指すものになる
					byte *c = (byte*)pic.GetPixelAddress(x, y);
					rgba[i * 4 + 3] = 128;
					rgba[i * 4 + 2] = *c; ++c;
					rgba[i * 4 + 1] = *c; ++c;
					rgba[i * 4 + 0] = *c;
				}
			}
		}
		else
		{
			for (int y = 0, i = 0; y < H; ++y)
			{
				for (int x = 0; x < W; ++x, ++i)
				{
					COLORREF c = pic.GetPixel(x, y); //遅い
					rgba[i * 4 + 3] = 128;
					rgba[i * 4 + 0] = GetRValue(c);
					rgba[i * 4 + 1] = GetGValue(c);
					rgba[i * 4 + 2] = GetBValue(c);
				}
			}
		}
		return true;
	}



	bool saveAsFile(const char *fname, int flg_BmpJpgPngTiff)
	{
		CImage pic;
		pic.Create(W, H, 24, 0);
		for (unsigned int y = 0, i = 0; y < H; ++y)
			for (unsigned int x = 0; x < W; ++x, ++i)
			{
				byte *c = (byte*)pic.GetPixelAddress(x, y);
				*c = rgba[i * 4 + 2]; ++c;
				*c = rgba[i * 4 + 1]; ++c;
				*c = rgba[i * 4 + 0];
			}
		if (     flg_BmpJpgPngTiff == 0)  pic.Save(_T(fname), Gdiplus::ImageFormatBMP );
		else if (flg_BmpJpgPngTiff == 1)  pic.Save(_T(fname), Gdiplus::ImageFormatJPEG);
		else if (flg_BmpJpgPngTiff == 2)  pic.Save(_T(fname), Gdiplus::ImageFormatPNG );
		else if (flg_BmpJpgPngTiff == 3)  pic.Save(_T(fname), Gdiplus::ImageFormatTIFF);
		return true;
	}
};



static inline bool t_isBlue(const int I, const byte* rgba)
{
	return rgba[I + 0] == 0 && rgba[I + 1] == 0 && rgba[I + 2] == 255;
}




class PixInf
{
public:
	//patch id
	int  patchID;

	//edge cost (if exist)
	double eCostX;
	double eCostY;


	PixInf(){
		patchID = -1;
		eCostX = eCostY = 0;
	}

	void setPatchId(const int _patchId){
		patchID =_patchId;
	}

};

static inline double t_calcEdgeVal(
	const byte *As, const byte *Bs,
	const byte *At, const byte *Bt )
{
	double ds = 
		(As[0] - Bs[0]) * (As[0] - Bs[0]) + 
		(As[1] - Bs[1]) * (As[1] - Bs[1]) + 
		(As[2] - Bs[2]) * (As[2] - Bs[2]);
	double dt =
		(At[0] - Bt[0]) * (At[0] - Bt[0]) +
		(At[1] - Bt[1]) * (At[1] - Bt[1]) +
		(At[2] - Bt[2]) * (At[2] - Bt[2]);

	return sqrt(ds) + sqrt(dt);
}

static void t_exportBmp(const char *fname, const int W, const int H,  const byte *rgba)
{
	TImage test;
	test.allocate(W, H);
	memcpy(test.rgba, rgba, 4 * sizeof(byte) * W * H);
	test.saveAsFile(fname, 0);
}




/*
eCostX represent (a) edge cost
eCostY represent (b) edge cost

       ( i , j ) -a- ( i ,j+1)
	       |            |
		   b            |
		   |            |
	   (i+1, j ) -- (i+1,j+1)


	   o: node
	   s: source
	   t: sink 

    +--- o o o o ---+  
(s)-+--- o o o o ---+- (t)
    +--- o o o o ---+ 
	
	pixel node id: (0, nodeN-1)
	edge  node id] (nodeN, nodeN+edgeNodeN-1)
	source and sink ID: (nodeN+edgeNodeN, nodeN+edgeNodeN+1)

	edgeNodePix is connected to (t)
*/




static void t_graphCutTexture_placePatch
(
	const int W, 
	const int H,
	const byte *imgA  , //rgba base image
	const byte *imgB  , //rgba new patch
	byte       *resImg, //rgba results
	PixInf     *pixInfo,  
	const int newPatchId
)
{
	//count pixel nodes (overlap pixel) and edge nodes

	byte *imgFlg       = new byte[W*H]; //0 non, 1:imgA, 2:imgB, 3:both
	int  *imgNodeID    = new int [W*H];
	int  *mapNodeToPix = new int [W*H];

	int N_pNode = 0, N_eNode = 0;

	for (int y = 0; y < H; ++y)
	{
		for (int x = 0; x < W; ++x)
		{
			const int I = x + y*W;
			bool isA = !t_isBlue(4 * I, imgA);
			bool isB = !t_isBlue(4 * I, imgB);
			imgFlg[ I ] = ( !isA && !isB ) ? 0 :
						  (  isA && !isB ) ? 1 :
						  ( !isA &&  isB ) ? 2 : 3;

			imgNodeID[I] = -1;

			if ( imgFlg[I] == 3){
				imgNodeID[I] = N_pNode;
				mapNodeToPix[N_pNode] = I;
				N_pNode++;
			}
		}
	}

	for (int y = 0; y < H; ++y)
	{
		for (int x = 0; x < W; ++x)
		{
			const int I = x + y*W;
			if (x != W - 1 && pixInfo[I].eCostX > 0 && imgNodeID[I] != -1 && imgNodeID[ I + 1 ] != -1) N_eNode++;
			if (y != H - 1 && pixInfo[I].eCostY > 0 && imgNodeID[I] != -1 && imgNodeID[ I + W ] != -1) N_eNode++;
		}
	}


	//generate a graph
	const int SourceID = N_eNode + N_pNode   ;
	const int SinkID   = N_eNode + N_pNode +1;
	TFlowNetwork_BK4 graph( N_pNode + N_eNode + 2, N_pNode * 5 + N_eNode * 2, SourceID, SinkID);

	int seamNodeI = 0;


	//gen n-Link
	for (int y = 0; y < H; ++y) for (int x = 0; x < W; ++x)
	{
		const int S = x + y*W;

		if (imgNodeID[ S ] == -1) continue;

		for( int k = 0; k < 2; ++k)
		{
			const int T = (k == 0) ? S + 1 : S + W;

			if (k == 0 && x == W - 1) continue;
			if (k == 1 && y == H - 1) continue;
			if (imgNodeID[T] == -1  ) continue;

			const double eCost = ( k == 0 ) ? pixInfo[S].eCostX : pixInfo[S].eCostY;
			const double capa  = t_calcEdgeVal( &imgA[4*S], &imgB[4 * S], &imgA[4*T], &imgB[4*T]);

			if ( eCost > 0 )
			{
				const int si = N_pNode + seamNodeI;
				graph.add_nLink(imgNodeID[S], si, capa);
				graph.add_nLink(si, imgNodeID[T], capa);
				graph.add_tLink(SourceID, SinkID, si, 0, eCost);
				seamNodeI++;
			}
			else
				graph.add_nLink(imgNodeID[S], imgNodeID[T], capa);
		}
	}

	//gen t Link (strict constrains)
	int fixAn = 0, fixBn = 0;
	for (int y = 0; y < H; ++y) for (int x = 0; x < W; ++x)
	{
		const int I = x + y*W;
		if ( imgNodeID[I] == -1 ) continue;

		bool bAdjA = (x > 0    && imgFlg[I - 1] == 1) || (y > 0    && imgFlg[I - W] == 1) || 
			         (x <W - 1 && imgFlg[I + 1] == 1) || (y <H - 1 && imgFlg[I + W] == 1);
		bool bAdjB = (x > 0    && imgFlg[I - 1] == 2) || (y > 0    && imgFlg[I - W] == 2) ||
			         (x <W - 1 && imgFlg[I + 1] == 2) || (y <H - 1 && imgFlg[I + W] == 2);
		
		if ( bAdjA && !bAdjB) graph.add_tLink(SourceID, SinkID, imgNodeID[I], 100000, 0);
		if (!bAdjA &&  bAdjB) graph.add_tLink(SourceID, SinkID, imgNodeID[I], 0, 100000);
	}


	//graph cut
	byte *minCut = new byte[N_pNode + N_eNode + 2];
	graph.calcMaxFlow(SourceID, SinkID, minCut);

	//update edge info
	for (int y = 0; y < H; ++y) for (int x = 0; x < W; ++x)
	{
		const int I = x + y*W, I4 = I * 4;
		if (imgNodeID[I] == -1) continue;

		//imgBに飲み込まれてたら削除
		//imgAとBの境界にあったら新たに追加
	}

	//update image
	for (int y = 0; y < H; ++y) for (int x = 0; x < W; ++x)
	{
		const int I = x + y*W, I4 = I*4;
		
		resImg[I4  ] = resImg[I4+1] = 0;
		resImg[I4+2] = 255;

		if      (imgFlg[I] == 1) memcpy(&resImg[I4], &imgA[I4], sizeof(byte) * 4);
		else if (imgFlg[I] == 2) memcpy(&resImg[I4], &imgB[I4], sizeof(byte) * 4);
		else if (imgFlg[I] == 3)
		{		
			if( minCut[imgNodeID[I]] ) memcpy(&resImg[I4], &imgA[I4], sizeof(byte) * 4);
			else                       memcpy(&resImg[I4], &imgB[I4], sizeof(byte) * 4);
		}
	}


	delete[] imgFlg;
	delete[] imgNodeID;
	delete[] mapNodeToPix;
	delete[] minCut;
}






static void t_graphCutTexture(const vector<TImage> &imgs, TImage &outImg)
{
	const int W = imgs[0].W;
	const int H = imgs[0].H;

	PixInf *pixInfo = new PixInf[W*H];
	byte   *resImg  = new byte  [W*H * 4];



	//一枚目を配置し
	memcpy(resImg, imgs[0].rgba, sizeof(byte) * W * H * 4);

	for( int i=0; i < W*H; ++i)
	{
		if( !t_isBlue(4 * i, resImg) ) pixInfo[i].setPatchId( 0 );
	}


	//2枚目以降を追加
	for( int i = 1; i < imgs.size(); ++i)
	{
		printf("counter %d\n", i);

		byte *tmp = new byte[W*H * 4];
		t_graphCutTexture_placePatch ( W,H, resImg, imgs[i].rgba, tmp, pixInfo, i );
		memcpy( resImg, tmp, sizeof(byte) * W * H * 4);

		CString str;
		str.Format("test%d.bmp", i);
		t_exportBmp( str, W,H,resImg);

		delete[] tmp;

	}

	delete[] pixInfo;
	delete[] resImg ;
}






int main()
{
    int nRetCode = 0;

    HMODULE hModule = ::GetModuleHandle(nullptr);

    if (hModule != nullptr)
    {
        // MFC を初期化して、エラーの場合は結果を印刷します。
        if (!AfxWinInit(hModule, nullptr, ::GetCommandLine(), 0))
        {
            // TODO: 必要に応じてエラー コードを変更してください。
            wprintf(L"致命的なエラー: MFC の初期化ができませんでした。\n");
            nRetCode = 1;
        }
        else
        {
            // TODO: アプリケーションの動作を記述するコードをここに挿入してください。
        }
    }
    else
    {
        // TODO: 必要に応じてエラー コードを変更してください。
        wprintf(L"致命的なエラー: GetModuleHandle が失敗しました\n");
        nRetCode = 1;
    }


	//input image
	vector<TImage> imgs(3);
	imgs[0].load("p1.bmp");
	imgs[1].load("p2.bmp");
	imgs[2].load("p3.bmp");
	//imgs[3].load("p4.bmp");

	//resolutions of images are suppose to be the same
	const int W = imgs[0].W;
	const int H = imgs[0].H;

	//output image 
	TImage outImg;
	outImg.allocate(W, H);

	t_graphCutTexture(imgs, outImg);







    return nRetCode;
}
