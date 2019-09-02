#include <stdio.h>
#include "list.h"
#include "object.h"

typedef struct
{
    int x;
    int y;
} Point;

DECLARE_OBJECT(PointObject, Point);

PointObject new_point(Scope scope, int x, int y)
{
    PointObject point = newPointObject(scope);
    attr(point, x) = x;
    attr(point, y) = y;
    return point;
}

typedef struct
{
    PointObject start;
    PointObject end;
    Scope scope;
} Line;

DECLARE_OBJECT(LineObject, Line);

LineObject new_line(Scope scope, PointObject start, PointObject end)
{
    LineObject line = newLineObject(scope);
    attr(line, start) = start;
    attr(line, end) = end;
    return line;
}

int main()
{
    Scope scope = newScope();

    {
        LineObject line = new_line(scope, new_point(scope, 0, 0), new_point(scope, 10, 10));

        Point *pt1 = objectRawPtr(attr(line, start));
        Point *pt2 = objectRawPtr(attr(line, end));

        printf("Start point : %d, %d\n", pt1->x, pt1->y);
        printf("End point : %d, %d\n", pt2->x, pt2->y);
    }

    freeScope(scope);
    return 0;
}