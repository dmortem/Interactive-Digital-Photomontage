#include <QtWidgets>
#include "opencv.hpp"
#include "PhotoMontage.h"
#include "ScribbleWindow.h"

using namespace std;
using namespace cv;

ScribbleWindow::ScribbleWindow(){
	penColor = Qt::blue;
	penWidth = 10;
	sourceImages = new QTabWidget;
	setCentralWidget(sourceImages);

	setWindowTitle(tr("Scribble Window"));
	resize(600, 400);
}

void ScribbleWindow::setPenColor()
{
	QColor newColor = QColorDialog::getColor(penColor);
	if (newColor.isValid()) {
		int num = sourceImages->count();
		for (int i = num - 1; i >= 0; i--) {
			((ScribbleArea*)sourceImages->widget(i))->setPenColor(newColor);
		}
		penColor = newColor;
	}
}

void ScribbleWindow::setPenWidth()
{
	bool ok;
	int newWidth = QInputDialog::getInt(this, tr("Scribble"),
		tr("Select pen width:"),
		penWidth,
		1, 50, 1, &ok);
	if (ok) {
		int num = sourceImages->count();
		for (int i = num - 1; i >= 0; i--) {
			((ScribbleArea*)sourceImages->widget(i))->setPenWidth(newWidth);
		}
		penWidth = newWidth;
	}
}

void ScribbleWindow::openStack()		// 加载stacks名字到stackfiles
{
	stackFiles = QFileDialog::getOpenFileNames(
		this,
		"Select one or more files to open",
		NULL,
		"Images (*.png *.xpm *.jpg *.gif *.bmp)");

	if (!stackFiles.isEmpty()) {
		createScribbleWindow();
		show();
	}
}

void ScribbleWindow::createScribbleWindow(){	// 把stackfiles里的图像文件打开，存入sourceImage
	if(!stackFiles.isEmpty()){
        sourceImages->clear();
		for(int i = 0; i < stackFiles.size(); i++){
			sourceImages->addTab(new ScribbleArea(stackFiles.at(i), i, penWidth, penColor), tr(QString::number(i).toLatin1()));	// 设置标签名
		}
	}
}

void ScribbleWindow::work(){
	// 根据stackfiles中的文件名读取图像数据
	vector<Mat> Images;
	for (int i = 0; i < stackFiles.size(); i++)
		Images.push_back(imread(stackFiles.at(i).toStdString()));
	//imshow("images", Images[0]);

	// 标签数据
	cv::Mat Label(Images[0].rows, Images[0].cols, CV_8SC1);
	Label.setTo(PhotoMontage::undefined);

	// 读取绘制点数据
    vector<myPoint> tempVector;
    int c = sourceImages->count();
    for(int i = c-1 ; i>=0; i--)
	{
        vector<myPoint> *tmpPointVector = &(((ScribbleArea*)sourceImages->widget(i))->myPointVector);
        tempVector.insert(tempVector.begin(),tmpPointVector->begin(),tmpPointVector->end());
    }

	// 根据绘制点数据生成Label
	for (int i = 0; i < tempVector.size(); i++)	// 第i条笔画
	{
		int x = tempVector[i].x;
		int y = tempVector[i].y;
		int label_num = tempVector[i].label;

		for (int ii = x - penWidth; ii <= x + penWidth; ii++)
		{
			if (ii >= 0 && ii < Label.cols)
			{
				for (int jj = y - penWidth; jj <= y + penWidth; jj++)
					if (jj >= 0 && jj < Label.rows)
						Label.at<uchar>(jj, ii) = label_num;
			}
		}
	}

	PhotoMontage PM;
	PM.Run(Images, Label);
}
