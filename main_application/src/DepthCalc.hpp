#ifndef DEPTHCALC_H
#define DEPTHCALC_H

#define DEPTH_HEIGHT 480
#define DEPTH_WIDTH 640

#include "BBox.hpp"

int getDistance(std::vector<uint16_t> depthVector, Rectangle* rec);
int getDepth(uint16_t depthArray[], std::list<Rectangle*> BBoxes);

#endif
