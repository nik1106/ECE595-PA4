/*
 * bt_to_list.c
 *
 *  Created on: Nov 21, 2018
 *      Author: wei100
 */
#include "zst.h"
void find_inv_loc(node* center, double radius, node* inv, int mode) 
{
    double parent_x = center->x_coordinate;
    double parent_y = center->y_coordinate;
    //Find y intercepts of the diamond centered at the root node
    double y1 = parent_y - parent_x + radius;
    double y2 = parent_y + parent_x + radius;
    double y3 = parent_y - parent_x - radius;
    double y4 = parent_y + parent_x - radius;

    //If inverter is inserted in the left branch
    if(mode == 0) {
        double child_x = center->left->x_coordinate;
        double child_y = center->left->y_coordinate;
        //Find y intercepts of the diamond centered at the child node
        double y1 = parent_y - parent_x + radius;
        double y2 = parent_y + parent_x + radius;
        double y3 = parent_y - parent_x - radius;
        double y4 = parent_y + parent_x - radius;

    }
    else{
        double parent_x = center->x_coordinate;
        double parent_y = center->y_coordinate;
        //Find y intercepts of the diamond centered at the child node
        double y1 = parent_y - parent_x + radius;
        double y2 = parent_y + parent_x + radius;
        double y3 = parent_y - parent_x - radius;
        double y4 = parent_y + parent_x - radius;
    }
        if(regl.slope == 1) {
            double y4 = Max(yl4, regl.y4);
            (*root)->left->co.y = (y4 + regl.y1)/2;
            (*root)->left->co.x = (*root)->left->co.y - regl.y1;
        }
        else{
            double y3 = Max(yl3, regl.y3);
            (*root)->left->co.y = (y3 + regl.y2) / 2;
            (*root)->left->co.x = regl.y2 - (*root)->left->co.y;
        }
}