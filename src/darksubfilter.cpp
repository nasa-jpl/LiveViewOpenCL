#include "darksubfilter.h"

DarkSubFilter::DarkSubFilter(size_t frame_size) :
    mask_collected(true), frSize(frame_size)
{
    mask.resize(frSize);
    mask_accum.resize(frSize);
}

DarkSubFilter::~DarkSubFilter()
{
    mask_collected = false;
}

void DarkSubFilter::start_mask_collection()
{
    mask_collected = false;
    nSamples = 0;
    std::fill (mask.begin(), mask.end(), 0.0);
    std::fill (mask_accum.begin(), mask_accum.end(), 0.0);
}

void DarkSubFilter::finish_mask_collection()
{
    for (size_t i = 0; i < frSize; i++) {
        mask[i] = mask_accum[i] / (float)nSamples;
    }
    mask_collected = true;

    qDebug("Mask collected!");
}

void DarkSubFilter::collect_mask(uint16_t* in_frame)
{
    for (size_t i = 0; i < frSize; i++) {
        mask_accum[i] = in_frame[i] + mask_accum[i];
    }

    nSamples++;
}

void DarkSubFilter::dark_subtract(uint16_t* in_frame, float* out_frame)
{
    for (size_t i = 0; i < frSize; i++) {
        out_frame[i] = (float)in_frame[i] - mask[i];
    }
}

void DarkSubFilter::dsf_callback(uint16_t *in_frame, float *out_frame)
{
    if (mask_collected) {
        dark_subtract(in_frame, out_frame);
    } else {
        mask_mutex.lock();
        for (size_t i = 0; i < frSize; i++) {
            out_frame[i] = (float)in_frame[i];
        }
        collect_mask(in_frame);
        mask_mutex.unlock();
    }
}

void DarkSubFilter::apply_mask_file(const QString &file_name)
{
    std::ifstream mask_fp;
    std::streampos file_size = 0;

    mask_fp.open(file_name.toStdString(), std::ios::in | std::ios::binary);
    if (!mask_fp.is_open()) {
        qDebug() << "Could not open file" << file_name << ". Does it exist?";
        return;
    }
    mask_fp.unsetf(std::ios::skipws);

    file_size = mask_fp.tellg();
    mask_fp.seekg(0, std::ios::end);
    file_size = mask_fp.tellg() - file_size;
    mask_fp.clear();
    mask_fp.seekg(0, std::ios::beg);

    if ((file_size / sizeof(float)) < frSize) {
        qWarning("Mask file contains less than one frame of data!");
        return;
    }

    // Read one frame worth of floating-point numbers from the mask file
    mask_collected = false;
    mask_mutex.lock();
    mask_fp.read(reinterpret_cast<char*>(mask.data()), frSize * sizeof(float));
    mask_collected = true;
    mask_mutex.unlock();
    mask_fp.close();

}

void DarkSubFilter::save_mask_file(const QString &file_name)
{
    std::ofstream mask_fp;

    mask_fp.open(file_name.toStdString(), std::ios::out | std::ios::binary);
    if (mask_fp.is_open()) {
        mask_fp.write(reinterpret_cast<char*>(mask.data()), frSize * sizeof(float));
        mask_fp.close();
    } else {
        qDebug() << "Unable to save file:" << file_name;
    }

}

