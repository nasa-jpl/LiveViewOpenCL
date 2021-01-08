#include "xiocamera.h"

XIOCamera::XIOCamera(int frWidth,
        int frHeight, int dataHeight,
        QObject *parent
) : CameraModel(parent), nFrames(32), framesize(0),
    headsize(frWidth * int(sizeof(uint16_t))), image_no(0),
    tmoutPeriod(100) // milliseconds
{
    source_type = XIO;
    camera_type = SSD_XIO;
    frame_width = frWidth;
    frame_height = frHeight;
    data_height = dataHeight;
    is_reading = false;

    // EMITFPIED-331
    frameAcquistionSupended = false;

    header.resize(size_t(headsize));
    std::fill(header.begin(), header.end(), 0);

    dummy.resize(size_t(frame_width * data_height));
    std::fill(dummy.begin(), dummy.end(), 0);
    for (int n = 0; n < nFrames; n++) {
        frame_buf.emplace_back(std::vector<uint16_t>(size_t(frame_width * data_height), 0));
    }
}

XIOCamera::~XIOCamera()
{
    running.store(false);
    emit timeout();
    is_reading = false;
    readLoopFuture.waitForFinished();
}

void XIOCamera::setDir(const char *dirname)
{
    is_reading = false;
    while (!frame_buf.empty()) {
        frame_buf.pop_back();
    }

    if (readLoopFuture.isRunning()) {
        readLoopFuture.waitForFinished();
    }
    data_dir = dirname;
    if (data_dir.empty()) {
        if (running.load()) {
            running.store(false);
            emit timeout();
        }
        return;
    }
    xio_files.clear();
    dev_p.clear();
    dev_p.close();
    image_no = 0;
    std::vector<std::string> fname_list;
    os::listdir(fname_list, data_dir);

    // Sort the frames in the product directory by filename, as mtime is unreliable.
    std::sort(fname_list.begin(), fname_list.end(), doj::alphanum_less<std::string>());

    for (auto &f : fname_list) {
        std::string ext = os::getext(f);
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        if (f.empty() or std::strcmp(ext.data(), "xio") != 0 or std::strcmp(ext.data(), "decomp") != 0)
            continue;

        xio_files.emplace_back(f);
    }

    running.store(true);
    emit started();

    is_reading = true;
    readLoopFuture = QtConcurrent::run(this, &XIOCamera::readLoop);
} // end of XIOCamera::setDir() 

// EMITFPIED-331
void XIOCamera::suspendFrameAcquistion( bool status )
{
  frameAcquistionSupended = status;
  qDebug() << "PK Debug - XIOCamera::suspendFrameAcquistion()";
  qDebug() << "PK Debug - XIOCamera::frameAcquistionSupended = " << frameAcquistionSupended; 
  
} // end of void XIOCamera::suspendFrameAcquistion()
// EMITFPIED-331



//
// PK Debug - need to document this function's purpose !!
//
//
// getFname() - returns a xio file !!
//
//     When this function is called the very FIRST time, it reads 
//     all the image files (*.xio) found in the directory to build
//     an image files list 'xio_files', and returns the first
//     image file on the list.
//
//     After xio_files list is loaded, all subsequent calls of this
//     function will return the NEXT file in the xio_files list.
//
//       if (image_no < xio_files.size()) {
//            fname = xio_files[image_no++];
//       }
//
//           
std::string XIOCamera::getFname()
{
    std::string fname; // will return empty string if no unread files are found.
    std::vector<std::string> fname_list;
    bool has_file = false;

    static bool getFname_logged = false;

    if (data_dir.empty()) {
        return fname;
    }

    if( !getFname_logged )
        qInfo() << "PK Debug info: XIOCamera::getFname() image_no: " << image_no <<  " xio_files size: " << xio_files.size();

    if( xio_files.size() > 0 )
    {
        //
        // image files have ALREADY been loaded into xio_files list.
        if (image_no < xio_files.size())
        {
            //
            // return the next file from the xio_files list.
            fname = xio_files[image_no++];
            return fname;
        }
        else
        {
            if( image_no == xio_files.size() )
            {
                // all image files found in the directory 'data_dir' have been read.
                // so nothing to be done, and returns an empty string.
                if( !getFname_logged )
                {
                    qInfo() << "PK Debug info: XIOCamera::getFname() image_no: " << image_no <<  " xio_files size: " << xio_files.size();
                    qInfo() << "PK Debug info: XIOCamera::getFname() nothing done, and returns an empty fname";
                    getFname_logged = true;
                }
                return fname;
            }
        }
    }
    else
    {
        auto fileCount = 0;
        os::listdir(fname_list, data_dir);
        if (fname_list.size() < 1) {
            return fname;
        }
        /* if necessary, there may need to be code to sort the "frames" in the data directory
        * by product name, as mtime is unreliable.
        */
        std::sort(fname_list.begin(), fname_list.end(), doj::alphanum_less<std::string>());
        for ( auto &f : fname_list )
        {
            std::string ext = os::getext(f);
            std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

            if ((f).empty() or (std::strcmp(ext.data(), "xio") != 0 and
                                 std::strcmp(ext.data(), "decomp") != 0))
            {
                // corrupted file or file has NO expected extension 
                continue;
            }
            else
            {
                xio_files.emplace_back(f);
                fileCount++;
                qDebug() << "PK Debug XIOCamera::getFname(): add file #" << fileCount << ":  " << f.data() << "to xio_files";
            }

        } // bottom of the FOR loop
        
        qDebug() << "PK Debug XIOCamera::getFname(): xio_files list is filled with all the images files found.";
        if (image_no < xio_files.size())
        {
            qDebug() << "PK Debug XIOCamera::getFname(): image_no = " << image_no << " return the 1st file on the list, xio_files";
            fname = xio_files[image_no++];
        }
    }

    return fname;
    
} // end of XIOCamera::getFname()

void XIOCamera::readFile()
{
    bool validFile = false;

    //
    // EMITFPIED-331
    // This is where to insert a check for frame display suspension status
    static bool logged = false;
    static int  validFileCount = 0;

    while(!validFile) {

        if( frameAcquistionSupended == true )
        {
            //
            // if frameAcquisitionSuspended is ON, stop reading new frames.
            if( !logged )
            {
                qDebug() << "frameAcquistionSupended is ON, XIOCamera::readFile() do NOTHING";
                logged = true;
            }
            // qDebug() << "XIOCamera::readLoop() sleep 1 msec"
            // sleep for 10 msec
            QThread::usleep( 10000 );
            continue;
        }


        // qDebug() << "PK Debug XIOCamera::readFile() begin ....";
        // qDebug() << "PK Debug XIOCamera::readFile() image_no: " << image_no << ", xio_files size: " << xio_files.size();

        ifname = getFname();
        if (ifname.empty()) {
            if (dev_p.is_open()) {
                dev_p.close();
            }

            if (running.load()) {
                running.store(false);
                emit timeout();
            }
            return; //If we're out of files, give up
        }
        // otherwise check if data is valid
        dev_p.open(ifname, std::ios::in | std::ios::binary);
        if (!dev_p.is_open()) {
            qDebug() << "Could not open file" << ifname.data() << ". Does it exist?";
            dev_p.clear();
            readFile();
            return;
        }

        qDebug() << "PK Debug XIOCamera::readFile() ifname: " << ifname.data();
        // qDebug() << "Successfully opened " << ifname.data();

        dev_p.unsetf(std::ios::skipws);

        dev_p.read(reinterpret_cast<char*>(header.data()), headsize);

        std::streampos filesize(0);
        std::string ext = os::getext(ifname);
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        if (!std::strcmp(ext.data(), "decomp")) {
            dev_p.seekg(0, std::ios::end);
            filesize = dev_p.tellg();
            dev_p.seekg(headsize, std::ios::beg);
        } else {
            // convert the raw hex string to decimal, one digit at a time.
            filesize = int(header[7]) * 16777216 + int(header[6]) * 65536 + int(header[5]) * 256 + int(header[4]);
        }

        framesize = static_cast<size_t>(filesize / nFrames);
        if (framesize == 0) { //If header reports a 0 filesize (invalid data), then skip this file.
            dev_p.close();
            qDebug().nospace() << "Skipped file \"" << ifname.data() << "\" due to invalid data.";
        } else { //otherwise we load it
            validFile = true;
            dev_p.seekg(headsize, std::ios::beg);

            // qDebug() << "File size is" << filesize << "bytes, which corresponds to a framesize of" << framesize << "bytes.";

            std::vector<uint16_t> zero_vec(size_t(frame_width * data_height) - (size_t(framesize) / sizeof(uint16_t)));
            std::fill(zero_vec.begin(), zero_vec.end(), 0);

            std::vector<uint16_t> copy_vec(size_t(framesize), 0);

            for (int n = 0; n < nFrames; ++n) {
                dev_p.read(reinterpret_cast<char*>(copy_vec.data()), std::streamsize(framesize));
                frame_buf.emplace_front(copy_vec);

                if (framesize / sizeof(uint16_t) < size_t(frame_width * data_height)) {
                    std::copy(zero_vec.begin(), zero_vec.end(), frame_buf[size_t(n)].begin() + framesize / sizeof(uint16_t));
                }
            }

            validFileCount++;
            qDebug() << "PK Debug - XIOCamera::readFile() file#" << validFileCount << ": " << ifname.data() << " is loaded";
            running.store(true);
            emit started();
            dev_p.close();
        }
    }

} // end of XIOCamera::readFile()

void XIOCamera::readLoop()
{
    QTime remTime;
    do {
        // Yeah yeah whatever it's a magic buffer size recommendation
        if (frame_buf.size() <= 96) {
            //
            // PK Debug 12-18-20 added
            // qDebug() << "PK Debug XIOCamera::readLoop() calls readFile()";
            readFile();
        } else {
            remTime = QTime::currentTime().addMSecs(tmoutPeriod);
            while(QTime::currentTime() < remTime) {
                QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
            }
        }
    } while (is_reading);
}

uint16_t* XIOCamera::getFrame()
{
    if (!frame_buf.empty() && is_reading) {
        temp_frame = frame_buf.back();
        // prev_frame = &temp_frame;
        frame_buf.pop_back();
        return temp_frame.data();
    } else {
        return dummy.data();
    }
}
