#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <iostream>

cv::Mat img;

std::vector<std::vector<int>> points;

std::vector<std::vector<int>> colors = {
        {29, 106, 51, 48, 197, 146},
        {10, 111, 135, 18, 240, 192}
};

std::vector<cv::Scalar> colorsPaint = {
        {0, 255, 0},
        {0, 255, 250}
};


void draw() {
    for(int i = 0 ; i < points.size() ; i++) {
        cv::circle(img, cv::Point(points[i][0], points[i][1]), 20, cv::Scalar(colorsPaint[points[i][2]]), cv::FILLED);
    }
}

cv::Point getContours(cv::Mat imgDil ) {
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(imgDil, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
//    cv::drawContours(img, contours, -1, cv::Scalar(241, 52, 123), 2);
    std::vector<std::vector<cv::Point>> conPoly(contours.size());
    std::vector<cv::Rect> boundRect(contours.size());

    for(int i = 0 ; i < contours.size() ; i++) {
        int area = cv::contourArea(contours[i]);
        std::cout << area << std::endl;

        if(area > 100) {
            float peri = cv::arcLength(contours[i], true);
            cv::approxPolyDP(contours[i], conPoly[i], 0.02 * peri, true);

            boundRect[i] = cv::boundingRect(conPoly[i]);

            cv::drawContours(img, conPoly, i, cv::Scalar(241, 52, 123), 2);
//            cv::rectangle(img, boundRect[i].tl(), boundRect[i].br(), cv::Scalar(255, 0, 0), 3);
//            cv::circle(img, cv::Point(boundRect[i].tl().x + boundRect[i].width/2, boundRect[i].tl().y), 20, cv::Scalar(colorsPaint[0]), cv::FILLED);

            return {boundRect[i].tl().x + boundRect[i].width/2, boundRect[i].tl().y};
        }
    }
}



void findColor(cv::Mat img) {
    cv::Mat imgHSV, mask;
    cv::cvtColor(img, imgHSV, cv::COLOR_BGR2HSV);

    for(int i = 0 ; i < colors.size() ; i++) {
        cv::Scalar lower(colors[i][0], colors[i][1], colors[i][2]);
        cv::Scalar upper(colors[i][3], colors[i][4], colors[i][5]);

        cv::inRange(imgHSV, lower, upper, mask);

        cv::Point p = getContours(mask);
        if(p.x != 0 && p.y != 0)
            points.push_back({p.x, p.y, i});
    }
}


int main() {

    //Kamerka
    cv::VideoCapture cap(0);

    while(true) {
        cap.read(img);
        findColor(img);
        draw();

        cv::imshow("Image", img);
        cv::waitKey(1);
    }

}