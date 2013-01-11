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

		// Colorカメラの情報を取得
		const auto colorInfo = device.getSensorInfo(openni::SensorType::SENSOR_COLOR);
		const openni::Array<openni::VideoMode> &colorVideoMode = colorInfo->getSupportedVideoModes();
		
		std::stringstream ss;
		char tmp[256];

		ss << " width,height,   fps,format:" << std::endl;
		for(int i=0; i<colorVideoMode.getSize(); i++)
		{
			sprintf(tmp, "%6d,%6d,%6d,%6d",
			colorVideoMode[i].getResolutionX(),
			colorVideoMode[i].getResolutionY(),
			colorVideoMode[i].getFps(),
			colorVideoMode[i].getPixelFormat());
			ss << tmp <<std::endl;
		}
		// Depthカメラの情報を取得
		auto depthInfo = device.getSensorInfo(openni::SensorType::SENSOR_DEPTH);
		const openni::Array<openni::VideoMode> &depthVideoMode = depthInfo->getSupportedVideoModes();
		for(int i=0; i<depthVideoMode.getSize(); i++)
		{
			sprintf(tmp, "%6d,%6d,%6d,%6d",
			depthVideoMode[i].getResolutionX(),
			depthVideoMode[i].getResolutionY(),
			depthVideoMode[i].getFps(),
			depthVideoMode[i].getPixelFormat());
			ss << tmp <<std::endl;
		}
		std::cout << ss.str();

        // depthとRGBをsyncさせる
        device.setDepthColorSyncEnabled(true);

        // depthとrgbを重ね合わせする設定らしいが動いてない。
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

