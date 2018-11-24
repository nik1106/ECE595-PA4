/*
 * bt_to_list.c
 *
 *  Created on: Nov 21, 2018
 *      Author: wei100
 */
#include "zst.h"
void find_inv_loc(node* node_parent, double wire_length, double radius, node* node_child, node* inv) 
{
    if(node_parent == NULL || node_child == NULL || inv == NULL) {
        perror("Parameters are null\n");
        return;
    }
    tilted_rect_reg* parent = malloc(sizeof(tilted_rect_reg));
    parent->core = malloc(sizeof(manhattan_arc));
    parent->radius = wire_length - radius;
    parent->core->x1_coordinate = parent->core->x2_coordinate = node_parent->x_coordinate;
    parent->core->y1_coordinate = parent->core->y2_coordinate = node_parent->y_coordinate;
    parent->core->slope = 0;


    tilted_rect_reg* child = malloc(sizeof(tilted_rect_reg));
    child->core = malloc(sizeof(manhattan_arc));
    child->radius = radius;
    child->core->x1_coordinate = child->core->x2_coordinate = node_child->x_coordinate;
    child->core->y1_coordinate = child->core->y2_coordinate = node_child->y_coordinate;
    child->core->slope = 0;

    inv->trr->core = zero_skew_merge(inv->trr->core, parent, child);
    inv->x_coordinate = inv->trr->core->x1_coordinate;
    inv->y_coordinate = inv->trr->core->y1_coordinate;
    free(parent->core);
    free(child->core);
    free(parent);
    free(child);
    return;
}
