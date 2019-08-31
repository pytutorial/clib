#include <stdio.h>
#include <math.h>
#include <assert.h>

#include "list.h"
#include "string_util.h"
#include "csv.h"

#define TRUE    1
#define FALSE   0

typedef int bool;
typedef ListDouble OutputData;

typedef struct 
{
    int input_dim;
    ListListDouble data;
}InputData;

typedef struct 
{
    InputData input;
    OutputData output;
}DataSet;

struct _TreeNode;

typedef Object(struct _TreeNode) TreeNodeObject;

typedef struct _TreeNode
{
    bool is_leaf;
    TreeNodeObject left_child;
    TreeNodeObject right_child;
    int split_feature_id;
    double split_val;
    double weight;
}TreeNode;

typedef struct {
    double gamma;
    double lamda;
    double learning_rate;
    int max_depth;
    double min_split_gain;
}Param;

#define swap(a, b) { typeof(a) tmp = b; b = a; a = tmp; }

void get_arg_sort_indexes(const ListDouble lst, ListInt indexes)
{
    assert(list_size(lst) == list_size(indexes));

    int N = list_size(lst);
    
    Scope* scope = new_scope();
    ListDouble lst_clone = new_list_double(scope, N);
    list_copy(lst_clone, 0, lst, 0, N);
    
    double* lst_ptr = list_data_ptr(lst_clone);
    int* indexes_ptr = list_data_ptr(indexes);
    
    for(int i = 0; i < N; i++) 
    {
        lst_ptr[i] = i;
    }

    for(int i = 0; i < N -1; i++)
    {
        int jmin = i + 1;
        for(int j = i + 1; j < N; j++)
        {
            if(lst_ptr[j] < lst_ptr[jmin]) 
            {
                jmin = j;
            }
        }
        if(lst_ptr[i] > lst_ptr[jmin])
        {
            swap(lst_ptr[i], lst_ptr[jmin]);
            swap(indexes_ptr[i], indexes_ptr[jmin]);
        }
    }

    free_scope(scope);
}

double calc_term(double g, double h, double lambd)
{
    return g*g / (h + lambd);
}

double _calc_split_gain(double G, double H, double  G_l, double  H_l, double  G_r, double  H_r, double  lambd) 
{
    return calc_term(G_l, H_l, lambd) + calc_term(G_r, H_r, lambd) - calc_term(G, H, lambd);
}

double _calc_leaf_weight(ListDouble grad, ListDouble hessian, double lambd)
{
    double grad_sum, hessian_sum;
    
    get_list_sum(grad, &grad_sum);
    get_list_sum(hessian, &hessian_sum);

    return grad_sum / (hessian_sum + lambd);
}

void build(TreeNodeObject nodeObj, int input_dim, ListListDouble instances, ListDouble grad, ListDouble hessian, double shrinkage_rate, int depth, Param param) {
       
    int N = list_size(instances);

    assert(list_size(grad) == N && list_size(hessian) == N);

    TreeNode* node = raw_ptr(nodeObj);
    
    if(depth > param.max_depth) {
        node->is_leaf = TRUE;
        node->weight = _calc_leaf_weight(grad, hessian, param.lamda) * shrinkage_rate;
        return;
    }

    Scope* scope = new_scope();

    double G;  get_list_sum(grad, &G);
    double H;  get_list_sum(hessian, &H);

    double best_gain = 0.0;
    int best_feature_id = -1;
    double best_val = 0.0;
    int n_left = 0;
    
    ListDouble instance_features = new_list_double(scope, N);
    ListInt sorted_instance_ids = new_list_int(scope, N);
    ListInt best_left_instance_ids = new_list_int(scope, 0);
    ListInt best_right_instance_ids = new_list_int(scope, 0);

    for(int feature_id = 0; feature_id < input_dim; feature_id++)  {
        double G_l = 0.0, H_l = 0.0;
                
        for(int i = 0; i < N; i++) {
            ListDouble row = at_q(instances, i);
            at_q(instance_features, i) = at(row, feature_id);
        }

        get_arg_sort_indexes(instance_features, sorted_instance_ids);

        for(int i = 0; i < N; i++) {
            int id = at_q(sorted_instance_ids, i);
            ListDouble row = at(instances, id);
            G_l += at(grad, id);
            H_l += at(hessian, id);
            
            double G_r = G - G_l,  H_r = H - H_l;
            double current_gain = _calc_split_gain(G, H, G_l, H_l, G_r, H_r, param.lamda);

            if(current_gain > best_gain) {
                best_gain = current_gain;
                best_feature_id = feature_id;
                best_val = at(row, feature_id);
                
                n_left = i + 1;
                
                resize_list(best_left_instance_ids, n_left);
                resize_list(best_right_instance_ids, N - n_left);

                list_copy(best_left_instance_ids, 0, sorted_instance_ids, 0, n_left);
                list_copy(best_right_instance_ids, 0, sorted_instance_ids, n_left, N);
            }
        }
    }

    if(best_gain < param.min_split_gain) {
        node->is_leaf = TRUE;
        node->weight = _calc_leaf_weight(grad, hessian, param.lamda) * shrinkage_rate;
    } else {
        node->split_feature_id = best_feature_id;
        node->split_val = best_val;

        TreeNodeObject left_child = new_object(TreeNode, scope);
        TreeNodeObject right_child = new_object(TreeNode, scope);
        
        ListListDouble best_left_instances = new_list_list_double(scope, n_left);
        ListDouble best_left_grad = new_list_double(scope, n_left);
        ListDouble best_left_hessian = new_list_double(scope, n_left);
        
        for(int i = 0; i < n_left; i++) {
            int id = at_q(best_left_instance_ids, i);
            at_q(best_left_instances, i) = at(instances, id);
            at_q(best_left_grad, i) = at(grad, id);
            at_q(best_left_hessian, i) = at(hessian, id);
        }

        build(left_child, input_dim, best_left_instances, best_left_grad, best_left_hessian, shrinkage_rate, depth+1, param);

        int n_right = N - n_left;

        ListListDouble best_right_instances = new_list_list_double(scope, n_right);
        ListDouble best_right_grad = new_list_double(scope, n_right);
        ListDouble best_right_hessian = new_list_double(scope, n_right);
        
        for(int i = 0; i < n_right; i++) {
            int id = at_q(best_right_instance_ids, i);
            at_q(best_right_instances, i) = at(instances, id);
            at_q(best_right_grad, i) = at(grad,id);
            at_q(best_right_hessian, i) = at(hessian, id);
        }

        build(right_child, input_dim, best_right_instances, best_right_grad, best_right_hessian, shrinkage_rate, depth+1, param);

        node->left_child = left_child;
        node->right_child = right_child;
    }

    free_scope(scope);
}

DataSet read_data(Scope* scope)
{
    ListListDouble table = new_list_list_double(scope, 0);
    read_csv("admit.csv", ",", 1, table);

    DataSet data_set;
    memset(&data_set, 0, sizeof(DataSet));
    
    if(list_size(table) > 0 && list_size(at(table, 0)) >= 2)
    {
        int N = list_size(table);
        ListListDouble X = new_list_list_double(scope, N);
        ListDouble Y = new_list_double(scope, N);

        InputData input = { list_size(at(table, 0)) - 1, X };
        OutputData output = Y;

        for(int i = 0; i <N; i++)
        {
            ListDouble row = at(table, i);
            int M = list_size(row);
            
            if(M >= 2)
            {
                at(Y, i) = at(row, 0);
                ListDouble x = new_list_double(scope, M);
                for(int j = 1; j < M; j++)
                {
                    at(x, j - 1) = at(row, j);
                }
                at(input.data, i) = x;
            }
        }

        data_set.input = input;
        data_set.output = output;
    }

    return data_set;
}

void fprint_input_item(FILE* f, ListDouble item) 
{
	fprint_list(f, item, "%0.2f");
}

int main()
{
    Scope* scope = new_scope(); 
    
    DataSet data_set = read_data(scope);

    printf("Input = ");  fprint_list_obj(stdout, data_set.input.data, fprint_input_item);

    printf("\nOutput = "); fprint_list(stdout, data_set.output, "%0.0f");

    printf("\n");

    free_scope(scope);
    return 0;
}