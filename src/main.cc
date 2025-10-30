#include <stdio.h>
#include <memory>
#include <sys/time.h>
#include <fstream>
#include <iostream>

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "rkYolov5s.hpp"
#include "rknnPool.hpp"

using namespace cv;
using namespace std;

int main(int argc, char **argv)
{
    float f;
    float FPS[16];
    int i, Fcnt=0;
    Mat frame;
    chrono::steady_clock::time_point Tbegin, Tend;

    for(i=0;i<16;i++) FPS[i]=0.0;

    // Initialize the rknn thread pool
    int threadNum = 12;

    rknnPool<rkYolov5s, cv::Mat, cv::Mat> testPool("./model/RK3588/yolov5n.rknn", threadNum);

    if (testPool.init() != 0)
    {
        printf("rknnPool init fail!\n");
        return -1;
    }

    for(int z=0;z<12;z++){

        //cv::VideoCapture cap;
        //cap.open("./James.mp4");
        //if (!cap.isOpened()) {
         //   cerr << "ERROR: Unable to open the camera" << endl;
         //   return 0;
        //}
	std::string pipeline =
            "rtspsrc location=rtsp://besquare:Kernel%40123@192.168.0.109:554/stream1 protocols=udp latency=0 "
            "! queue "
            "! decodebin "
            "! queue "
            "! videoconvert "
            "! videoscale "
            "! video/x-raw,width=640,height=640 "
            "! queue "
            "! appsink";

	cv::VideoCapture cap(pipeline, cv::CAP_GSTREAMER);

	if (!cap.isOpened())
	{
    		std::cerr << "ERROR: Unable to open hardware accelerated RTSP stream" << std::endl;
    		return -1;
	}

        struct timeval time;
        gettimeofday(&time, nullptr);
        auto startTime = time.tv_sec * 1000 + time.tv_usec / 1000;

        int frames = 0;
        auto beforeTime = startTime;
        while (cap.isOpened())
        {
            cv::Mat img;
            if (cap.read(img) == false)
                break;
            if (testPool.put(img) != 0)
                break;

            if (frames >= threadNum && testPool.get(img) != 0)
                break;
            cv::imshow("Camera FPS", img);
            if (cv::waitKey(1) == 'q') // Press q to exit
                break;
            frames++;

            if (frames % 120 == 0)
            {
                gettimeofday(&time, nullptr);
                auto currentTime = time.tv_sec * 1000 + time.tv_usec / 1000;
                printf("%f fps/s\n", 120.0 / float(currentTime - beforeTime) * 1000.0);
                beforeTime = currentTime;
            }
        }

        // Clear the thread pool
        while (true)
        {
            cv::Mat img;
            if (testPool.get(img) != 0)
                break;
            cv::imshow("Camera FPS", img);
            if (cv::waitKey(1) == 'q') // Press q to exit
                break;
            frames++;
        }

        gettimeofday(&time, nullptr);
        auto endTime = time.tv_sec * 1000 + time.tv_usec / 1000;

        printf("Average:\t %f fps/s\n", float(frames) / float(endTime - startTime) * 1000.0);
    }
    return 0;
}
