#include <iostream>
#include <vector>
#include <list>
#include <iostream>
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <iterator>
#include <time.h>

#include "HandDetection\HandFinder.h"
#include "HandDetection\HandTracker.h"
#include "KNNChar.h"
#include "WindowActivity.h"

using namespace cv;
using namespace std;

const int maxLineSize = 10;
const cv::Scalar gestureLineColor = cv::Scalar(0, 0, 0);
const cv::Scalar gestureBackground = cv::Scalar(255, 255, 255);
const int gestureThickness = 2;
const int gestureHeight = 480;
const int gestureWidth = 640;



int main(int argc, const char * argv[])
{
	//print_help();

	HandFinder hand_finder("palm.xml");
	HandTracker hand_tracker;
	KNNChar recognizerChar;
	WindowActivity windowActivity;



	// setup camera
	cv::VideoCapture cap;
	if(!cap.open(0)){
		cout << "failed to open camera 0..." << endl;
		return -1;
	}
	cap.set(CV_CAP_PROP_FRAME_WIDTH, gestureWidth);
	cap.set(CV_CAP_PROP_FRAME_HEIGHT, gestureHeight);
	cap.set(CV_CAP_PROP_AUTO_EXPOSURE, 0);

	cv::Mat raw_img;
	cv::Mat ui_img;
	cv::Mat pic = cv::Mat::zeros(gestureHeight, gestureWidth, CV_8UC1);
	cv::Mat gestureImage(gestureHeight, gestureWidth, CV_8UC3, gestureBackground);
	cv::Mat tempImage = gestureImage.clone();



	// normalized hand points
	list<cv::Point2f> hand_npoints;

	bool running = true;
	/*
		0: circle gesture
		1: pigtail
		2: triangle
		3: v
	*/
	int template_id = 0;
	float score_threshold = 0.8f;
	int count_down_count = 0;
	bool debug = false;
	clock_t start = clock();
	int first = 0;

	while (running) {
		if (!cap.read(raw_img)) {
			cout << "failed to read from camera..." << endl;
			return -1;
		}
		// Flip Mirror image to Normal Image
		cv::flip(raw_img, raw_img, 1);
		ui_img = raw_img.clone();

		// Find Hand
		cv::Rect hand_rect = hand_finder.find(raw_img);
		cv::Point2f hand_pos(hand_rect.x + hand_rect.width / 2, hand_rect.y + hand_rect.height / 2);

		if (hand_rect.width * hand_rect.height ) {
			// hand found, draw bounding rect
			/*if (debug)*/
			cv::rectangle(ui_img, hand_rect.tl(), hand_rect.br(), cv::Scalar(0, 255, 0));
			cv::circle(ui_img, hand_pos, 3, cv::Scalar(255, 0, 0));

			/* Only to show when detected */

			//imshow("Latest", ui_img);

			// normalize hand position
			hand_pos = cv::Point2f((float)hand_pos.x / (float)raw_img.cols, (float)hand_pos.y / (float)raw_img.rows);
		} else {
			// no hands found
			hand_pos = cv::Point2f(-1.0f, -1.0f);
		}
		// track hand
		hand_tracker.track(hand_pos.x, hand_pos.y);

		// get hand trace
		list<cv::Point2f> hand_points = hand_tracker.get_current_trace();
		
		if (hand_points.size() >= 2) {
			if (hand_tracker.getState() != 0 && hand_tracker.getState() != 2) {
				Point2f pointOf = hand_points.back();
				hand_points.pop_back();
				Point2f secPointOf = hand_points.back();

				hand_points.push_back(pointOf);

				float newx = secPointOf.x - pointOf.x;
				float newy = secPointOf.y - pointOf.y;

				//Override Mouse
				cout << "state " << ((hand_tracker.getState() == 1) ? "TRACKING" : "LOST") << endl;
				windowActivity.overrideMouse(newx, newy, gestureHeight, gestureWidth);
			}
		}



		// recognize gesture
		/* Format Image*/
		/*if (hand_points.size() >  maxLineSize + 2) {
			std::list<Point2f>::const_iterator iterator;
			std::list<Point2f>::const_iterator temp;

			iterator = hand_points.end() ;
			iterator--;

			for (; (iterator != hand_points.begin()); ) {
				Point x((*iterator).x * gestureHeight, (*iterator).y * gestureWidth);
				iterator--;
				// De-Normalizing and drawing line on the image to test gesture
				cv::line(gestureImage, x ,Point((*iterator).x * gestureHeight ,(*iterator).y * gestureWidth), gestureLineColor, gestureThickness);
			}
			cout << ((clock()  * 1000 / CLOCKS_PER_SEC)) - ((start  * 1000 / CLOCKS_PER_SEC) )<<endl;
			if ( first == 0 || ((clock() * 1000 / CLOCKS_PER_SEC)) - ((start  * 1000/ CLOCKS_PER_SEC)) > 2000) {
				string gestureDetected = recognizerChar.recognizeCharacter(gestureImage);
				gestureImage = tempImage.clone();
				first = 1;
				// stop for input and clear gesture signal
				if (windowActivity.performAction(gestureDetected)) {

					//Clear when detected
					hand_tracker.clearPoints();
					hand_points.clear();

					start = clock();
					cout << gestureDetected << "  Hand Point size :: " << hand_points.size() << endl;
				}
			}
		}*/


		// draw hand trace
		if(hand_points.size()>= 1 ){
			for(list<cv::Point2f>::iterator it = hand_points.begin(), jt=--hand_points.end(); it!=jt;++it){
				cv::Point pt1((int)(it->x*ui_img.cols), (int)(it->y*ui_img.rows));
				++it;
				cv::Point pt2((int)(it->x*ui_img.cols), (int)(it->y*ui_img.rows));
				--it;
				cv::line(ui_img, pt1, pt2, cv::Scalar(255,0,255), 2);
			}
		}
		if(debug){
			// draw normalized hand trace
			if(hand_npoints.size()>0){
				for(list<cv::Point2f>::iterator it=hand_npoints.begin(), jt=--hand_npoints.end(); it!=jt;++it){
					if(it == hand_npoints.begin()){
						cv::circle(ui_img, cv::Point((int)(it->x), (int)(it->y)), 3, cv::Scalar(255,255,0));
					}
					cv::Point pt1((int)(it->x), (int)(it->y)); ++it;
					cv::Point pt2((int)(it->x), (int)(it->y)); --it;
					cv::line(ui_img, pt1, pt2, cv::Scalar(0,0,255), 1);
				}
			}
		}

		imshow("ui", ui_img);

		string gesture_str[4] = {"circle", "pigtail", "triangle", "victory"};

		switch(255 & cv::waitKey(30)){
			case 27:    //exit
				running = false;
				break;
			case 'd':
				debug = !debug;
				cout << "debug mode: " << debug << endl;
				break;
			case ' ':
				template_id++;
				if(template_id == 4) template_id = 0;
				cout << "gesture template: " << gesture_str[template_id] << endl;
				break;
		}

	}
	return 0;
}

