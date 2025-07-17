#include <opencv2/opencv.hpp>
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

int main() {
    cv::Mat image = cv::imread("../pics/WechatIMG25.jpg");
    if (image.empty()) {
        std::cerr << "Error: Image not found!" << std::endl;
        return -1;
    }
    std::cout << "Find the image." << std::endl;
    cv::imshow("Display", image);
    cv::waitKey(0);
    return 0;
}