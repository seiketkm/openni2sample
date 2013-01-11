#include <stdio.h>
#include <iostream>
#include <vector>
#include <OpenNI.h>
#include <opencv2\opencv.hpp>

void main(int argc, char* argv[])
{
    // OpenNI 2.0.0.32 Unstable (Win32)
    try {
        openni::OpenNI::initialize();
        openni::Device device;
        auto ret = device.open( openni::ANY_DEVICE );
        if ( ret != openni::STATUS_OK ) {
            throw std::runtime_error("can't open device");
        }

        // depthÇ∆RGBÇsyncÇ≥ÇπÇÈ
        device.setDepthColorSyncEnabled(true);

        // depthÇ∆rgbÇèdÇÀçáÇÌÇπÇ∑ÇÈê›íËÇÁÇµÇ¢Ç™ìÆÇ¢ÇƒÇ»Ç¢ÅB
        if(device.isImageRegistrationModeSupported(openni::IMAGE_REGISTRATION_DEPTH_TO_COLOR))
            device.setImageRegistrationMode(openni::ImageRegistrationMode::IMAGE_REGISTRATION_DEPTH_TO_COLOR);


        openni::VideoStream colorStream;
        colorStream.create( device, openni::SensorType::SENSOR_COLOR );
        colorStream.start();

        openni::VideoStream depthStream;
        depthStream.create( device, openni::SensorType::SENSOR_DEPTH );
        depthStream.start();

        std::vector<openni::VideoStream*> streams;
        streams.push_back( &colorStream );
        streams.push_back( &depthStream );

        cv::Mat colorImage;
        cv::Mat depthImage;
        cv::Mat depthoutputImage;
        cv::Mat maskImage;

        while ( 1 ) {
            openni::VideoFrameRef colorFrame;
            colorStream.readFrame( &colorFrame );
            if ( colorFrame.isValid() ) {
                colorImage = cv::Mat( colorStream.getVideoMode().getResolutionY(),
                    colorStream.getVideoMode().getResolutionX(),
                    CV_8UC3, (char*)colorFrame.getData() );
                cv::cvtColor(colorImage, colorImage, CV_RGB2BGR);
            }
            cv::imshow( "Color Camera", colorImage );
            openni::VideoFrameRef depthFrame;
            depthStream.readFrame( &depthFrame );
            if ( depthFrame.isValid() ) {
                depthImage = cv::Mat( depthFrame.getVideoMode().getResolutionY(),
                    depthFrame.getVideoMode().getResolutionX(),
                    CV_16UC1, (unsigned short*)depthFrame.getData() );


                depthImage.convertTo( depthoutputImage, CV_8UC1, 255.0/10000);
                cv::cvtColor(depthoutputImage, depthoutputImage, CV_GRAY2BGR);
            }

            cv::imshow( "Depth Camera", depthoutputImage );

            int key = cv::waitKey( 10 );
            if ( key == 'q'  || key == 0x1b) {
                break;
            }
        }

        openni::OpenNI::shutdown();
    }
    catch ( std::exception& ) {
        std::cout << openni::OpenNI::getExtendedError() << std::endl;
    }
}

