#include <stdio.h>
#include <iostream>
#include <fstream>
#include <math.h>
#include <opencv2/opencv.hpp>
#include <string>
#include <typeinfo>

/*
Note: Sample files Input1.txt and Input-2.txt do not present vertices in counter-clockwise order, they are in clockwise-order.

If points are in clockwise order, pointInPoly function needs to be modified, as it expect points to be in counter-clockwise order.
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


struct polygon{
    vector<Point> coords;
    Vec3b color; //Default color will be (255,255,255).
};

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
    for(int i = 0; i < shapeList.size(); i++)
    {
        pointList = shapeList.at(i).coords;
        drawPolygons(canvas, pointList);
        imshow("Display Image", canvas);
    }
    imshow("Display Image", canvas);
    waitKey(0);
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
            P1.color = Vec3b(255,255,255);
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
                getline(file, line);
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
    imshow("Display Image", canvas);
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
    for(int i = 0; i < pointList.size() - 1; i++)
    {
        int x0 = pointList.at(i).x;
        int y0 = pointList.at(i).y;
        int x1 = pointList.at(i + 1).x;
        int y1 = pointList.at(i + 1).y;
        value = (x1 - x0) * (y - y0) - (x - x0) * (y1 - y0);
        if(value == 0)
        {
            return 0; //On the boundary
        }
        else if(value > 0)
        {
            return -1;//Left side of line; outside polygon.
        }
    }
    return 1; //Point is on the right side of every line in polygon; Point is inside polygon.
}

void fillPolygons(Mat canvas, std::vector<Point> pointList, Vec3b color)
{

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



/*
    Sources:
    Determining octants for Bresenham Line Algorithm: https://www.cs.helsinki.fi/group/goa/mallinnus/lines/bresenh.html

    Algorithm for Bresnham: https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm

    Determining what side of line point is on: http://stackoverflow.com/questions/22668659/calculate-on-which-side-of-a-line-a-point-is
*/


