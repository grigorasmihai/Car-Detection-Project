#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include "../common.h"
#include "mihai.h"
#include <queue>

#define DEBUG 0
using namespace cv;
using namespace std;

Mat src; Mat src_gray;
int thresh = 100;
int max_thresh = 255;
RNG rng(12345);


int di[8] = { 0, -1, -1, -1, 0, 1, 1, 1 };
int dj[8] = { 1, 1, 0, -1, -1, -1, 0, 1 };

int sizes[] = { 30, 40, 50, 60, 70, 80 };

int isCar(Point2i center, Mat_<uchar> canny);
Mat_<uchar> getBox(Point2i center, Mat_<uchar> canny, int size);


void showHistogram(const string& name, int* hist, const int  hist_cols, const int hist_height)
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


void bounding_box(Mat_<uchar> edge, vector<Point2i> center, Mat_<uchar> img)
{
    for (Point2i it : center)
    {
        int size;
        if ((size = isCar(it, edge)) != 0)
        {
            for (int i = 0; i < size; i++)
            {
                img[it.x - size / 2][i + it.y - size/2] = 0;
                img[it.x + size / 2][i + it.y - size / 2] = 0;

                img[i + it.x - size / 2][it.y - size / 2] = 0;
                img[i + it.x - size / 2][it.y + size / 2] = 0;
            }
            //imshow("test", img);
            //waitKey();
        }
        
    }
    
}

int isCar(Point2i center, Mat_<uchar> canny) {

    for (int size = 0; size < 6; size++) {
        Mat_<uchar> box = getBox(center, canny, sizes[size]/2);
        if (box.rows == 0 || box.cols == 0)
        {
            continue;
        }
        imshow("Test box", box);
        

        int faza = 0;

        int scaderi = 0;
        int ct_scazut = 0;
        int ct_crescut = 0;

        int scazut = 0;
        int old = 0;

        //continue;
        vector<int> projection(box.cols, 0);
        bool fail = false;

        for (int j = 0; j < box.cols; j++)
        {

            for (int i = box.rows - 1; i >= 0; i--)
            {
                if (box[i][j] == 255)
                {
                    projection[j]++;
                }
            }

            if (scazut == 0)
            {
                if (old > projection[j])
                {
                    if (ct_scazut < 3)
                    {
                        ct_scazut++;
                    }
                    else
                    {
                        scazut = 1;
                    }
                }
                else
                {
                    if (ct_scazut > 0)
                    {
                        ct_scazut = 0;
                    }
                }
            }
            else
            {
                if (old > projection[j])
                {
                    if (ct_crescut > 0)
                    {
                        ct_crescut = 0;
                    }
                }
                else
                {
                    if (ct_crescut < 3)
                    {
                        ct_crescut++;
                    }
                    else
                    {
                        fail = true;
                    }
                }
            }
        }
        if (!fail) {
            return sizes[size];
        }
        
        /*
        for (int j = 0; j < box.cols; j++)
        {
            
            for (int i = box.rows - 1; i >= 0; i--)
            {
                if (box[i][j] == 255)
                {
                    projection[j]++;
                }
            }
            if (projection[j] < old && scazut ==0) {
                scazut = 1;
            }
            else if(projection[j]>old && scazut == 1) {
                fail = true;
            }
            old = projection[j];
        }
        if (!fail) {
            return sizes[size];
        }
        */

        showHistogram("Proiectie", &projection[0], box.cols, 100);



        //waitKey();
    }

    return 0;
}

//iti da cutia in jur la punct vezi ca-i dam size/2
Mat_<uchar> getBox(Point2i center, Mat_<uchar> canny,int size) {
    Mat_<uchar> nullMat(0, 0);
    if (center.x - size < 0 || center.x + size > canny.rows)
        return nullMat;
    if (center.y - size < 0 || center.y + size > canny.cols)
        return nullMat;

    Mat_<uchar> res(size*2 + 1, size*2 + 1, (uchar)0);
    for (int i = center.x - size; i < center.x + size + 1; i++) {
        int ct = 0;
        for (int j = center.y - size; j < center.y + 1; j++) {
            if (canny[i][j] == canny[i][center.y + size - (j - (center.y - size))]) {
                res[i - (center.x - size)][j - (center.y - size)] = canny[i][j];
                res[i - (center.x - size)][size * 2 + 1 - (j - (center.y - size))] = canny[i][j];
            }
        }
    }

    return res;
}

vector<Point2i> getNeighbours(Mat_<int> s, int oi, int oj) {
    vector<Point2i> res;
    int distantX = 6;
    int distantY = 3;
    for (int i = oi - distantY; i < oi + distantY + 1; i++) {
        for (int j = oj - distantX; j < oj + distantX + 1; j++){
            if (i >= 0 && i < s.rows && j >= 0 && j < s.cols) {
                res.push_back(Point2i(i, j));

            }
        }
    }
    return res;
}


vector<Point2i> clustering(Mat_<int> s, int max,int offseti,int offsetj)
{   
    int distance = 0;
    int label = 0;
    Mat_<int> m(s.rows, s.cols, 0);
    int threshold = max / 3 * 2;
    for (int i = 0; i < s.rows; i++) {
        for (int j = 0; j < s.cols; j++) {
            if (m[i][j] != 0)
                continue;
            if (s[i][j] == 1) {
                label++;
                std::queue<Point2i> Q;

                m[i][j] = label;

                Q.push(Point2i(i,j));

                while (!Q.empty())
                {
                    Point2i q = Q.front();
                    Q.pop();

                    vector<Point2i> n = getNeighbours(s, q.x, q.y);
                    for (Point2i it : n) {
                        if (m[it.x][it.y] == 0 && s[it.x][it.y] == 1)
                        {
                            m[it.x][it.y] = label;
                            Q.push(Point2i(it.x, it.y));
                        }
                    }
                }
                
            }
        }
    }

    int z = 0;
    vector<Point2i> res(label, Point2i(0, 0));
    vector<int> no_pixels(label, 0);
    for (int i = 0; i < s.rows; i++) {
        for (int j = 0; j < s.cols; j++) {
            if (m[i][j] == 0)
                continue;
            z = m[i][j];
            res[m[i][j]-1].x += i;
            res[m[i][j]-1].y += j;
            no_pixels[m[i][j]-1]++;
        }
    }
    for (int i = 0; i < label; i++) {
        res[i].x /= no_pixels[i];
        res[i].y /= no_pixels[i];
        res[i].x = res[i].x * 4 + offseti;
        res[i].y = res[i].y + offsetj;
    }
    return res;
}



void peek(Mat_<int> s, int rows, int cols, int hoffset, int woffset)
{

    Mat_<uchar> see(rows, cols, (uchar)0);

    for (int i = 0; i < s.rows; i++)
    {
        for (int j = 0; j < s.cols; j++)
        {
            if (s[i][j] > 92)
            {
                s[i][j] = 1;
                see[i * 4 + hoffset][j + woffset] = 255;
            }
            else
            {
                s[i][j] = 0;
                see[i * 4 + hoffset][j + woffset] = 0;
            }
        }
    }

    imshow("symetry points", see);

}

void print_matrix(Mat_<int> matrix)
{
    FILE *f;
    fopen_s(&f, "matice.txt", "w");
    fprintf(f, "%4d ", 99);
    for (int i = 0; i < matrix.cols; i++)
    {
        fprintf(f, "%4d ", i);
    }
    fprintf(f, "\n");
    
    for (int i = 0; i < matrix.rows; i++) {
        fprintf(f, "%4d ", i);
        for (int j = 0; j < matrix.cols; j++) {
            fprintf(f, "%4d ", matrix[i][j]);
        }
        fprintf(f, "\n");
    }


    fclose(f);
}

int getSymmetry(Mat_<uchar> img, int oi, int oj, int W, int H) {

    int sum = 0;
    for (int j = 0;j < W / 2; j++) {
        for (int i = oi - H / 2; i < oi + H / 2; i++) {
            if (img[i][oj - j] == img[i][oj + j] && img[i][oj + j]==255) {
                sum += 2;
            }
            else if (img[i][oj - j] == img[i][oj + j]) {
                sum += 0;
            }
            else {
                sum -= 1;
            }
        }
    }

    return sum;

}

//edges
void roi_detect(Mat_<uchar> img, Mat_<uchar> orig) {

    int height_offset = img.rows / 5;
    int width_offset = img.cols / 10;
    int line_number = (img.rows - 2 * height_offset) / 4;
    int line_length = (img.cols - 2 * width_offset);

    Mat_<int> s(line_number, line_length, 0); //simetria

    //symmetry box
    int W = 15;
    int H = 10;

    int maxS = 0;
    for (int i = 0; i < line_number; i++)
    {
        for (int j = 0; j < line_length; j++)
        {

            //printf("%d %d\n", i, j);
            if (i == 89 && j == 1919)
            {
                printf("break\n");
            }
            s[i][j] = getSymmetry(img, i * 4 + height_offset, j + width_offset,W,H);
            if (s[i][j] > maxS) {
                maxS = s[i][j];
            }
        }
    }
    printf("Max s = %d\n", maxS);


    peek(s, img.rows, img.cols, height_offset, width_offset);
    print_matrix(s);



    vector<Point2i> centers = clustering(s, maxS, height_offset, width_offset);

    
    Mat_<uchar> c(img.rows, img.cols, (uchar)0);
    for (Point2i it : centers) {
        c[it.x][it.y] = 255;
    }
    imshow("Centre de clustere", c);
    printf("%d", centers.size());


    bounding_box(img, centers, orig);

}


/** @function thresh_callback */
void thresh_callback(int, void*)
{
    Mat canny_output;
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;

    /// Detect edges using canny
    Canny(src_gray, canny_output, thresh, thresh * 2, 3);
    imshow("canny", canny_output);
    //canny_output = dilatation(8, canny_output);


    /// Find contours
    findContours(canny_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

    /// Draw contours
    Mat drawing = Mat::zeros(canny_output.size(), CV_8UC1);
    for (int i = 0; i< contours.size(); i++)
    {
        //Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
        Vec3b color = { 255, 255, 255 };
        drawContours(drawing, contours, i, color, 2, 8, hierarchy, 0, Point());
    }

    /// Show in a window
    namedWindow("Contours", CV_WINDOW_AUTOSIZE);
    imshow("Contours", drawing);

    roi_detect(drawing, src_gray);

    imshow("car_detect", src_gray);
    //waitKey();
}



void test()
{
    VideoCapture capture;
    char fname[MAX_PATH];
    if (openFileDlg(fname))
    {
        //src = imread(fname, CV_LOAD_IMAGE_COLOR);
        
        capture.open(fname);
        if (!capture.isOpened())
        {
            perror("error");
        }

        

    }

    while (1) {
        capture >> src;

        if (src.empty())
        {
            break;
        }

        Mat img_thresh;


        //src = imread(argv[1], 1);

        /// Convert image to gray and blur it
        cvtColor(src, src_gray, CV_BGR2GRAY);
        blur(src_gray, src_gray, Size(3, 3));

        /// Create Window
        char* source_window = "Source";
        namedWindow(source_window, CV_WINDOW_AUTOSIZE);
        imshow(source_window, src);
        if (DEBUG) imshow("grey_scale", src_gray);
        threshold(src_gray, img_thresh, 100, 255.0, CV_THRESH_BINARY);



        if (DEBUG) imshow("Thres", img_thresh);
        createTrackbar(" Canny thresh:", "Source", &thresh, max_thresh, thresh_callback);
        thresh_callback(0, 0);

        if (waitKey(30) > 0) break;
    }
}