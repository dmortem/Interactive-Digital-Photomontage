#pragma once

#define _LIBPATH_	"Z:/OpenCV23/lib/"

		#ifdef _DEBUG
			#pragma comment( lib, ""_LIBPATH_"opencv_calib3d231d.lib" )
			#pragma comment( lib, ""_LIBPATH_"opencv_contrib231d.lib" )
			#pragma comment( lib, ""_LIBPATH_"opencv_core231d.lib" )
			#pragma comment( lib, ""_LIBPATH_"opencv_features2d231d.lib" )
			#pragma comment( lib, ""_LIBPATH_"opencv_highgui231d.lib" )
			#pragma comment( lib, ""_LIBPATH_"opencv_imgproc231d.lib" )
			#pragma comment( lib, ""_LIBPATH_"opencv_flann231d.lib" )
		#else
			#pragma comment( lib, ""_LIBPATH_"opencv_calib3d231.lib" )
			#pragma comment( lib, ""_LIBPATH_"opencv_contrib231.lib" )
			#pragma comment( lib, ""_LIBPATH_"opencv_core231.lib" )
			#pragma comment( lib, ""_LIBPATH_"opencv_features2d231.lib" )
			#pragma comment( lib, ""_LIBPATH_"opencv_highgui231.lib" )
			#pragma comment( lib, ""_LIBPATH_"opencv_imgproc231.lib" )
			#pragma comment( lib, ""_LIBPATH_"opencv_flann231.lib" )
		#endif
		
#undef _LIBPATH_