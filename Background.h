#ifndef BACKGROUND_H
#define BACKGROUND_H

#include "../kinect.h"
#include <fstream>
#include <vector>

class Background {

private:
	//int noOfFrames; 
	//std::string file;
	cv::Mat model;
	void findLargestContour(cv::Mat&, cv::Mat&);
	void extractBackgroundModel(std::string, int);
public:
	Background();
	Background(std::string, int);
	cv::Mat getModel();
	void removeBackground(cv::Mat&, cv::Mat&);

};

#endif