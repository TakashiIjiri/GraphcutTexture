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
	
	TImage(){ W = H = 0; rgba = 0; }
	~TImage(){ 
		if( rgba ) delete[] rgba;
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



static void t_exportBmp(const char *fname, const int W, const int H,  const byte *rgba)
{
	TImage test;
	test.allocate(W, H);
	memcpy(test.rgba, rgba, 4 * sizeof(byte) * W * H);
	test.saveAsFile(fname, 0);
}


static inline bool t_isBlue(const int I, const byte* rgba)
{
	return rgba[I + 0] == 0 && rgba[I + 1] == 0 && rgba[I + 2] == 255;
}




class PixInf
{
public:
	//patch id
	int    patchID;
	double eCostX ;
	double eCostY ;

	PixInf(){
		patchID = -1;
		eCostX = eCostY = 0;
	}
};


static inline double t_calcEdgeVal(
	const byte *As, const byte *Bs,
	const byte *At, const byte *Bt )
{
	double ds = (As[0] - Bs[0]) * (As[0] - Bs[0]) + 
				(As[1] - Bs[1]) * (As[1] - Bs[1]) + 
				(As[2] - Bs[2]) * (As[2] - Bs[2]);
	double dt = (At[0] - Bt[0]) * (At[0] - Bt[0]) +
				(At[1] - Bt[1]) * (At[1] - Bt[1]) +
				(At[2] - Bt[2]) * (At[2] - Bt[2]);
	return sqrt(ds) + sqrt(dt);
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
	const int   W, 
	const int   H,
	const byte *imgA  , //rgba base image
	const byte *imgB  , //rgba new patch
	byte       *resImg, //rgba results
	PixInf     *pixInfo,  
	const int newPatchId
)
{
	const int WH = W*H;

	int N_pNode = 0, N_eNode = 0;
	byte *imgFlg       = new byte[WH]; //0 non, 1:imgA, 2:imgB, 3:both
	int  *imgNodeID    = new int [WH];

	//count pixel nodes (overlap pixels) 
	for( int i = 0; i < WH; ++i)
	{
		bool isA = !t_isBlue(4 * i, imgA);
		bool isB = !t_isBlue(4 * i, imgB);
		imgFlg[ i ] = ( !isA && !isB ) ? 0 :
					  (  isA && !isB ) ? 1 :
					  ( !isA &&  isB ) ? 2 : 3;

		imgNodeID[i] = -1;
		if ( imgFlg[i] == 3) imgNodeID[i] = N_pNode++;
	}

	//count pixel nodes (existing edges)
	for( int i = 0; i < WH; ++i)
	{
		if (pixInfo[i].eCostX > 0 && imgFlg[i] == 3 && imgFlg[ i + 1 ] == 3) N_eNode++;
		if (pixInfo[i].eCostY > 0 && imgFlg[i] == 3 && imgFlg[ i + W ] == 3) N_eNode++;
	}


	//generate a graph
	const int SourceID = N_pNode + N_eNode   ;
	const int SinkID   = N_pNode + N_eNode +1;
	TFlowNetwork_BK4 graph( N_pNode + N_eNode + 2, N_pNode * 5 + N_eNode * 2, SourceID, SinkID);

	int seamNodeI = 0;

	//gen n-Link (img[S]-img[T])
	for (int S = 0; S < WH; ++S) if( imgNodeID[ S ] >= 0)
	{
		const int x = S % W;
		const int y = S / W;

		for( int k = 0; k < 2; ++k)
		{
			if ( (k == 0 && x == W - 1) || (k == 1 && y == H - 1) ) continue;

			const int T = (k == 0) ? S + 1 : S + W;
			if (imgNodeID[T] == -1  ) continue;

			const double eCost = ( k == 0 ) ? pixInfo[S].eCostX : pixInfo[S].eCostY;
			const double capa  = t_calcEdgeVal( &imgA[4*S], &imgB[4*S], &imgA[4*T], &imgB[4*T]);

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
	for (int y = 0; y < H; ++y)
	{
		for (int x = 0; x < W; ++x)
		{
			const int I = x + y*W;
			if ( imgNodeID[I] == -1 ) continue;

			bool bAdjA = (x > 0 && imgFlg[I-1] == 1) || (y > 0 && imgFlg[I-W] == 1) || (x < W-1 && imgFlg[I+1] == 1) || (y < H-1 && imgFlg[I+W] == 1);
			bool bAdjB = (x > 0 && imgFlg[I-1] == 2) || (y > 0 && imgFlg[I-W] == 2) || (x < W-1 && imgFlg[I+1] == 2) || (y < H-1 && imgFlg[I+W] == 2);
		
			if ( bAdjA && !bAdjB) graph.add_tLink(SourceID, SinkID, imgNodeID[I], 100000, 0);
			if (!bAdjA &&  bAdjB) graph.add_tLink(SourceID, SinkID, imgNodeID[I], 0, 100000);
		}
	}

	//graph cut
	byte *minCut = new byte[N_pNode + N_eNode + 2];
	graph.calcMaxFlow(SourceID, SinkID, minCut);

	//add new edge
	for (int y = 0; y < H; ++y)
	{
		for (int x = 0; x < W; ++x)
		{
			const int S  = x + y * W, Tx = S + 1, Ty = S + W;
			if (x < W - 1 && imgFlg[S] == 3 && imgFlg[Tx] == 3 && minCut[imgNodeID[S]] != minCut[imgNodeID[Tx]])
			{
				pixInfo[S].eCostX = t_calcEdgeVal( &imgA[4*S], &imgB[4*S], &imgA[4*Tx], &imgB[4*Tx]);
			}
			if (y < H - 1 && imgFlg[S] == 3 && imgFlg[Ty] == 3 && minCut[imgNodeID[S]] != minCut[imgNodeID[Ty]])
			{
				pixInfo[S].eCostY = t_calcEdgeVal( &imgA[4*S], &imgB[4*S], &imgA[4*Ty], &imgB[4*Ty]);
			}
		}
	}

	//update image
	byte blue[] = {0,0,255,0};
	for (int i = 0; i < WH; ++i) memcpy( &resImg[i*4], blue, sizeof(byte)*4 );

	for (int i = 0; i < WH; ++i) if( imgFlg[i] != 0 )
	{
		const int I4 = i * 4;
		if ( imgFlg[i] == 1 || (imgFlg[i] == 3 && minCut[imgNodeID[i]] ))
		{
			memcpy(&resImg[I4], &imgA[I4], sizeof(byte) * 4);
		}
		else if (imgFlg[i] == 2 || (imgFlg[i] == 3 && !minCut[imgNodeID[i]] )) 
		{
			memcpy(&resImg[I4], &imgB[I4], sizeof(byte) * 4);
			pixInfo[i].patchID = newPatchId;
		}
	}

	//remove unnecessary edge
	for (int y = 0; y < H; ++y)
	{
		for (int x = 0; x < W; ++x)
		{
			const int S  = x + y * W, Tx = S + 1, Ty = S + W;
			if( x < W - 1 && pixInfo[S].patchID == pixInfo[Tx].patchID ) pixInfo[S].eCostX = 0;
			if( y < H - 1 && pixInfo[S].patchID == pixInfo[Ty].patchID ) pixInfo[S].eCostY = 0;
		}
	}

	delete[] imgFlg;
	delete[] imgNodeID;
	delete[] minCut;
}


static byte Col[5][4] = { {64,64,64,0}, {0,0,255,0}, {0,255,128,0}, {255,0,0,0}, {255,255,0,0} };



static void t_graphCutTexture(const vector<TImage> &imgs, TImage &outImg)
{
	const int W = imgs[0].W;
	const int H = imgs[0].H;

	PixInf *pixInfo = new PixInf[W*H];
	byte   *resImg  = new byte  [W*H * 4];


	//一枚目 配置
	memcpy(resImg, imgs[0].rgba, sizeof(byte) * W * H * 4);

	for( int i=0; i < W*H; ++i) pixInfo[i].patchID = t_isBlue(4 * i, resImg) ? -1 : 0; 


	//2枚目以降を追加
	for( int i = 1; i < imgs.size(); ++i)
	{
		byte *tmp = new byte[W*H * 4];
		t_graphCutTexture_placePatch ( W,H, resImg, imgs[i].rgba, tmp, pixInfo, i );
		memcpy( resImg, tmp, sizeof(byte) * W * H * 4);

		fprintf( stderr, "add patch\n");

		CString str;
		str.Format("test%d.bmp", i);
		t_exportBmp( str, W,H,resImg);

		delete[] tmp;

		//export
		byte *patchId = new byte[ W * H * 4 ];
		byte *seamImg = new byte[ W * H * 4 ];
		for (int i = 0; i < W*H; ++i)
		{
			int ci = (pixInfo[i].patchID + 1) % 5;
			memcpy( &patchId[4*i], Col[ci], sizeof(byte) * 4);
			seamImg[4*i + 0] = pixInfo[i].eCostX > 0 || pixInfo[i].eCostY > 0 ? 255 : 0;
		}
		t_exportBmp(str + "patch.bmp", W,H, patchId);
		t_exportBmp(str + "seam.bmp" , W,H, seamImg);
		delete[] patchId;
		delete[] seamImg;
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
	vector<TImage> imgs(4);
	imgs[0].load("p1.bmp");
	imgs[1].load("p2.bmp");
	imgs[2].load("p3.bmp");
	imgs[3].load("p4.bmp");

	//resolutions of images are suppose to be the same
	const int W = imgs[0].W;
	const int H = imgs[0].H;

	//output image 
	TImage outImg;
	outImg.allocate(W, H);

	t_graphCutTexture(imgs, outImg);

    return nRetCode;
}
