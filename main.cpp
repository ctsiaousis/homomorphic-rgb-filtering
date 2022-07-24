#include<iostream>
#include<opencv2/opencv.hpp>
#include "definitions.h"

#define SHOW_ALL 1

int main()
{
    std::string name = "lena";
    std::string suffix = ".png";
    cv::Mat image, imLAB, image_gray, image_output, image_transform;   //Define input image, gray image and output image
    image = cv::imread(name+suffix);  //Read the image;
    if (image.empty())
    {
        std::cout << "Read error" << std::endl;
        return -1;
    }
    #ifdef SHOW_ALL
    cv::imshow("image", image);
    cv::waitKey(0);
    #endif
    cv::imwrite("Figures/"+name+"_orig"+suffix, image);


    image.convertTo(image, CV_8UC3);
    cv::cvtColor(image, imLAB, cv::COLOR_BGR2Lab);
    std::vector<cv::Mat> imLABchannels;
    cv::split(imLAB, imLABchannels);

    image_gray = imLABchannels.at(0);
    #ifdef SHOW_ALL
    cv::imshow("image_gray", image_gray); //Display grayscale image
    cv::waitKey(0);
    #endif
    cv::imwrite("Figures/"+name+"_gray"+suffix, image_gray);

    //1. Perform ln transformation
    cv::Mat image_gray2(image_gray.size(), CV_32F);
    for (int i = 0; i < image_gray.rows; i++)
    {
        for (int j = 0; j < image_gray.cols; j++)
        {
            image_gray2.at<float>(i, j) = cv::log(image_gray.at<uchar>(i, j) + 0.1);
        }
    }

    //2. Fourier transform, image_output is the displayable spectrum diagram, image_transform is the complex result of Fourier transform
    discreteFourierTransform(image_gray2, image_output, image_transform);
    #ifdef SHOW_ALL
    cv::imshow("image_output", image_output);
    cv::waitKey(0);
    #endif
    cv::imwrite("Figures/"+name+"_output"+suffix, image_output);

    //3. Gaussian high pass filter
    cv::Mat planes[] = { cv::Mat_<float>(image_output), cv::Mat::zeros(image_output.size(),CV_32F) };
    cv::split(image_transform, planes);//Separate channels to obtain real and imaginary parts
    cv::Mat image_transform_real = planes[0];
    cv::Mat image_transform_imag = planes[1];

    int core_x = image_transform_real.rows / 2;//Spectrum center coordinates
    int core_y = image_transform_real.cols / 2;
    int d0 = 10;  //Filter radius
    float h;
    //Parameters:
    float rh = 3;
    float rl = 0.5;
    float c = 5;
    for (int i = 0; i < image_transform_real.rows; i++)
    {
        for (int j = 0; j < image_transform_real.cols; j++)
        {
            h = (rh-rl) * (1 - exp(-c * ((i - core_x) * (i - core_x) + (j - core_y) * (j - core_y)) / (d0 * d0))) + rl;
            image_transform_real.at<float>(i, j) = image_transform_real.at<float>(i, j) * h;
            image_transform_imag.at<float>(i, j) = image_transform_imag.at<float>(i, j) * h;

        }
    }
    planes[0] = image_transform_real;
    planes[1] = image_transform_imag;
    cv::Mat image_transform_ilpf;//Define Gaussian high pass filtering results
    cv::merge(planes, 2, image_transform_ilpf);

    //4. Inverse Fourier transform
    cv::Mat iDft[] = { cv::Mat_<float>(image_output), cv::Mat::zeros(image_output.size(),CV_32F) };
    cv::idft(image_transform_ilpf, image_transform_ilpf);//inverse Fourier transform
    cv::split(image_transform_ilpf, iDft);//Separate channel, mainly obtain channel 0
    cv::magnitude(iDft[0], iDft[1], iDft[0]); //Calculate the amplitude of complex number and save it in iDft[0]
    cv::normalize(iDft[0], iDft[0], 0, 1, cv::NORM_MINMAX);//Normalization processing
    cv::exp(iDft[0], iDft[0]);
    cv::normalize(iDft[0], iDft[0], 0, 1, cv::NORM_MINMAX);//Normalization processing
    iDft[0].convertTo(iDft[0], CV_8U, 255 / 1.0, 0);

    cv::Mat rgb_out;
    imLABchannels.at(0) = iDft[0];
    cv::merge(imLABchannels, imLAB);
    cv::cvtColor(imLAB, rgb_out, cv::COLOR_Lab2BGR);

    cv::imshow("idft", rgb_out);//Display inverse transform image
    cv::imwrite("Figures/"+name+"_idft"+suffix, rgb_out);
    cv::waitKey(0);  //Pause, hold the image display, and wait for the key to end
    return 0;
}
