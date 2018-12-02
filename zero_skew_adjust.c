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
    while(curr->max_delay > SINK_BOUND) {
        curr->num_node_inv++;
        curr->total_cap += inv_cout;
        double propagation_delay_new = SKEW_CONST * inv_rout * 1 / curr->num_node_inv * curr->total_cap;
        curr->min_delay = curr->min_delay - propagation_delay + propagation_delay_new;
        curr->max_delay = curr->max_delay - propagation_delay + propagation_delay_new;
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
            left_cap = curr->left->total_cap;
        }
        else{
            left_cap = curr->left->num_node_inv * inv_cin;
        }
        if(curr->right_wire_len != -1) {
            if(curr->right->node_num != -1 && curr->right->num_node_inv == 0){
                right_cap = curr->right->total_cap;
            }
            else{
                right_cap = curr->right->num_node_inv * inv_cin;
            }
        }
        if(curr->right != NULL) {
            curr->total_cap = inv_cout * curr->num_node_inv + left_cap + right_cap + c * (curr->left_wire_len + curr->right_wire_len);
        }
        else{
            curr->total_cap = inv_cout * curr->num_node_inv + left_cap + right_cap + c * (curr->left_wire_len + curr->right_wire_len);
        }
    }
}
void zero_skew_adjust(node *curr)
{
    recalc_total_cap(curr);
    //if the current node is an inverter
    if(curr->node_num == -1) {
        double wire_delay_l = r * curr->left_wire_len * (curr->total_cap - c * curr->left_wire_len / 2);
        //double propagation_delay = SKEW_CONST * inv_rout * 1 / curr->num_node_inv * (curr->total_cap - c * curr->left_wire_len / 2);
        double propagation_delay = SKEW_CONST * inv_rout * 1 / curr->num_node_inv * curr->total_cap;

        curr->min_delay = curr->left->min_delay + wire_delay_l + propagation_delay;
        curr->max_delay = curr->left->max_delay + wire_delay_l + propagation_delay;
        if(curr->max_delay > SINK_BOUND) {
            if(curr->left_wire_len == 0 && curr->right_wire_len == -1) {
                // double child_propagation_delay = SKEW_CONST * inv_rout * 1 / curr->left->num_node_inv 
                //     * (curr->left->total_cap - c * curr->left->left_wire_len / 2);
                double child_propagation_delay = SKEW_CONST * inv_rout * 1 / curr->left->num_node_inv 
                    * curr->left->total_cap;
                while(curr->max_delay > SINK_BOUND) {
                    curr->left->num_node_inv++;
                    curr->left->total_cap += inv_cout;
                    curr->total_cap += inv_cin;
                    // propagation_delay = SKEW_CONST * inv_rout * 1 / curr->num_node_inv * (curr->total_cap - c * curr->left_wire_len / 2);
                    // double child_propagation_delay_new = SKEW_CONST * inv_rout * 1 / curr->left->num_node_inv 
                    // * (curr->left->total_cap - c * curr->left->left_wire_len / 2);
                    propagation_delay = SKEW_CONST * inv_rout * 1 / curr->num_node_inv * curr->total_cap; 
                    double child_propagation_delay_new = SKEW_CONST * inv_rout * 1 / curr->left->num_node_inv * curr->left->total_cap;
                    curr->left->min_delay = curr->left->min_delay - child_propagation_delay + child_propagation_delay_new;
                    curr->left->max_delay = curr->left->max_delay - child_propagation_delay + child_propagation_delay_new;
                    curr->min_delay = curr->left->min_delay + propagation_delay;
                    curr->max_delay = curr->left->max_delay + propagation_delay;
                    child_propagation_delay = child_propagation_delay_new;
                }
            }
            else{
                while(curr->max_delay > SINK_BOUND) {
                    curr->num_node_inv++;
                    curr->total_cap += inv_cout;
                    // double propagation_delay_new = SKEW_CONST * inv_rout * 1 / curr->num_node_inv 
                    // * (curr->total_cap - c * curr->left_wire_len / 2);
                    double propagation_delay_new = SKEW_CONST * inv_rout * 1 / curr->num_node_inv * curr->total_cap;
                    curr->min_delay = curr->min_delay - propagation_delay + propagation_delay_new;
                    curr->max_delay = curr->max_delay - propagation_delay + propagation_delay_new;
                    propagation_delay = propagation_delay_new;
                }

            }
        }
        return;
    }
    //if the current node is a sink node
    else if(curr->leaf_node_label != -1) {
        return;
    }



    //Deal with internal nodes
    double wire_delay_l, wire_delay_r;
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


    double lmax = curr->left->max_delay + wire_delay_l;
    double lmin = curr->left->min_delay + wire_delay_l;
    double rmax, rmin;
    if(curr->right != NULL) {
        rmax = curr->right->max_delay + wire_delay_r;
        rmin = curr->right->min_delay + wire_delay_r;
    }
    else{
        //The only time the right child of an internal node is null is the root of the entire tree
        //At this point, only need to make sure sink bound has been met 
        adjust_internal_inv(curr);
        return;
    }
    double propagation_delay_node = 0;
    if(curr->num_node_inv > 0) {
        propagation_delay_node = SKEW_CONST * inv_rout * 1 / curr->num_node_inv * curr->total_cap;
    }
    //Wrap around skew situation. Since the function is called in post order, we assume the subtrees already have obtained
    //Zero skew properties. Thus we need to worry about if the sink bound has been met or not 
    if(rmax >= lmax && rmin <= lmin) {
        //Do nothing;
        curr->max_delay = rmax + propagation_delay_node;
        curr->min_delay = rmin + propagation_delay_node;
        if(curr->max_delay > SINK_BOUND) {
            if(curr->num_node_inv > 0) {
                //There is a inverter at this internal node, resize inverter size to meet sink constraints
                while(curr->max_delay > SINK_BOUND) {
                    curr->num_node_inv++;
                    curr->total_cap += inv_cout;
                    double propagation_delay_new = SKEW_CONST * inv_rout * 1 / curr->num_node_inv * curr->total_cap;
                    curr->min_delay = curr->min_delay - propagation_delay_node + propagation_delay_new;
                    curr->max_delay = curr->max_delay - propagation_delay_node + propagation_delay_new;
                    propagation_delay_node = propagation_delay_new;

                }
            }
            else{
                printf("1.We are in trouble. What should we do since there is no inverter at this internal node and yet the sink constraint is not met\n");
            }
        }
        return;
    }

    if(lmax >= rmax && lmin <= rmin) {
        //Do nothing;
        curr->max_delay = lmax + propagation_delay_node;
        curr->min_delay = lmin + propagation_delay_node;
        if(curr->max_delay > SINK_BOUND) {
            if(curr->num_node_inv > 0) {
                //There is a inverter at this internal node, resize inverter size to meet sink constraints
                while(curr->max_delay >SINK_BOUND) {
                    curr->num_node_inv++;
                    curr->total_cap += inv_cout;
                    double propagation_delay_new = SKEW_CONST * inv_rout * 1 / curr->num_node_inv * curr->total_cap;
                    curr->min_delay = curr->min_delay - propagation_delay_node + propagation_delay_new;
                    curr->max_delay = curr->max_delay - propagation_delay_node + propagation_delay_new;
                    propagation_delay_node = propagation_delay_new;

                }
            }
            else{
                printf("2.We are in trouble. What should we do since there is no inverter at this internal node and yet the sink constraint is not met\n");
            }
        }
        return;
    }
    if(rmax - lmin >= lmax - rmin) {
        curr->max_delay = rmax;
        curr->min_delay = lmin;
        double temp = fabs(curr->max_delay - curr->min_delay);
        //Right branch arrival too late, adjust right branch
        if(temp > SKEW_BOUND) {
            if(curr->right != NULL && curr->right->node_num == -1) {
                //If the right child is an inverter, we can adjust sizes
                if(curr->right->left_wire_len == 0 && curr->right->right_wire_len == -1) {
                    //if the right child is in fact the top inverter of a buffer, adjust its child's sizes
                    node* top_inv = curr->right;
                    node* bottom_inv = curr->right->left;
                    double propagation_delay_top_inv = SKEW_CONST * inv_rout * 1 / top_inv->num_node_inv * bottom_inv->total_cap;
                    double propagation_delay_bottom_inv = SKEW_CONST * inv_rout * 1 / bottom_inv->num_node_inv * bottom_inv->left->total_cap;
                    while(temp > SKEW_BOUND) {
                        bottom_inv->num_node_inv++;
                        bottom_inv->total_cap += inv_cout;
                        top_inv->total_cap += inv_cin;
                        curr->total_cap += inv_cin;
                        double propagation_delay_top_inv_new = SKEW_CONST * inv_rout * 1 / top_inv->num_node_inv * bottom_inv->total_cap;
                        double propagation_delay_bottom_inv_new = SKEW_CONST * inv_rout * 1 / bottom_inv->num_node_inv * bottom_inv->left->total_cap;
                        bottom_inv->min_delay = bottom_inv->min_delay - propagation_delay_bottom_inv + propagation_delay_bottom_inv_new;
                        bottom_inv->max_delay = bottom_inv->max_delay - propagation_delay_bottom_inv + propagation_delay_bottom_inv_new;
                        top_inv->min_delay = top_inv->min_delay - propagation_delay_top_inv + propagation_delay_top_inv_new;
                        top_inv->max_delay = top_inv->max_delay - propagation_delay_top_inv + propagation_delay_top_inv_new;
                        propagation_delay_bottom_inv = propagation_delay_bottom_inv_new;
                        propagation_delay_top_inv = propagation_delay_top_inv_new;
                        curr->max_delay = top_inv->max_delay + wire_delay_r;
                        temp = fabs(curr->max_delay - curr->min_delay); 
                    }
                }
                else{
                    double propagation_delay_single_inv = SKEW_CONST * inv_rout * 1 / curr->right->num_node_inv * curr->right->total_cap;
                    //If the right child is a stand-alone inverter, adjust its own sizes
                    while(temp > SKEW_BOUND) {
                        curr->right->num_node_inv++;
                        curr->right->total_cap += inv_cout;
                        curr->total_cap += inv_cin;
                        double propagation_delay_single_inv_new = SKEW_CONST * inv_rout * 1 / curr->right->num_node_inv * curr->right->total_cap;
                        curr->right->min_delay = curr->right->min_delay - propagation_delay_single_inv + propagation_delay_single_inv_new;
                        curr->right->max_delay = curr->right->max_delay - propagation_delay_single_inv + propagation_delay_single_inv_new;
                        wire_delay_r = r * curr->right_wire_len * (curr->right->num_node_inv * inv_cin + c * curr->right_wire_len / 2);
                        curr->max_delay = curr->right->max_delay + wire_delay_r;
                        temp = fabs(curr->max_delay - curr->min_delay); 

                    }

                }
            }
            else{
                printf(" the right child is not an inverter, add buffer to the right child and recalculate delay from the right child\n");
            }
        }
        if(curr->max_delay > SINK_BOUND) {
            if(curr->num_node_inv > 0) {
                adjust_internal_inv(curr);
            }
            else{
                printf("3.We are in trouble. What should we do since there is no inverter at this internal node and yet the sink constraint is not met\n");
            }

        }
    }

    else if (rmax - lmin < lmax - rmin) {
        curr->max_delay = lmax;
        curr->min_delay = rmin;
        double temp = fabs(curr->max_delay - curr->min_delay);
        //Left branch arrival too late, adjust left branch
        if(temp > SKEW_BOUND) {
            if(curr->left->node_num == -1) {
                //If the left child is an inverter, we can adjust sizes
                if(curr->left->left_wire_len == 0 && curr->left->right_wire_len == -1) {
                    //if the right child is in fact the top inverter of a buffer, adjust its child's sizes
                    node* top_inv = curr->left;
                    node* bottom_inv = curr->right->left;
                    double propagation_delay_top_inv = SKEW_CONST * inv_rout * 1 / top_inv->num_node_inv * bottom_inv->total_cap;
                    double propagation_delay_bottom_inv = SKEW_CONST * inv_rout * 1 / bottom_inv->num_node_inv * bottom_inv->left->total_cap;
                    while(temp > SKEW_BOUND) {
                        bottom_inv->num_node_inv++;
                        bottom_inv->total_cap += inv_cout;
                        top_inv->total_cap += inv_cin;
                        curr->total_cap += inv_cin;
                        double propagation_delay_top_inv_new = SKEW_CONST * inv_rout * 1 / top_inv->num_node_inv * bottom_inv->total_cap;
                        double propagation_delay_bottom_inv_new = SKEW_CONST * inv_rout * 1 / bottom_inv->num_node_inv * bottom_inv->left->total_cap;
                        bottom_inv->min_delay = bottom_inv->min_delay - propagation_delay_bottom_inv + propagation_delay_bottom_inv_new;
                        bottom_inv->max_delay = bottom_inv->max_delay - propagation_delay_bottom_inv + propagation_delay_bottom_inv_new;
                        top_inv->min_delay = top_inv->min_delay - propagation_delay_top_inv + propagation_delay_top_inv_new;
                        top_inv->max_delay = top_inv->max_delay - propagation_delay_top_inv + propagation_delay_top_inv_new;
                        propagation_delay_bottom_inv = propagation_delay_bottom_inv_new;
                        propagation_delay_top_inv = propagation_delay_top_inv_new;
                        curr->max_delay = top_inv->max_delay + wire_delay_l;
                        temp = fabs(curr->max_delay - curr->min_delay); 
                    }
                }
                else{
                    double propagation_delay_single_inv = SKEW_CONST * inv_rout * 1 / curr->left->num_node_inv * curr->left->total_cap;
                    //If the right child is a stand-alone inverter, adjust its own sizes
                    while(temp > SKEW_BOUND) {
                        curr->left->num_node_inv++;
                        curr->left->total_cap += inv_cout;
                        curr->total_cap += inv_cin;
                        double propagation_delay_single_inv_new = SKEW_CONST * inv_rout * 1 / curr->left->num_node_inv * curr->left->total_cap;
                        curr->left->min_delay = curr->left->min_delay - propagation_delay_single_inv + propagation_delay_single_inv_new;
                        curr->left->max_delay = curr->left->max_delay - propagation_delay_single_inv + propagation_delay_single_inv_new;
                        wire_delay_l = r * curr->left_wire_len * (curr->left->num_node_inv * inv_cin + c * curr->left_wire_len / 2);
                        curr->max_delay = curr->left->max_delay + wire_delay_l;
                        temp = fabs(curr->max_delay - curr->min_delay); 

                    }

                }
            }
            else{
                printf(" the left child is not an inverter, add buffer to the left child and recalculate delay from the left child\n");
            }
        }
        if(curr->max_delay > SINK_BOUND) {
            if(curr->num_node_inv > 0) {
                adjust_internal_inv(curr);
            }
            else{
                printf("4.We are in trouble. What should we do since there is no inverter at this internal node and yet the sink constraint is not met\n");
            }

        }
    }
}
