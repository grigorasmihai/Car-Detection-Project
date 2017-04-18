// OpenCVApplication.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "common.h"
#include <queue>
#include <random>
#include <algorithm>
#include <iostream>
#include <stdio.h>

using namespace std;


void testOpenImage()
{
	char fname[MAX_PATH];
	while(openFileDlg(fname))
	{
		Mat src;
		src = imread(fname);
		imshow("image",src);
		waitKey();
	}
}

void testOpenImagesFld()
{
	char folderName[MAX_PATH];
	if (openFolderDlg(folderName)==0)
		return;
	char fname[MAX_PATH];
	FileGetter fg(folderName,"bmp");
	while(fg.getNextAbsFile(fname))
	{
		Mat src;
		src = imread(fname);
		imshow(fg.getFoundFileName(),src);
		if (waitKey()==27) //ESC pressed
			break;
	}
}


void testResize()
{
	char fname[MAX_PATH];
	while(openFileDlg(fname))
	{
		Mat src;
		src = imread(fname);
		Mat dst1,dst2;
		//without interpolation
		resizeImg(src,dst1,320,false);
		//with interpolation
		resizeImg(src,dst2,320,true);
		imshow("input image",src);
		imshow("resized image (without interpolation)",dst1);
		imshow("resized image (with interpolation)",dst2);
		waitKey();
	}
}


void testVideoSequence()
{
	VideoCapture cap("Videos/rubic.avi"); // off-line video from file
	//VideoCapture cap(0);	// live video from web cam
	if (!cap.isOpened()) {
		printf("Cannot open video capture device.\n");
		waitKey(0);
		return;
	}
		
	Mat edges;
	Mat frame;
	char c;

	while (cap.read(frame))
	{
		Mat grayFrame;
		cvtColor(frame, grayFrame, CV_BGR2GRAY);
		imshow("source", frame);
		imshow("gray", grayFrame);
		c = cvWaitKey(0);  // waits a key press to advance to the next frame
		if (c == 27) {
			// press ESC to exit
			printf("ESC pressed - capture finished\n"); 
			break;  //ESC pressed
		};
	}
}


void testSnap()
{
	VideoCapture cap(0); // open the deafult camera (i.e. the built in web cam)
	if (!cap.isOpened()) // openenig the video device failed
	{
		printf("Cannot open video capture device.\n");
		return;
	}

	Mat frame;
	char numberStr[256];
	char fileName[256];
	
	// video resolution
	Size capS = Size((int)cap.get(CV_CAP_PROP_FRAME_WIDTH),
		(int)cap.get(CV_CAP_PROP_FRAME_HEIGHT));

	// Display window
	const char* WIN_SRC = "Src"; //window for the source frame
	namedWindow(WIN_SRC, CV_WINDOW_AUTOSIZE);
	cvMoveWindow(WIN_SRC, 0, 0);

	const char* WIN_DST = "Snapped"; //window for showing the snapped frame
	namedWindow(WIN_DST, CV_WINDOW_AUTOSIZE);
	cvMoveWindow(WIN_DST, capS.width + 10, 0);

	char c;
	int frameNum = -1;
	int frameCount = 0;

	for (;;)
	{
		cap >> frame; // get a new frame from camera
		if (frame.empty())
		{
			printf("End of the video file\n");
			break;
		}

		++frameNum;
		
		imshow(WIN_SRC, frame);

		c = cvWaitKey(10);  // waits a key press to advance to the next frame
		if (c == 27) {
			// press ESC to exit
			printf("ESC pressed - capture finished");
			break;  //ESC pressed
		}
		if (c == 115){ //'s' pressed - snapp the image to a file
			frameCount++;
			fileName[0] = NULL;
			sprintf(numberStr, "%d", frameCount);
			strcat(fileName, "Images/A");
			strcat(fileName, numberStr);
			strcat(fileName, ".bmp");
			bool bSuccess = imwrite(fileName, frame);
			if (!bSuccess) 
			{
				printf("Error writing the snapped image\n");
			}
			else
				imshow(WIN_DST, frame);
		}
	}

}

void MyCallBackFunc(int event, int x, int y, int flags, void* param)
{
	//More examples: http://opencvexamples.blogspot.com/2014/01/detect-mouse-clicks-and-moves-on-image.html
	Mat* src = (Mat*)param;
	if (event == CV_EVENT_LBUTTONDOWN)
		{
			printf("Pos(x,y): %d,%d  Color(RGB): %d,%d,%d\n",
				x, y,
				(int)(*src).at<Vec3b>(y, x)[2],
				(int)(*src).at<Vec3b>(y, x)[1],
				(int)(*src).at<Vec3b>(y, x)[0]);
		}
}

void testMouseClick()
{
	Mat src;
	// Read image from file 
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		src = imread(fname);
		//Create a window
		namedWindow("My Window", 1);

		//set the callback function for any mouse event
		setMouseCallback("My Window", MyCallBackFunc, &src);

		//show the image
		imshow("My Window", src);

		// Wait until user press some key
		waitKey(0);
	}
}

/* Histogram display function - display a histogram using bars (simlilar to L3 / PI)
Input:
name - destination (output) window name
hist - pointer to the vector containing the histogram values
hist_cols - no. of bins (elements) in the histogram = histogram image width
hist_height - height of the histogram image
Call example:
showHistogram ("MyHist", hist_dir, 255, 200);
*/
void showHistogram(const std::string& name, int* hist, const int  hist_cols, const int hist_height)
{
	Mat imgHist(hist_height, hist_cols, CV_8UC3, CV_RGB(255, 255, 255)); // constructs a white image

	//computes histogram maximum
	int max_hist = 0;
	for (int i = 0; i<hist_cols; i++)
	if (hist[i] > max_hist)
		max_hist = hist[i];
	double scale = 1.0;
	scale = (double)hist_height / max_hist;
	int baseline = hist_height - 1;

	for (int x = 0; x < hist_cols; x++) {
		Point p1 = Point(x, baseline);
		Point p2 = Point(x, baseline - cvRound(hist[x] * scale));
		line(imgHist, p1, p2, CV_RGB(255, 0, 255)); // histogram bins colored in magenta
	}

	imshow(name, imgHist);
}

void negative_image(){
	Mat img = imread("Images/cameraman.bmp",
		CV_LOAD_IMAGE_GRAYSCALE);
	for (int i = 0; i < img.rows; i++){
		for (int j = 0; j < img.cols; j++){
			img.at<uchar>(i, j) = 255 - img.at<uchar>(i, j);
		}
	}
	imshow("negative image", img);
	waitKey(0);
}

unsigned char const_grey = 100;
unsigned char const_grey_fact = 2;

void change_gray_lvl()
{
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		Mat src, src_org;
		src = imread(fname, CV_LOAD_IMAGE_GRAYSCALE);
	    src_org = imread(fname, CV_LOAD_IMAGE_GRAYSCALE);
		for (int i = 0; i < src.rows; i++)
			for (int j = 0; j < src.cols; j++)
			{
				src.at<uchar>(i, j) = min(src.at<uchar>(i, j) + const_grey, 255);
			}
		imshow("image", src);
		imshow("original image", src_org);
		waitKey();
	}
}

void change_gray_lvl_fact()
{
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		Mat src, src_org;
		src = imread(fname, CV_LOAD_IMAGE_GRAYSCALE);
		src_org = imread(fname, CV_LOAD_IMAGE_GRAYSCALE);
		for (int i = 0; i < src.rows; i++)
			for (int j = 0; j < src.cols; j++)
			{
				src.at<uchar>(i, j) = min(max(0, src.at<uchar>(i, j) * const_grey_fact), 255);
			}
		imshow("image", src);
		imwrite("newimg", src);
		imshow("original image", src_org);
		waitKey();
	}
}

void create_img()
{
	Mat src(256, 256, CV_8UC3);
	for (int i = 0; i < src.rows / 2; i++)
		for (int j = 0; j < src.cols / 2; j++)
		{
			Vec3b pixel = src.at<Vec3b>(i, j);
			src.at<Vec3b>(i, j)[0] = 255;
			src.at<Vec3b>(i, j)[1] = 255;
			src.at<Vec3b>(i, j)[2] = 255;
		}

	for (int i = src.rows / 2; i < src.rows; i++)
		for (int j = src.cols / 2; j < src.cols; j++)
		{
			Vec3b pixel = src.at<Vec3b>(i, j);
			src.at<Vec3b>(i, j)[0] = 0;
			src.at<Vec3b>(i, j)[1] = 255;
			src.at<Vec3b>(i, j)[2] = 255;
		}

	for (int i = src.rows / 2; i < src.rows; i++)
		for (int j = 0; j < src.cols / 2; j++)
		{
			Vec3b pixel = src.at<Vec3b>(i, j);
			src.at<Vec3b>(i, j)[0] = 0;
			src.at<Vec3b>(i, j)[1] = 0;
			src.at<Vec3b>(i, j)[2] = 255;
		}

	for (int i = 0; i < src.rows / 2; i++)
		for (int j = src.cols / 2; j < src.cols; j++)
		{
			Vec3b pixel = src.at<Vec3b>(i, j);
			src.at<Vec3b>(i, j)[0] = 0;
			src.at<Vec3b>(i, j)[1] = 255;
			src.at<Vec3b>(i, j)[2] = 0;
		}
	imshow("img", src);
	waitKey(0);
}

void img_c()
{
	int R = 50;
	int ic, jc;
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		Mat src;
		src = imread(fname);
		ic = src.rows / 2;
		jc = src.cols / 2;

		for (int i = 0; i < src.rows; i++)
		{
			for (int j = 0; j < src.cols; j++)
			{
				if (pow(i - ic, 2) + pow(j - jc, 2) > pow(R, 2)){
					src.at<Vec3b>(i,j)[0] = 255;
					src.at<Vec3b>(i, j)[1] = 255;
					src.at<Vec3b>(i, j)[2] = 255;
				}
			}
		}


		imshow("image", src);
		waitKey();
	}
}

void init_mat(Mat src)
{
	for (int i = 0; i < src.rows; i++)
	{
		for (int j = 0; j < src.cols; j++)
		{
			src.at<Vec3b>(i, j)[0] = 0;
			src.at<Vec3b>(i, j)[1] = 0;
			src.at<Vec3b>(i, j)[2] = 0;
		}
	}
}

void separete_colours()
{
	char fname[MAX_PATH];
	if (openFileDlg(fname))
	{
		Mat src;
		src = imread(fname);
		Mat srcRed(src.rows, src.cols, CV_8UC1);
		Mat srcGreen(src.rows, src.cols, CV_8UC1);
		Mat srcBlue(src.rows, src.cols, CV_8UC1);
		for (int i = 0; i < src.rows; i++)
		{
			for (int j = 0; j < src.cols; j++)
			{
				srcRed.at<uchar>(i, j) = src.at<Vec3b>(i, j)[2];
				srcGreen.at<uchar>(i, j) = src.at<Vec3b>(i, j)[1];
				srcBlue.at<uchar>(i, j) = src.at<Vec3b>(i, j)[0];
			}
		}
		imshow("imageRed", srcRed);
		imshow("imageBlue", srcBlue);
		imshow("imageGreen", srcGreen);
		waitKey();
	}
}

void convert_GreyScale()
{
	char fname[MAX_PATH];
	if (openFileDlg(fname))
	{
		Mat src;
		src = imread(fname);
		Mat srcGrey(src.rows, src.cols, CV_8UC1);
		for (int i = 0; i < src.rows; i++)
		{
			for (int j = 0; j < src.cols; j++)
			{
				srcGrey.at<uchar>(i, j) = (src.at<Vec3b>(i, j)[2] + src.at<Vec3b>(i, j)[1] + src.at<Vec3b>(i, j)[0]) / 3;
			}
		}
		imshow("image", src);
		imshow("imageGrey", srcGrey);
		waitKey();
	}
}

void grey_to_whiteBlack()
{
	char fname[MAX_PATH];
	if (openFileDlg(fname))
	{
		Mat src;
		int threshold;
		src = imread(fname, CV_LOAD_IMAGE_GRAYSCALE);
		Mat srcWhite(src.rows, src.cols, CV_8UC1);
		printf("give threshold: ");
		scanf("%d", &threshold);
		for (int i = 0; i < src.rows; i++)
		{
			for (int j = 0; j < src.cols; j++)
			{
				if (src.at<uchar>(i, j) < threshold)
				{
					srcWhite.at<uchar>(i, j) = 0;
				}
				else
				{
					srcWhite.at<uchar>(i, j) = 255;
				}
			}
		}
		imshow("White", srcWhite);
		imshow("Original", src);
		waitKey();

	}
}

void separete_hsv()
{
	char fname[MAX_PATH];
	if (openFileDlg(fname))
	{
		Mat src;
		src = imread(fname);
		Mat srcH(src.rows, src.cols, CV_8UC1);
		Mat srcS(src.rows, src.cols, CV_8UC1);
		Mat srcV(src.rows, src.cols, CV_8UC1);
		for (int i = 0; i < src.rows; i++)
		{
			for (int j = 0; j < src.cols; j++)
			{
				Vec3b pixel1 = src.at<Vec3b>(i, j);
				float pixel[3];
				for (int k = 0; k < 3; k++)
				{
					pixel[k] = (float)pixel1[k] / 255;
				}
				float M = max(pixel[2], pixel[1], pixel[0]);
				float m = min(pixel[2], pixel[1], pixel[0]);
				float C = M - m;

				float V = M;
				float S;
				if (C)
				{
					S = C / V;
				}
				else
				{
					S = 0;
				}
				float H;
				if (C)
				{
					if (M == pixel[2]) H = 60 * (pixel[1] - pixel[0]) / C;
					if (M == pixel[1]) H = 120 + 60 * (pixel[0] - pixel[2]) / C;
					if (M == pixel[0]) H = 240 + 60 * (pixel[2] - pixel[1]) / C;
				}
				else
				{
					H = 0;
				}

				if (H < 0)
				{
					H += 360;
				}

				uchar H_norm = (uchar)(H * 255 / 360);
				uchar S_norm = (uchar)(S * 255);
				uchar V_norm = (uchar)(V * 255);

				srcH.at<uchar>(i, j) = H_norm;
				srcS.at<uchar>(i, j) = S_norm;
				srcV.at<uchar>(i, j) = V_norm;
			}
		}
		imshow("imageH", srcH);
		imshow("imageS", srcS);
		imshow("imageV", srcV);
		waitKey();
	}
}

void calculate_histogram()
{
    char fname[MAX_PATH];
    if (openFileDlg(fname))
    {
        Mat src;
        src = imread(fname);
        int histogram[256];
        for (int i = 0; i < 256; i++)
        {
            histogram[i] = 0;
        }
        for (int i = 0; i < src.rows; i++)
        {
            for (int j = 0; j < src.cols; j++)
            {
                histogram[src.at<uchar>(i, j)] ++;
            }
        }
        showHistogram("Histogram", histogram, 256, 300);
        waitKey();
    }
}

void multilevel_thresholding()
{
    char fname[MAX_PATH];
    if (openFileDlg(fname))
    {
        Mat src;
        src = imread(fname, CV_LOAD_IMAGE_GRAYSCALE);
        int histogram[256];
        float PDF[256];
        int WH = 5;
        float TH = 0.0003;
        for (int i = 0; i < 256; i++)
        {
            histogram[i] = 0;
        }

        for (int i = 0; i < src.rows; i++)
        {
            for (int j = 0; j < src.cols; j++)
            {
                histogram[src.at<uchar>(i, j)] ++;
            }
        }

        for (int i = 0; i < 256; i++)
        {
            PDF[i] = histogram[i] / (float)(src.rows * src.cols);
        }

        std::vector<uchar> maxima;
        maxima.push_back(0);
        for (int i = WH; i < 256 - WH; i++)
        {
            float average = 0;
            float maxim = -1;
            for (int j = i - WH; j <= i + WH; j++)
            {
                average += PDF[j];
                if (PDF[j] > maxim)
                {
                    maxim = PDF[j];
                }

            }
            average = average / (float)(2 * WH + 1);

            if (PDF[i] > (average + TH) && PDF[i] >= maxim)
            {
                maxima.push_back(i);
            }
        }
        maxima.push_back(255);

        for (int k = 0; k < maxima.size(); k++)
        {
            printf("%d\n", maxima[k]);
        }
        int ct = 0;

        for (int i = 0; i < src.rows; i++)
        {
            for (int j = 0; j < src.cols; j++)
            {
                int min = 99999999;
                int pos;
                for (int k = 0; k < maxima.size(); k++)
                {
                    if (abs(src.at<uchar>(i, j) - maxima[k]) < min)
                    {
                        min = abs(src.at<uchar>(i, j) - maxima[k]);
                        pos = k;
                    }
                    
                }
                src.at<uchar>(i, j) = (uchar)maxima[pos];
                ct++;
            }
        }
        printf("%d\n%d\n ct=%d\n",src.rows, src.cols,ct);

        imshow("sper ca merge", src);
        waitKey();
    }
}


float * create_PDF(Mat src, int histogram[256])
{
    float PDF[256];

    for (int i = 0; i < 256; i++)
    {
        histogram[i] = 0;
    }

    for (int i = 0; i < src.rows; i++)
    {
        for (int j = 0; j < src.cols; j++)
        {
            histogram[src.at<uchar>(i, j)] ++;
        }
    }

    for (int i = 0; i < 256; i++)
    {
        PDF[i] = histogram[i] / (float)(src.rows * src.cols);
    }
    return PDF;
}

void enhance_multilevel_threshholding()
{
    char fname[MAX_PATH];
    if (openFileDlg(fname))
    {
        Mat src;
        src = imread(fname, CV_LOAD_IMAGE_GRAYSCALE);
        int histogram[256];
        float PDF[256];
        int WH = 5;
        float TH = 0.0003;
        for (int i = 0; i < 256; i++)
        {
            histogram[i] = 0;
        }

        for (int i = 0; i < src.rows; i++)
        {
            for (int j = 0; j < src.cols; j++)
            {
                histogram[src.at<uchar>(i, j)] ++;
            }
        }

        for (int i = 0; i < 256; i++)
        {
            PDF[i] = histogram[i] / (float)(src.rows * src.cols);
        }

        std::vector<uchar> maxima;
        maxima.push_back(0);
        for (int i = WH; i < 256 - WH; i++)
        {
            float average = 0;
            float maxim = -1;
            for (int j = i - WH; j <= i + WH; j++)
            {
                average += PDF[j];
                if (PDF[j] > maxim)
                {
                    maxim = PDF[j];
                }

            }
            average = average / (float)(2 * WH + 1);

            if (PDF[i] > (average + TH) && PDF[i] >= maxim)
            {
                maxima.push_back(i);
            }
        }
        maxima.push_back(255);

        for (int k = 0; k < maxima.size(); k++)
        {
            printf("%d\n", maxima[k]);
        }
        int ct = 0;
        /*
        for (int i = 0; i < src.rows; i++)
        {
            for (int j = 0; j < src.cols; j++)
            {
                int min = 99999999;
                int pos;
                for (int k = 0; k < maxima.size(); k++)
                {
                    if (abs(src.at<uchar>(i, j) - maxima[k]) < min)
                    {
                        min = abs(src.at<uchar>(i, j) - maxima[k]);
                        pos = k;
                    }

                }
                src.at<uchar>(i, j) = (uchar)maxima[pos];
                ct++;
            }
        }

        imshow("sper ca merge", src);
        */

        for (int i = src.rows - 2; i >= 1; i--)
        {
            for (int j = 1; j < src.cols - 1; j++)
            {
                uchar oldPixel = src.at<uchar>(i, j);
                uchar newPixel;
                int min = 255;
                int pos;
                for (int k = 0; k < maxima.size(); k++)
                {
                    if (abs(src.at<uchar>(i, j) - maxima[k]) < min)
                    {
                        min = abs(src.at<uchar>(i, j) - maxima[k]);
                        pos = k;
                    }

                }
                newPixel = maxima[pos];
                src.at<uchar>(i, j) = newPixel;
                int  error = oldPixel - newPixel;
                src.at<uchar>(i, j + 1) = max(min(src.at<uchar>(i, j + 1) + (7 * error) / 16, 255), 0);
                src.at<uchar>(i - 1, j - 1) = max(min(src.at<uchar>(i - 1, j - 1) + (3 * error) / 16, 255), 0);
                src.at<uchar>(i - 1, j) = max(min(src.at<uchar>(i - 1, j) + (5 * error) / 16, 255), 0);
                src.at<uchar>(i - 1, j + 1) = max(min(src.at<uchar>(i - 1, j + 1) + error / 16, 255), 0);
            }
        }
        imshow("last img", src);
        waitKey();
    }
}


int calculate_perimeter(Mat src, uchar R, uchar G, uchar B)
{
    int perimeter = 0;
    uchar pixelR, pixelG, pixelB, nr, ng, nb;
    for (int i = 0; i < src.rows; i++)
    {
        for (int j = 0; j < src.cols; j++)
        {
            pixelR = src.at<Vec3b>(i, j)[2];
            pixelG = src.at<Vec3b>(i, j)[1];
            pixelB = src.at<Vec3b>(i, j)[0];
            int ok = 0;
            if (R == pixelR && G == pixelG && pixelB == B)
            {
                for (int x = i - 1; x <= i + 1; x++)
                {
                    for (int y = j - 1; y <= j + 1; y++)
                    {
                        nr = src.at<Vec3b>(x, y)[2];
                        ng = src.at<Vec3b>(x, y)[1];
                        nb = src.at<Vec3b>(x, y)[0];
                        if (nr != pixelR || ng != pixelG || nb != B)
                        {
                            ok = 1;
                        }
                    }
                }
                if (ok)
                {
                    perimeter++;
                }
            }
        }
    }
    return perimeter;
}


float thinness(int arie, int p)
{
    return 4 * CV_PI * (arie / pow(p, 2));
}


float aspectratio(Mat src, uchar R, uchar G, uchar B)
{
    uchar pixelR, pixelG, pixelB;
    int cmax = 0, cmin = src.cols, rmax = 0, rmin = src.rows;
    for (int i = 0; i < src.rows; i++)
    {
        for (int j = 0; j < src.cols; j++)
        {
            pixelR = src.at<Vec3b>(i, j)[2];
            pixelG = src.at<Vec3b>(i, j)[1];
            pixelB = src.at<Vec3b>(i, j)[0];
            int ok = 0;
            if (R == pixelR && G == pixelG && pixelB == B)
            {
                if (j < cmin)
                {
                    cmin = j;
                }
                if (j > cmax)
                {
                    cmax = j;
                }
                if (i < rmin)
                {
                    rmin = i;
                }
                if (i > rmax)
                {
                    rmax = i;
                }
            }
        }
    }
    printf("%d %d %d %d\n", rmin, rmax, cmin, cmax);
    return (cmax - cmin + 1) / (float)(rmax - rmin + 1);
}

void project_object(Mat src, uchar R, uchar G, uchar B)
{
    uchar pixelR, pixelG, pixelB;
    int cmax = 0, cmin = src.cols, rmax = 0, rmin = src.rows;

    Mat newSRC(src.rows, src.cols, CV_8UC3);
    Mat newSRC2(src.rows, src.cols, CV_8UC3);
    Vec3b white;
    white[0] = 255;
    white[1] = 255;
    white[2] = 255;

    for (int i = 0; i < src.rows; i++)
    {
        for (int j = 0; j < src.cols; j++)
        {
            newSRC.at<Vec3b>(i, j) = white;
        }
    }

    for (int i = 0; i < src.rows; i++)
    {
        for (int j = 0; j < src.cols; j++)
        {
            newSRC2.at<Vec3b>(i, j) = white;
        }
    }

    for (int i = 0; i < src.rows; i++)
    {
        for (int j = 0; j < src.cols; j++)
        {
            pixelR = src.at<Vec3b>(i, j)[2];
            pixelG = src.at<Vec3b>(i, j)[1];
            pixelB = src.at<Vec3b>(i, j)[0];
            int ok = 0;
            if (R == pixelR && G == pixelG && pixelB == B)
            {
                if (j < cmin)
                {
                    cmin = j;
                }
                if (j > cmax)
                {
                    cmax = j;
                }
                if (i < rmin)
                {
                    rmin = i;
                }
                if (i > rmax)
                {
                    rmax = i;
                }
            }
        }
    }
    int ct;
    for (int i = rmin; i <= rmax; i++)
    {
        ct = 0;
        for (int j = cmin; j <= cmax; j++)
        {
            pixelR = src.at<Vec3b>(i, j)[2];
            pixelG = src.at<Vec3b>(i, j)[1];
            pixelB = src.at<Vec3b>(i, j)[0];
            if (R == pixelR && G == pixelG && pixelB == B)
            {
                newSRC.at<Vec3b>(i, cmax - ct) = src.at<Vec3b>(i, j);
                ct++;
            }

        }
    }
    
    for (int i = cmin; i <= cmax; i++)
    {
        ct = 0;
        for (int j = rmin; j <= rmax; j++)
        {
            pixelR = src.at<Vec3b>(j, i)[2];
            pixelG = src.at<Vec3b>(j, i)[1];
            pixelB = src.at<Vec3b>(j, i)[0];
            if (R == pixelR && G == pixelG && pixelB == B)
            {
                newSRC2.at<Vec3b>(rmax - ct, i) = src.at<Vec3b>(j, i);
                ct++;
            }
            //printf("%d %d\n", i, j);

        }
    }

    imshow("new img1", newSRC);
    imshow("new img", newSRC2);

}

void circle_perimeter(Mat src, uchar R, uchar G, uchar B)
{
    Mat newSRC(src.rows, src.cols, CV_8UC3);
    Vec3b white;
    white[0] = 255;
    white[1] = 255;
    white[2] = 255;

    for (int i = 0; i < src.rows; i++)
    {
        for (int j = 0; j < src.cols; j++)
        {
            newSRC.at<Vec3b>(i, j) = white;
        }
    }


    int perimeter = 0;
    uchar pixelR, pixelG, pixelB, nr, ng, nb;
    for (int i = 0; i < src.rows; i++)
    {
        for (int j = 0; j < src.cols; j++)
        {
            pixelR = src.at<Vec3b>(i, j)[2];
            pixelG = src.at<Vec3b>(i, j)[1];
            pixelB = src.at<Vec3b>(i, j)[0];
            int ok = 0;
            if (R == pixelR && G == pixelG && pixelB == B)
            {
                for (int x = i - 1; x <= i + 1; x++)
                {
                    for (int y = j - 1; y <= j + 1; y++)
                    {
                        nr = src.at<Vec3b>(x, y)[2];
                        ng = src.at<Vec3b>(x, y)[1];
                        nb = src.at<Vec3b>(x, y)[0];
                        if (nr == 255 || nb == 255 || ng == 255)
                        {
                            ok = 1;
                        }
                    }
                }
                if (ok)
                {
                    newSRC.at<Vec3b>(i, j)[0] = 0;
                    newSRC.at<Vec3b>(i, j)[1] = 0;
                    newSRC.at<Vec3b>(i, j)[2] = 0;
                }
            }
        }
    }
    imshow("perimeter", newSRC);
}



void selectObject(int event, int x, int y, int flags, void* param)
{
    //More examples: http://opencvexamples.blogspot.com/2014/01/detect-mouse-clicks-and-moves-on-image.html
    Mat* src = (Mat*)param;
    uchar R, G, B, pixelR, pixelG, pixelB;
    int nr_rows = 0, nr_cols = 0;
    int arie = 0;
    int center_massi = 0;
    int center_massj = 0;
    int perimeter;
    float thinn;
    float aspect;
    if (event == CV_EVENT_LBUTTONDOWN)
    {
        printf("Pos(x,y): %d,%d  Color(RGB): %d,%d,%d\n",
            x, y,
            (int)(*src).at<Vec3b>(y, x)[2],
            (int)(*src).at<Vec3b>(y, x)[1],
            (int)(*src).at<Vec3b>(y, x)[0]);

        R = (*src).at<Vec3b>(y, x)[2];
        G = (*src).at<Vec3b>(y, x)[1];
        B = (*src).at<Vec3b>(y, x)[0];

        int sum1 = 0;
        int sum2 = 0;
        int sum3 = 0;

        for (int i = 0; i < (*src).rows; i++)
        {
            for (int j = 0; j < (*src).cols; j++)
            {
                pixelR = (*src).at<Vec3b>(i, j)[2];
                pixelG = (*src).at<Vec3b>(i, j)[1];
                pixelB = (*src).at<Vec3b>(i, j)[0];
                if (R == pixelR && G == pixelG && pixelB == B)
                {
                    arie++;
                    center_massi += i;
                    center_massj += j;

                }
            }
        }


        center_massi /= arie;
        center_massj /= arie;

        for (int i = 0; i < (*src).rows; i++)
        {
            for (int j = 0; j < (*src).cols; j++)
            {
                pixelR = (*src).at<Vec3b>(i, j)[2];
                pixelG = (*src).at<Vec3b>(i, j)[1];
                pixelB = (*src).at<Vec3b>(i, j)[0];
                if (R == pixelR && G == pixelG && pixelB == B)
                {
                    sum1 += (i - center_massi)*(j - center_massj);
                    //sum2 += pow((j - center_massj), 2);
                    //sum3 += pow((i - center_massi), 2);
                    sum3 += (i - center_massi) * (i - center_massi);
                    sum2 += (j - center_massj) * (j - center_massj);
                }
            }
        }
        printf("%d %d %d\n", sum1, sum2, sum3);
        float angle = atan2(2 * sum1, sum2 - sum3);
        angle /= 2;

        if (angle < 0)
        {
            angle += CV_PI;
        }

        angle = angle * 180 / CV_PI;

        perimeter = calculate_perimeter(*src, R, G, B);
        thinn = thinness(arie, perimeter);
        aspect = aspectratio(*src, R, G, B);

        project_object(*src, R, G, B);

        printf("arie = %d\n", arie);
        printf("Center of Mass - row %d, column %d\n", center_massi, center_massj);
        printf("Angle of the elongation axis - %f degrees\n", angle);
        printf("Perimeter with 8 connectivity: %d\n", perimeter);
        printf("Thinness ratio: %f\n", thinn);
        printf("Aspect ratio: %f\n", aspect);


        circle_perimeter(*src, R, G, B);

        circle(*src, Point(center_massj, center_massi), 10, Scalar(0, 0, 0));

        imshow("circle", *src);
    }

    

}

void objectColor()
{
    Mat src;
    // Read image from file 
    char fname[MAX_PATH];
    if (openFileDlg(fname))
    {
        src = imread(fname);
        //Create a window
        namedWindow("My Window", 1);

        //set the callback function for any mouse event
        setMouseCallback("My Window", selectObject, &src);

        //show the image
        imshow("My Window", src);

        // Wait until user press some key
        waitKey(0);
    }
}


uchar *return_neighors_8(Mat_<uchar> src, int i, int j)
{
	int di[8] = { -1, 0, 1, 0, -1, +1, -1, +1 };
	int dj[8] = { 0, -1, 0, 1, -1, +1, +1, -1 };
	uchar neighbors[8];
	for (int k = 0; k<8; k++)
		neighbors[k] = src.at<uchar>(i + di[k], j + dj[k]);

	return neighbors;
}


uchar *return_neighors_4(Mat_<uchar> src, int i, int j)
{
	int di[4] = { -1, 0, 1, 0};
	int dj[4] = { 0, -1, 0, 1};
	uchar neighbors[4];
	for (int k = 0; k<4; k++)
		neighbors[k] = src.at<uchar>(i + di[k], j + dj[k]);

	return neighbors;
}


void assign_color(Mat_<uchar> src)
{
	Vec3b coloring[256];
	int nrColor = 0;
	Mat_<Vec3b> img(src.rows, src.cols, Vec3b(255,255,255));
	std::default_random_engine gen;
	std::uniform_int_distribution<int> d(0, 255);

	for (int i = 0; i < 256; i++)
	{
		coloring[i][0] = d(gen);
		coloring[i][1] = d(gen);
		coloring[i][2] = d(gen);
	}

	for (int i = 0; i < src.rows; i++)
	{
		for (int j = 0; j < src.cols; j++)
		{
			if (src(i, j) != 0)
			{
				img(i, j) = coloring[src(i, j)];
			}
		}
	}
	imshow("colored", img);
	waitKey();
}

void  object_coloring(int n)
{
	Mat_<uchar> src;
	
	char fname[MAX_PATH];
	if (openFileDlg(fname))
	{
		uchar label = 0;
		src = imread(fname, CV_LOAD_IMAGE_GRAYSCALE);
		Mat_<uchar> img(src.rows, src.cols, (uchar)0); //variqanta 1
	    //img.setTo(0); //varianta2
		for (int i = 0; i < src.rows; i++)
		{
			for (int j = 0; j < src.cols; j++)
			{
				if (src(i, j) == 0 && img(i, j) == 0)
				{
					label++;
					std::queue<Point2i> Q;
					img(i, j) = label;
					Q.push({ i, j });
					while (!Q.empty())
					{
						Point2i q = Q.front();
						Q.pop();
						//uchar *neighbors = return_neighors_8(src, i, j);
						int di[8] = { -1, 0, 1, 0, -1, +1, -1, +1 };
						int dj[8] = { 0, -1, 0, 1, -1, +1, +1, -1 };
						for (int k = 0; k < n; k++)
						{
							//uchar neighbor = neighbors[k];
							if (src(q.x, q.y) == 0 && img(q.x + di[k], q.y + dj[k]) == 0)
							{
								img(q.x + di[k], q.y + dj[k]) = label;
								Q.push({ q.x + di[k], q.y + dj[k] });
							}
						}
					}
				}
			}
		}

		assign_color(img);
	}
}


void two_pass()
{
	Mat_<uchar> src;
	char fname[MAX_PATH];
	if (openFileDlg(fname))
	{
		uchar label = 0;
		src = imread(fname, CV_LOAD_IMAGE_GRAYSCALE);
		Mat_<uchar> img(src.rows, src.cols, (uchar)0);
		vector<vector<int>> edges;
		for (int i = 0; i < src.rows; i++)
		{
			for (int j = 0; j < src.cols; j++)
			{
				if (src(i, j) == 0 && img(i, j) == 0)
				{
					vector<int> L;
					int di[8] = { -1, 0, -1, -1};
					int dj[8] = { -1, -1, 0, 1};
					for (int k = 0; k < 4; k++)
					{
						if (img(i + di[k], j + dj[k]) > 0)
						{
							L.push_back(img(i + di[k], j + dj[k]));
						}
					}
					if (L.size() == 0)
					{
						label++;
						img(i, j) = label;
					}
					else
					{
						int x = *std::min_element(L.begin(), L.end());
						img(i, j) = x;
						for each (int y in L)
						{
							if (y != x)
							{
								edges.resize(label + 1);
								edges[x].push_back(y);
								edges[y].push_back(x);
							}
						}
					}
				}
			}
		}

		uchar newLabel = 0;
		
		vector<int> newLabels(label + 1, 0);
		
		for (int i = 1; i <= label; i++)
		{
			if (newLabels[i] == 0)
			{
				newLabel++;
				std::queue<int> Q;
				newLabels[i] = newLabel;
				Q.push(i);
				while (!Q.empty())
				{
					int x = Q.front();
					Q.pop();
					for (int y : edges[x])
					{
						if (newLabels[y] == 0)
						{
							newLabels[y] = newLabel;
							Q.push(y);
						}
					}
				}
			}
		}

		for (int i = 0; i < src.rows; i++)
		{
			for (int j = 0; j < src.cols; j++)
			{
				img(i, j) = newLabels[img(i, j)];
			}
		}
		assign_color(img);
	}
}


void creare_contur()
{
    Mat_<uchar> src;
    char fname[MAX_PATH];
    if (openFileDlg(fname))
    {
        uchar label = 0;
        src = imread(fname, CV_LOAD_IMAGE_GRAYSCALE);
        Mat_<uchar> img(src.rows, src.cols, (uchar)255);
        Point2i P0, P1, Pn_1, Pn;
        P1.x = 0;
        P1.y = 0;
        for (int i = 0; i < src.rows; i++)
        {
            for (int j = 0; j < src.cols; j++)
            {
                if (src(i, j) != 255)
                {
                    img(i, j) = 0;
                    P0.x = i;
                    P0.y = j;
                    i = src.rows;
                    j = src.cols;
                    
                    
                }
            }
        }

        int di[8] = { 0, -1, -1, -1, 0, 1, 1, 1};
        int dj[8] = { 1, 1, 0, -1,-1, -1, 0, 1 };
        int dir = 7;
        Pn.x = P0.x;
        Pn.y = P0.y;
        int ok = 0;
        vector<int> AC, DC;
        do
        {
            ok++;
            if (dir % 2 == 0)
            {
                dir = (dir + 7) % 8;
            }
            else
            {
                dir = (dir + 6) % 8;
            }
            while (src(Pn.x + di[dir], Pn.y + dj[dir]) != 0)
            {
                dir = (dir + 1) % 8;
            }
            if (P1.x == 0 && P1.y == 0)
            {
                P1.x = Pn.x + di[dir];
                P1.y = Pn.y + dj[dir];

                Pn_1.x = P0.x;
                Pn_1.y = P0.y;

                Pn.x = Pn.x + di[dir];
                Pn.y = Pn.y + dj[dir];
            }
            else
            {
                Pn_1 = Pn;
                Pn.x = Pn.x + di[dir];
                Pn.y = Pn.y + dj[dir];
            }
            AC.push_back(dir);
            if (ok >= 2)
            {
                int c = (AC[AC.size() - 1] - AC[AC.size() - 2] + 8) % 8;
                DC.push_back(c);
            }
            img(Pn.x, Pn.y) = 0;
        } while (!((Pn_1 == P0) && (Pn == P1) && (ok >= 2)));

        
        printf("AC:");
        for (int i = 0; i < AC.size() - 1; i++)
        {
            printf("%d ", AC[i]);
        }
        printf("\n");

        printf("DC:");
        for (int i = 0; i < DC.size() - 1; i++)
        {
            printf("%d ", DC[i]);
        }
        printf("\n");
        

        imshow("mergeee", img);
        waitKey();
    }

}


void recontrusct()
{
    Mat_<uchar> src;
    char fname[MAX_PATH];
    char fname2[MAX_PATH];
    if (openFileDlg(fname))
    {
        if (openFileDlg(fname2))
        {
            src = imread(fname2, CV_LOAD_IMAGE_GRAYSCALE);
        }
        FILE *f = fopen(fname, "r");
        Point2i P0;
        int n;
        fscanf(f, "%d %d", &P0.x, &P0.y);
        fscanf(f, "%d", &n);
        src(P0.x, P0.y) = 0;
        int di[8] = { 0, -1, -1, -1, 0, 1, 1, 1 };
        int dj[8] = { 1, 1, 0, -1,-1, -1, 0, 1 };
        for (int i = 0; i < n; i++)
        {
            int dir;
            fscanf(f,"%d", &dir);
            P0.x = P0.x + di[dir];
            P0.y = P0.y + dj[dir];
            src(P0.x, P0.y) = 0;
        }

        imshow("excelent", src);
        waitKey();
    }
}

Mat_<uchar> img_alb(0,0);

Mat_<uchar> dilatation(int di[], int dj[], int n, Mat_<uchar> mat = img_alb)
{
    
    Mat_<uchar> src;
    char fname[MAX_PATH];
    if (mat.rows != 0 && mat.cols != 0)
    {
        src = mat;
    }
    else
    {
        if (openFileDlg(fname))
        {
            src = imread(fname, CV_LOAD_IMAGE_GRAYSCALE);
        }
    }
    Mat_<uchar> img(src.rows, src.cols, (uchar)255);
    src.copyTo(img);

    for (int i = 1; i < src.rows - 1; i++)
    {
        for (int j = 1; j < src.cols - 1; j++)
        {
            if (src(i, j) == 0)
            {
                for (int k = 0; k < n; k++)
                {
                    img(i + di[k], j + dj[k]) = 0;
                }
            }
            //img(i, j) = src(i, j);

        }
    }
    if (mat.rows == 0 && mat.cols == 0)
    {
        imshow("dilatation", img);
        imshow("original", src);
        waitKey();
    }
     return img;
}



Mat_<uchar> compresion(int di[], int dj[], int n, Mat_<uchar> mat = img_alb)
{
    Mat_<uchar> src;
    char fname[MAX_PATH];
    if (mat.rows != 0 && mat.cols != 0)
    {
        src = mat;
    }
    else
    {
        if (openFileDlg(fname))
        {
            src = imread(fname, CV_LOAD_IMAGE_GRAYSCALE);
        }
    }
    Mat_<uchar> img(src.rows, src.cols, (uchar)255);


    for(int i = 1; i < src.rows - 1; i++)
    {
        for (int j = 1; j < src.cols - 1; j++)
        {
            if (src(i, j) == 0)
            {
                int ok = 0;
                for (int k = 0; k < n; k++)
                {
                    if (src(i + di[k], j + dj[k]) == 255)
                    {
                        ok = 1;
                    }
                }
                if (ok == 1)
                {
                    img(i, j) = 255;
                }
                else
                {
                    img(i, j) = src(i, j);
                }
            }
                

        }
    }
    if (mat.rows == 0 && mat.cols == 0)
    {
        imshow("dilatation", img);
        imshow("original", src);
        waitKey();
    }
    return img;
}


void opening()
{

    int di[4] = { 0, 1, -1, 0 };
    int dj[4] = { 1, 0, 0, -1 };

   

    Mat_<uchar> src;
    char fname[MAX_PATH];
    if (openFileDlg(fname))
    {
        src = imread(fname, CV_LOAD_IMAGE_GRAYSCALE);
    }
    Mat_<uchar> img = compresion(di, dj, 4, src);
    img = dilatation(di, dj, 4, img);

    imshow("opening", img);
    imshow("originala", src);
    waitKey();
}


void closing()
{

    int di[4] = { 0, 1, -1, 0 };
    int dj[4] = { 1, 0, 0, -1 };

    Mat_<uchar> src;
    char fname[MAX_PATH];
    if (openFileDlg(fname))
    {
        src = imread(fname, CV_LOAD_IMAGE_GRAYSCALE);
    }
    Mat_<uchar>img = dilatation(di, dj, 4, src); 
    img = compresion(di, dj, 4, img);

    imshow("closing", img);
    imshow("originala", src);
    waitKey();
}


Mat_<uchar> perimetru(Mat_<uchar> mat = img_alb)
{
    int di[4] = { 0, 1, -1, 0 };
    int dj[4] = { 1, 0, 0, -1 };

    Mat_<uchar> src;
    char fname[MAX_PATH];
    if (mat.rows != 0 && mat.cols != 0)
    {
        src = mat;
    }
    else
    {
        if (openFileDlg(fname))
        {
            src = imread(fname, CV_LOAD_IMAGE_GRAYSCALE);
        }
    }

    Mat_<uchar> img = compresion(di, dj, 4, src);
    Mat_<uchar> destination(src.rows, src.cols, (uchar)255);
    for (int i = 0; i < src.rows; i++)
    {
        for (int j = 0; j < src.cols; j++)
        {
            if (img(i, j) != src(i, j))
            {
                destination(i, j) = 0;
            }
        }
    }
    if (mat.rows == 0 && mat.cols == 0)
    {
        imshow("dilatation", destination);
        imshow("original", src);
        waitKey();
    }
    return destination;
}


boolean mat_equal(Mat_<uchar> src1, Mat_<uchar> src2)
{
    for (int i = 0; i < src1.rows; i++)
    {
        for (int j = 0; j < src1.cols; j++)
        {
            if (src1(i, j) != src2(i, j))
            {
                return FALSE;
            }
        }
    }
    return TRUE;
}


void region_filling()
{
    int di[4] = { 0, 1, -1, 0 };
    int dj[4] = { 1, 0, 0, -1 };

    Mat_<uchar> src;
    char fname[MAX_PATH];
    if (openFileDlg(fname))
    {
        src = imread(fname, CV_LOAD_IMAGE_GRAYSCALE);
    }

    int centrui = src.rows / 2;
    int centruj = src.cols / 2;

    Mat_<uchar> currentImg(src.rows, src.cols, (uchar)255);
    currentImg(centrui, centruj) = (uchar)0;

    Mat_<uchar> prev(src.rows, src.cols, (uchar)255);
    Mat_<uchar> D(src.rows, src.cols, (uchar)255);
    do {
        currentImg.copyTo(prev);
        D = dilatation(di, dj, 4, currentImg);
        for (int i = 0; i < src.rows; i++)
        {
            for (int j = 0; j < src.cols; j++)
            {
                if (D(i, j) == 0 && src(i, j) == 255)
                {
                    currentImg(i, j) = 0;
                }
                else
                {
                    currentImg(i, j) = 255;
                }
            }
        }
    } while (mat_equal(currentImg, prev) == FALSE);

    Mat_<uchar> output(src.rows, src.cols, (uchar)255);
    currentImg.copyTo(output);
    for (int i = 0; i < src.rows; i++)
    {
        for (int j = 0; j < src.cols; j++)
        {
            if (src(i, j) == 0)
            {
                output(i, j) = 0;
            }
           
        }
    }
    

    imshow("currentImg", output);
    waitKey();

}




int main()
{
    int di[4] = { 0, 1, -1, 0 };
    int dj[4] = { 1, 0, 0, -1 };

    int di2[8] = { 0, -1, -1, -1, 0, 1, 1, 1 };
    int dj2[8] = { 1, 1, 0, -1,-1, -1, 0, 1 };

    int n;
	int op;
	do
	{
		system("cls");
		destroyAllWindows();
		printf("Menu:\n");
		printf(" 1 - Open image\n");
		printf(" 2 - Open BMP images from folder\n");
		printf(" 3 - Resize image\n");
		printf(" 4 - Process video\n");
		printf(" 5 - Snap frame from live video\n");
		printf(" 6 - Mouse callback demo\n");
		printf(" 7 - Negative_image\n");
		printf(" 8 - Change gray  levels of an image by an additive factor\n");
		printf(" 9 - Change gray levels of an image by a multiplicative factor\n");
		printf(" 10 - create img\n");
		printf(" 11 - Circle\n");
        printf(" 12 - Separate colours\n");
        printf(" 13 - Convert GreyScale\n");
        printf(" 14 - grey to whiteBlack\n");
        printf(" 15 - separete_hsv\n");
		printf(" 0 - Exit\n\n");
		printf("Option: ");
		scanf("%d",&op);
		switch (op)
		{
			case 1:
				testOpenImage();
				break;
			case 2:
				testOpenImagesFld();
				break;
			case 3:
				testResize();
				break;
			case 4:
				testVideoSequence();
				break;
			case 5:
				testSnap();
				break;
			case 6:
				testMouseClick();
				break;
			case 7:
				negative_image();
				break;
			case 8:
				change_gray_lvl();
				break;
			case 9:
				change_gray_lvl_fact();
				break;
			case 10:
				create_img();
				break;
			case 11:
				img_c();
				break;
			case 12:
				separete_colours();
				break;
			case 13:
				convert_GreyScale();
				break;
			case 14:
				grey_to_whiteBlack();
				break;
			case 15:
				separete_hsv();
				break;
            case 16:
                calculate_histogram();
                break;
            case 17:
                multilevel_thresholding();
                break;
            case 18:
                enhance_multilevel_threshholding();
                break;
            case 19:
                objectColor();
                break;
			case 20:
				object_coloring(8);
				break;
			case 21:
				object_coloring(4);
				break;
			case 22:
				two_pass();
				break;
            case 23:
                creare_contur();
                break;
            case 24:
                recontrusct();
            case 25:
                printf("numarul de directii: ");
                scanf("%d", &n);
                if (n == 4)
                {
                    dilatation(di, dj, 4);
                }
                else
                {
                    dilatation(di2, dj2, 8);
                }
                break;

            case 26:
                printf("numarul de directii: ");
                
                scanf("%d", &n);
                if (n == 4)
                {
                    compresion(di, dj, 4);
                }
                else
                {
                    compresion(di2, dj2, 8);
                }
                break;

            case 27:
                opening();
                break;

            case 28:
                closing();
                break;

            case 29:
                perimetru();
                break;

            case 30:
                region_filling();
                break;
				
		}
	}
	while (op!=0);
	return 0;
}