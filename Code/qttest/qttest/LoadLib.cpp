#pragma once

#define _LIBPATH_	"./OpenCV23/lib/"

#ifdef _DEBUG
#pragma comment( lib, ""_LIBPATH_"opencv_calib3d240d.lib" )
#pragma comment( lib, ""_LIBPATH_"opencv_contrib240d.lib" )
#pragma comment( lib, ""_LIBPATH_"opencv_core240d.lib" )
#pragma comment( lib, ""_LIBPATH_"opencv_features2d240d.lib" )
#pragma comment( lib, ""_LIBPATH_"opencv_highgui240d.lib" )
#pragma comment( lib, ""_LIBPATH_"opencv_imgproc240d.lib" )
#pragma comment( lib, ""_LIBPATH_"opencv_flann240d.lib" )
#else
#pragma comment( lib, ""_LIBPATH_"opencv_calib3d240.lib" )
#pragma comment( lib, ""_LIBPATH_"opencv_contrib240.lib" )
#pragma comment( lib, ""_LIBPATH_"opencv_core240.lib" )
#pragma comment( lib, ""_LIBPATH_"opencv_features2d240.lib" )
#pragma comment( lib, ""_LIBPATH_"opencv_highgui240.lib" )
#pragma comment( lib, ""_LIBPATH_"opencv_imgproc240.lib" )
#pragma comment( lib, ""_LIBPATH_"opencv_flann240.lib" )
#endif

#undef _LIBPATH_