#ifndef simple_hand_tracking_handtracker_hpp
#define simple_hand_tracking_handtracker_hpp

#include <vector>
#include <list>
#include <opencv2/opencv.hpp>


using namespace std;

#define MAX_LOSING_FRAMES 5
#define MAX_FRAME_DISTANCE 0.2f
#define SMOOTH_COEFFICIENT 0.5f
#define MAX_POINTS 20
#define GESTURE_TIME_LIMIT 2

#define DISTANCE(x1,y1,x2,y2) (sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2)))
int lostCounter = 0;
int losingCounter = 0;

class HandTracker{
private:

    enum state_t {LOST, TRACKING, LOSING} state;
    list<cv::Point2f> points;

    int recognize_guesture(){
        int gesture = 0;
        return gesture;
    }

public:
    HandTracker(){
        state = LOST;
    }

	// Clear when detected
	void clearPoints() {
		points.clear();
	}

    list<cv::Point2f> get_current_trace(){
        return points;
    }

    int track(float x, float y){
        int ret = 0;
        static int losing_frames = MAX_LOSING_FRAMES;
        static cv::Point2f last_pt;

        switch(this->state){
            case LOST:
				lostCounter++;
                if(x >= 0 && y >= 0){
                    //cout << "found object at: " << x << "," << y << endl;
                    if(points.size() == MAX_POINTS) points.pop_front();
                    points.push_back(cv::Point2f(x,y));
                    last_pt = cv::Point2f(x,y);
                    this->state = TRACKING;
					lostCounter = 0;
                }else{
					if (lostCounter >= GESTURE_TIME_LIMIT) {
						points.clear();
						lostCounter = 0;
					}
                }
                break;
            case TRACKING:
                if(x >= 0 && y >= 0){
                    // new points
                    if(DISTANCE(x,y,last_pt.x,last_pt.y) > MAX_FRAME_DISTANCE){
                        cout << "too much movement, lost!" << endl;
                        this->state = LOST;
                        points.clear();
                    }else{
                        // smooth
                        x = x*SMOOTH_COEFFICIENT + last_pt.x*(1-SMOOTH_COEFFICIENT);
                        y = y*SMOOTH_COEFFICIENT + last_pt.y*(1-SMOOTH_COEFFICIENT);
                        if(points.size() == MAX_POINTS) points.pop_front();
                        points.push_back(cv::Point2f(x,y));
                        last_pt = cv::Point2f(x,y);
                        ret = recognize_guesture();
                    }
                }else{
                    //cout << "losing object..." << endl;
                    this->state = LOSING;
                    //points.clear();
                }
                break;
            case LOSING:
                if(x >= 0 && y >= 0){
                    // back to tracking
                    losing_frames = MAX_LOSING_FRAMES;
                    this->state = TRACKING;
                }else{
                    // can't find object for several times, then lost
                    losing_frames--;
                    if(losing_frames == 0){
                        //cout << "lost!" << endl;
                        losing_frames = MAX_LOSING_FRAMES;
                        this->state = LOST;
					    //points.clear();
                    }
                }
                break;
        }

        return ret;
    }

};

#endif
