#pragma once
#include <opencv2\core.hpp>
#include<opencv2\highgui\highgui.hpp>
#include<opencv2\imgproc\imgproc.hpp>
#include<opencv2\ml\ml.hpp>

#include <string>
#include<iostream>
#include<sstream>


    const int MIN_CONTOUR_AREA = 100;
	const int RESIZED_IMAGE_WIDTH = 20;
	const int RESIZED_IMAGE_HEIGHT = 30;

class KNNChar {
public:
	KNNChar();
	~KNNChar();

	class ContourWithData {
	public:
		/* member variables */
		std::vector<cv::Point> ptContour;           // contour
		cv::Rect boundingRect;                      // bounding rect for contour
		float fltArea;                              // area of contour

		bool checkIfContourIsValid() {                              // obviously in a production grade program
			if (fltArea < MIN_CONTOUR_AREA) return false;           // we would have a much more robust function for
			return true;                                            // identifying if a contour is valid !!
		}


		static bool sortByBoundingRectXPosition(const ContourWithData& cwdLeft, const ContourWithData& cwdRight) {      // this function allows us to sort
			return(cwdLeft.boundingRect.x < cwdRight.boundingRect.x);                                                   // the contours from left to right
		}

	};

	std::string recognizeCharacter(cv::Mat testImage);
private:
	cv::Mat matClassificationInts;      // we will read the classification numbers into this variable as though it is a vector
	cv::Mat matTrainingImagesAsFlattenedFloats;         // we will read multiple images into this single image variable as though it is a vector

};

