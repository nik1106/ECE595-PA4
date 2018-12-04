#include "zst.h"
void Calc_Elmore(node* curr) {
    if(curr->leaf_node_label != -1) {
        return;
    }
    if(curr->node_num == -1) {
        double propagation_delay = SKEW_CONST * inv_rout * 1 / curr->num_node_inv * curr->total_cap;
        if(curr->left->node_num != -1 && curr->left->num_left_inv == 0) {
            curr->left->delay += curr->delay + r * curr->left_wire_len * curr->left->total_cap + propagation_delay;
        }
        else{
            curr->left->delay += curr->delay + r * curr->left_wire_len * curr->left->num_node_inv * inv_cin + propagation_delay;
        }
    }
    else{
        double propagation_delay = 0;
        if(curr->num_node_inv > 0) {
            propagation_delay = SKEW_CONST * inv_rout * 1 / curr->num_node_inv * curr->total_cap;
        }
        if(curr->left->node_num != -1 && curr->left->num_node_inv == 0) {
            curr->left->delay += curr->delay + r * curr->left_wire_len * curr->left->total_cap + propagation_delay; 
        }
        else{
            curr->left->delay += curr->delay + r * curr->left_wire_len * curr->left->num_node_inv * inv_cin + propagation_delay;
        }
        if(curr->right != NULL) {
            if(curr->right->node_num != -1 && curr->right->num_node_inv == 0) {
                curr->right->delay += curr->delay + r * curr->right_wire_len * curr->right->total_cap + propagation_delay; 
            }
            else{
                curr->right->delay += curr->delay + r * curr->right_wire_len * curr->right->num_node_inv * inv_cin + propagation_delay;
            }
        }
        else{
            curr->delay = 0;
        }
    }
    Calc_Elmore(curr->left);
    Calc_Elmore(curr->right);
}


void Adjust(node* curr) {
    if(curr->node_num == -1 || curr->leaf_node_label != -1){
        return;
    }
    if(curr->right == NULL) {
        return;
    }
    if(!almost_equal_relative(curr->left->delay, curr->right->delay)) {
        if(curr->left->delay < curr->right->delay) {
            double a, b, c_new;
            double temp = curr->right->delay - curr->left->delay;
            if(curr->left->node_num != -1 && curr->left->num_node_inv == 0) {
                a = r*c / 2;
                b = r * curr->left->total_cap;
                c_new = -temp;
                curr->left_wire_len = (-b + sqrt(pow(b, 2) - 4 * a * c_new)) / (2 * a);
            }
            else{
                a = r*c / 2;
                b = r * curr->left->num_node_inv * inv_cin;
                c_new = -temp;
                curr->left_wire_len = (-b + sqrt(pow(b, 2) - 4 * a * c_new)) / (2 * a);
            }
        }
        else{
            double a, b, c_new;
            //Left branch arrival too late, make the right wire longer 
            double temp = curr->left->delay - curr->right->delay;
            if(curr->right->node_num != -1 && curr->right->num_node_inv == 0) {
                a = r*c / 2;
                b = r * curr->right->total_cap;
                c_new = -temp;
                curr->right_wire_len = (-b + sqrt(pow(b, 2) - 4 * a * c_new)) / (2 * a);
            }
            else{
                a = r*c / 2;
                b = r * curr->right->num_node_inv * inv_cin;
                c_new = -temp;
                curr->right_wire_len = (-b + sqrt(pow(b, 2) - 4 * a * c_new)) / (2 * a);
            }
        }
    }
}