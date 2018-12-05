/*
 * zero_skew_adjust.c
 *
 *  Created on: Nov 19, 2018
 *      Author: wei100
 */

#include "zst.h"

void recalc_total_cap(node* curr) {
    if(curr->leaf_node_label != -1) {
        return;
    }
    else {
        double left_cap = 0.0;
        double right_cap = 0.0;
        if(curr->left->node_num != -1 && curr->left->num_node_inv == 0){
            //If the left child is not an inverter nor an internal node with an inverter
            left_cap = curr->left->total_cap;
        }
        else {
            //If the left child is an inverter or an internal node with an inverter
            left_cap = curr->left->num_node_inv * inv_cin;
        }
        if(curr->right != NULL) {
            //If the node has a right child
            if(curr->right->node_num != -1 && curr->right->num_node_inv == 0){
                //If the right child is not an inverter or an internal node with an inverter
                right_cap = curr->right->total_cap;
            }
            else {
                //If the right child is an inverter or an internal node with an inverter
                right_cap = curr->right->num_node_inv * inv_cin;
            }
        }
        if(curr->right != NULL) {
            curr->total_cap = inv_cout * curr->num_node_inv + left_cap + right_cap + c * (curr->left_wire_len + curr->right_wire_len);
        }
        else {
            curr->total_cap = inv_cout * curr->num_node_inv + left_cap + right_cap + c * curr->left_wire_len;
        }
    }
}

void zero_skew_adjust(node* curr) {
    recalc_total_cap(curr);

    if(curr->leaf_node_label != -1) {
        curr->delay = 0.0;
        return;
    }

    double wire_delay_l = 0.0;
    double wire_delay_r = 0.0;
    if(curr->left->node_num != -1 && curr->left->num_node_inv == 0) {
        wire_delay_l = r * curr->left_wire_len * (curr->left->total_cap + c * curr->left_wire_len / 2);
    }
    else {
        wire_delay_l = r * curr->left_wire_len * (curr->left->num_node_inv * inv_cin + c * curr->left_wire_len / 2);
    }

    if(curr->node_num == -1) {
        double propagation_delay = SKEW_CONST * inv_rout * 1 / curr->num_node_inv * curr->total_cap;
        curr->delay = curr->left->delay + wire_delay_l + propagation_delay;

        return;
    }

    if(curr->right != NULL) {
        if(curr->right->node_num != -1 && curr->right->num_node_inv == 0) {
            wire_delay_r = r * curr->right_wire_len * (curr->right->total_cap + c * curr->right_wire_len / 2);
        }
        else {
            wire_delay_r = r * curr->right_wire_len * (curr->right->num_node_inv * inv_cin + c * curr->right_wire_len / 2);
        }
    }

    double left_time = curr->left->delay + wire_delay_l;
    double right_time;
    if(curr->right != NULL) {
    	right_time = curr->right->delay + wire_delay_r;
    } else {
    	return;
    }

    curr->delay = left_time;
    if(!almost_equal_relative(left_time, right_time)) {
        if(left_time < right_time) {
            double a, b, c_new;
            if(curr->left->node_num != -1 && curr->left->num_node_inv == 0) {
                a = r * c / 2;
                b = r * curr->left->total_cap;
                if(curr->right->node_num != -1 && curr->right->num_node_inv == 0) {
                	c_new = curr->left->delay - curr->right->delay - r * curr->right->total_cap * curr->right_wire_len - 0.5 * r * c * pow(curr->right_wire_len, 2);
                }
                else {
                    c_new = curr->left->delay - curr->right->delay - r * curr->right->num_node_inv * inv_cin * curr->right_wire_len - 0.5 * r * c * pow(curr->right_wire_len, 2);
                }
                curr->left_wire_len = (-b + sqrt(pow(b, 2) - 4 * a * c_new)) / (2 * a);
                wire_delay_l = r * curr->left_wire_len * (curr->left->total_cap + c * curr->left_wire_len / 2);
            }
            else {
                a = r * c / 2;
                b = r * curr->left->num_node_inv * inv_cin;
                if(curr->right->node_num != -1 && curr->right->num_node_inv == 0) {
                	c_new = curr->left->delay - curr->right->delay - r * curr->right->total_cap * curr->right_wire_len - 0.5 * r * c * pow(curr->right_wire_len, 2);
                }
                else {
                    c_new = curr->left->delay - curr->right->delay - r * curr->right->num_node_inv * inv_cin * curr->right_wire_len - 0.5 * r * c * pow(curr->right_wire_len, 2);
                }
                curr->left_wire_len = (-b + sqrt(pow(b, 2) - 4 * a * c_new)) / (2 * a);
                wire_delay_l = r * curr->left_wire_len * (curr->left->num_node_inv * inv_cin + c * curr->left_wire_len / 2);
            }
            curr->delay = curr->left->delay + wire_delay_l;
        }
        else {
            double a, b, c_new;
            //Left branch arrival too late, make the right wire longer
            if(curr->right->node_num != -1 && curr->right->num_node_inv == 0) {
                a = r * c / 2;
                b = r * curr->right->total_cap;
                if(curr->left->node_num != -1 && curr->left->num_node_inv == 0) {
                    c_new = curr->right->delay - curr->left->delay - r * curr->left->total_cap * curr->left_wire_len - 0.5 * r * c * pow(curr->left_wire_len, 2);
                }
                else {
                    c_new = curr->right->delay - curr->left->delay - r * curr->left->num_node_inv * inv_cin * curr->left_wire_len - 0.5 * r * c * pow(curr->left_wire_len, 2);
                }
                curr->right_wire_len = (-b + sqrt(pow(b, 2) - 4 * a * c_new)) / (2 * a);
                wire_delay_r = r * curr->right_wire_len * (curr->right->total_cap + c * curr->right_wire_len / 2);
            }
            else {
                a = r * c / 2;
                b = r * curr->right->num_node_inv * inv_cin;
                if(curr->left->node_num != -1 && curr->left->num_node_inv == 0) {
                     c_new = curr->right->delay - curr->left->delay - r * curr->left->total_cap * curr->left_wire_len - 0.5 * r * c * pow(curr->left_wire_len, 2);
                 }
                 else {
                     c_new = curr->right->delay - curr->left->delay - r * curr->left->num_node_inv * inv_cin * curr->left_wire_len - 0.5 * r * c * pow(curr->left_wire_len, 2);
                 }
                curr->right_wire_len = (-b + sqrt(pow(b, 2) - 4 * a * c_new)) / (2 * a);
                wire_delay_r = r * curr->right_wire_len * (curr->right->num_node_inv * inv_cin + c * curr->right_wire_len / 2);
            }
            curr->delay = curr->left->delay + wire_delay_l;
        }
    }
    double propagation_delay_node = 0.0;
    recalc_total_cap(curr);
    if(curr->num_node_inv > 0) {
        propagation_delay_node = SKEW_CONST * inv_rout * 1 / curr->num_node_inv * curr->total_cap;
    }
    curr->delay += propagation_delay_node;
}
