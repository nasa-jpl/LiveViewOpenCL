#ifndef IMAGE_TYPE_H
#define IMAGE_TYPE_H

#include <unordered_map>
#include <string>

enum image_t {BASE, DSF, STD_DEV, SPATIAL_PROFILE, SPECTRAL_PROFILE, SPATIAL_MEAN, SPECTRAL_MEAN};

enum camera_t {SSD_ENVI, SSD_XIO, CL_6604A, CL_6604B, SSD_RC};

enum source_t {
    XIO = 0,
    ENVI = 1,
    CAMERA_LINK = 2,
    RC = 3
};

enum org_t {fwBIL, fwBIP, fwBSQ};

struct save_req_t
{
    org_t bit_org;
    std::string file_name;
    int64_t nFrames;
    int64_t nAvgs;
};

#endif // IMAGE_TYPE_H

