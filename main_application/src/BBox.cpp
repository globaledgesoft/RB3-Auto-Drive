#include <vector>
#include <list>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include "BBox.hpp"

/************************************************************************
* name : compareContourArea
* function: compare the countour areas
************************************************************************/
bool compareContourAreas ( std::vector<cv::Point> contour1, std::vector<cv::Point> contour2 ) {
    double i = fabs( cv::contourArea(cv::Mat(contour1)) );
    double j = fabs( cv::contourArea(cv::Mat(contour2)) );
    return ( i < j );
}

/************************************************************************
* name : getBbox
* function: Returns a list of bounding boxes
************************************************************************/
std::list<Rectangle*> getBbox(cv::Mat gray_image)
{
    float h, w;
    double peri;
    int N;
    std::vector<std::vector<cv::Point>> contours, cnts;
    std::list<Rectangle*> bBoxes;
    std::vector<cv::Point> approx;
    std::vector<cv::Vec4i> hierarchy;

    h = gray_image.rows;
    w = gray_image.cols;

    cv::threshold(gray_image, gray_image, THRESH, MAX_VAL, cv::THRESH_BINARY);
    cv::findContours(gray_image, contours, hierarchy,  cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
    if (contours.size() >= 5) {
	    N = 5;
    } else {
	    N = contours.size();
    }
    if (!contours.empty()) {
        std::sort(contours.begin(), contours.end(), compareContourAreas);
        std::reverse(contours.begin(), contours.end());
        cnts = std::vector<std::vector<cv::Point>>(contours.begin(), contours.begin() + N);
        for (int i = 0; i < cnts.size(); i++)
        {
            peri = cv::arcLength(cnts[i], true);
            if (peri < 10)
                continue;
            cv::approxPolyDP(cv::Mat(cnts[i]), approx, 0.04 * peri, true);
            Rectangle* r = new Rectangle(cv::boundingRect(approx));
            bBoxes.push_back(r);
        }
    } else {
    	std::cout<<"No contor"<<std::endl;
    }
    return bBoxes;
}
