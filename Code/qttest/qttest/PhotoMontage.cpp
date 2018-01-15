#include "stdafx.h"
#include "PhotoMontage.h"
#include "gco-v3.0/GCoptimization.h"
#include <Eigen/Sparse>
#include "Sparse_Matrix.h"
#include <time.h>

using namespace cv;

clock_t start, stop;
double duration;

const double large_penalty = 1000.0;
Mat _data;

#define M_PI       3.14159265358979323846

struct __ExtraData
{
	std::vector<cv::Mat> Images;
	cv::Mat Label;
	cv::flann::Index * kdtree;
};

// 数据项
double dataFn(int p, int l, void *data)	// p是坐标值，l是用户在交互界面上选择的源图像标签，data是数据
{
#if 1
	// designed image
	__ExtraData * ptr_extra_data = (__ExtraData *)data;
	cv::Mat & Label = ptr_extra_data->Label;		// Label 图像，即在复合图像上使用的pixel对应原图像
	//cv::flann::Index * ptr_kdtree = ptr_extra_data->kdtree;

	int width = Label.cols;
	int height = Label.rows;

	int y = p / width;
	int x = p % width;

	assert(l >= 0);
	if (Label.at<char>(y, x) != PhotoMontage::undefined)	// user specified
	{
		if (Label.at<char>(y, x) == l)		// L(p) == u. l is user defined source image number
		{
			return 0.0;
		}
		else
		{
			return large_penalty;
		}
	}
	else
	{
		return large_penalty;
	}
#endif

#if 0
	// maximum luminance
	__ExtraData * ptr_extra_data = (__ExtraData *)data;
	cv::Mat & Label = ptr_extra_data->Label;		// Label 图像，即在复合图像上使用的pixel对应原图像
	std::vector<cv::Mat> & Images = ptr_extra_data->Images;

	int width = Label.cols;
	int height = Label.rows;
	int y = p / width;
	int x = p % width;
	assert(l >= 0);

	int max = 0;
	// 枚举像素点
	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++)
		{
			// 寻找span中(i, j)位置像素点亮度最大的值
			max = 0;
			for (int k = 0; k < Images.size(); k++)
			{
				int tmp = 0.3 * Images[k].at<Vec3b>(i,j)[2] 
					+ 0.59 * Images[k].at<Vec3b>(i, j)[1] 
					+ 0.11 * Images[k].at<Vec3b>(i, j)[0];
				if (tmp > max)
					max = tmp;
			}
		}

	int ans = max - (0.3 * Images[l].at<Vec3b>(y, x)[2] + 0.59 * Images[l].at<Vec3b>(y, x)[1] + 0.11 * Images[l].at<Vec3b>(y, x)[0]);
	
	return ans;
#endif
	return 0;
}

// 欧式距离
double euc_dist(const Vec3b & a, const Vec3b & b)
{
	Vec3d double_diff = a - b;
	return sqrt(double_diff[0] * double_diff[0] + double_diff[1] * double_diff[1] + double_diff[2] * double_diff[2]);
}

// 平滑项
double smoothFn(int p, int q, int lp, int lq, void * data)	// p和q分别为接缝两端像素点坐标；lp和lq为p和q的labeling
{
#if 1
	if (lp == lq)
	{
		return 0.0;
	}

	assert(lp != lq);	
	__ExtraData * ptr_extra_data = (__ExtraData *)data;
	cv::Mat & Label = ptr_extra_data->Label;
	std::vector<cv::Mat> & Images = ptr_extra_data->Images;
	int n_label = Images.size();
	assert(lp < n_label && lq < n_label);
		
	int width = Label.cols;
	int height = Label.rows;

	int yp = p / width;
	int xp = p % width;
	int yq = q / width;
	int xq = q % width;
	
	double X_term1 = euc_dist(Images[lp].at<Vec3b>(yp,xp), Images[lq].at<Vec3b>(yq,xq));
	double X_term2 = euc_dist(Images[lp].at<Vec3b>(yq,xq), Images[lq].at<Vec3b>(yp,xp));
	assert(X_term1 + X_term1 >= 0.0);
	return (X_term1 + X_term2)*100;
#endif

	return 0;
}

// 程序入口
void PhotoMontage::Run( const std::vector<cv::Mat> & Images, const cv::Mat & Label )
{	
	start = clock();
	assert(Images[0].rows == Label.rows);
	assert(Images[0].cols == Label.cols);
		
	BuildSolveMRF( Images, Label );	
}

// Graph Cut
void PhotoMontage::BuildSolveMRF( const std::vector<cv::Mat> & Images, const cv::Mat & Label )
{
	const int n_imgs = Images.size();
	__ExtraData extra_data;
	extra_data.Images.resize(n_imgs);
	for (int i = 0 ; i < n_imgs; i++)
	{
		extra_data.Images[i] = Images[i];
	}
	extra_data.Label = Label;
	//extra_data.kdtree = AddInertiaConstraint( Label );
	int width = Label.cols;
	int height = Label.rows;
	int n_label = n_imgs;


	try
	{
        //VisResultLabelMap(Label,n_label);

		GCoptimizationGridGraph *gc = new GCoptimizationGridGraph(width,height,n_imgs);

		// set up the needed data to pass to function for the data costs
		gc->setDataCost(&dataFn, &extra_data);

		// smoothness comes from function pointer
		gc->setSmoothCost(&smoothFn,&extra_data);

		printf("\nBefore optimization energy is %f",gc->compute_energy());
		gc->swap(10);// run expansion for 2 iterations. For swap use gc->swap(num_iterations); 
		printf("\nAfter optimization energy is %f",gc->compute_energy());

		Mat result_label(height, width, CV_8UC1);
		
		for (int y = 0 ; y < height ; y++)
		{
			for (int x = 0 ; x < width; x++)
			{
				int idx = y * width + x;

				result_label.at<uchar>(y,x) = gc->whatLabel(idx);
			}
		}		
		delete gc;

		VisResultLabelMap( result_label, n_label );
        VisCompositeImage( result_label, Images );

        BuildSolveGradientFusion(Images, result_label);;
	}
	catch (GCException e)
	{
		e.Report();

	}
}

// 求梯度
void PhotoMontage::GradientAt( const cv::Mat & Image, int x, int y, cv::Vec3f & grad_x, cv::Vec3f & grad_y )
{	

	Vec3i color1 = Image.at<Vec3b>(y, x);
	Vec3i color2 = Image.at<Vec3b>(y, x + 1);
	Vec3i color3 = Image.at<Vec3b>(y + 1, x);
	grad_x = color2 - color1;
	grad_y = color3 - color1;

}

// Gradient-domain fusion
void PhotoMontage::BuildSolveGradientFusion( const std::vector<cv::Mat> & Images, const cv::Mat & ResultLabel )
{

	int width = ResultLabel.cols;
	int height = ResultLabel.rows;
	Mat color_result(height, width, CV_8UC3);
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			color_result.at<Vec3b>(y, x) = Images[ResultLabel.at<uchar>(y, x)].at<Vec3b>(y, x);
		}
	}
	cvtColor(color_result, color_result, CV_BGR2YUV);
	Mat color_gradient_x(height, width, CV_32FC3);
	Mat color_gradient_y(height, width, CV_32FC3);
	
	for (int y = 0 ; y < height - 1; y++)
	{
		for (int x = 0 ;x < width - 1; x++)
		{
			GradientAt( Images[ResultLabel.at<uchar>(y,x)], x, y, color_gradient_x.at<Vec3f>(y,x), color_gradient_y.at<Vec3f>(y,x) );
		}
	}	
	// 将颜色空间转换到YUV，对Y通道进行计算
	cvtColor(color_gradient_x, color_gradient_x, CV_BGR2YUV);
	cvtColor(color_gradient_y, color_gradient_y, CV_BGR2YUV);

	Vec3b color0 = Images[0].at<Vec3b>(0,0);
	Vec3b color00;
	color00[0] = 0.3 * color0[2] + 0.59 * color0[1] + 0.11 * color0[0];
//	cvtColor(color0, color0, CV_BGR2YUV);

	// 单通道计算
	SolveChannel( 0, color00[0], color_gradient_x, color_gradient_y , color_result );
	//SolveChannel( 1, color0[1], color_gradient_x, color_gradient_y , color_result );
	//SolveChannel( 2, color0[2], color_gradient_x, color_gradient_y , color_result );

	// 将颜色空间转回RGB
	cvtColor(color_result, color_result, CV_YUV2BGR);


	// imshow("color result",color_result);
	// waitKey(0);
	imwrite("result.jpg", color_result);
	stop = clock();
	duration = ((double)(stop - start)) / CLK_TCK / 100;
	FILE *fp = fopen("test.txt", "w");
	fprintf(fp, "time is :  %f\n",duration);
	fclose(fp);

	/*
	Mat color_gradient_x(height, width, CV_32FC3);
	Mat color_gradient_y(height, width, CV_32FC3);

	for (int y = 0; y < height - 1; y++)
	{
		for (int x = 0; x < width - 1; x++)
		{
			GradientAt(Images[ResultLabel.at<uchar>(y, x)], x, y, color_gradient_x.at<Vec3f>(y, x), color_gradient_y.at<Vec3f>(y, x));
		}
	}


	Vec3b color0 = Images[0].at<Vec3b>(0, 0);
	SolveChannel(0, color0[0], color_gradient_x, color_gradient_y, color_result);
	SolveChannel(1, color0[1], color_gradient_x, color_gradient_y, color_result);
	SolveChannel(2, color0[2], color_gradient_x, color_gradient_y, color_result);


	//imshow("color result", color_result);
	imwrite("result.jpg", color_result);
	stop = clock();
	duration = ((double)(stop - start)) / CLK_TCK / 100;
	FILE *fp = fopen("test.txt", "w");
	fprintf(fp, "time is :  %f\n",duration);
	fclose(fp);
	*/
	
}


// 显示Label
void PhotoMontage::VisResultLabelMap( const cv::Mat & ResultLabel, int n_label )
{
	int width = ResultLabel.cols;
	int height = ResultLabel.rows;
	Mat color_result_map(height, width, CV_8UC3);
	std::vector<Vec3b> label_colors;
	for (int i = 0 ; i < n_label; i++)//给每种Label分配随机的颜色
	{
		label_colors.push_back(Vec3b(rand()%256,rand()%256,rand()%256));
	}
	//label_colors.push_back(Vec3b(255,0,0));
	//label_colors.push_back(Vec3b(0,255,0));
	//label_colors.push_back(Vec3b(0,0,255));
	//label_colors.push_back(Vec3b(255,255,0));
	//label_colors.push_back(Vec3b(0,255,255));

	//给Label图上色
	for (int y = 0 ; y < height ; y++)
	{
		for (int x = 0 ; x < width ; x++)
		{
			color_result_map.at<Vec3b>(y,x) = label_colors[ResultLabel.at<uchar>(y,x)];
		}
	}

    //imshow("result labels",color_result_map);
    imwrite("labels.jpg",color_result_map);
	//waitKey(0);
}


// 显示image
void PhotoMontage::VisCompositeImage( const cv::Mat & ResultLabel, const std::vector<cv::Mat> & Images )
{
	int width = ResultLabel.cols;
	int height = ResultLabel.rows;
	Mat composite_image(height, width, CV_8UC3);

	for (int y = 0 ; y < height ; y++)
	{
		for (int x = 0 ; x < width ; x++)
		{
			composite_image.at<Vec3b>(y,x) = Images[ResultLabel.at<uchar>(y,x)].at<Vec3b>(y,x);
		}
	}

    //imshow("composite image",composite_image);
	imwrite("image_graphcut.jpg", composite_image);
	//waitKey(0);
}


cv::flann::Index *  PhotoMontage::AddInertiaConstraint( const cv::Mat & Label )
{
	int height = Label.rows;
	int width = Label.cols;
	std::vector<Point2f> _data_vec;
	for (int y = 0 ; y < height; y++)
	{
		for (int x = 0 ; x < width; x++)
		{
			if (Label.at<char>(y,x) > 0)
			{
				_data_vec.push_back(Point2f(x,y));
			}
		}
	}

	_data.create(_data_vec.size(),2,CV_32FC1);	
	for (int i = 0 ; i < _data_vec.size(); i++)
	{
		_data.at<float>(i,0) = _data_vec[i].x;
		_data.at<float>(i,1) = _data_vec[i].y;
	}
	cv::flann::KDTreeIndexParams indexParams; 
	return new cv::flann::Index(_data, indexParams); 

	//std::vector<int> indices(1); 
	//std::vector<float> dists(1); 
	//Mat query(1,2,CV_32FC1);
	//query.at<float>(0,0) = 522;
	//query.at<float>(0,1) = 57;
	//kdtree->knnSearch(query, indices, dists, 1,cv::flann::SearchParams(64)); 
}

// 求解线性方程组
void PhotoMontage::SolveChannel( int channel_idx, int constraint, const cv::Mat &color_gradient_x, const cv::Mat &color_gradient_y , cv::Mat & output )
{
	/*
	// x方向梯度
	std::vector<double> vals1;
	std::vector<int> rows1;
	std::vector<int> cols1;
	int m, n;
	double p;

	int width = color_gradient_x.cols;
	int height = color_gradient_x.rows;

	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++)
		{
			vals1.push_back(color_gradient_x.at<Vec3f>(j, i));
			rows1.push_back(i);
			cols1.push_back(j);
		}

	// y方向梯度
	std::vector<double> vals2;
	std::vector<int> rows2;
	std::vector<int> cols2;

	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++)
		{
		vals2.push_back(color_gradient_y.at<Vec3f>(j, i));
		rows2.push_back(i);
		cols2.push_back(j);
		}

	Sparse_Matrix mat1, mat2;
	mat1.initializeFromVector(rows1, cols1, vals1);
	mat2.initializeFromVector(rows2, cols2, vals2);

	std::vector<double> b1, b2;
	for (int i = 0; i < height; i++)
	{
		b1.push_back();
		b2.push_back();
	}

	std::vector<Vec3f> ans1 = mat1.Solver(b1);
	std::vector<Vec3f> ans2 = mat2.Solver(b2);

	for (int y = 0 ; y < height ; y++)
	{
		for (int x = 0 ; x < width ; x++)
		{
			Vec3b & temp = output.at<Vec3b>(y,x);
			temp[channel_idx] = uchar(std::max(std::min(solution(y * width + x),255.0),0.0));
		}
	}
	*/

	int width = color_gradient_x.cols;
	int height = color_gradient_x.rows;


	int NumOfUnknownTerm = 2 * width * height + 1;
	std::vector<Eigen::Triplet<double>> NonZeroTerms;
	Eigen::VectorXd b(NumOfUnknownTerm); 
	for (int y = 0 ; y < height - 1; y++)
	{
		for (int x = 0 ;x < width - 1; x++)
		{
			int unknown_idx = y * width + x;

			/// gradient x
			int eq_idx1 = 2 * unknown_idx;
			NonZeroTerms.push_back(Eigen::Triplet<double>(eq_idx1,unknown_idx,-1));
			int other_idx1 = y * width + (x + 1);
			NonZeroTerms.push_back(Eigen::Triplet<double>(eq_idx1,other_idx1,1));
			Vec3f grads_x = color_gradient_x.at<Vec3f>(y,x);
			b(eq_idx1) = grads_x[channel_idx];

			/// gradient y
			int eq_idx2 = 2 * unknown_idx + 1;
			NonZeroTerms.push_back(Eigen::Triplet<double>(eq_idx2,unknown_idx,-1));
			int other_idx2 = (y + 1) * width + x;
			NonZeroTerms.push_back(Eigen::Triplet<double>(eq_idx2,other_idx2,1));
			Vec3f grads_y = color_gradient_y.at<Vec3f>(y,x);
			b(eq_idx2) = grads_y[channel_idx];
		}
	}


	///constraint
	int eq_idx = width * height * 2;
	NonZeroTerms.push_back(Eigen::Triplet<double>(eq_idx, 0, 1));
	b(eq_idx) = constraint;



	Eigen::SparseMatrix<double> A(NumOfUnknownTerm, width * height);	
	A.setFromTriplets(NonZeroTerms.begin(),NonZeroTerms.end());
	NonZeroTerms.erase(NonZeroTerms.begin(),NonZeroTerms.end());

	Eigen::SparseMatrix<double> ATA(width * height, width * height);
	ATA = A.transpose() * A;
	Eigen::VectorXd ATb = A.transpose() * b; 

	printf("\nSolving...\n");
	Eigen::ConjugateGradient<Eigen::SparseMatrix<double>> CGSolver(ATA);
	Eigen::VectorXd solution = CGSolver.solve(ATb);
	printf("Solved!\n");

	for (int y = 0 ; y < height ; y++)
	{
		for (int x = 0 ; x < width ; x++)
		{
			Vec3b & temp = output.at<Vec3b>(y,x);
			temp[channel_idx] = uchar(std::max(std::min(solution(y * width + x),255.0),0.0));
			//printf("%d,%d,  %f, %f\n",y,x, solution(y * width + x), ATb(y*width + x));
			//system("pause");
		}
	}


}
