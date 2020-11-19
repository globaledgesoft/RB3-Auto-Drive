#include <vector>
#include <list>
#include <fstream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

#include "BBox.hpp"
#include "DepthCalc.hpp"

/************************************************************************
* name : getDistance
* function: returns the distance
************************************************************************/
int getDistance(std::vector<uint16_t> depthVector, Rectangle* rec) {
	std::vector<int> freqMap(MAX_DEPTH, 0);
	int i, j;
	int x = (int)(rec->get_x() * DEPTH_WIDTH);
	int y = (int)(rec->get_y() * DEPTH_HEIGHT);
	int w = (int)(rec->get_width() * DEPTH_WIDTH);
	int h = (int)(rec->get_height() * DEPTH_HEIGHT);
	int max = 0;

	if (h < 20 || w < 20)
		return MAX_DEPTH;
	for (i = y; i < (y + h); i++) {
		for (j = x ; j < (x + w); j++) {
			freqMap[depthVector[i * DEPTH_WIDTH + j]]++;
			if(freqMap[depthVector[i * DEPTH_WIDTH + j]]>freqMap[max])
				max = depthVector[i*DEPTH_WIDTH+j];
			
		}
	}
	rec->setDepth(max);
	return int(max);
}

/************************************************************************
* name : getDepth
* function: returns the minimum distance
************************************************************************/
int getDepth(uint16_t depthArray[], std::list<Rectangle*> BBoxes)
{
	int min_depth = MAX_DEPTH;
	int d;
	int N = DEPTH_WIDTH*DEPTH_HEIGHT;
	std::vector<uint16_t> depthVector(depthArray, depthArray+N);

	for (auto it = BBoxes.begin(); it != BBoxes.end(); it++) {
		d = getDistance(depthVector, *it);
		if(d < min_depth && d != 0) {
			min_depth = d;
		}
	}
	return min_depth;
}
