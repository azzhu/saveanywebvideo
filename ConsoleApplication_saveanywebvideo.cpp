// ConsoleApplication_saveanywebvideo.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

using namespace std;
using namespace cv;


#include "stdafx.h"
#include "cv.h"
#include "highgui.h"

//����ͼƬ����
int image_width;
int image_height;
int image_depth;
int image_nchannels;
IplImage* screemImage;
int flag = 0;

//��������
void CopyScreenToBitmap(int x, int y, int w, int h)
{
	int right = w + x,
		left = x,
		top = y,
		bottom = h + y;			
	int nWidth, nHeight;
	HDC hSrcDC = NULL, 
		hMemDC = NULL;
	HBITMAP hBitmap = NULL, 
		hOldBitmap = NULL;

	hSrcDC = CreateDC("DISPLAY", NULL, NULL, NULL);
	hMemDC = CreateCompatibleDC(hSrcDC);
	nWidth = right - left;
	nHeight = bottom - top;

	hBitmap = CreateCompatibleBitmap(hSrcDC, nWidth, nHeight);
	hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);

	BitBlt(hMemDC, 0, 0, nWidth, nHeight, hSrcDC, left, top, SRCCOPY);
	hBitmap = (HBITMAP)SelectObject(hMemDC, hOldBitmap);

	BITMAP bmp;
	int nChannels, depth;
	BYTE *pBuffer;
	GetObject(hBitmap, sizeof(BITMAP), &bmp);
	image_nchannels = bmp.bmBitsPixel == 1 ? 1 : bmp.bmBitsPixel / 8;
	image_depth = bmp.bmBitsPixel == 1 ? IPL_DEPTH_1U : IPL_DEPTH_8U;
	image_width = bmp.bmWidth;
	image_height = bmp.bmHeight;

	if (flag == 0)
	{
		screemImage = cvCreateImage(cvSize(image_width, image_height), image_depth, image_nchannels);
		flag = 1;
	}
	pBuffer = new BYTE[image_width*image_height*image_nchannels];
	GetBitmapBits(hBitmap, image_height*image_width*image_nchannels, pBuffer);
	memcpy(screemImage->imageData, pBuffer, image_height*image_width*image_nchannels);
	delete pBuffer;

	SelectObject(hMemDC, hOldBitmap);
	DeleteObject(hOldBitmap);
	DeleteDC(hMemDC);
	SelectObject(hSrcDC, hBitmap);
	DeleteDC(hMemDC);
	DeleteObject(hBitmap);
}

int _tmain(int argc, _TCHAR* argv[])
{
	//��ȡ�����������ж�
	char exeDirectory[260];
	GetCurrentDirectory(260, exeDirectory);
	string pathtemp = exeDirectory;
	string inipath = pathtemp + "\\configuration.ini";
	int x, y, w, h, fps, waitKeyTime, imgSave, aviSave;
	char imgres[260];
	char avires[260];
	x = GetPrivateProfileInt("param", "x", 0, inipath.c_str());
	y = GetPrivateProfileInt("param", "y", 0, inipath.c_str());
	w = GetPrivateProfileInt("param", "w", 100, inipath.c_str());
	h = GetPrivateProfileInt("param", "h", 100, inipath.c_str());
	fps = GetPrivateProfileInt("param", "fps", 16, inipath.c_str());
	waitKeyTime = GetPrivateProfileInt("param", "waitKeyTime", 25, inipath.c_str());
	imgSave = GetPrivateProfileInt("param", "imgSave", 0, inipath.c_str());
	aviSave = GetPrivateProfileInt("param", "aviSave", 1, inipath.c_str());
	GetPrivateProfileString("param", "imgres", "null", imgres, MAX_PATH, inipath.c_str());
	GetPrivateProfileString("param", "avires", "null", avires, MAX_PATH, inipath.c_str());
	int pingmuw = GetSystemMetrics(SM_CXSCREEN);
	int pingmuh = GetSystemMetrics(SM_CYSCREEN);
	if (x <0 || y <0 ||
		pingmuw <= 0 || pingmuh <= 0 ||
		x + w > pingmuw ||
		y + h > pingmuh)
	{
		cout << "���������򳬹���Ļ��Χ��" << endl;
		return 0;
	}
	if (fps<10 || fps>200)
	{
		cout << "fps��Χ��10-200��" << endl;
		return 0;
	}

	cout << "*******************************************" << endl
		<< "����" << x << ", " << y << ", " << w << ", " << h << endl
		<< "֡�ʣ�" << fps << endl
		<< "�����ʣ�" << waitKeyTime << endl
		//<< "�����ڣ�" << avires << endl
		<< "����s����ʼ¼�ƣ���e��ֹͣ��" << endl
		<< "*******************************************" << endl;
	
	VideoWriter vw(avires, CV_FOURCC('M', 'J', 'P', 'G'), fps, Size(w, h));
	if (!vw.isOpened())
	{
		cout << "������Ƶʧ�ܣ�����·����" << endl;
		return 0;
	}

	Mat img, img2;
	while (1)	//ֻ������ʾ
	{
		CopyScreenToBitmap(x, y, w, h);
		img = cvarrToMat(screemImage);
		cvtColor(img, img2, CV_RGBA2RGB);
		imshow("REC", img2);
		if (waitKey(waitKeyTime) == 's')	//start
		{
			cout << "��ʼ¼�ơ���" << endl;
			break;
		}
	}
	if (imgSave != 1 &&
		aviSave != 1)
	{
		cout << "��ѡ�񱣴����ͣ�" << endl;
		system("pause");
		return 0;
	}
	int ind = 1;
	while (1)	//������ʾ����ʼ¼��
	{
		CopyScreenToBitmap(x, y, w, h);
		img = cvarrToMat(screemImage);
		cvtColor(img, img2, CV_RGBA2RGB);
		imshow("REC", img2);
		if (imgSave == 1)
		{
			string temp = imgres;
			string imgrespath = temp + "\\" + to_string(ind++) + ".jpg";
			imwrite(imgrespath, img2);
		}
		if (aviSave == 1)
			vw << img2;
		if (waitKey(waitKeyTime) == 'e')	//end
			break;
	}
	vw.release();
	if (imgSave == 1)
		cout << "ͼ�񱣴���ɣ������ڣ�" << imgres << endl;
	if (aviSave == 1)
		cout << "��Ƶ¼����ɣ������ڣ�" << avires << endl;

	system("pause");
	return 0;
}
