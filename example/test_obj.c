#include <stdio.h>
#include "list.h"
#include "object.h"

typedef struct
{
    int x;
    int y;
} Point;

typedef Object(Point) PointObject;

PointObject new_point(Scope *scope, int x, int y)
{
    PointObject point = new_object(Point, scope);
    attr(point, x) = x;
    attr(point, y) = y;
    return point;
}

typedef struct
{
    PointObject start;
    PointObject end;
    Scope *scope;
} Line;

typedef Object(Line) LineObject;

LineObject new_line(Scope *scope, PointObject start, PointObject end)
{
    LineObject line = new_object(Line, scope);
    attr(line, start) = start;
    attr(line, end) = end;
    return line;
}

int main()
{
    Scope *scope = new_scope();

    {
        LineObject line; // = new_line(scope, new_point(scope, 0, 0), new_point(scope, 10, 10));

        Point *pt1 = raw_ptr(attr(line, start));
        Point *pt2 = raw_ptr(attr(line, end));

        printf("Start point : %d, %d\n", pt1->x, pt1->y);
        printf("End point : %d, %d\n", pt2->x, pt2->y);
    }

    free_scope(scope);
    return 0;
}