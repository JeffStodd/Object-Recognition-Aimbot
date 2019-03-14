#define NOMINMAX
#include "yolo_v2_class.hpp"
#include <opencv2/opencv.hpp>    
#include "opencv2/core/version.hpp"
#include <Windows.h>
#include <cuda.h>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <thread>

int nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
int nScreenHeight = GetSystemMetrics(SM_CYSCREEN);
int xOffset = 0;

bbox_t lastTarget;

clock_t targetLocked;
clock_t lockDuration;

//30 - 70 ms difference
void MouseInput()
{
    INPUT Input = { 0 };
    Input.type = INPUT_MOUSE;


    Input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;

    SendInput(1, &Input, sizeof(INPUT));

    Input.mi.dwFlags = MOUSEEVENTF_LEFTUP;

    float r = (float)rand() / (float)RAND_MAX;
    Sleep(40 * r + 30);

    SendInput(1, &Input, sizeof(INPUT));
}

void aim(int x, int y, int speed)
{
    //std::cout << (double)(targetLocked) / CLOCKS_PER_SEC << std::endl;
    //if (((double)(targetLocked)/CLOCKS_PER_SEC < 0.5) & (abs((int)(lastTarget.x - x)) > 50 || (abs((int)lastTarget.y - y) > 50)))
    //    return;
    //targetLocked = clock();
    //lastTarget.x = x;
    //lastTarget.y = y;
    //std::cout << "Target: " << x - 1900/2 << " " << y-1080/2 << std::endl;
    float currX = nScreenWidth / 2;
    float currY = nScreenHeight / 2;

    INPUT in;
    in.type = INPUT_MOUSE;
    in.mi.dwFlags = (MOUSE_MOVED);

    in.mi.mouseData = 0;
    in.mi.time = 0;
    in.mi.dwExtraInfo = 0;
    in.mi.dx = x - currX;
    in.mi.dy = y - currY;

    SendInput(1, &in, sizeof(in));
    //aimer(x, y, speed, 25);
}



void drawBounds(HDC deviceContext, HBRUSH brush)
{
    RECT frame;
    frame.left = 1920 / 2 - 316 / 2 + xOffset;
    frame.top = 1080 / 2 - 316 / 2;
    frame.right = 1920 / 2 - 316 / 2 + 316 + xOffset;
    frame.bottom = 1080 / 2 - 316 / 2 + 316;
    RECT triggerbot;
    triggerbot.top = 1080 / 2 - 10;
    triggerbot.left = 1920 / 2 - 10;
    triggerbot.right = 1920 / 2 + 10;
    triggerbot.bottom = 1080 / 2 + 10;
    while (true)
    {
        FrameRect(deviceContext, &frame, brush);
        //FrameRect(deviceContext, &triggerbot, brush);
        Sleep(17);
    }
}

void drawBox(RECT frame, HDC deviceContext, HBRUSH brush)
{
    int i = 0;
    while (i++ < 61)
    {
        FrameRect(deviceContext, &frame, brush);
        Sleep(17);
    }
}


int main()
{
    //tiny faster but more inaccurate
    //use regular yolov3 for accuracy
    std::string  names_file = "C:/darknet/build/darknet/x64/data/coco.names";

    std::string  cfg_file = "C:/darknet/build/darknet/x64/cfg/yolov3-tiny.cfg";
    std::string  weights_file = "C:/darknet/build/darknet/x64/yolov3-tiny.weights";

    //std::string  cfg_file = "C:/darknet/build/darknet/x64/cfg/yolov3.cfg";
    //std::string  weights_file = "C:/darknet/build/darknet/x64/yolov3.weights";

    //std::string  cfg_file = "C:/darknet/build/darknet/x64/net.cfg";
    //std::string  weights_file = "C:/darknet/build/darknet/x64/net.weights";

    std::ifstream file(names_file);
    std::vector<std::string> file_lines;
    if (file.is_open())
        for (std::string line; file >> line;) file_lines.push_back(line);
    std::cout << "object names loaded \n";
   // for (int i = 0; i < file_lines.size(); i++)
       // std::cout << file_lines[i];

    Detector detector(cfg_file, weights_file,0);

    std::string windowName = " ";//"FortniteClient - Win64 - Shipping.exe";

    HWND window;
    std::cout << (window = FindWindowA(NULL, "FortniteClient-Win64-Shipping.exe")) << std::endl;//GetDesktopWindow();
    HDC deviceContext = GetDC(window);
    HDC MemDC = CreateCompatibleDC(deviceContext);
    BringWindowToTop(window);

    //RECT dimensions;
    //GetWindowRect(window, &dimensions);
    //dimensions = RECT{ 0, 0, 800, 600 };

    //std::cout << dimensions.bottom - dimensions.top;


    //316
    HBITMAP hCaptureBitmap = CreateCompatibleBitmap(deviceContext, 316, 316);
    SelectObject(MemDC, hCaptureBitmap);

    //RGBQUAD *pPixels = new RGBQUAD[416 * 416];

    BITMAPINFO bmi = { 0 };
    bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
    bmi.bmiHeader.biWidth = 316;
    bmi.bmiHeader.biHeight = -316;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    //using namespace cuda;
    //std::cout << cv::getBuildInformation() << std::endl;
    //GpuMat mat;
    //mat.create(nScreenHeight, nScreenWidth, CV_8UC4);

    cv::Mat imgMat;
    imgMat.create(316, 316, CV_8UC4);

    //char windowName[] = "Test";
    //namedWindow(windowName, WINDOW_NORMAL);

    clock_t deltaTime = 0;
    unsigned int frames = 0;

    HBRUSH brush = CreateSolidBrush(RGB(0, 255, 255));
    //HBRUSH eraser = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
    //SetBkMode(deviceContext, TRANSPARENT);
    cv::Scalar color(60, 160, 260);
    RECT temp;
    

    int topLeftX = 1920/2 - 316/2 + xOffset;
    int topLeftY = 1080/2 - 316/2;

    std::thread (drawBounds,deviceContext,brush).detach();

    RECT triggerbot;
    triggerbot.top = 1920 / 2 - 10;
    triggerbot.bottom = 1080 / 2 + 10;
    triggerbot.left = 1920 / 2 - 10 + xOffset;
    triggerbot.right = 1920 / 2 + 10 + xOffset;

    bbox_t prev;
    prev.x = -1;
    prev.y = -1;

    //targetLocked = clock();
    while (true)
    {
        //std::cout << (double)(targetLocked) / CLOCKS_PER_SEC << std::endl;
        clock_t beginFrame = clock();
        frames++;

        BitBlt(
            MemDC,
            0, //upper left x dest
            0, //upper left y dest
            316, //width
            316, //height
            deviceContext,
            topLeftX, //upper left x src
            topLeftY, //upper left y src
            SRCCOPY
        );

        GetDIBits(
            MemDC,
            hCaptureBitmap,
            0, //start
            316, //lines
            imgMat.data,
            &bmi,
            DIB_RGB_COLORS
        );

        //flip(imgMat, imgMat, 0);
        
        std::vector<bbox_t> result_vec = detector.detect(imgMat);
        std::vector<bbox_t> people;
        std::vector<int> area;
        
        int index = 0;
        //bool found = false;
        //int min = 224;

        
        for (auto &i : result_vec) {
            if (prev.x == i.x & prev.y == i.y)
            {
                index++;
                continue;
            }
            else if (file_lines.size() > i.obj_id)
            {
                if (file_lines[i.obj_id] == "person")
                {
                    people.push_back(i);

                    temp.top = result_vec[index].y + topLeftY;
                    temp.left = result_vec[index].x + topLeftX;
                    temp.right = temp.left + result_vec[index].w;
                    temp.bottom = temp.top + result_vec[index].h;


                    std::thread(FrameRect, deviceContext, &temp, brush).join();//.detach();

                    //std::cout << temp.top << " " << triggerbot.top << " " << temp.bottom << " " << triggerbot.bottom << " " << temp.left << " " << triggerbot.left << " " << temp.right << " " << triggerbot.right;
                    //std::cout << std::endl;

                    /*
                    if (temp.top <= triggerbot.top & temp.bottom >= triggerbot.bottom & temp.left <= triggerbot.left & temp.right >= triggerbot.right)
                    {
                        std::thread(MouseInput).detach();
                        break;
                    }
                    */
                    area.push_back((temp.right -temp.left) * (temp.bottom-temp.top));

                    //std::cout << index << ": " << temp.top << " " << temp.left << std::endl;

                    //rectangle(imgMat, cv::Rect(i.x, i.y, i.w, i.h), color, 3);
                    //putText(imgMat, file_lines[i.obj_id], cv::Point2f(i.x, i.y - 10), cv::FONT_HERSHEY_COMPLEX_SMALL, 1, color);
                    //if (i.track_id > 0)
                        //putText(imgMat, std::to_string(i.track_id), cv::Point2f(i.x + 5, i.y + 15), cv::FONT_HERSHEY_COMPLEX_SMALL, 1, color);
                }
            }
            prev = i;
            index++;
        }

        
        if (people.size() > 0)
        {
            int max = area[0];
            int index = 0;
            for (int i = 0; i < area.size(); i++)
            {
                if (area[i] > max)
                {
                    max = area[i];
                    index = i;
                }
                i++;
            }
            bbox_t instance = people[index];
            //VK_RBUTTON
            //if (GetAsyncKeyState(0x56) & 0x8000)
            //{
                //std::thread(aim, instance.x + instance.w / 2 + topLeftX, instance.y + instance.h / 2 + topLeftY).detach();
           // }
            //int diff = sqrt( pow(abs(lastTarget.x - instance.x),2) + pow(abs(lastTarget.x - instance.y),2));
            //lockDuration = clock();
            aim(instance.x + instance.w / 2 + topLeftX, instance.y + instance.h / 2 + topLeftY - (instance.h/2*0.8), 2);
            //lastTarget = instance;
        }
        

        clock_t endFrame = clock();
        deltaTime += endFrame - beginFrame;

        if (deltaTime / (double)(CLOCKS_PER_SEC) > 1)
        {
            std::cout << "\n\nFPS: " << frames << std::endl << std::endl;
            deltaTime = 0;
            frames = 0;
        }

        //imshow(windowName, imgMat);
        //waitKey(1);
    }
}
