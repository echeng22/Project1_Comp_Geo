#include <stdio.h>
#include <iostream>
#include <fstream>
#include <math.h>
#include <opencv2/opencv.hpp>
#include <string>
#include <typeinfo>
#include <stdlib.h>
#include <time.h>

/*
Need to increase stack limit in order to run recursive flood fill algorithm. On linux, can be done with ulimit -s 32000.
*/

using namespace std;
using namespace cv;

void drawLines(Mat canvas, int x0, int y0, int x1, int y1);
Point convertToOctant0(int octant, int x, int y);
Point convertFromOctant0(int octant, int x, int y);
int findOctant(int x0, int y0, int x1, int y1);
void drawPolygons(Mat canvas, std::vector<Point> pointList);
int pointInPoly(std::vector<Point> pointList, int x, int y);
void fillPolygons(Mat canvas, std::vector<Point> pointList, Vec3b color);
int minX(std::vector<Point> pointList);
int maxX(std::vector<Point> pointList);
int minY(std::vector<Point> pointList);
int maxY(std::vector<Point> pointList);
void fill(Mat canvas, std::vector<Point> pointList, int x, int y, Vec3b color);

struct polygon{
    vector<Point> coords;
    Vec3b color; //Default color will be (0,255,0) (Green).
};

int determineUnion(std::vector<polygon> shapeList);
std::vector<polygon> readPolygonList(string fname);


int main(int argc, char** argv )
{
    if(argc != 2)
    {
        printf("Usage: ./polygons <File Name>\n");
        return -1;
    }

    Mat canvas(1000, 1000, CV_8UC3); //Color is in BGR format

    std::vector<polygon> shapeList = readPolygonList(argv[1]);
    std::vector<Point> pointList;
    Vec3b color;
    for(int i = 0; i < shapeList.size(); i++)
    {
        pointList = shapeList.at(i).coords;
        color = shapeList.at(i).color;
        drawPolygons(canvas, pointList);
        imshow("Display Image", canvas);
        waitKey(0);
    }
    if (shapeList.size() == 2)
    {
        int minX1 = minX(shapeList.at(0).coords);
        int minX2 = minX(shapeList.at(1).coords);
        if (minX1 < minX2)
        {
            fillPolygons(canvas, shapeList.at(1).coords, shapeList.at(1).color);
            fillPolygons(canvas, shapeList.at(0).coords, shapeList.at(0).color);
            imshow("Display Image", canvas);
            waitKey(0);
        }
        else
        {
            fillPolygons(canvas, shapeList.at(0).coords, shapeList.at(0).color);
            fillPolygons(canvas, shapeList.at(1).coords, shapeList.at(1).color);
            imshow("Display Image", canvas);
            waitKey(0);

        }
        if(determineUnion(shapeList) == 1)
        {
            Mat unioncanvas(1000, 1000, CV_8UC3, Scalar::all(0)); //Color is in BGR format
            drawPolygons(unioncanvas, shapeList.at(0).coords);
            drawPolygons(unioncanvas, shapeList.at(1).coords);
            fillPolygons(unioncanvas, shapeList.at(0).coords, Vec3b(0,0,255));
            fillPolygons(unioncanvas, shapeList.at(1).coords, Vec3b(0,0,255));
            imshow("Union Image", unioncanvas);
            waitKey(0);
        }
        else
        {
            cout<<"No Union between Polygons!"<<endl;
        }

    }
    else
    {
            fillPolygons(canvas, shapeList.at(0).coords, shapeList.at(0).color);
            imshow("Display Image", canvas);
            waitKey(0);
    }

    return 0;
}

std::vector<polygon> readPolygonList(string fname)
{
    ifstream file(fname.c_str());
    string line;
    std::vector<polygon> polyInfo;
    if(file.is_open())
    {
        getline(file, line);
        if(line.at(0) == 'P')
        {
            int x, y;
            char c1, c2, c3;
            std::vector<Point> polyshape;
            while(getline(file,line))
            {
                //string format = line.substr(1, line.length() -1);
                std::istringstream format(line);
                format >> c1 >> x >> c2 >> y >> c3;
                polyshape.push_back(Point(x,y));
            }
            polygon P1;
            P1.coords = polyshape;
            P1.color = Vec3b(0,255,0);
            polyInfo.push_back(P1);
            file.close();
            return polyInfo;
        }
        else if(line.at(0) == 'T')
        {
            getline(file, line);
            //cout <<line<<endl;
            int x, y;
            int b, g, r;
            char c1, c2, c3, c4;
            std::vector<Point> polyshape1;
            std::vector<Point> polyshape2;

            getline(file, line);
            //cout <<line<<endl;
            polygon P1;
            std::istringstream color1(line);
            color1 >> c1 >> b >> c2 >> g >> c3 >> r >> c4;
            P1.color = Vec3b(b,g,r);
            getline(file,line);
            while(line.substr(0,2).compare("P2") != 0)
            {
                std::istringstream point1(line);
                point1 >> c1 >> x >> c2 >> y >> c3;
                polyshape1.push_back(Point(x,y));
                getline(file,line);
            }
            P1.coords = polyshape1;
            polyInfo.push_back(P1);

            getline(file, line);
            polygon P2;
            std::istringstream color2(line);
            color2 >> c1 >> b >> c2 >> g >> c3 >> r >> c4;
            P2.color = Vec3b(b,g,r);
            while(getline(file, line))
            {
                std::istringstream point2(line);
                point2 >> c1 >> x >> c2 >> y >> c3;
                polyshape2.push_back(Point(x,y));
            }
            P2.coords = polyshape2;
            polyInfo.push_back(P2);
            file.close();
            return polyInfo;
        }
    }
}


void drawPolygons(Mat canvas, std::vector<Point> pointList)
{
    std::vector<Point> pList = pointList;
    int size = pointList.size();
    Point first;
    Point second;
    for(int i = 0; i < size - 1; i++)
    {
        first = pList.at(i);
        second = pList.at(i+1);
        //cout << "First " << first.x << ", " << first.y << endl;
        //cout << "Second " << second.x << ", " << second.y << endl << endl;
        drawLines(canvas, first.x, first.y, second.x, second.y);
    }
    drawLines(canvas,second.x, second.y, pList.at(0).x, pList.at(0).y);
}


void drawLines(Mat canvas, int x0, int y0, int x1, int y1)
{
    // Use Bresenham's Line Algorithm
    int oct_val = findOctant(x0,y0,x1,y1);
    if (oct_val == -1)
    {
        cout << "Negative octant value. Error Occurred!";
        return;
    }

    Point first = convertToOctant0(oct_val, x0,y0);
    Point second = convertToOctant0(oct_val, x1,y1);

    int diff_x = second.x - first.x;
    int diff_y = second.y - first.y;
    int decision = 2*diff_y - diff_x;
    //cout <<"Octant: " << oct_val<< " ,First: " << first << ", Second: " << second << endl;

    int y_init = first.y;
    for(int x_init = first.x; x_init < second.x; x_init++)
    {
        Point temp = convertFromOctant0(oct_val, x_init, y_init);
        //cout << "Points drawn " << temp.x << ", " << temp.y << endl;
        canvas.at<Vec3b>(Point(temp.x, temp.y)) = Vec3b(255,255,255);
        if (decision >= 0)
        {
            y_init = y_init + 1;
            decision = decision - diff_x;
        }
        decision = decision + diff_y;

    }

}

Point convertToOctant0(int octant, int x, int y)
{
    switch(octant){
        case 0: return Point(x, y);
        case 1: return Point(y, x);
        case 2: return Point(y, -x);
        case 3: return Point(-x, y);
        case 4: return Point(-x,-y);
        case 5: return Point(-y, -x);
        case 6: return Point(-y, x);
        case 7: return Point(x, -y);
    }
}

Point convertFromOctant0(int octant, int x, int y)
{
    switch(octant){
        case 0: return Point(x, y);
        case 1: return Point(y, x);
        case 2: return Point(-y, x);
        case 3: return Point(-x, y);
        case 4: return Point(-x,-y);
        case 5: return Point(-y, -x);
        case 6: return Point(y, -x);
        case 7: return Point(x, -y);
    }
}

int findOctant(int x0, int y0, int x1, int y1)
{
    float slope = (float(y1-y0))/(x1-x0);
    if (x0 < x1 && (slope >= 0 && slope <=1))
    {
        return 0;
    }
    else if (y0 < y1 && (slope > 1 || isinf(slope)))
    {
        return 1;
    }
    else if (y0 < y1 && (slope < -1 || isinf(slope)))
    {
        return 2;
    }
    else if (x1 < x0 && (slope >= -1 && slope <=0))
    {
        return 3;
    }
    else if (x1 < x0 && (slope > 0 && slope <=1))
    {
        return 4;
    }
    else if (y1 < y0 && (slope > 1 || isinf(slope)))
    {
        return 5;
    }
    else if (y1 < y0 && (slope < -1 || isinf(slope)))
    {
        return 6;
    }
    else if (x0 < x1 && (slope >= -1 && slope < 0))
    {
        return 7;
    }
    return -1;
}


int newfindOctant(int x0, int y0, int x1, int y1)
{
    int diff_x = x1 - x0;
    int diff_y = y1 - y0;
    if(x1 > x0)
    {
        if(diff_y > 0)
        {
            if(diff_y > diff_x)
            {
                return 1;
            }
            else
            {
                return 0;
            }
        }
        else
        {
            if(diff_y > -diff_x)
            {
                return 6;
            }
            else
            {
                return 7;
            }
        }
    }
    else
    {
        if(diff_y > 0)
        {
            if(diff_y > -diff_x)
            {
                return 2;
            }
            else
            {
                return 3;
            }
        }
        else
        {
            if(diff_y > diff_x)
            {
                return 5;
            }
            else
            {
                return 4;
            }
        }
    }
    return -1;
}

int pointInPoly(std::vector<Point> pointList, int x, int y)
{
    int value;
    if(x < 0 || x > 1000 || y < 0 || y > 1000)
    {
        printf("Invalid Point. Error!\n");
        return -1; //Point is outside of canvas. Invalid Point.
    }
    for(int i = 0; i < pointList.size(); i++)
    {
        int x0, y0, x1, y1;
        if(i == pointList.size() - 1)
        {
            x0 = pointList.at(i).x;
            y0 = pointList.at(i).y;
            x1 = pointList.at(0).x;
            y1 = pointList.at(0).y;
        }
        else
        {
            x0 = pointList.at(i).x;
            y0 = pointList.at(i).y;
            x1 = pointList.at(i + 1).x;
            y1 = pointList.at(i + 1).y;
        }
        value = (x1 - x0) * (y - y0) - (x - x0) * (y1 - y0);
        if(value == 0)
        {
            return 0; //On the boundary
        }
        else if(value < 0)
        {
            return -1;//Right side of line; outside polygon.
        }
    }
    return 1; //Point is on the left side of every line in polygon; Point is inside polygon.
}

void fillPolygons(Mat canvas, std::vector<Point> pointList, Vec3b color)
{
    int min_x = minX(pointList);
    int max_x = maxX(pointList);
    int min_y = minY(pointList);
    int max_y = maxY(pointList);
    int randx = rand() % min_x + (max_x - min_x);
    int randy = rand() % min_y + (max_y - min_y);
    srand(time(NULL));
    while(pointInPoly(pointList, randx, randy) != 1)
    {
        srand(time(NULL));
        randx = rand() % min_x + (max_x - min_x);
        randy = rand() % min_y + (max_y - min_y);
    }
    fill(canvas, pointList, randx, randy, color);
}

void fill(Mat canvas, std::vector<Point> pointList, int x, int y, Vec3b color)
{
    int side = pointInPoly(pointList, x, y);
    //cout << "X: "<<x<<"Y: "<<y<<endl;
    //imshow("display image", canvas);
    //waitKey(1);
    Vec3b pointColor = canvas.at<Vec3b>(Point(x, y));
    //cout <<"test1"<<endl;
    if(side == 0 || side == -1 || pointColor == color)
    {
        //cout <<"test3"<<endl;
        return;
    }
    else
    {
        canvas.at<Vec3b>(Point(x, y)) = color;
        fill(canvas, pointList, x, y - 1, color);
        fill(canvas, pointList, x, y + 1, color);
        fill(canvas, pointList, x + 1, y, color);
        fill(canvas, pointList, x - 1, y, color);
    }
}

int minX(std::vector<Point> pointList)
{
    int smallest = pointList.at(0).x;
    for(int i = 1; i < pointList.size(); i++)
    {
        Point temp = pointList.at(i);
        if(temp.x < smallest)
        {
            smallest = temp.x;
        }
    }
    return smallest;
}

int maxX(std::vector<Point> pointList)
{
    int largest = pointList.at(0).x;
    for(int i = 1; i < pointList.size(); i++)
    {
        Point temp = pointList.at(i);
        if(temp.x > largest)
        {
            largest = temp.x;
        }
    }
    return largest;
}

int minY(std::vector<Point> pointList)
{
    int smallest = pointList.at(0).y;
    for(int i = 1; i < pointList.size(); i++)
    {
        Point temp = pointList.at(i);
        if(temp.y < smallest)
        {
            smallest = temp.y;
        }
    }
    return smallest;
}

int maxY(std::vector<Point> pointList)
{
    int largest = pointList.at(0).y;
    for(int i = 1; i < pointList.size(); i++)
    {
        Point temp = pointList.at(i);
        if(temp.y > largest)
        {
            largest = temp.y;
        }
    }
    return largest;
}

int determineUnion(std::vector<polygon> shapeList)
{
    polygon P1 = shapeList.at(0);
    polygon P2 = shapeList.at(1);
    std::vector<Point> pointList1 = P1.coords;
    std::vector<Point> pointList2 = P2.coords;
    int min_X = minX(pointList1);
    int max_X = maxX(pointList1);
    int min_Y = minY(pointList1);
    int max_Y = maxY(pointList1);
    for(int i = min_X; i <= max_X; i++)
    {
        for(int j = min_Y; j <= max_Y; j++)
        {
            if(pointInPoly(pointList2, i,j) == 1 && pointInPoly(pointList1, i,j) == 1)
            {
                return 1;
            }
        }
    }
    return 0;
}


/*
    Sources:
    Determining octants for Bresenham Line Algorithm: https://www.cs.helsinki.fi/group/goa/mallinnus/lines/bresenh.html

    Algorithm for Bresnham: https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm

    Determining what side of line point is on: http://stackoverflow.com/questions/22668659/calculate-on-which-side-of-a-line-a-point-is
*/


