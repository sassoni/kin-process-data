#include <opencv\highgui.h>
//#include <pcl\io\pcd_io.h>
#include <pcl\visualization\cloud_viewer.h>
#include <pcl\visualization\pcl_visualizer.h>
//#include <pcl\io\pcd_io.h>
#include <string>
#include "../kinect.h"
#include "Background.h"

int main ( int argc, char *argv[] ) {
	
	// Open configuration file
	char* filename;
	if ( argc != 2 ) {
		std::cout<<"usage: "<< argv[0] <<" <xml_kinect_config_file>\nPress ENTER to exit\n";
		cin.get();
		return 1;
	}
	else {
		filename = argv[1];
	}	
	cv::FileStorage configFile; 
	configFile.open(filename, cv::FileStorage::READ);
	if (!configFile.isOpened()) {
		std::cout<<"Could not open configuration file\nPress ENTER to exit\n";
		cin.get();
		return 1;
	}
	
	std::string saveFilename;
	configFile["save_folder"] >> saveFilename;
	std::string kinectID;
	configFile["kinect_id"] >> kinectID;

	// Setup the kinect intrinsics (color and depth camera matrices)
	std::string intrinsicsFilename;
	cv::Mat depthCameraMat;
	cv::Mat colorCameraMat;
	configFile["kinect_intrinsics"] >> intrinsicsFilename;
	kin::setupCamera(intrinsicsFilename, depthCameraMat, colorCameraMat);

	// -------------------- SPLIT COLOR AND DEPTH DATA TO FRAMES -------------------- //
	// Depth
	std::string depthDataFilename;
	std::string depthInfoFilename;
	configFile["depth_data"] >> depthDataFilename;
	configFile["depth_info"] >> depthInfoFilename;
	std::vector<cv::Mat> depthFrames;
	std::vector<int> depthSerials;
	bool depthSplit = kin::splitDepthDataFile(depthDataFilename, depthInfoFilename, depthFrames, depthSerials);

	if (!depthSplit) {
		std::cout<<"Could not split depth data. Check file paths.\nPress ENTER to exit\n";
		cin.get();
		return 1;
	}
    // debug (show depth frames)
	for (int i=0, s=depthFrames.size(); i<s; ++i) {
		cv::imshow("Frames", depthFrames.at(i));
		cv::waitKey(100);
	}

	// Color
	std::string colorDataFilename;
	configFile["color_data"] >> colorDataFilename;
	std::vector<cv::Mat> colorFrames;
	bool colorSplit = kin::splitColorDataFile(colorDataFilename, colorFrames);
	if (!colorSplit) {
		std::cout<<"Could not split color data. Check file paths.\nPress ENTER to exit\n";
		cin.get();
		return 1;
	}

	// debug (show color frames)
	for (int i=0, s=colorFrames.size(); i<s; ++i) {
		cv::imshow("Frames", colorFrames.at(i));
		cv::waitKey(100);
	}

    // -------------------- BACKGROUND REMOVAL -------------------- //
	std::cout<<"\nStarting background removal...\n";
	std::cout<<"Calculating background model...\n";
	std::string backgroundDataFilename;
	int noOfFramesUsed;
	configFile["background_data"] >> backgroundDataFilename;
	configFile["background_frames_no"] >> noOfFramesUsed;
	Background b;
	try {
		b = Background(backgroundDataFilename, noOfFramesUsed); 
	} catch (int e) {
		if (e == FILE_NOT_OPEN) {
			std::cout<<"Could not open background data file...\n";
			cin.get();
			return 1;
		}
	}

	std::cout<<"Removing background from frames...\n";
	for (int i=0, s=depthFrames.size(); i<s; ++i) {
		cv::Mat temp;
		b.removeBackground(depthFrames[i], temp);
		depthFrames[i] = temp;
	}
	std::cout<<"Done!\n\n";
	
	// debug (show depth frames)
	//for (int i=0, s=depthFrames.size(); i<s; ++i) {
	//	cv::imshow("Frames", depthFrames.at(i)*10);
	//	cv::waitKey(100);
	//}
	//cv::destroyWindow("Frames");

	std::cout<<"Press ENTER to start conversion of frames.\nIn the viewer that will open press R to reset the viewpoint and see the clouds\n";
	std::cout<<"NOTE: In order for the clouds to be saved, folder "<<saveFilename<<" must exist.\n";
	cin.get();

	// -------------------- SETUP THE VIEWER -------------------- //
	
	pcl::visualization::PCLVisualizer viewer; 
	pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud (new pcl::PointCloud<pcl::PointXYZRGB>);
	viewer.setBackgroundColor (0, 0, 0);
	viewer.addCoordinateSystem (1.0);
	viewer.initCameraParameters ();
	pcl::visualization::PointCloudColorHandlerRGBField<pcl::PointXYZRGB> rgb(cloud);
	viewer.addPointCloud<pcl::PointXYZRGB> (cloud, rgb, "colorcloud");
	viewer.setPointCloudRenderingProperties (pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 1, "colorcloud");

	// -------------------- CONVERT FRAMES TO COLORED CLOUDS AND SAVE THEM -------------------- //
	
	for (int i=0, s=depthFrames.size(); i<s; ++i) {
		cloud->clear();
		// Convert current frame to cloud
		kin::frameToCloud(depthFrames[i], depthCameraMat, *cloud);
		// Color it
		kin::colorCloud(cloud, colorFrames[i], intrinsicsFilename);
		// Save it
		std::stringstream ss;
		ss<<saveFilename<<"/"<<kinectID<<"_"<<depthSerials[i]<<".pcd";
		pcl::io::savePCDFileBinary(ss.str(), *cloud);
		std::cout<<"Saving file "<<ss.str()<<std::endl;
		// Show it in the viewer
		viewer.updatePointCloud(cloud, "colorcloud");
		viewer.spinOnce(100);
	}
	viewer.close();

	// -------------------- DONE -------------------- //
	std::cout<<"\nNice working with you. Process finished. Press ENTER to exit.\n";
	std::cin.get();
	
	return 0;
}


/*
	while (!viewer.wasStopped ()) {
		viewer.spinOnce (100);
	}*/

	//viewer.registerKeyboardCallback(keyClickCallback);
//
//void keyClickCallback (const pcl::visualization::KeyboardEvent &event, void *cookie) {
//	if( cloudCounter < framesSize ) {
//		if (event.getKeySym () == "n" && event.keyDown ()) {
//			
//			cloud->clear();
//			
//			// Convert current frame to cloud
//			frameToCloud(depthFrames[cloudCounter], depthCameraMat, *cloud);
//			// Color it
//			colorCloud(cloud, colorFrames[cloudCounter], intrinsicsFilename);
//			// Save it
//			std::stringstream s;
//			s<<saveFilename<<"/"<<kinectID<<"_"<<depthSerials[cloudCounter]<<".pcd";
//			pcl::io::savePCDFileBinary(s.str(), *cloud);
//			// Show it in the viewer
//			viewer.updatePointCloud(cloud, "colorcloud");
//			
//			cloudCounter++;
//		}
//	}
//}