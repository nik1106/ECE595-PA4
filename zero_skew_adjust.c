/*
 * zero_skew_adjust.c
 *
 *  Created on: Nov 19, 2018
 *      Author: wei100
 */

#include "zst.h"

void zero_skew_adjust(node *curr) {
    double wire_delay_l = r * curr->left_wire_len * (curr->left->total_cap + c * curr->left_wire_len / 2);
    double wire_delay_r = r * curr->right_wire_len * (curr->right->total_cap + c * curr->right_wire_len / 2);

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

    if(rmax - lmin >= lmax - rmin) {
        double temp = rmax - lmin;
        if(temp > SKEW_BOUND) {
            //Adjust left
        }
    }

    else if (rmax - lmin < lmax - rmin) {
        double temp = lmax - rmin;
        if(temp > SKEW_BOUND) {
            //Adjust right
        }
    }
}
