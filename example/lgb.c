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
    int input_dim;
    ListListDouble data;
} InputData;

typedef struct
{
    InputData input;
    OutputData output;
} DataSet;

typedef struct
{
    bool is_leaf;
    struct TreeNodeObjectData *left_child;
    struct TreeNodeObjectData *right_child;
    int split_feature_id;
    double split_val;
    double weight;
} TreeNode;

DECLARE_OBJECT(TreeNodeObject, TreeNode);

typedef struct
{
    double gamma;
    double lamda;
    double learning_rate;
    int max_depth;
    double min_split_gain;
} Param;

#define swap(a, b)         \
    {                      \
        typeof(a) tmp = b; \
        b = a;             \
        a = tmp;           \
    }

double list_sum(ListDouble lst)
{
    double sum = 0;
    int N = list_size(lst);
    for(int i = 0; i < N; i++) sum += list_at_q(lst, i);
    return sum;
}

void get_arg_sort_indexes(const ListDouble lst, ListInt indexes)
{
    assert(list_size(lst) == list_size(indexes));

    int N = list_size(lst);

    Scope scope = newScope();
    ListDouble lst_clone = newListDouble(scope, N);
    list_copy(lst_clone, 0, lst, 0, N);

    double *lst_ptr = list_data_ptr(lst_clone);
    int *indexes_ptr = list_data_ptr(indexes);

    for (int i = 0; i < N; i++)
    {
        lst_ptr[i] = i;
    }

    for (int i = 0; i < N - 1; i++)
    {
        int jmin = i + 1;
        for (int j = i + 1; j < N; j++)
        {
            if (lst_ptr[j] < lst_ptr[jmin])
            {
                jmin = j;
            }
        }
        if (lst_ptr[i] > lst_ptr[jmin])
        {
            swap(lst_ptr[i], lst_ptr[jmin]);
            swap(indexes_ptr[i], indexes_ptr[jmin]);
        }
    }

    free_scope(scope);
}

double calc_term(double g, double h, double lambd)
{
    return g * g / (h + lambd);
}

double _calc_split_gain(double G, double H, double G_l, double H_l, double G_r, double H_r, double lambd)
{
    return calc_term(G_l, H_l, lambd) + calc_term(G_r, H_r, lambd) - calc_term(G, H, lambd);
}

double _calc_leaf_weight(ListDouble grad, ListDouble hessian, double lambd)
{
    double grad_sum = list_sum(grad), hessian_sum = list_sum(hessian);
    return grad_sum / (hessian_sum + lambd);
}

void build(TreeNodeObject nodeObj, int input_dim, ListListDouble instances, ListDouble grad, ListDouble hessian, double shrinkage_rate, int depth, Param param)
{

    int N = list_size(instances);

    assert(list_size(grad) == N && list_size(hessian) == N);

    TreeNode *node = raw_ptr(nodeObj);

    if (depth > param.max_depth)
    {
        node->is_leaf = TRUE;
        node->weight = _calc_leaf_weight(grad, hessian, param.lamda) * shrinkage_rate;
        return;
    }

    Scope scope = newScope();

    double G = list_sum(grad);
    double H = list_sum(hessian);

    double best_gain = 0.0;
    int best_feature_id = -1;
    double best_val = 0.0;
    int n_left = 0;

    ListDouble instance_features = newListDouble(scope, N);
    ListInt sorted_instance_ids = newListInt(scope, N);
    ListInt best_left_instance_ids = newListInt(scope, 0);
    ListInt best_right_instance_ids = newListInt(scope, 0);

    for (int feature_id = 0; feature_id < input_dim; feature_id++)
    {
        double G_l = 0.0, H_l = 0.0;

        for (int i = 0; i < N; i++)
        {
            ListDouble row = list_at_q(instances, i);
            list_at_q(instance_features, i) = list_at(row, feature_id);
        }

        get_arg_sort_indexes(instance_features, sorted_instance_ids);

        for (int i = 0; i < N; i++)
        {
            int id = list_at_q(sorted_instance_ids, i);
            ListDouble row = list_at(instances, id);
            G_l += list_at(grad, id);
            H_l += list_at(hessian, id);

            double G_r = G - G_l, H_r = H - H_l;
            double current_gain = _calc_split_gain(G, H, G_l, H_l, G_r, H_r, param.lamda);

            if (current_gain > best_gain)
            {
                best_gain = current_gain;
                best_feature_id = feature_id;
                best_val = list_at(row, feature_id);

                n_left = i + 1;

                list_resize(best_left_instance_ids, n_left);
                list_resize(best_right_instance_ids, N - n_left);

                list_copy(best_left_instance_ids, 0, sorted_instance_ids, 0, n_left);
                list_copy(best_right_instance_ids, 0, sorted_instance_ids, n_left, N);
            }
        }
    }

    if (best_gain < param.min_split_gain)
    {
        node->is_leaf = TRUE;
        node->weight = _calc_leaf_weight(grad, hessian, param.lamda) * shrinkage_rate;
    }
    else
    {
        node->split_feature_id = best_feature_id;
        node->split_val = best_val;

        TreeNodeObject left_child = newTreeNodeObject(scope);
        TreeNodeObject right_child = newTreeNodeObject(scope);

        ListListDouble best_left_instances = newListListDouble(scope, n_left);
        ListDouble best_left_grad = newListDouble(scope, n_left);
        ListDouble best_left_hessian = newListDouble(scope, n_left);

        for (int i = 0; i < n_left; i++)
        {
            int id = list_at_q(best_left_instance_ids, i);
            list_at_q(best_left_instances, i) = list_at(instances, id);
            list_at_q(best_left_grad, i) = list_at(grad, id);
            list_at_q(best_left_hessian, i) = list_at(hessian, id);
        }

        build(left_child, input_dim, best_left_instances, best_left_grad, best_left_hessian, shrinkage_rate, depth + 1, param);

        int n_right = N - n_left;

        ListListDouble best_right_instances = newListListDouble(scope, n_right);
        ListDouble best_right_grad = newListDouble(scope, n_right);
        ListDouble best_right_hessian = newListDouble(scope, n_right);

        for (int i = 0; i < n_right; i++)
        {
            int id = list_at_q(best_right_instance_ids, i);
            list_at_q(best_right_instances, i) = list_at(instances, id);
            list_at_q(best_right_grad, i) = list_at(grad, id);
            list_at_q(best_right_hessian, i) = list_at(hessian, id);
        }

        build(right_child, input_dim, best_right_instances, best_right_grad, best_right_hessian, shrinkage_rate, depth + 1, param);

        node->left_child = left_child;
        node->right_child = right_child;
    }

    free_scope(scope);
}

DataSet read_data(Scope scope)
{
    Scope scope2 = newScope();
    ListListDouble table = read_csv(scope2, "admit.csv", ",", 1);

    DataSet data_set;
    memset(&data_set, 0, sizeof(DataSet));

    if (list_size(table) > 0 && list_size(list_at(table, 0)) >= 2)
    {
        int N = list_size(table);
        ListListDouble X = newListListDouble(scope, N);
        ListDouble Y = newListDouble(scope, N);

        InputData input = {list_size(list_at(table, 0)) - 1, X};
        OutputData output = Y;

        for (int i = 0; i < N; i++)
        {
            ListDouble row = list_at(table, i);
            int M = list_size(row);

            if (M >= 2)
            {
                list_at(Y, i) = list_at(row, 0);
                ListDouble x = newListDouble(scope, M);
                for (int j = 1; j < M; j++)
                {
                    list_at(x, j - 1) = list_at(row, j);
                }
                list_at(input.data, i) = x;
            }
        }

        data_set.input = input;
        data_set.output = output;
    }

    free_scope(scope2);
    return data_set;
}

void print_double(double x) {printf("%0.2f", x);}

void print_input_item(ListDouble item)
{
    print_list(item, print_double);
}

int main()
{
    Scope scope = newScope();

    DataSet data_set = read_data(scope);

    printf("Input = ");
    print_list(data_set.input.data, print_input_item);

    printf("\nOutput = ");
    print_list(data_set.output, print_double);

    printf("\n");

    free_scope(scope);
    return 0;
}