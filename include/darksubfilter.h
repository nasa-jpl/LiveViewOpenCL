#ifndef DARKSUBFILTER_H
#define DARKSUBFILTER_H

#include <algorithm>
#include <fstream>
#include <mutex>
#include <stdint.h>
#include <vector>

#include <QDebug>
#include <QString>

class DarkSubFilter
{
public:
    DarkSubFilter(size_t frame_size);
    virtual ~DarkSubFilter();

    void dsf_callback(uint16_t* in_frame, float* out_frame);
    void collect_mask(uint16_t* in_frame);
    void dark_subtract(uint16_t* in_frame, float *out_frame);

    void start_mask_collection();
    void finish_mask_collection();

    void apply_mask_file(const QString &file_name);
    void save_mask_file(const QString &file_name);

    std::mutex mask_mutex;

private:
    bool mask_collected;
    size_t frSize;
    unsigned int nSamples;

    std::vector<double> mask_accum;
    std::vector<float> mask;

};

#endif // DARKSUBFILTER_H
