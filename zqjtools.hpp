/**
*
*    Created by ZQJ, 2016.8.8 ‏‎18:04:17 PM.
*    Copyright (c) 2016, 228812066@qq.com All Rights Reserved.
*
*                #####################################################
*                #                                                   #
*                #                       _oo0oo_                     #
*                #                      o8888888o                    #
*                #                      88" . "88                    #
*                #                      (| -_- |)                    #
*                #                      0\  =  /0                    #
*                #                    ___/`---'\___                  #
*                #                  .' \\|     |# '.                 #
*                #                 / \\|||  :  |||# \                #
*                #                / _||||| -:- |||||- \              #
*                #               |   | \\\  -  #/ |   |              #
*                #               | \_|  ''\---/''  |_/ |             #
*                #               \  .-\__  '-'  ___/-. /             #
*                #             ___'. .'  /--.--\  `. .'___           #
*                #          ."" '<  `.___\_<|>_/___.' >' "".         #
*                #         | | :  `- \`.;`\ _ /`;.`/ - ` : | |       #
*                #         \  \ `_.   \_ __\ /__ _/   .-` /  /       #
*                #     =====`-.____`.___ \_____/___.-`___.-'=====    #
*                #                       `=---='                     #
*                #     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~   #
*                #                                                   #
*                #####################################################
**/


#pragma once

#define USE_OPENCV

#ifdef USE_OPENCV
#include <opencv/cv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core.hpp>
#endif
#include <math.h>
#include <string>
#include <fstream>
#include <iostream>
#include <time.h>
#include <windows.h>
#include <io.h>
#include <vector>
#include <psapi.h>    
#pragma comment(lib,"psapi.lib")  

using namespace std;

namespace az
{

	//显示进程当前使用内存和历史峰值使用内存，在控制台窗口打印；
	void showMemoryInfo(void);

	//对数组随机排序
	void random_sort(int* x, int size);

	//判断一个文件夹下有多少个指定格式的图像；
	//图像必须是以数字命名，必须连续。
	//参数含义：文件夹路径、开始的命名序号、格式
	//eg：int num = how_many_pics("d:/csimg/", 0, ".jpg")
	//////不建议使用第一个函数，效率太低，建议使用第二个函数。
	//int how_many_pics(char* path, int starnum, char* fmt);
	int how_many_pics(char* path);	//此方法对子文件夹下文件也计算在内

	//获取当前时间，返回毫秒
	long getTime();

#ifdef USE_OPENCV
	//图像转换大小
	cv::Mat imgto64(cv::Mat *img);
	cv::Mat imgto(cv::Mat *img, int w, int h);
#endif

	//整理结果类，5个结果里选出出现次数最多的
	//用法：一句话，ZLJG zl; zl.doit(str);
	class ZLJG
	{
	public:
		ZLJG();
		std::string doit(std::string str);	//使用

	private:
		void push(std::string ss);
		std::string get();
		std::string s[5];
		int size_now;
	};

	/*
	//文件复制，删除，重命名
	string inf = "F:\\迅雷下载\\Anaconda2-4.1.1-Windows-x86_64.exe";
	string outf = "F:\\迅雷下载\\Anaconda2-4.1.1-Windows-x86_642.exe";
	CopyFile(inf.c_str(), outf.c_str(), true);
	DeleteFile(outf.c_str());
	rename(inf.c_str(), outf.c_str());
	*/

	//获取文件夹下所有文件（包括子文件夹下的文件）的绝对路径,结果存到vector<string>& files中
	//速度很快，一万多个文件也是秒获，也很健壮，文件夹命名中含有点“.”也能通过。
	void getAllFilesPath(string path, vector<string>& files);

	/*
	win7风格mfc，只需在代码前面加上：
	#pragma comment(linker,"\"/manifestdependency:type='win32' \
	name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
	processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

	MFC程序窗口一直最前端显示方法：InitDialog中添加如下代码，
	AfxGetMainWnd()->SetWindowPos(&CWnd::wndTopMost,0,0,0,0,SWP_NOSIZE | SWP_NOMOVE);
	*/

	//大津法求阈值,输入灰度图，默认灰阶256，开启mask后自动过滤黑色背景
	int get_th_useOtsu(cv::Mat *img, int gray_scale = 256, bool mask = false);

	//根据HSV空间，筛选某一通道在某范围的像素，默认h通道
	cv::Mat sel_fromhsv(cv::Mat* img, int min, int max, char flg = 'h');

	//判断一个点是否在一个图像内
	bool p_isinimg(CvPoint p, cv::Mat* m);
	//很据一个点，求附近的连通区域，返回一个区域
	std::vector<CvPoint> connectedregion(cv::Mat& m, CvPoint p);
	//求二值图像的连通区域，结果保存到vs，返回一组连通区域
	void findconnectedregions(cv::Mat* m, std::vector<std::vector<CvPoint>> &vs);

	//二值图像去噪，比如黑色背景的，去掉白色前景中黑色小连通区域
	void quxiaodong(cv::Mat *pic, int max_th);



}

