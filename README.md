EECS 395/495 Project #1

Source File: polygons.cpp

Run 'cmake .' to create the make files needed to compile and run file. Then run 'make' to create executable.

To run this file, run the make file to obtain the executable. The executable takes in a file name as input, ie:
    ./polygons Input-2.txt

Note: Because the flood fill algorithm is using the recursive method, the stack limit can easily be reached with this program and can cause issues when running if the stack limit size is not increased.

If using linux, the stack limit size can be increased using the command line
    ulimit -s 32000

This will increase your stack limit memory to about 32 Mbytes.


Description of main functions and structures used:

void drawLines(Mat canvas, int x0, int y0, int x1, int y1);
    Input:  Mat canvas – 3-D matrix used to draw out the polygons.

            int x0, y0, x1, y1 - X and Y coordinates of the end of line segment to draw

    drawLines uses Bresenham algorithm to draw the lines from points (x0,y0) to (x1, y1). It utilizes functions findOctant(), convertToOctant0() and convertFromOctant0() to draw lines with slopes that appear in different octants.

    Bresenham Algorithm, convertToOctant0 and convertFromOctant0 source:
    Source: https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm

    findOctant algorithm source:
    https://www.cs.helsinki.fi/group/goa/mallinnus/lines/bresenh.html

void fillPolygonsMat canvas, std::vector<Point> pointList, Vec3b color);
    Input:  Mat canvas - 3-D matrix used to draw out the polygons.
            std::vector<Point> pointList - List of vertices of the polygon. Assumed to be in counter-clockwise order.
            Vec3b color - Color to fill polygon in.

    fillPolygons is the driver function that will start the floodfill process. Given the list, it will create a bounding box around the polygon and select a random point inside the box. If that point is inside the polygon, determined using the function pointInPoly(), this point will be used as the starting point in the flood fill function fill()

void fill(Mat canvas, std::vector<Point> pointList, int x, int y, Vec3b color);
    Input:  Mat canvas - 3-D matrix used to draw out the polygons.
            std::vector<Point> pointList - List of vertices of the polygon. Assumed to be in counter-clockwise order.
            int x, int y - Current pixel location
            Vec3b color - Color to draw polygon.

    fill is the recursive function used for the flood fill of the polygon. Base cases for this function are when the polygon has reached the boundary, is outside the polygon, or has hit another pixel that has the same color as the color being filled. Determining if pixel is inside, outside, or on the border is done by the function pointInPoly(). pointInPoly() works assuming that the list of vertices given is in counter-clockwise order.

    pointInPoly source:
    http://stackoverflow.com/questions/22668659/calculate-on-which-side-of-a-line-a-point-is

int determineUnion(std::vector<polygon> shapeList);
    Input:
            std::vector<polygon> shapeList - A list of polygons read from the text file. Each polygon in list contains info about its vertices, and color.

    determineUnion checks to see if a union is possible between the two polygons. Algorithm for this is similar to scan line algorithm. Using the bounding box of the first polygon in the list, it scans through each point in the box. If the point is a point in polygon 1 and polygon 2, it will end the function and return 1 (True) for a union. Otherwise, it will return 0 and print out a statement that no union can occur.

    If union can occur, both polygons are filled in with color red in seperate window.






















