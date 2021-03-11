#include <QMutex>     // PK 2-17-21 image-line-debug
#include "lvframe.h"     // PK 2-15-21 image-line-debug
#include "xiocamera.h"

extern bool frameLineDebugLog;   // PK 2-5-21 image-line-debug
extern QMutex frameDataMutex;    // PK 2-17-21 image-line-debug

XIOCamera::XIOCamera(int frWidth,
        int frHeight, int dataHeight,
        QObject *parent
) : CameraModel(parent), nFrames(32), framesize(0),
    headsize(EMIT_FRAME_HEADER_SIZE), image_no(0),
    tmoutPeriod(100) // milliseconds
{
    source_type = XIO;
    camera_type = SSD_XIO;
    frame_width = frWidth;
    frame_height = frHeight;
    data_height = dataHeight;
    is_reading = false;

    // EMITFPIED-331
    frameAcquisitionSuspended = false;

    // PK 1-13-21 added ... Forward button support
    frameAcquisitionCount = 0;

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
} // end of XIOCamera::~XIOCamera()

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
void XIOCamera::suspendFrameAcquisition( bool status )
{
  frameAcquisitionSuspended = status;
  qDebug() << "PK Debug - XIOCamera::suspendFrameAcquisition()";
  qDebug() << "PK Debug - XIOCamera::frameAcquisitionSuspended = " << frameAcquisitionSuspended; 
  
} // end of void XIOCamera::suspendFrameAcquisition()
// EMITFPIED-331


//
// PK 1-13-21 added ... Forward button support
int XIOCamera::getFrameAcquisitionCount( void )
{
    return frameAcquisitionCount;
} // end of XIOCamera::getFrameAcquisitionCount()

void XIOCamera::setFrameAcquisitionCount( int count )
{
    frameAcquisitionCount = count;
    qDebug() << "PK Debug - XIOCamera::setFrameAcquisitionCount() frameAcquisitionCount is set to " << count;
} // end of XIOCamera::setFrameAcquisitionCount()
// PK 1-13-21 added ... Forward button support




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


/********************************************************
 *
 * readFile()
 * 
 *      reads and loads all image frame files found
 *      in the directory.
 * 
 * returns:
 *      true - after all files are loaded.
 *      false otherwise
 * 
 ********************************************************/
bool XIOCamera::readFile()
{
    bool validFile = false;

    //
    // EMITFPIED-331
    // This is where to insert a check for frame display suspension status
    static bool logged = false;
    static int  validFileCount = 0;

    while(!validFile) {

        //
        // PK 1-13-21 added ... Forward button support
        if( frameAcquisitionSuspended == true && frameAcquisitionCount == 0 )
        {
            //
            // if frameAcquisitionSuspended is ON, stop reading new frames.
            if( !logged )
            {
                qDebug() << "frameAcquisitionSuspended is ON, XIOCamera::readFile() do NOTHING";
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
        if (ifname.empty())
        {
            if (dev_p.is_open()) {
                dev_p.close();
            }

            if (running.load()) {
                running.store(false);
                qDebug() << "PK Debug XIOCamera::readFile() no more files, running=false";
                emit timeout();
            }
            // PK 3-6-21 debug-LV-hang
            // returns 'true' after all files are read and loaded.
            return true;
        }

        // otherwise check if data is valid
        dev_p.open(ifname, std::ios::in | std::ios::binary);
        if (!dev_p.is_open())
        {
            qDebug() << "Could not open file" << ifname.data() << ". Does it exist?";
            dev_p.clear();
            readFile();
            // PK 3-6-21 debug-LV-hang
            // return 'false' if there is an error. 
            return false;
        }

        qDebug() << "PK Debug XIOCamera::readFile() ifname: " << ifname.data();
        // qDebug() << "Successfully opened " << ifname.data();

        dev_p.unsetf(std::ios::skipws);

        dev_p.read(reinterpret_cast<char*>(header.data()), headsize);

        
        std::streampos filesize(0);
        std::string ext = os::getext(ifname);
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        if (!std::strcmp(ext.data(), "decomp"))
        {
            dev_p.seekg(0, std::ios::end);
            filesize = dev_p.tellg();
            dev_p.seekg(headsize, std::ios::beg);
        }
        else
        {
            // convert the raw hex string to decimal, one digit at a time.
            filesize = int(header[7]) * 16777216 + int(header[6]) * 65536 + int(header[5]) * 256 + int(header[4]);
        }

        //
        // framesize is actually the size of frame line in bytes.
        framesize = static_cast<size_t>(filesize / nFrames);
        if (framesize == 0)
        { //If header reports a 0 filesize (invalid data), then skip this file.
            dev_p.close();
            qDebug().nospace() << "Skipped file \"" << ifname.data() << "\" due to invalid data.";
        }
        else
        { //otherwise we load it
            validFile = true;
            dev_p.seekg(headsize, std::ios::beg);

            // qDebug() << "File size is" << filesize << "bytes, which corresponds to a framesize of" << framesize << "bytes.";

            std::vector<uint16_t> zero_vec(size_t(frame_width * data_height) - (size_t(framesize) / sizeof(uint16_t)));
            std::fill(zero_vec.begin(), zero_vec.end(), 0);


            std::vector<uint16_t> copy_vec(size_t(framesize), 0);

            frameDataFile newFrame;     // PK 2-15-21 image-line-debug
            struct frameLineData lineData;
            for (int n = 0; n < nFrames; ++n)
            {
                dev_p.read(reinterpret_cast<char*>(copy_vec.data()), std::streamsize(framesize));

                // PK 2-15-21 image-line-control ...
                //
                // initialize imageFileData
                if( !n )
                {
                    newFrame.filename  = ifname.substr(ifname.rfind("/")+1) ;
                    newFrame.frameSize = framesize;
                }

                // PK 2-15-21 line-control-debug ...
                lineData = parseLineData( copy_vec );
                newFrame.lineData.emplace_back( lineData );
                
                if( frameLineDebugLog )
                {
                    qDebug() << "PK Debug - XIOCamera::readFile() frame line #:" << (n+1);

                    qDebug() << "PK Debug - XIOCamera::readFile() debug line header info: ";
                    qDebug() << "\t\tframeline timeStamp:" << lineData.timeStamp;
                    qDebug() << "\t\tframeline lineCount:" << lineData.lineCount;
                    qDebug() << "\t\tframeline dataId:"    << lineData.dataId;

                }

                // ... PK 2-15-21 image-line-debug test code

                
            } // bottom of reading frame lines for loop 

            // PK 2-15-21 image-line-control ...
            //
            // All image lines in the file are read and loaded. Add the
            // new image frame to the list beginning
            frameDataMutex.lock();
            frameDataFileList.emplace_back( newFrame );
            frameDataMutex.unlock();
            qDebug() << "PK Debug - XIOCamera::readFile() newFrame added, release frameDataMutex";

            validFileCount++;
            qDebug() << "PK Debug - XIOCamera::readFile() file#" << validFileCount << ": " << ifname.data() << " is loaded";
            running.store(true);
            emit started();
            dev_p.close();
        }

        //
        // PK 1-13-21 ... added Forward button support
        //
        // After ONE frame was loaded, reset frameAcquisition count to 0
        if( getFrameAcquisitionCount() != 0 )
            setFrameAcquisitionCount( 0 );
    } // bottom of the while(!validFile) loop

} // end of XIOCamera::readFile()

void XIOCamera::readLoop()
{
    bool done = false;   // PK 3-6-21 debug-LV-hang

    QTime remTime;
    do {
        // Yeah yeah whatever it's a magic buffer size recommendation

        //
        // 3-2-21 image-line-debug
        //        increase frame_buf size limit ...
        if (frame_buf.size() <= 96) {
            //
            // PK 3-6-21 debug-LV-hang
            //
            // If all files are read and loaded, readLoop() should exit too
            done = readFile();
            if( !done )
                continue;
            else
                break;

        } else {
            qDebug() << "PK Debug XIOCamera::readLoop() frame_buf.size() exceeds 96";
            remTime = QTime::currentTime().addMSecs(tmoutPeriod);
            while(QTime::currentTime() < remTime) {
                QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
            }
        }
    } while (is_reading);
}


void XIOCamera::dumpFrameFileData( frameDataFile *f )
{
    std::vector <frameLineData> line = f->lineData;
    qDebug() << "PK Debug - dumpFrameFileData starts ...\n";
    qDebug() << "\tfilename:" << f->filename.data() << "\n";
    int lineId = 1;
    for( frameLineData l : line )
    {
        qDebug() << "\tLine #:" << lineId++;
        qDebug() << "\ttimeStamp:" << l.timeStamp;
        qDebug() << "\tlineCount:" << l.lineCount;
        qDebug() << "\tdataId:"    << l.dataId;
    }

    qDebug() << "PK Debug - dumpFrameFileData ends ...\n";
} // end of dumpFrameFileData()



/***************************************************************
 *
 * XIOCamera::getFrame() - 
 *
 *      This is invoked by frameworker thread to retrieve ONE
 *      frame line data.
 * 
 **************************************************************/
uint16_t* XIOCamera::getFrame()
{
    // PK 1-27-21 image-line-debug
    static int count = 0;

    // qDebug() << "PK Debug - XIOCamera::getFrame() frameDataFileList size:" << frameDataFileList.size();

    // if (!frame_buf.empty() && is_reading)
    if( !frameDataFileList.empty() && is_reading )  // PK 2-17-21 image-line-debug
    {
        //
        // get and lock the Mutex ... before accessing 
        // shared data 'frameDataFileList'
        frameDataMutex.lock();
        
        // PK 2-15-21 image-line-debug
        frameData = frameDataFileList.back();

        count++;
        qDebug() << "PK Debug - XIOCamera::getFrame() called #" << count;
        if( frameLineDebugLog )
        {
            // PK 2-15-21 image-line-debug ...

            if( !frameDataFileList.empty() )
            {
                //
                // Dump frame data ... 
                // dumpFrameFileData( &(frameDataFileList.back()) );
                dumpFrameFileData( &frameData );
            }

            // ... PK 2-15-21 image-line-debug
        }

        // remove the element from the list
        frameDataFileList.pop_back();

        //
        // release the mutex lock to share data frameDataFileList.
        frameDataMutex.unlock();
        qDebug() << "PK Debug - XIOCamera::getFrame() before returning the data";        

#ifdef ORIGINAL_CODE        

        return temp_frame.data();

#else // NEW_CODE PK-2-15-21 ...


        return( (uint16_t*) reinterpret_cast<frameDataFile *> (&frameData) );

#endif // ... NEW_CODE PK-2-15-21
    }
    else
    {

#ifdef ORIGINAL_CODE        

        return dummy.data();

#else // NEW_CODE PK 2-3-21 image-line-debug

        //
        // PK 2-3-21 image-line-debug
        // return NULL indicating no data
        return NULL;

#endif // NEW_CODE PK 2-3-21 image-line-debug
    }
} // end of XIOCamera::getFrame()



//
// This function parses the image frame line for line header info.
// and returns a imageLineData structure.
//
// Header info. includes timeStamp, lineCount, and data
// collection id (dataId).
//
// Image frame line header specification is documented in
// EMIT_NGIS_DataDictionaries_FrameHeader_20200712.xlsx spreadsheet
// under 'LineHeaderSpec&Impl FPAROIC' tab.
//
frameLineData XIOCamera::parseLineData( std::vector<uint16_t> line )
{
    frameLineData header;
    char buf[16], *p = buf;

    //
    // Extract line header info. from the leading 16-byte of image line 
    memcpy( (void *) buf,
            (void *) reinterpret_cast<char*>(line.data()), sizeof(buf) );

    //
    // Fill in the image line data header info.
    memcpy( (void *) &header.timeStamp, (void *) p,      sizeof(uint32_t) );
    memcpy( (void *) &header.lineCount, (void *) (p+4),  sizeof(uint32_t) );
    memcpy( (void *) &header.dataId,    (void *) (p+12), sizeof(uint16_t) );

    header.data = line;
    return header;

} // end of XIOCamera::parseLineData()

