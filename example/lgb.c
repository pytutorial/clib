#include <stdio.h>
#include <math.h>
#include <assert.h>

#include "list.h"
#include "string_util.h"
#include "csv.h"

#define TRUE 1
#define FALSE 0

typedef int bool;
typedef ListDouble OutputData;

typedef struct
{
    int inputDim;
    ListListDouble data;
} InputData;

typedef struct
{
    InputData input;
    OutputData output;
} DataSet;

typedef struct
{
    bool isLeaf;
    struct TreeNodeObjectData *leftChild;
    struct TreeNodeObjectData *rightChild;
    int splitFeatureId;
    double splitVal;
    double weight;
} TreeNode;

DECLARE_OBJECT(TreeNodeObject, TreeNode);

typedef struct
{
    double gamma;
    double lamda;
    double learningRate;
    int maxDepth;
    double minSplitGain;
} Param;

#define swap(a, b)         \
    {                      \
        typeof(a) tmp = b; \
        b = a;             \
        a = tmp;           \
    }

double listSum(ListDouble lst)
{
    double sum = 0;
    int N = listSize(lst);
    for(int i = 0; i < N; i++) sum += listAtQ(lst, i);
    return sum;
}

void getArgSortIndexes(const ListDouble lst, ListInt indexes)
{
    assert(listSize(lst) == listSize(indexes));

    int N = listSize(lst);

    Scope scope = newScope();
    ListDouble lstClone = newListDouble(scope, N);
    listCopy(lstClone, 0, lst, 0, N);

    double *lstDataPtr = listDataPtr(lstClone);
    int *indexesDataPtr = listDataPtr(indexes);

    for (int i = 0; i < N; i++)
    {
        lstDataPtr[i] = i;
    }

    for (int i = 0; i < N - 1; i++)
    {
        int jmin = i + 1;
        for (int j = i + 1; j < N; j++)
        {
            if (lstDataPtr[j] < lstDataPtr[jmin])
            {
                jmin = j;
            }
        }
        if (lstDataPtr[i] > lstDataPtr[jmin])
        {
            swap(lstDataPtr[i], lstDataPtr[jmin]);
            swap(indexesDataPtr[i], indexesDataPtr[jmin]);
        }
    }

    freeScope(scope);
}

double calcTerm(double g, double h, double lambd)
{
    return g * g / (h + lambd);
}

double _calcSplitGain(double G, double H, double Gl, double Hl, double Gr, double Hr, double lambd)
{
    return calcTerm(Gl, Hl, lambd) + calcTerm(Gr, Hr, lambd) - calcTerm(G, H, lambd);
}

double _calcLeafWeight(ListDouble grad, ListDouble hessian, double lambd)
{
    double gradSum = listSum(grad), hessianSum = listSum(hessian);
    return gradSum / (hessianSum + lambd);
}

void build(TreeNodeObject nodeObj, int inputDim, ListListDouble instances, ListDouble grad, ListDouble hessian, double shrinkageRate, int depth, Param param)
{

    int N = listSize(instances);

    assert(listSize(grad) == N && listSize(hessian) == N);

    TreeNode *node = objectRawPtr(nodeObj);

    if (depth > param.maxDepth)
    {
        node->isLeaf = TRUE;
        node->weight = _calcLeafWeight(grad, hessian, param.lamda) * shrinkageRate;
        return;
    }

    Scope scope = newScope();

    double G = listSum(grad);
    double H = listSum(hessian);

    double bestGain = 0.0;
    int bestFeatureId = -1;
    double bestVal = 0.0;
    int nLeft = 0;

    ListDouble instanceFeatures = newListDouble(scope, N);
    ListInt sortedInstanceIds = newListInt(scope, N);
    ListInt bestLeftInstanceIds = newListInt(scope, 0);
    ListInt bestRightInstanceIds = newListInt(scope, 0);

    for (int featureId = 0; featureId < inputDim; featureId++)
    {
        double Gl = 0.0, Hl = 0.0;

        for (int i = 0; i < N; i++)
        {
            ListDouble row = listAtQ(instances, i);
            listAtQ(instanceFeatures, i) = listAt(row, featureId);
        }

        getArgSortIndexes(instanceFeatures, sortedInstanceIds);

        for (int i = 0; i < N; i++)
        {
            int id = listAtQ(sortedInstanceIds, i);
            ListDouble row = listAt(instances, id);
            Gl += listAt(grad, id);
            Hl += listAt(hessian, id);

            double Gr = G - Gl, Hr = H - Hl;
            double currentGain = _calcSplitGain(G, H, Gl, Hl, Gr, Hr, param.lamda);

            if (currentGain > bestGain)
            {
                bestGain = currentGain;
                bestFeatureId = featureId;
                bestVal = listAt(row, featureId);

                nLeft = i + 1;

                listResize(bestLeftInstanceIds, nLeft);
                listResize(bestRightInstanceIds, N - nLeft);

                listCopy(bestLeftInstanceIds, 0, sortedInstanceIds, 0, nLeft);
                listCopy(bestRightInstanceIds, 0, sortedInstanceIds, nLeft, N);
            }
        }
    }

    if (bestGain < param.minSplitGain)
    {
        node->isLeaf = TRUE;
        node->weight = _calcLeafWeight(grad, hessian, param.lamda) * shrinkageRate;
    }
    else
    {
        node->splitFeatureId = bestFeatureId;
        node->splitVal = bestVal;

        TreeNodeObject leftChild = newTreeNodeObject(scope);
        TreeNodeObject rightChild = newTreeNodeObject(scope);

        ListListDouble bestLeftInstances = newListListDouble(scope, nLeft);
        ListDouble bestLeftGrad = newListDouble(scope, nLeft);
        ListDouble bestLeftHessian = newListDouble(scope, nLeft);

        for (int i = 0; i < nLeft; i++)
        {
            int id = listAtQ(bestLeftInstanceIds, i);
            listAtQ(bestLeftInstances, i) = listAt(instances, id);
            listAtQ(bestLeftGrad, i) = listAt(grad, id);
            listAtQ(bestLeftHessian, i) = listAt(hessian, id);
        }

        build(leftChild, inputDim, bestLeftInstances, bestLeftGrad, bestLeftHessian, shrinkageRate, depth + 1, param);

        int nRight = N - nLeft;

        ListListDouble bestRightInstances = newListListDouble(scope, nRight);
        ListDouble bestRightGrad = newListDouble(scope, nRight);
        ListDouble bestRightHessian = newListDouble(scope, nRight);

        for (int i = 0; i < nRight; i++)
        {
            int id = listAtQ(bestRightInstanceIds, i);
            listAtQ(bestRightInstances, i) = listAt(instances, id);
            listAtQ(bestRightGrad, i) = listAt(grad, id);
            listAtQ(bestRightHessian, i) = listAt(hessian, id);
        }

        build(rightChild, inputDim, bestRightInstances, bestRightGrad, bestRightHessian, shrinkageRate, depth + 1, param);

        node->leftChild = leftChild;
        node->rightChild = rightChild;
    }

    freeScope(scope);
}

DataSet readData(Scope scope)
{
    Scope scope2 = newScope();
    ListListDouble table = readCsv(scope2, "admit.csv", ",", 1);

    DataSet dataSet;
    memset(&dataSet, 0, sizeof(DataSet));

    if (listSize(table) > 0 && listSize(listAt(table, 0)) >= 2)
    {
        int N = listSize(table);
        ListListDouble X = newListListDouble(scope, N);
        ListDouble Y = newListDouble(scope, N);

        InputData input = {listSize(listAt(table, 0)) - 1, X};
        OutputData output = Y;

        for (int i = 0; i < N; i++)
        {
            ListDouble row = listAt(table, i);
            int M = listSize(row);

            if (M >= 2)
            {
                listAt(Y, i) = listAt(row, 0);
                ListDouble x = newListDouble(scope, M);
                for (int j = 1; j < M; j++)
                {
                    listAt(x, j - 1) = listAt(row, j);
                }
                listAt(input.data, i) = x;
            }
        }

        dataSet.input = input;
        dataSet.output = output;
    }

    freeScope(scope2);
    return dataSet;
}

void printDouble(double x) {printf("%0.2f", x);}

void printInputItem(ListDouble item)
{
    printList(item, printDouble);
}

int main()
{
    Scope scope = newScope();

    DataSet dataSet = readData(scope);

    printf("Input = ");
    printList(dataSet.input.data, printInputItem);

    printf("\nOutput = ");
    printList(dataSet.output, printDouble);

    printf("\n");

    freeScope(scope);
    return 0;
}