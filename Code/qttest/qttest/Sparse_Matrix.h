/*
* @brief ϡ�����
* @author ����� 3140100333
* @date 2017 - 04 - 06
*/

#include <stdio.h>
#include <opencv.hpp>

using namespace cv;

class Sparse_Matrix		// ���μ�p89������rows[i]��¼����ԭ�����е�i�е�һ�������ֵ�Ԫ�ش���cols�����е�rows[i]λ�ã�vals[j]��cols[j]��ֱ��¼���ֵ�Լ������ڵ��к�
{
private:
	std::vector<int> rows;
	std::vector<int> cols;
	std::vector<double> vals;
	int n;		// �����С

public:
	Sparse_Matrix();	// �޲ι��캯������û�б�Ĺ��캯���ɲ�д
	double at(int row, int col);
	void insert(double val, int row, int col);
	void initializeFromVector(std::vector<int> rows, std::vector<int> cols, std::vector<double> vals);	// �൱�ڹ��캯��
	std::vector<double> Solver(std::vector<double> b);
	~Sparse_Matrix();	// ����������Ҫ�ͷ�vector��
};