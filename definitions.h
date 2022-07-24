#ifndef DEFINITIONS_H
#define DEFINITIONS_H
#include<iostream>
#include<opencv2/opencv.hpp>
#include<cmath>

void discreteFourierTransform(cv::Mat input_image, cv::Mat& output_image, cv::Mat& transform_image)
{
    //1. The operation speed is fast when the extended image matrix is a multiple of 2, 3 and 5
    int m = cv::getOptimalDFTSize(input_image.rows);
    int n = cv::getOptimalDFTSize(input_image.cols);
    cv::copyMakeBorder(input_image, input_image, 0, m - input_image.rows, 0, n - input_image.cols, cv::BORDER_CONSTANT, cv::Scalar::all(0));

    //2. Create a two channel matrix planes to store the real and imaginary parts of the complex number
    cv::Mat planes[] = { cv::Mat_<float>(input_image), cv::Mat::zeros(input_image.size(), CV_32F) };

    //3. Create a multi-channel array from multiple single channel arrays: transform_image.  The function Merge combines several arrays into a multi-channel array, that is, each element of the output array will be a cascade of the input array elements
    cv::merge(planes, 2, transform_image);

    //4. Fourier transform
    cv::dft(transform_image, transform_image);

    //5. Calculate the amplitude of complex number and save it in output_image (spectrum diagram)
    cv::split(transform_image, planes); // The dual channel is divided into two single channels, one represents the real part and the other represents the imaginary part
    cv::Mat transform_image_real = planes[0];
    cv::Mat transform_image_imag = planes[1];

    cv::magnitude(planes[0], planes[1], output_image); //Calculate the amplitude of the complex number and save it in output_image (spectrum diagram)

    //6. The previous spectrum is too large to display, so it is difficult to convert
    output_image += cv::Scalar(1);   // Before taking logarithm, add 1 to all pixels to prevent log0
    cv::log(output_image, output_image);   // Take logarithm
    cv::normalize(output_image, output_image, 0, 1, cv::NORM_MINMAX); //normalization

    //7. Clipping and redistribution amplitude image limit
    output_image = output_image(cv::Rect(0, 0, output_image.cols & -2, output_image.rows & -2));

    // Rearrange the quadrants in the Fourier image so that the origin is in the center of the image
    int cx = output_image.cols / 2;
    int cy = output_image.rows / 2;
    cv::Mat q0(output_image, cv::Rect(0, 0, cx, cy));   // Upper left area
    cv::Mat q1(output_image, cv::Rect(cx, 0, cx, cy));  // Upper right area
    cv::Mat q2(output_image, cv::Rect(0, cy, cx, cy));  // Lower left area
    cv::Mat q3(output_image, cv::Rect(cx, cy, cx, cy)); // Lower right area

      //Switching quadrant centralization
    cv::Mat tmp;
    q0.copyTo(tmp); q3.copyTo(q0); tmp.copyTo(q3);//Swap top left and bottom right
    q1.copyTo(tmp); q2.copyTo(q1); tmp.copyTo(q2);//Exchange top right and bottom left


    cv::Mat q00(transform_image_real, cv::Rect(0, 0, cx, cy));   // Upper left area
    cv::Mat q01(transform_image_real, cv::Rect(cx, 0, cx, cy));  // Upper right area
    cv::Mat q02(transform_image_real, cv::Rect(0, cy, cx, cy));  // Lower left area
    cv::Mat q03(transform_image_real, cv::Rect(cx, cy, cx, cy)); // Lower right area
    q00.copyTo(tmp); q03.copyTo(q00); tmp.copyTo(q03);//Swap top left and bottom right
    q01.copyTo(tmp); q02.copyTo(q01); tmp.copyTo(q02);//Exchange top right and bottom left

    cv::Mat q10(transform_image_imag, cv::Rect(0, 0, cx, cy));   // Upper left area
    cv::Mat q11(transform_image_imag, cv::Rect(cx, 0, cx, cy));  // Upper right area
    cv::Mat q12(transform_image_imag, cv::Rect(0, cy, cx, cy));  // Lower left area
    cv::Mat q13(transform_image_imag, cv::Rect(cx, cy, cx, cy)); // Lower right area
    q10.copyTo(tmp); q13.copyTo(q10); tmp.copyTo(q13);//Swap top left and bottom right
    q11.copyTo(tmp); q12.copyTo(q11); tmp.copyTo(q12);//Exchange top right and bottom left

    planes[0] = transform_image_real;
    planes[1] = transform_image_imag;
    cv::merge(planes, 2, transform_image);//Centralize Fourier transform results
}

#endif //DEFINITIONS_H
