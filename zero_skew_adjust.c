/*
 * zero_skew_adjust.c
 *
 *  Created on: Nov 19, 2018
 *      Author: wei100
 */

#include "zst.h"
#include <string.h>
//Sink bound not met and the internal node has num_node_inv > 0, so we add inverters to this node until the sink bound is met
double get_delay(double total_cap, int num_inv) {
    char command[50];
    FILE* fptr_in = fopen("inv.spice", "w");
    fprintf(fptr_in, ".options temp=75\n");
    fprintf(fptr_in, ".tran 0.001n 1.0n 0.0n 0.001n\n");
    fprintf(fptr_in, ".include tuned_45nm_HP.pm\n");
    fprintf(fptr_in, ".include clkinv0.subckt\n");
    fprintf(fptr_in, "vdd vdd 0 1.000\n");
    fprintf(fptr_in, "vin in 0 pulse(0 1 0.0 10p 10p 500p 1.0n 0.0)\n");
    int i = 0;
    for(i = 0; i < num_inv; i++) {
        fprintf(fptr_in, "x%d in out vdd inv0\n", i);
    }
    fprintf(fptr_in, "cout out 0 %le\n", total_cap - num_inv * inv_cout);
    fprintf(fptr_in, ".ic v(in) = 1\n");
    fprintf(fptr_in, ".ic v(out) = 0\n");
    fprintf(fptr_in, ".measure tran rdelay trig v(in) val='0.5' fall=1 targ v(out) val='0.5' rise=1\n");
    fprintf(fptr_in, ".measure tran fdelay trig v(in) val='0.5' rise=1 targ v(out) val='0.5' fall=1\n");
    fprintf(fptr_in, ".end\n");
    fclose(fptr_in);
    strcpy(command, "ngspice -b inv.spice > inv.lis");
    system(command);
    return 0.0;


}
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
        double propagation_delay = get_delay(curr->total_cap - curr->num_node_inv * inv_cout, curr->num_node_inv);
        return;
        //double propagation_delay = SKEW_CONST * inv_rout * 1 / curr->num_node_inv * curr->total_cap;
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




