#include "Background.h"
#include <opencv\highgui.h> // debug
Background::Background() {
}

Background::Background(std::string fileName, int framesNo) {
	extractBackgroundModel(fileName, framesNo);
}

cv::Mat Background::getModel() {
	return model;
}

void Background::findLargestContour(cv::Mat& inputFrame, cv::Mat& outputFrame) {
	
	// Convert frame to binary
	cv::Mat binaryFrame(cv::Size(FRAME_WIDTH, FRAME_HEIGHT), CV_8UC1);
	inputFrame.convertTo(binaryFrame, CV_8UC1);
	
	// Find all contours
	vector<vector<cv::Point> > contours;
	cv::findContours(binaryFrame, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	
	// Find the largest contour
	double contourArea = 0;
	double maxContourArea = 0;
	int maxContourIndex = 0;
	
	for (int i=0; i<contours.size(); ++i) {
		cv::Mat tempMat(contours[i]);
		contourArea = cv::contourArea(tempMat);

		if(contourArea > maxContourArea) {
			maxContourArea = contourArea;
			maxContourIndex = i;
		}
	}	

	// Fill the largest contour to create a mask
	cv::Mat maxContourMask = cv::Mat::zeros(FRAME_HEIGHT, FRAME_WIDTH, CV_8U);
	drawContours(maxContourMask, contours, maxContourIndex, cv::Scalar(255,255,255), CV_FILLED); 
	
	// Get the final depth after applying contour mask	
	inputFrame.copyTo(outputFrame, maxContourMask);
}

void Background::extractBackgroundModel(std::string fileName, int framesNo) {
		
	// Read first framesNo frames and save them in a vector
	std::vector<cv::Mat> backgroundFrames;	
	bool splitSuccess =	kin::splitDepthDataFile(fileName, framesNo, backgroundFrames);
	if (!splitSuccess) {
		throw FILE_NOT_OPEN;
	}

	// For each pixel position, choose the min from all the frames
	model = cv::Mat(FRAME_HEIGHT, FRAME_WIDTH, CV_16U);
	int min;
	for (int m = 0; m<FRAME_HEIGHT; ++m) {
		for (int n=0; n<FRAME_WIDTH; ++n) {
			min = 5000;
			int noOfZeros = 1;

			for (int j=1; j<framesNo; j++) {
				if (backgroundFrames.at(j).at<ushort>(m,n) == 0)
					noOfZeros++ ;
				else if (backgroundFrames.at(j).at<ushort>(m,n) < min)
					min = backgroundFrames.at(j).at<ushort>(m,n);
			}
			if (noOfZeros == framesNo) // if for that pixel, we have 0 in all the frames
				min = 0; // if we don't do that, min will stay 5000

			model.at<ushort>(m,n) = min;
		}
	}	
}

void Background::removeBackground(cv::Mat& inputFrame, cv::Mat& outputFrame) {
	cv::Mat frameCopy = inputFrame.clone();

	cv::imshow("before subtraction", frameCopy);
	cv::imshow("background", model);
	std::cout<<"test"<<std::endl;

	// 1. Subtract background model
	for (int m = 0; m<FRAME_HEIGHT; ++m) {
		for (int n=0; n<FRAME_WIDTH; ++n) {
			if (model.at<ushort>(m,n) !=0) {
				if (model.at<ushort>(m,n) <= inputFrame.at<ushort>(m,n) /*|| inputFrame.at<ushort>(m,n)>3000*/ ) {
					frameCopy.at<ushort>(m,n) = 0;
				}
			}
		}
	}

	cv::imshow("after subtraction", frameCopy);

	// 2. Pass through median filter
	cv::medianBlur(frameCopy, frameCopy, 5);

	// 3. Largest contour tracking
	findLargestContour(frameCopy, outputFrame);

    cv::imshow("laregst contour", outputFrame);
	//cv::waitKey();
}


// ---------- UNUSED METHODS ---------- //

//// Unused method instead of inputFrame.copyTo(contourFrame, maxContourMat); in findLargestContour
//// It was replaced for time, but it actually takes the same time
//contourFrame = cv::Mat::zeros(FRAME_HEIGHT, FRAME_WIDTH, CV_16U);
//for (int k=0; k<contourFrame.rows; k++)
//{
//	for (int l=0; l<contourFrame.cols; l++)
//	{
//		if (maxContourMat.at<unsigned short>(k,l) != 0)
//		{
//			contourFrame.at<unsigned short>(k,l) = inputFrame.at<unsigned short>(k,l);
//		}
//	}		
//}