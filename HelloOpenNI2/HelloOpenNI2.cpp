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

		std::vector<openni::VideoStream*> streams;
		streams.push_back( &colorStream );
		streams.push_back( &depthStream );

		cv::Mat colorImage;
		cv::Mat depthImage;

		while ( 1 ) {
            int changedIndex;
            openni::OpenNI::waitForAnyStream( &streams[0], streams.size(), &changedIndex );
            if ( changedIndex == 0 ) {
				openni::VideoFrameRef colorFrame;
                colorStream.readFrame( &colorFrame );
                if ( colorFrame.isValid() ) {
                    colorImage = cv::Mat( colorStream.getVideoMode().getResolutionY(),
                                          colorStream.getVideoMode().getResolutionX(),
                                          CV_8UC3, (char*)colorFrame.getData() );
					cv::cvtColor(colorImage, colorImage, CV_RGB2BGR);
                }
				cv::imshow( "Color Camera", colorImage );
            }
			if( changedIndex == 1 ){
			    openni::VideoFrameRef depthFrame;
                depthStream.readFrame( &depthFrame );
                if ( depthFrame.isValid() ) {
                    depthImage = cv::Mat( colorStream.getVideoMode().getResolutionY(),
                                          colorStream.getVideoMode().getResolutionX(),
                                          CV_16UC1, (char*)depthFrame.getData() );
					depthImage.convertTo( depthImage, CV_8UC1, 255.0/10000);
					cv::cvtColor(depthImage, depthImage, CV_GRAY2BGR);
                }
				
				auto videoMode = colorStream.getVideoMode();
				int centerX = videoMode.getResolutionX() / 2;
				int centerY = videoMode.getResolutionY() / 2;
				int centerIdx = centerX + videoMode.getResolutionX() * centerY;
				
				cv::circle(depthImage, cv::Point(centerX,centerY), 5, CV_RGB(255,0,0), 3, CV_AA);

				short* depth = (short*)depthFrame.getData();

				std::stringstream ss;
                    ss << "Center Point :" << depth[centerIdx];
                    cv::putText( depthImage, ss.str(), cv::Point( 0, 50 ),
                        cv::FONT_HERSHEY_SIMPLEX, 1.0, CV_RGB(255,0,0), 1 );

                cv::imshow( "Depth Camera", depthImage );
 			}
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

