/*
* @brief 稀疏矩阵
* @author 吴驰域 3140100333
* @date 2017 - 04 - 06
*/

#include <opencv.hpp>
#include <stdio.h>
#include "Sparse_Matrix.h"
#include <math.h>

using namespace cv;

// Mat a = (Mat_<float>(4, 4) << 10, -1, 2, 0, -1, 11, -1, 3, 2, -1, 10, -1, 0, 3, -1, 8);

/*
10 0 0 0 -2  0
3 9 0 0  0  3
0 7 8 7  0  0
3 0 8 7  5  0
0 8 0 9  9 13
0 4 0 0  2 -1
num   0  1 2 3  4  5  6 7 8 9 10 11 12 13 14 15 16 17 18
vals 10 -2 3 9  3  7  8 7 3 8  7  5  8  9  9 13  4  2 -1
cols  0  4 0 1  5  1  2 3 0 2  3  4  1  3  4  5  1  4  5
rows  0  2 5 8 12 16 19
*/

Sparse_Matrix::Sparse_Matrix() {}

Sparse_Matrix::~Sparse_Matrix() {}

// 访问原矩阵中(row, col)处的值
double Sparse_Matrix::at(int row, int col)
{
	for (int i = rows[row]; i < rows[row + 1]; i++)
	{
		if (cols[i] == col)
			return vals[i];
	}
	return 0;		// 没在向量中找到这个数，说明这个位置的数字是0，没有被CRS记录下来
}

// 修改原矩阵在(row, col)处的值为val
void Sparse_Matrix::insert(double val, int row, int col)
{
	int i;
	for (i = rows[row]; i < rows[row + 1]; i++)
	{
		if (cols[i] == col)		// 若这个数存在，则需要修改
		{
			vals[i] = val;
			return;
		}
		if (cols[i] > col)		// 若这个数不存在，则需要插入
			break;
	}
	// 插入
	for (int j = row + 1; j < rows.size(); j++)		// 修改行向量
		rows[j]++;
	vals.insert(vals.begin() + i, val);		// 修改数值向量
	cols.insert(cols.begin() + i, col);		// 修改列向量
	return;
}

// 把读入的矩阵转化为自己定义的稀疏矩阵数据结构，由三个向量存储稀疏矩阵的内容：rows, cols, vals
void Sparse_Matrix::initializeFromVector(std::vector<int> rows, std::vector<int> cols, std::vector<double> vals)
{
	this->n = (int)(sqrt(rows.size()));
	int row = -1, col = -1;		// row记录当前在处理哪一行；col枚举CRS结构中vals和cols向量中的值
	for (int i = 0; i < rows.size(); i++)
	{
		if (vals[i] != 0)		// 非0的点需要被存储
		{
			col++;
			this->cols.push_back(cols[i]);	// this->cols[col] = cols[i];
			this->vals.push_back(vals[i]);  // this->vals[col] = vals[i];
			if (rows[i] != row)			// 换行了，需要记录rows的值了（rows[i]记录第i行第一个非0元素在CRS结构中cols向量中的下标）
			{
				this->rows.push_back(col);
				row = rows[i];
			}
		}
	}
	this->rows.push_back(col + 1);
	this->rows.push_back(-1);	// -1标志着rows向量的结尾
	this->cols.push_back(-1);	// -1标志着cols向量的结尾

								// 输出测试
								/*for (int i = 0; i < this->rows.size(); i++)
								std::cout << "row = " << " " << this->rows[i] << std::endl;	// 由于rows是private，所以只能在Sparse_Matrix类的函数中才能调用输出
								for (int i = 0; i < this->cols.size(); i++)
								std::cout << "col = " << " " << this->cols[i] << std::endl;
								for (int i = 0; i < this->vals.size(); i++)
								std::cout << "val = " << " " << this->vals[i] << std::endl;*/

}

std::vector<double> Sparse_Matrix::Solver(std::vector<double> b)
{
	
	// GaussSeidel
	int n = this->n;
	std::vector<double> ans(n, 0);	// 记录方程组的解
	double con1 = 0;	// x(k+1)的和，用来判断迭代是否收敛
	double con2 = 0;	// x(k)的和
	while (1)
	{
		con2 = con1;
		con1 = 0;
		for (int i = 0; i < n; i++)
		{
			double sum = 0, aii = 0;
			int j = this->rows[i];
			while (j < rows[i + 1])		// 在稀疏矩阵中找第i行下一个有数值的点
			{
				if (cols[j] != i)		// 从迭代法的迭代式得到
					sum += vals[j] * ans[cols[j]];
				else aii = vals[j];
				j++;
			}
			ans[i] = (b[i] - sum) / aii;
			con1 += ans[i];
		}
		if (abs(con1 - con2) < 1e-9)	// 收敛
			break;
	}
	return ans;

	// Conjugate
	/*int n = this->n;
	Mat ans1(n, 1, CV_32FC1);		// xk+1
	Mat ans2(n, 1, CV_32FC1);		// xk
	Mat r(n, 1, CV_32FC1);			// rk+1
	Mat r2(n, 1, CV_32FC1);			// rk
	Mat r_t(n, 1, CV_32FC1);		// rk+1^T
	Mat r2_t(n, 1, CV_32FC1);		// rk^T
	Mat p(n, 1, CV_32FC1);			// pk
	Mat p_t(n, 1, CV_32FC1);		// pk^T
	Mat bb(n, 1, CV_32FC1);			// b
	double ak;						// alphak

	// 把b转成Mat类型，便于计算
	for (int i = 0; i < n; i++)
		bb.at<float>(i, 0) = b[i];
	
	// 初始化x0，r0，p0
	for (int i = n - 1; i >= 0; i--)
		ans1.at<float>(n - i - 1, 0) = b[i];
	r = (bb - a * ans1);
	r.copyTo(p);

	// 开始迭代
	while (1)
	{
		// 记录xk和rk的值
		ans1.copyTo(ans2);
		r.copyTo(r2);
		
		// 计算rk和pk的转置
		transpose(r2, r2_t);
		transpose(p, p_t);
		
		// 计算alphak
		Mat t = r2_t * r2 / (p_t * a * p);
		ak = t.at<float>(0, 0);
		
		// 计算xk+1和rk+1
		ans1 = ans2 + ak * p;
		r = r2 - ak * a * p;
		
		// 判断rk+1是否足够小，收敛
		double sum = 0;
		for (int i = 0; i < n; i++)
			sum += r.at<float>(i, 0);
		if (sum < 1e-6)
			break;
		
		// 计算rk+1的转置
		transpose(r, r_t);
		
		// 计算beta
		Mat tmp3 = (r_t * r / (r2_t * r2));
		double beita = tmp3.at<float>(0, 0);
		
		// 更新pk+1
		p = r + beita * p;
	}

	std::vector<double> ans(n, 0);	// 记录方程组的解
	for (int i = 0; i < n; i++)
		ans[i] = ans1.at<float>(i, 0);

	return ans;*/
}