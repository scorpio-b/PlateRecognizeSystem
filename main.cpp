#include <opencv2/opencv.hpp>
#include <iostream>
#include <filesystem>
#include <chrono>

namespace fs = std::filesystem;

// 自适应显示图像函数
void adaptiveDisplay(const std::string& windowName, const cv::Mat& image, double maxHeight = 800.0) {
    // 计算缩放比例
    double scale = 1.0;
    if (image.rows > maxHeight) {
        scale = maxHeight / image.rows;
    }

    // 计算新尺寸
    cv::Size newSize(static_cast<int>(image.cols * scale),
                    static_cast<int>(image.rows * scale));

    // 缩放图像用于显示
    cv::Mat displayImage;
    cv::resize(image, displayImage, newSize, 0, 0, cv::INTER_AREA);

    // 创建窗口并显示
    cv::namedWindow(windowName, cv::WINDOW_NORMAL);
    cv::imshow(windowName, displayImage);
}

/**
 * @brief 对JPG图像进行二值化处理并保存结果到原始路径
 * @param inputPath 输入的JPG图像路径
 * @param showResult 是否显示处理结果
 * @param useOtsu 是否使用Otsu自动阈值算法 (true-使用, false-使用固定阈值)
 * @param fixedThreshold 固定阈值值 (当useOtsu=false时有效)
 * @return std::string 输出文件路径，空字符串表示失败
 */
std::string binarizeAndSaveJPG(const std::string& inputPath,
                              bool showResult = true,
                              bool useOtsu = true,
                              int fixedThreshold = 128) {
    try {
        // 验证输入文件格式
        fs::path inputFilePath(inputPath);
        if (inputFilePath.extension() != ".jpg" &&
            inputFilePath.extension() != ".jpeg") {
            std::cerr << "错误: 仅支持JPG格式图像，当前文件: "
                      << inputFilePath.extension() << std::endl;
            return "";
        }

        // 检查输入文件是否存在
        if (!fs::exists(inputFilePath)) {
            std::cerr << "错误: 输入文件不存在 - " << inputFilePath << std::endl;
            return "";
        }

        // 创建输出文件名：添加"binary_"前缀
        fs::path outputFilePath = inputFilePath.parent_path() /
                                 ("binary_" + inputFilePath.stem().string() + ".png");

        // 开始计时
        auto startTime = std::chrono::high_resolution_clock::now();

        // 读取图像
        cv::Mat image = cv::imread(inputFilePath.string(), cv::IMREAD_COLOR);
        if (image.empty()) {
            std::cerr << "错误: 无法读取图像 - " << inputFilePath << std::endl;
            return "";
        }

        // 转换为灰度图
        cv::Mat grayImage;
        cv::cvtColor(image, grayImage, cv::COLOR_BGR2GRAY);

        // 二值化处理
        cv::Mat binaryImage;
        double thresholdValue = 0;

        if (useOtsu) {
            // 使用Otsu自动阈值算法
            thresholdValue = cv::threshold(
                grayImage, binaryImage,
                0, 255,
                cv::THRESH_BINARY | cv::THRESH_OTSU
            );
        } else {
            // 使用固定阈值
            thresholdValue = fixedThreshold;
            cv::threshold(
                grayImage, binaryImage,
                fixedThreshold, 255,
                cv::THRESH_BINARY
            );
        }

        // 保存二值化图像
        std::vector<int> compression_params;
        compression_params.push_back(cv::IMWRITE_PNG_COMPRESSION);
        compression_params.push_back(6); // 中等压缩率

        if (!cv::imwrite(outputFilePath.string(), binaryImage, compression_params)) {
            std::cerr << "错误: 无法保存二值化图像 - " << outputFilePath << std::endl;
            return "";
        }

        // 结束计时
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

        // 打印处理信息
        std::cout << "\n================ 图像处理完成 ================\n";
        std::cout << "输入文件: " << fs::absolute(inputFilePath) << "\n";
        std::cout << "输出文件: " << fs::absolute(outputFilePath) << "\n";
        std::cout << "图像尺寸: " << image.cols << "x" << image.rows << "\n";
        std::cout << "使用阈值: " << thresholdValue
                  << (useOtsu ? " (Otsu自动计算)" : " (固定阈值)") << "\n";
        std::cout << "处理时间: " << duration.count() << " 毫秒\n";
        std::cout << "============================================\n";

        // 验证输出文件是否创建成功
        if (!fs::exists(outputFilePath) || fs::file_size(outputFilePath) == 0) {
            std::cerr << "严重错误: 输出文件未创建或为空 - " << outputFilePath << std::endl;
            return "";
        }

        // 显示处理结果
        if (showResult) {
            // 创建彩色版本用于显示（原始图像）
            cv::Mat colorBinary;
            cv::cvtColor(binaryImage, colorBinary, cv::COLOR_GRAY2BGR);

            // 添加处理信息到图像
            std::string thresholdInfo = "阈值: " + std::to_string(static_cast<int>(thresholdValue));
            cv::putText(colorBinary, thresholdInfo,
                        cv::Point(20, 40),
                        cv::FONT_HERSHEY_SIMPLEX,
                        1.0, cv::Scalar(0, 255, 0), 2); // 绿色文本

            std::string sizeInfo = "尺寸: " + std::to_string(binaryImage.cols) +
                                 "x" + std::to_string(binaryImage.rows);
            cv::putText(colorBinary, sizeInfo,
                        cv::Point(20, 80),
                        cv::FONT_HERSHEY_SIMPLEX,
                        0.8, cv::Scalar(0, 200, 255), 2); // 橙色文本

            // 自适应显示
            adaptiveDisplay("原始图像: " + inputFilePath.filename().string(), image);
            adaptiveDisplay("二值化结果: " + inputFilePath.filename().string(), colorBinary);

            // 等待用户按键
            cv::waitKey(0);
            cv::destroyAllWindows();
        }

        return outputFilePath.string();
    } catch (const std::exception& e) {
        std::cerr << "异常错误: " << e.what() << std::endl;
        return "";
    } catch (...) {
        std::cerr << "未知异常错误" << std::endl;
        return "";
    }
}

// 使用示例
int main() {
    // 输入文件路径
    const std::string inputImage = "../pics/WechatIMG25.jpg";

    std::cout << "正在处理图像: " << inputImage << std::endl;

    // 使用Otsu自动阈值处理并显示结果
    std::string outputPath = binarizeAndSaveJPG(inputImage);

    // 使用固定阈值处理但不显示结果
    // std::string outputPath = binarizeAndSaveJPG(inputImage, false, false, 150);

    if (!outputPath.empty()) {
        std::cout << "图像二值化处理成功! 输出文件: "
                  << fs::absolute(outputPath) << std::endl;
    } else {
        std::cerr << "处理失败!" << std::endl;
    }

    return 0;
}