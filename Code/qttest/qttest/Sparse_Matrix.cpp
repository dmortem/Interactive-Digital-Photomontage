/*
* @brief ϡ�����
* @author ����� 3140100333
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

// ����ԭ������(row, col)����ֵ
double Sparse_Matrix::at(int row, int col)
{
	for (int i = rows[row]; i < rows[row + 1]; i++)
	{
		if (cols[i] == col)
			return vals[i];
	}
	return 0;		// û���������ҵ��������˵�����λ�õ�������0��û�б�CRS��¼����
}

// �޸�ԭ������(row, col)����ֵΪval
void Sparse_Matrix::insert(double val, int row, int col)
{
	int i;
	for (i = rows[row]; i < rows[row + 1]; i++)
	{
		if (cols[i] == col)		// ����������ڣ�����Ҫ�޸�
		{
			vals[i] = val;
			return;
		}
		if (cols[i] > col)		// ������������ڣ�����Ҫ����
			break;
	}
	// ����
	for (int j = row + 1; j < rows.size(); j++)		// �޸�������
		rows[j]++;
	vals.insert(vals.begin() + i, val);		// �޸���ֵ����
	cols.insert(cols.begin() + i, col);		// �޸�������
	return;
}

// �Ѷ���ľ���ת��Ϊ�Լ������ϡ��������ݽṹ�������������洢ϡ���������ݣ�rows, cols, vals
void Sparse_Matrix::initializeFromVector(std::vector<int> rows, std::vector<int> cols, std::vector<double> vals)
{
	this->n = (int)(sqrt(rows.size()));
	int row = -1, col = -1;		// row��¼��ǰ�ڴ�����һ�У�colö��CRS�ṹ��vals��cols�����е�ֵ
	for (int i = 0; i < rows.size(); i++)
	{
		if (vals[i] != 0)		// ��0�ĵ���Ҫ���洢
		{
			col++;
			this->cols.push_back(cols[i]);	// this->cols[col] = cols[i];
			this->vals.push_back(vals[i]);  // this->vals[col] = vals[i];
			if (rows[i] != row)			// �����ˣ���Ҫ��¼rows��ֵ�ˣ�rows[i]��¼��i�е�һ����0Ԫ����CRS�ṹ��cols�����е��±꣩
			{
				this->rows.push_back(col);
				row = rows[i];
			}
		}
	}
	this->rows.push_back(col + 1);
	this->rows.push_back(-1);	// -1��־��rows�����Ľ�β
	this->cols.push_back(-1);	// -1��־��cols�����Ľ�β

								// �������
								/*for (int i = 0; i < this->rows.size(); i++)
								std::cout << "row = " << " " << this->rows[i] << std::endl;	// ����rows��private������ֻ����Sparse_Matrix��ĺ����в��ܵ������
								for (int i = 0; i < this->cols.size(); i++)
								std::cout << "col = " << " " << this->cols[i] << std::endl;
								for (int i = 0; i < this->vals.size(); i++)
								std::cout << "val = " << " " << this->vals[i] << std::endl;*/

}

std::vector<double> Sparse_Matrix::Solver(std::vector<double> b)
{
	
	// GaussSeidel
	int n = this->n;
	std::vector<double> ans(n, 0);	// ��¼������Ľ�
	double con1 = 0;	// x(k+1)�ĺͣ������жϵ����Ƿ�����
	double con2 = 0;	// x(k)�ĺ�
	while (1)
	{
		con2 = con1;
		con1 = 0;
		for (int i = 0; i < n; i++)
		{
			double sum = 0, aii = 0;
			int j = this->rows[i];
			while (j < rows[i + 1])		// ��ϡ��������ҵ�i����һ������ֵ�ĵ�
			{
				if (cols[j] != i)		// �ӵ������ĵ���ʽ�õ�
					sum += vals[j] * ans[cols[j]];
				else aii = vals[j];
				j++;
			}
			ans[i] = (b[i] - sum) / aii;
			con1 += ans[i];
		}
		if (abs(con1 - con2) < 1e-9)	// ����
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

	// ��bת��Mat���ͣ����ڼ���
	for (int i = 0; i < n; i++)
		bb.at<float>(i, 0) = b[i];
	
	// ��ʼ��x0��r0��p0
	for (int i = n - 1; i >= 0; i--)
		ans1.at<float>(n - i - 1, 0) = b[i];
	r = (bb - a * ans1);
	r.copyTo(p);

	// ��ʼ����
	while (1)
	{
		// ��¼xk��rk��ֵ
		ans1.copyTo(ans2);
		r.copyTo(r2);
		
		// ����rk��pk��ת��
		transpose(r2, r2_t);
		transpose(p, p_t);
		
		// ����alphak
		Mat t = r2_t * r2 / (p_t * a * p);
		ak = t.at<float>(0, 0);
		
		// ����xk+1��rk+1
		ans1 = ans2 + ak * p;
		r = r2 - ak * a * p;
		
		// �ж�rk+1�Ƿ��㹻С������
		double sum = 0;
		for (int i = 0; i < n; i++)
			sum += r.at<float>(i, 0);
		if (sum < 1e-6)
			break;
		
		// ����rk+1��ת��
		transpose(r, r_t);
		
		// ����beta
		Mat tmp3 = (r_t * r / (r2_t * r2));
		double beita = tmp3.at<float>(0, 0);
		
		// ����pk+1
		p = r + beita * p;
	}

	std::vector<double> ans(n, 0);	// ��¼������Ľ�
	for (int i = 0; i < n; i++)
		ans[i] = ans1.at<float>(i, 0);

	return ans;*/
}