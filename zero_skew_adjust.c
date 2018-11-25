/*
 * zero_skew_adjust.c
 *
 *  Created on: Nov 19, 2018
 *      Author: wei100
 */

#include "zst.h"

void adjust_left(node* parent, double skew_diff) {
    if(parent->left->node_num == -1) {
        double a = - r * parent->left_wire_len * inv_cin;
        double b, c;
        if(parent->left->left->node_num == -1){
            b = r * parent->left_wire_len + 
                        0.69 * inv_rout * (inv_cin + c * parent->left->left_wire_len / 2- skew_diff);
            c  = -0.69 * inv_rout * (inv_cin + c * parent->left->left_wire_len / 2);
        }
        else{
            b = r * parent->left_wire_len + 
                        0.69 * inv_rout * (parent->left->left->total_cap + c * parent->left->left_wire_len / 2- skew_diff);
            c  = -0.69 * inv_rout * (parent->left->left->total_cap + c * parent->left->left_wire_len / 2);
        }
        int n = (-b + sqrt(pow(b,2) - 4 * a * c)) / (2 * a);
        if(n <= 0) {
            perror("Invalid number of parallel inverters\n");
        }
        parent->left->num_node_inv = n;
    }
}
void adjust_right(node* parent, double skew_diff) {
    if(parent->right->node_num == -1) {
        double a = - r * parent->right_wire_len * inv_cin;
        double b, c;
        if(parent->right->left->node_num == -1){
            b = r * parent->right_wire_len + 
                        0.69 * inv_rout * (inv_cin + c * parent->right->left_wire_len / 2 - skew_diff);
            c  = -0.69 * inv_rout * (inv_cin + c * parent->right->left_wire_len / 2);
        }
        else{
            b = r * parent->right_wire_len + 
                        0.69 * inv_rout * (parent->right->left->total_cap + c * parent->right->left_wire_len / 2- skew_diff);
            c  = -0.69 * inv_rout * (parent->right->left->total_cap + c * parent->right->left_wire_len / 2);
        }
        int n = (-b + sqrt(pow(b,2) - 4 * a * c)) / (2 * a);
        if(n <= 0) {
            perror("Invalid number of parallel inverters\n");
        }
        parent->right->num_node_inv = n;
    }

}
void zero_skew_adjust(node *curr)
{
    //if the current node is an inverter
    if(curr->node_num == -1) {
        double wire_delay_l = r * curr->left_wire_len * (curr->left->total_cap + c * curr->left_wire_len / 2);
        curr->min_delay = curr->left->min_delay + wire_delay_l;
        curr->max_delay = curr->left->max_delay + wire_delay_l;
        return;
    }
    //if the current node is a sink node
    else if(curr->leaf_node_label != -1) {
        return;
    }
    double wire_delay_l, wire_delay_r;
    if(curr->left->node_num == -1) {
        wire_delay_l = r * curr->left_wire_len * (inv_cin + c * curr->left_wire_len / 2);
    }
    else{
        wire_delay_l = r * curr->left_wire_len * (curr->left->total_cap + c * curr->left_wire_len / 2);
    }
    if(curr->right->node_num == -1) {
        wire_delay_r = r * curr->right_wire_len * (inv_cin + c * curr->right_wire_len / 2);
    }
    else{
        wire_delay_r = r * curr->right_wire_len * (curr->right->total_cap + c * curr->right_wire_len / 2);
    }


    double lmax = curr->left->max_delay + wire_delay_l;
    double lmin = curr->left->min_delay + wire_delay_l;
    double rmax = curr->right->max_delay + wire_delay_r;
    double rmin = curr->right->min_delay + wire_delay_r;

    if(rmax >= lmax && rmin <= lmin) {
        //Do nothing;
        return;
    }

    if(lmax >= rmax && lmin <= rmin) {
        //Do nothing;
        return;
    }
    if(lmax >= SINK_BOUND) {
        adjust_left(curr, lmax-SINK_BOUND);
    }
    if(rmax >= SINK_BOUND) {
        adjust_right(curr, rmax-SINK_BOUND);
    }
    if(rmax - lmin >= lmax - rmin) {
        double temp = rmax - lmin;
        //Right branch arrival too late, adjust right branch
        if(temp > SKEW_BOUND) {
            adjust_right(curr, temp);
        }
    }

    else if (rmax - lmin < lmax - rmin) {
        double temp = lmax - rmin;
        //Left branch arrival too late, adjust left branch
        if(temp > SKEW_BOUND) {
            adjust_left(curr, temp);
        }
    }
}
