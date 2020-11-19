#ifndef BBOX_H
#define BBOX_H

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#define THRESH 50
#define MAX_VAL 255
#define MAX_DEPTH 4095

class Rectangle
{
private:
    /* data */
    float x, y, h, w, d;
    float IMG_WIDTH = 320.0;
    float IMG_HEIGHT = 240.0;
public:
    float get_x() {
	    return x;
    }

    float get_y() {
	    return y;
    }
    float get_width() {
	    return w;
    }
    float get_height() {
	    return h;
    }
    void setDepth(float depth) {
        d = depth;
    }
    Rectangle(cv::Rect rectangle) {
        x = rectangle.x / this->IMG_WIDTH;
        y = rectangle.y / this->IMG_HEIGHT;
        w = rectangle.width / this->IMG_WIDTH;
        h = rectangle.height / this->IMG_HEIGHT;
        d = 0;
    }
    ~Rectangle();
};

bool compareContourAreas(std::vector<cv::Point> contour1, std::vector<cv::Point> contour2);
std::list<Rectangle*> getBbox(cv::Mat gray_image);

#endif
