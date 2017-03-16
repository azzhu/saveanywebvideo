#include "stdafx.h"
#include "zqjtools.hpp"

//显示进程当前使用内存和历史峰值使用内存；
void az::showMemoryInfo(void)
{
	HANDLE handle = GetCurrentProcess();
	PROCESS_MEMORY_COUNTERS pmc;
	GetProcessMemoryInfo(handle, &pmc, sizeof(pmc));
	
	cout << "********************************\n\n"
		<< "内存使用:\n\n"
		<< "当前内存："<<pmc.WorkingSetSize / 1000000 << "MB\n\n" 
		<< "峰值内存："<< pmc.PeakWorkingSetSize / 1000000 << "MB\n\n" 
		<< "********************************"<< endl;
}

//对数组随机排序
void az::random_sort(int* x, int size)	//对数组随机排序
{
	for (int i = size; i >= 2; i--)
	{
		int index = rand() % i;
		int temp = x[index];
		x[index] = x[i - 1];
		x[i - 1] = temp;
	}
}

//判断一个文件夹下有多少个指定格式的图像；
//图像必须是以数字命名，必须连续。
//参数含义：文件夹路径、开始的命名序号、格式
//eg：int num = how_many_pics("d:/csimg/", 0, ".jpg");
//////不建议使用第一个函数，效率太低，建议使用第二个函数。
//int az::how_many_pics(char* path, int starnum, char* fmt)
//{
//	int num = 0;
//	for (int i = starnum;; i++)
//	{
//		string fn = path + to_string(i) + fmt;
//		cv::Mat temp = cv::imread(fn);
//		if (!temp.empty())
//			num++;
//		else
//			break;
//	}
//	return num;
//}
int az::how_many_pics(char* path)
{
	//此方法对子文件夹下文件也计算在内
	vector<string> vecfn;
	getAllFilesPath(path, vecfn);

	return vecfn.size();
}

//获取当前时间,返回毫秒
long az::getTime()
{
	return clock();
}

#ifdef USE_OPENCV
//图像大小转换
cv::Mat az::imgto64(cv::Mat *img)
{
	cv::Mat temp(64, 64, CV_32S);
	cv::Size s20(64, 64);
	cv::resize(*img, temp, s20);
	return temp;
}
cv::Mat az::imgto(cv::Mat *img, int w, int h)
{
	cv::Mat temp(h, w, CV_32S);
	cv::Size s20(w, h);
	cv::resize(*img, temp, s20);
	return temp;
}
#endif

az::ZLJG::ZLJG()
{
	size_now = 0;
}
void az::ZLJG::push(std::string ss)
{
	if (size_now < 5)
	{
		s[size_now] = ss;
		size_now++;
	}
	else
	{
		s[0] = s[1];
		s[1] = s[2];
		s[2] = s[3];
		s[3] = s[4];
		s[4] = ss;
	}
}
std::string az::ZLJG::get()
{
	if (size_now != 5)
	{
		return "处理中...";
	}
	std::vector<std::string> strtemp;
	for (int i = 0; i < 5; i++)
	{
		strtemp.push_back(s[i]);
	}
	//获取每个元素的出现次数
	std::vector<int> a_per;
	for (int r = 0; r < strtemp.size(); r++)
	{
		std::string stp = strtemp[r];
		int count = 0;
		for (int c = 0; c < strtemp.size(); c++)
		{
			if (stp == strtemp[c])
			{
				count++;
			}
		}
		a_per.push_back(count);
	}
	//找出出现次数最大的元素的ID
	int id;
	int itemp = a_per[0];
	id = 0;
	for (int i = 0; i < a_per.size(); i++)
	{
		if (a_per[i] >= itemp)
		{
			itemp = a_per[i];
			id = i;
		}
	}

	return strtemp[id];
}

std::string az::ZLJG::doit(std::string str)
{
	push(str);
	return get();
}

//获取文件夹下所有文件（包括子文件夹下的文件）的绝对路径,结果存到vector<string>& files中
//速度很快，一万多个文件也是秒获；也很健壮，文件夹命名中含有点“.”也能通过。
void az::getAllFilesPath(string path, vector<string>& files)
{
	//文件句柄
	long   hFile = 0;
	//文件信息
	struct _finddata_t fileinfo;
	string p;
	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			//如果是目录,迭代之
			//如果不是,加入列表
			if ((fileinfo.attrib &  _A_SUBDIR))
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
					getAllFilesPath(p.assign(path).append("\\").append(fileinfo.name), files);
			}
			else
			{
				files.push_back(p.assign(path).append("\\").append(fileinfo.name));
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
}


//大津法求阈值,输入灰度图，默认灰阶256
int az::get_th_useOtsu(cv::Mat *img, int gray_scale, bool mask)
{
#define GrayScale gray_scale   //frame灰度级  
	if (img->channels() != 1)
	{
		return 0;
	}

	int nr = img->rows;
	int nc = img->cols;
	int *pixelCount = new int[GrayScale];
	float *pixelPro = new float[GrayScale];
	for (int i = 0; i < GrayScale; i++)
	{
		pixelCount[i] = 0;
		pixelPro[i] = 0;
	}
	int i, j, pixelSum = 0, threshold = 0;

	//统计每个灰度级中像素的个数
	if (mask)
	{
		for (i = 0; i < nr; i++)
		{
			uchar* data = img->ptr<uchar>(i);
			for (j = 0; j < nc; j++)
			{
				int data_v = (int)*data++;
				if (data_v > 3)
				{
					pixelCount[data_v]++;
					pixelSum++;
				}
			}
		}
	}
	else
	{
		for (i = 0; i < nr; i++)
		{
			uchar* data = img->ptr<uchar>(i);
			for (j = 0; j < nc; j++)
			{
				pixelCount[(int)*data++]++;
				pixelSum++;
			}
		}
	}

	//计算每个灰度级的像素数目占整幅图像的比例  
	for (i = 0; i < GrayScale; i++)
	{
		pixelPro[i] = (float)pixelCount[i] / pixelSum;
	}

	//遍历灰度级,寻找合适的threshold  
	float w0, w1, u0tmp, u1tmp, u0, u1, deltaTmp, deltaMax = 0;
	for (i = 0; i < GrayScale; i++)
	{
		w0 = w1 = u0tmp = u1tmp = u0 = u1 = deltaTmp = 0;
		for (j = 0; j < GrayScale; j++)
		{
			if (j <= i)   //背景部分  
			{
				w0 += pixelPro[j];
				u0tmp += j * pixelPro[j];
			}
			else   //前景部分  
			{
				w1 += pixelPro[j];
				u1tmp += j * pixelPro[j];
			}
		}
		u0 = u0tmp / w0;
		u1 = u1tmp / w1;
		deltaTmp = (float)(w0 *w1* pow((u0 - u1), 2));
		if (deltaTmp > deltaMax)
		{
			deltaMax = deltaTmp;
			threshold = i;
		}
	}

	delete[] pixelCount;
	delete[] pixelPro;
	return threshold;
}


//根据HSV空间，筛选某一通道在某范围的像素，默认h通道
cv::Mat az::sel_fromhsv(cv::Mat* img, int min, int max, char flg)
{
	if (img->channels() != 3)
		return *img;
	cv::Mat pic;
	cv::Mat res(img->size(), CV_8UC1);
	img->copyTo(pic);	//用来转换空间
	cv::cvtColor(pic, pic, CV_BGR2HSV);
	std::vector<cv::Mat> picsplit;
	split(pic, picsplit);
	int nr = img->rows;
	int nc = img->cols;
	if (flg == 'h')
	{
		for (int i = 0; i < nr; i++)
		{
			uchar* data_p = pic.ptr<uchar>(i);
			uchar* data_r = res.ptr<uchar>(i);
			for (int j = 0; j < nc; j++)
			{
				int dp = *data_p;
				if (dp >= min&&dp <= max)
					*data_r = 255;
				else
					*data_r = 0;
				data_p += 3;
				data_r++;
			}
		}
	}
	if (flg == 's')
	{
		for (int i = 0; i < nr; i++)
		{
			uchar* data_p = pic.ptr<uchar>(i)+1;
			uchar* data_r = res.ptr<uchar>(i);
			for (int j = 0; j < nc; j++)
			{
				int dp = *data_p;
				if (dp >= min&&dp <= max)
					*data_r = 255;
				else
					*data_r = 0;
				data_p += 3;
				data_r++;
			}
		}
	}
	if (flg == 'v')
	{
		for (int i = 0; i < nr; i++)
		{
			uchar* data_p = pic.ptr<uchar>(i)+2;
			uchar* data_r = res.ptr<uchar>(i);
			for (int j = 0; j < nc; j++)
			{
				int dp = *data_p;
				if (dp >= min&&dp <= max)
					*data_r = 255;
				else
					*data_r = 0;
				data_p += 3;
				data_r++;
			}
		}
	}
	return res;
}

//判断一个点是否在一个图像内
bool az::p_isinimg(CvPoint p, cv::Mat* m)
{
	if (p.x >= 0 && p.x < m->size().width &&
		p.y >= 0 && p.y < m->size().height)
		return true;
	else
		return false;
}
//很据一个点，求附近的连通区域，返回一个区域
std::vector<CvPoint> az::connectedregion(cv::Mat& im, CvPoint p)
{
	cv::Mat m;
	im.copyTo(m);
	std::vector<CvPoint> ps;
	CvPoint *pss = new CvPoint[310000];	//存连通区域像素
	int p_ = 1;		//记录指针位置
	int numb = 1;		//新加入的像素的个数

	pss[0] = p;
	while (1)
	{
		int k = numb;	//轮廓长度
		numb = 0;		//归零
		for (int i = p_ - 1; i >= p_ - k; i--)	//求新加入的点
		{
			p = pss[i];
			CvPoint p1;
			p1 = CvPoint(p.x, p.y + 1);
			if (az::p_isinimg(p1, &m))
			{
				//std::cout << p1.x << " " << p1.y << std::endl;
				if (m.at<uchar>(p1) <=30)
				{
					pss[p_ + numb] = p1;
					numb++;
					m.at<uchar>(p1) = 255;
				}
			}
			p1 = CvPoint(p.x, p.y - 1);
			if (az::p_isinimg(p1, &m))	
				if (m.at<uchar>(p1) <= 30)	{ pss[p_ + numb] = p1;	numb++;	m.at<uchar>(p1) = 255; }
			p1 = CvPoint(p.x - 1, p.y + 1);
			if (az::p_isinimg(p1, &m))	
				if (m.at<uchar>(p1) <= 30)	{ pss[p_ + numb] = p1;	numb++;	m.at<uchar>(p1) = 255; }
			p1 = CvPoint(p.x - 1, p.y);
			if (az::p_isinimg(p1, &m))	
				if (m.at<uchar>(p1) <= 30)	{ pss[p_ + numb] = p1;	numb++;	m.at<uchar>(p1) = 255; }
			p1 = CvPoint(p.x - 1, p.y - 1);
			if (az::p_isinimg(p1, &m))	
				if (m.at<uchar>(p1) <= 30)	{ pss[p_ + numb] = p1;	numb++;	m.at<uchar>(p1) = 255; }
			p1 = CvPoint(p.x + 1, p.y + 1);
			if (az::p_isinimg(p1, &m))	
				if (m.at<uchar>(p1) <= 30)	{ pss[p_ + numb] = p1;	numb++;	m.at<uchar>(p1) = 255; }
			p1 = CvPoint(p.x + 1, p.y - 1);
			if (az::p_isinimg(p1, &m))	
				if (m.at<uchar>(p1) <= 30)	{ pss[p_ + numb] = p1;	numb++;	m.at<uchar>(p1) = 255; }
			p1 = CvPoint(p.x + 1, p.y);
			if (az::p_isinimg(p1, &m))	
				if (m.at<uchar>(p1) <= 30)	{ pss[p_ + numb] = p1;	numb++;	m.at<uchar>(p1) = 255; }
		}

		p_ = p_ + numb;		//指针往后移
		if (numb == 0)	break;		//没有新加入的点了跳出
	}

	for (int i = 0; i < p_; i++)
	{
		ps.push_back(pss[i]);
	}
	delete[] pss;

	return ps;
}
//求二值图像的连通区域，结果保存到vs
void az::findconnectedregions(cv::Mat* im, std::vector<std::vector<CvPoint>> &vs)
{
	std::vector<CvPoint> ptem;
	cv::Mat m;
	im->copyTo(m);
	int h = m.size().height;
	int w = m.size().width;

	for (int i = 0; i < h; i++)
	{
		uchar* data = m.ptr<uchar>(i);
		for (int j = 0; j < w; j++)
		{
			if (*data++ <= 30)
			{
				ptem = az::connectedregion(m, CvPoint(j, i));
				vs.push_back(ptem);
				for (int k = 0; k < ptem.size(); k++)	//把该区域置换成白色
				{
					m.at<uchar>(ptem[k]) = 255;
				}
			}
		}
	}
}

//二值图像去噪【连通区域法】，比如黑色背景的，去掉白色前景中黑色小连通区域
void az::quxiaodong(cv::Mat *pic, int max_th)
{
	std::vector<std::vector<CvPoint>> vs;
	az::findconnectedregions(pic, vs);
	for (int i = 0; i < vs.size(); i++)
	{
		if (vs[i].size() < max_th)
		{
			for (int j = 0; j < vs[i].size(); j++)
			{
				pic->at<uchar>(vs[i][j]) = 255;
			}
		}
	}

	return;
}