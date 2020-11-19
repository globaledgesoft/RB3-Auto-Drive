/*

Filename: image_resize.c
Author: Sahil M. Bandar

*/
#include "image_resize.hpp"


std::vector<float> resize_gesl(std::vector<float> img, int w, int h) 
{
    /*
        DONE: 
                1. Implementation of image resize.
                    a. Nearest neighboure
        TODO:
                2. Other Algorithm Bilinear & Bicubic should be added as a another option for resize.
    */

    float scale_w = (float)256/(float)w;
    float scale_h = (float)256/(float)h;

    int c = 3;
    int fold_w_h = w*h;
    int fold_c_w = c*w;
    int fold_c_img_w = c*256;

    std::vector<float> resized_img;
    for(int k = 0; k < c; k++) {
        #pragma omp parallel for
        for(int j = 0; j < h; j++) {
            for(int i = 0; i < w; i++) {
                    int j_j = (int)(j*scale_h);
                    int i_i = (int)(i*scale_w);

                    int dst_d = k + c*i + fold_c_w*j;
                    int src = k + c*i_i + fold_c_img_w*j_j;

                    resized_img[dst_d] = img[src];

            }
        }
   }

    return resized_img;

}
