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

        openni::VideoStream colorStream;
        colorStream.create( device, openni::SensorType::SENSOR_COLOR );
        colorStream.start();

        openni::VideoStream depthStream;
        depthStream.create( device, openni::SensorType::SENSOR_DEPTH );
        depthStream.start();

        // depthÇ∆RGBÇsyncÇ≥ÇπÇÈ
        device.setDepthColorSyncEnabled(true);

        // depthÇ∆rgbÇèdÇÀçáÇÌÇπÇ∑ÇÈ
        if(device.isImageRegistrationModeSupported(openni::IMAGE_REGISTRATION_DEPTH_TO_COLOR))
            device.setImageRegistrationMode(openni::IMAGE_REGISTRATION_DEPTH_TO_COLOR);

        cv::Mat colorImage;
        cv::Mat depthImage;
        cv::Mat depthoutputImage;
        cv::Mat maskImage;
		cv::Mat mixImage;
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
			
			cv::Mat nearMask, zeroMask;
			cv::compare(depthImage, 2000, nearMask, CV_CMP_LT);
			cv::compare(depthImage, 0, zeroMask, CV_CMP_GT);
			cv::bitwise_and(nearMask, zeroMask, nearMask);
			mixImage.setTo(0);
			colorImage.copyTo(mixImage, nearMask);
			cv::imshow("Mix Image", mixImage);

            int key = cv::waitKey( 10 );
			if ( key == 'q'  || key == VK_ESCAPE) {
                break;
            }
        }

        openni::OpenNI::shutdown();
    }
    catch ( std::exception& ) {
        std::cout << openni::OpenNI::getExtendedError() << std::endl;
    }
}

