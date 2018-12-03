/*
 * zero_skew_adjust.c
 *
 *  Created on: Nov 19, 2018
 *      Author: wei100
 */

#include "zst.h"
//Sink bound not met and the internal node has num_node_inv > 0, so we add inverters to this node until the sink bound is met
void adjust_internal_inv(node* curr) {
    double propagation_delay = SKEW_CONST * inv_rout * 1 / curr->num_node_inv * curr->total_cap;
    while(curr->delay > SINK_BOUND) {
        curr->num_node_inv++;
        curr->total_cap += inv_cout;
        double propagation_delay_new = SKEW_CONST * inv_rout * 1 / curr->num_node_inv * curr->total_cap;
        curr->delay = curr->delay - propagation_delay + propagation_delay_new;
        propagation_delay = propagation_delay_new;
    }
}
void recalc_total_cap(node* curr) {
    if(curr->leaf_node_label != -1) {
        return;
    }
    else{
        double left_cap = 0;
        double right_cap = 0;
        if(curr->left->node_num != -1 && curr->left->num_node_inv == 0){
            //If the left child is not an inverter nor an internal node with an inverter
            left_cap = curr->left->total_cap;
        }
        else{
            //If the left child is an inverter or an internal node with an inverter
            left_cap = curr->left->num_node_inv * inv_cin;
        }
        if(curr->right_wire_len != -1) {
            //If the node has a right child
            if(curr->right->node_num != -1 && curr->right->num_node_inv == 0){
                //If the right child is not an inverter or an internal node with an inverter
                right_cap = curr->right->total_cap;
            }
            else{
                //If the right child is an inverter or an internal node with an inverter
                right_cap = curr->right->num_node_inv * inv_cin;
            }
        }
        if(curr->right != NULL) {
            curr->total_cap = inv_cout * curr->num_node_inv + left_cap + right_cap + c * (curr->left_wire_len + curr->right_wire_len);
        }
        else{
            curr->total_cap = inv_cout * curr->num_node_inv + left_cap + right_cap + c * curr->left_wire_len;
        }
    }
}
void zero_skew_adjust(node* curr) {
    recalc_total_cap(curr);
    if(curr->left_wire_len == 0 && curr->right_wire_len == 0) {
        curr->delay = 0;
        return;
    }
    if(curr->node_num == -1) {
        double propagation_delay = SKEW_CONST * inv_rout * 1 / curr->num_node_inv * curr->total_cap;
        curr->delay = curr->left->delay + propagation_delay;
        adjust_internal_inv(curr);
        return;
    }
    double wire_delay_l = 0;
    double wire_delay_r = 0;
    if(curr->left->node_num != -1 && curr->left->num_node_inv == 0) {
        wire_delay_l = r * curr->left_wire_len * (curr->left->total_cap + c * curr->left_wire_len / 2);
    }
    else{
        wire_delay_l = r * curr->left_wire_len * (curr->left->num_node_inv * inv_cin + c * curr->left_wire_len / 2);
    }
    if(curr->right != NULL) {
        if(curr->right->node_num != -1 && curr->right->num_node_inv == 0) {
            wire_delay_r = r * curr->right_wire_len * (curr->right->total_cap + c * curr->right_wire_len / 2);
        }
        else{
            wire_delay_r = r * curr->right_wire_len * (curr->right->num_node_inv * inv_cin + c * curr->right_wire_len / 2);
        }
    }
    else{
        adjust_internal_inv(curr);
        return;
    }
    double propagation_delay_node = 0;
    if(curr->num_node_inv > 0) {
        propagation_delay_node = SKEW_CONST * inv_rout * 1 / curr->num_node_inv * curr->total_cap;
    }
    double left_time = curr->left->delay + wire_delay_l + propagation_delay_node;
    double right_time = curr->right->delay + wire_delay_r + propagation_delay_node;
    curr->delay = left_time;
    if(left_time != right_time) {
        if(left_time < right_time) {
            double a, b, c_new;
            double temp = right_time + wire_delay_r - left_time;
            if(curr->left->node_num != -1 && curr->left->num_node_inv == 0) {
                a = r*c / 2;
                b = r * curr->left->total_cap;
                c_new = -temp;
                curr->left_wire_len = (-b + sqrt(pow(b, 2) - 4 * a * c_new)) / (2 * a);
                wire_delay_l = r * curr->left_wire_len * (curr->left->total_cap + c * curr->left_wire_len / 2);
            }
            else{
                a = r*c / 2;
                b = r * curr->left->num_node_inv * inv_cin;
                c_new = -temp;
                curr->left_wire_len = (-b + sqrt(pow(b, 2) - 4 * a * c_new)) / (2 * a);
                wire_delay_l = r * curr->left_wire_len * (curr->left->num_node_inv * inv_cin + c * curr->left_wire_len / 2);
            }
            curr->delay = right_time;
            if(curr->delay > SINK_BOUND) {
                if(curr->num_node_inv > 0) {
                    adjust_internal_inv(curr);
                }
                else{
                    printf("2.We are in trouble. What should we do since there is no inverter at this internal node and yet the sink constraint is not met\n");
                }
            }
        }
        else{
            double a, b, c_new;
            //Left branch arrival too late, make the right wire longer 
            double temp = left_time + wire_delay_l - right_time;
            if(curr->right->node_num != -1 && curr->right->num_node_inv == 0) {
                a = r*c / 2;
                b = r * curr->right->total_cap;
                c_new = -temp;
                curr->right_wire_len = (-b + sqrt(pow(b, 2) - 4 * a * c_new)) / (2 * a);
                wire_delay_r = r * curr->right_wire_len * (curr->right->total_cap + c * curr->right_wire_len / 2);
            }
            else{
                a = r*c / 2;
                b = r * curr->right->num_node_inv * inv_cin;
                c_new = -temp;
                curr->right_wire_len = (-b + sqrt(pow(b, 2) - 4 * a * c_new)) / (2 * a);
                wire_delay_r = r * curr->right_wire_len * (curr->right->num_node_inv * inv_cin + c * curr->right_wire_len / 2);
            }
            curr->delay = left_time;
            if(curr->delay > SINK_BOUND) {
                if(curr->num_node_inv > 0) {
                    adjust_internal_inv(curr);
                }
                else{
                    printf("2.We are in trouble. What should we do since there is no inverter at this internal node and yet the sink constraint is not met\n");
                }
            }

        }
    }


}




