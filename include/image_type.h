#ifndef IMAGE_TYPE_H
#define IMAGE_TYPE_H

#include <unordered_map>
#include <string>

enum image_t {BASE, DSF, STD_DEV, SPATIAL_PROFILE, SPECTRAL_PROFILE, SPATIAL_MEAN, SPECTRAL_MEAN};

enum camera_t {DEFAULT, ITB, CL_6604A, CL_6604B};

enum source_t {
    SSD = 0,
    DEBUG = 1,
    CAMERA_LINK = 2};


#endif // IMAGE_TYPE_H

