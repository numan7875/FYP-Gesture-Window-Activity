#include <iostream>
#include <vector>
#include <list>
#include <iostream>
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <iterator>

#include "HandDetection\GestureData.h"
#include "HandDetection\HandFinder.hpp"
#include "HandDetection\HandTracker.hpp"
#include "HandDetection\OneDollar.hpp"

using namespace cv;
using namespace std;

const int maxLineSize = 10;
const cv::Scalar gestureLineColor = cv::Scalar(0, 0, 0);
const cv::Scalar gestureBackground = cv::Scalar(255, 255, 255);
const int gestureThickness = 2;
const int gestureHeight = 240;
const int gestureWidth = 320;

//void print_help(){
//    cout << "Simple hand tracking demo.\n" << \
//            "   [*] press 'd' to toggle debug mode.\n" << \
//            "   [*] press SPACE to change gesture template.\n" << \
//            "   [*] press ESC to exit.\n" << endl;
//}

int main(int argc, const char * argv[])
{
    //print_help();

    HandFinder hand_finder("cascade.xml");
    HandTracker hand_tracker;

    // setup camera
    cv::VideoCapture cap;
    if(!cap.open(0)){
        cout << "failed to open camera 0..." << endl;
        return -1;
    }
    cap.set(CV_CAP_PROP_FRAME_WIDTH, 640);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
    cap.set(CV_CAP_PROP_AUTO_EXPOSURE, 0);

    cv::Mat raw_img;
    cv::Mat ui_img;
    cv::Mat pic = cv::Mat::zeros(480, 640, CV_8UC1);

    // $1 gesture recognizer
    OneDollar one_dollar;

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

    while(running){
        if(!cap.read(raw_img)){
            cout << "failed to read from camera..." << endl;
            return -1;
        }
        // mirror raw image left &right
        cv::flip(raw_img, raw_img, 1);
        ui_img = raw_img.clone();

        // find hand
		cv::Rect hand_rect = hand_finder.find(raw_img);
		cv::Point2f hand_pos(hand_rect.x + hand_rect.width / 2, hand_rect.y + hand_rect.height / 2);
		if (hand_rect.width * hand_rect.height) {
			// hand found, draw bounding rect
			/*if (debug)*/ cv::rectangle(ui_img, hand_rect.tl(), hand_rect.br(), cv::Scalar(0, 255, 0));
			cv::circle(ui_img, hand_pos, 3, cv::Scalar(255, 0, 0));
			imshow("Latest", ui_img);
			// normalize hand position
			hand_pos = cv::Point2f((float)hand_pos.x / (float)raw_img.cols, (float)hand_pos.y / (float)raw_img.rows);
		} else {
            // no hands found
            hand_pos = cv::Point2f(-1.0f,-1.0f);
        }

        // track hand
        hand_tracker.track(hand_pos.x, hand_pos.y);

        // get hand trace
        list<cv::Point2f> hand_points = hand_tracker.get_current_trace();

        // recognize gesture
        //float score = one_dollar.recognize_cv(hand_points, hand_npoints, template_id);
        //if(score > score_threshold) {
        //    if(count_down_count == 0){
        //        count_down_count = 29;
        //    }
        //    if(debug) cout << "score = " << score << endl;
        //}
        //if(count_down_count > 0){
        //    // countdown
        //    char buf[8];
        //    sprintf(buf, "%d", count_down_count/10 + 1);
        //    cv::putText(ui_img, buf, cv::Point(ui_img.cols/2-20,ui_img.rows/2), cv::FONT_HERSHEY_SIMPLEX, 5, cv::Scalar(0,255,100), 20);
        //    count_down_count--;
        //    if(count_down_count == 0){
        //        // take a snapshot
        //        pic = raw_img.clone();
        //    }
        //}

        // draw hand trace
        if(hand_points.size()>0){
            for(list<cv::Point2f>::iterator it=hand_points.begin(), jt=--hand_points.end(); it!=jt;++it){
                cv::Point pt1((int)(it->x*ui_img.cols), (int)(it->y*ui_img.rows));
                ++it;
		cv::Point pt2((int)(it->x*ui_img.cols), (int)(it->y*ui_img.rows));
		--it;
                cv::line(ui_img, pt1, pt2, cv::Scalar(0,0,255), 2);
            }
        }
		if (hand_points.size() > maxLineSize + 2) {

			cv::Mat gestureImage(gestureHeight, gestureWidth, CV_8UC3, gestureBackground);
			std::list<Point2f>::const_iterator iterator;
			std::list<Point2f>::const_iterator temp;
			int count = maxLineSize;
			iterator = hand_points.end();
			iterator--;
			for (; count && (iterator != hand_points.end()); --count) {

				Point x((*iterator).x * gestureHeight, (*iterator).y * gestureWidth);
				iterator--;
				cv::line(gestureImage, x ,Point((*iterator).x * gestureHeight ,(*iterator).y * gestureWidth), gestureLineColor, gestureThickness);

			}
				cv::imwrite("gesture.jpg", gestureImage);
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

            // draw template trace
            one_dollar.cv_draw_template(ui_img, template_id);
        }

        imshow("ui", ui_img);
        imshow("pic", pic);

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

