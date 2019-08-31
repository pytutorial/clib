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
}Item;

void fprint_item(FILE* f, Item item) 
{
	fprintf(f, "{gre : %f, gpa : %f, rank : %f}", item.gre, item.gpa, item.rank);
}

DECLARE_LIST_TYPE(Item, ListItem, new_list_item);

double randf()
{
	return (rand() & 32767)/32768.0;
}

void read_data(ListItem X, ListDouble Y)
{
	Scope* scope = new_scope();
	
	ListListDouble data_set = new_list_list_double(scope, 0);
	
	read_csv("admit.csv", ",", 1, data_set);

	for(int i = 0; i < list_size(data_set); i++)
	{
		ListDouble row = at(data_set, i);
		
		if(list_size(row) >= 4)
		{
			push(Y, at(row, 0));
			push(X, ((Item) {at(row, 1), at(row, 2), at(row, 3)}));
		}
	}
	free_scope(scope);
}

int main()
{
	Scope* scope = new_scope();
	
	ListItem X = new_list_item(scope, 0);
	ListDouble Y = new_list_double(scope, 0);
	
	read_data(X, Y);
	
	int N = list_size(X);
		
	if(N > 0)
	{
		printf("Xhead = "); fprint_list_obj(stdout, list_view(X, 0, 10), fprint_item);
		printf("Yhead = "); fprint_list(stdout, list_view(Y, 0, 10), "%f");

		int epochs = 100;
		double lr = 0.001;
		double a1 = randf(), a2 = randf(), a3 = randf(), b = randf();
		double greMax = 800.0, gpaMax = 4.0, rankMax = 4.0;

		for(int step = 0;  step < epochs; step++)
		{
			double da1 = 0, da2 = 0, da3 = 0, db = 0, sse = 0;
			int nCorrect = 0;

			for(int i = 0; i < N; i++)
			{
				Item x = at(X, i);
				double x1 = x.gre/greMax, x2 = x.gpa/gpaMax, x3 = x.rank/rankMax;

				double y = at(Y, i);
				double p = 1.0/(1.0 + exp(-(b + a1*x1 + a2*x2 + a3*x3)));
				double e = p - y;
				da1 += e*p*(1-p)*a1;
				da2 += e*p*(1-p)*a2;
				da3 += e*p*(1-p)*a3;
				db += e*p*(1-p);
				sse += e*e;
				
				if((y-0.5) * (p-0.5) >= 0) 
					nCorrect += 1;
			}
			a1 += -lr * da1;
			a2 += -lr * da2;
			a3 += -lr * da3;
			b += -lr * db;

			double acc = (double) nCorrect/N;
			double rmse = sqrt(sse/N);
			printf("Step %d : acc=%f, rmse=%0.2f, a1=%0.5f, a2=%0.5f, a3=%0.5f, b=%0.5f\n", step, acc, rmse, a1, a2, a3, b);
		}
	}
	
	free_scope(scope);
}