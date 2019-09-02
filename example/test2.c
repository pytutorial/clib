#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "list.h"
#include "csv.h"

typedef struct
{
    double gre;
    double gpa;
    double rank;
} Item;

void printDouble(double x) { printf("%0.2f", x); }

void printItem(Item item)
{
    printf("{gre : %f, gpa : %f, rank : %f}", item.gre, item.gpa, item.rank);
}

DECLARE_LIST(ListItem, Item);

double randf()
{
    return (rand() & 32767) / 32768.0;
}

void readData(ListItem X, ListDouble Y)
{
    Scope scope = newScope();

    ListListDouble dataSet = newListListDouble(scope, 0);

    dataSet = readCsv(scope, "admit.csv", ",", 1);

    for (int i = 0; i < listSize(dataSet); i++)
    {
        ListDouble row = listAtQ(dataSet, i);

        if (listSize(row) >= 4)
        {
            listAdd(Y, listAt(row, 0));
            listAdd(X, ((Item){listAt(row, 1), listAt(row, 2), listAt(row, 3)}));
        }
    }
    freeScope(scope);
}

int main()
{
    Scope scope = newScope();

    ListItem X = newListItem(scope, 0);
    ListDouble Y = newListDouble(scope, 0);

    readData(X, Y);

    int N = listSize(X);

    if (N > 0)
    {
        ListItem Xhead = listSlice(X, 0, 10);
        ListDouble Yhead = listSlice(Y, 0, 10);
        
        printf("Xhead = "); printList(Xhead, printItem);
        printf("Yhead = "); printList(Yhead, printDouble);

        int epochs = 100;
        double lr = 0.001;
        double a1 = randf(), a2 = randf(), a3 = randf(), b = randf();
        double greMax = 800.0, gpaMax = 4.0, rankMax = 4.0;

        for (int step = 0; step < epochs; step++)
        {
            double da1 = 0, da2 = 0, da3 = 0, db = 0, sse = 0;
            int nCorrect = 0;

            for (int i = 0; i < N; i++)
            {
                Item x = listAtQ(X, i);
                double x1 = x.gre / greMax, x2 = x.gpa / gpaMax, x3 = x.rank / rankMax;

                double y = listAt(Y, i);
                double p = 1.0 / (1.0 + exp(-(b + a1 * x1 + a2 * x2 + a3 * x3)));
                double e = p - y;
                da1 += e * p * (1 - p) * a1;
                da2 += e * p * (1 - p) * a2;
                da3 += e * p * (1 - p) * a3;
                db += e * p * (1 - p);
                sse += e * e;

                if ((y - 0.5) * (p - 0.5) >= 0)
                    nCorrect += 1;
            }
            a1 += -lr * da1;
            a2 += -lr * da2;
            a3 += -lr * da3;
            b += -lr * db;

            double acc = (double)nCorrect / N;
            double rmse = sqrt(sse / N);
            printf("Step %d : acc=%f, rmse=%0.2f, a1=%0.5f, a2=%0.5f, a3=%0.5f, b=%0.5f\n", step, acc, rmse, a1, a2, a3, b);
        }
    }

    freeScope(scope);
}