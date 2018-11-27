#include "zst.h"
//After parity is calculated for every node (including inverters), pass the node into this function to calculate 
//the max and min delay and adjust inverter size accordingly
void test_calc_delay_while_insert(node* curr) {
    if(curr->leaf_node_label != -1) {
        return;
    }
    if(curr->node_num == -1) {
        double wire_delay;
        double propagation_delay; 
        if(curr->left->node_num == -1){
            propagation_delay = 0.69 * 1 / curr->num_node_inv *  inv_rout * (inv_cin * curr->left->num_node_inv + curr->num_node_inv * inv_cout); 
            wire_delay = 0;
        }
        else{
            propagation_delay = 0.69 * 1 / curr->num_node_inv * inv_rout *  
            (curr->left->total_cap + curr->left_wire_len * c / 2 + curr->num_node_inv * inv_cout); 
            wire_delay = r * curr->left_wire_len * (curr->left->total_cap + c * curr->left_wire_len / 2);
        }
        curr->max_delay = curr->left->max_delay + propagation_delay + wire_delay;
        curr->min_delay = curr->left->min_delay + propagation_delay + wire_delay;
    }
    else{
        double wire_delay_l, wire_delay_r;
        if(curr->left->node_num == -1) {
            wire_delay_l = r * curr->left_wire_len * (curr->left->num_node_inv * inv_cin + c * curr->left_wire_len / 2);
        }
        else{
            wire_delay_l = r * curr->left_wire_len * (curr->left->total_cap + c * curr->left_wire_len / 2);
        }
        if(curr->right->node_num == -1) {
            wire_delay_r = r * curr->right_wire_len * (curr->right->num_node_inv * inv_cin + c * curr->right_wire_len / 2);
        }
        else{
            wire_delay_r = r * curr->right_wire_len * (curr->right->total_cap + c * curr->right_wire_len / 2);
        }
        double lmax = curr->left->max_delay + wire_delay_l;
        double lmin = curr->left->min_delay + wire_delay_l;
        double rmax = curr->right->max_delay + wire_delay_r;
        double rmin = curr->right->min_delay + wire_delay_r;

    }

}