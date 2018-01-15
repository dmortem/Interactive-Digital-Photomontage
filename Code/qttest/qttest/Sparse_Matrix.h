/*
* @brief 稀疏矩阵
* @author 吴驰域 3140100333
* @date 2017 - 04 - 06
*/

#include <stdio.h>
#include <opencv.hpp>

using namespace cv;

class Sparse_Matrix		// 见课件p89，其中rows[i]记录的是原矩阵中第i行第一个有数字的元素存在cols向量中的rows[i]位置；vals[j]和cols[j]则分别记录存的值以及其所在的列号
{
private:
	std::vector<int> rows;
	std::vector<int> cols;
	std::vector<double> vals;
	int n;		// 矩阵大小

public:
	Sparse_Matrix();	// 无参构造函数，若没有别的构造函数可不写
	double at(int row, int col);
	void insert(double val, int row, int col);
	void initializeFromVector(std::vector<int> rows, std::vector<int> cols, std::vector<double> vals);	// 相当于构造函数
	std::vector<double> Solver(std::vector<double> b);
	~Sparse_Matrix();	// 析构函数，要释放vector？
};