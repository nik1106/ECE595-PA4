/*
 * parity_adjust.c
 *
 *  Created on: Nov 26, 2018
 *      Author: nchhabri
 */

#include "zst.h"

void parity_adjust(node *parent)
{
	node *new_node;

	if(parent->num_left_inv % 2 == 1 && parent->num_right_inv % 2 == 1) {
		if(parent->num_node_inv == 0) {
			parent->num_node_inv = 1;
			parent->num_left_inv += 1;
			parent->num_right_inv += 1;
		} else {
			new_node = create_internal_node(-1, parent->left, parent->right);

			new_node->x_coordinate = parent->x_coordinate;
			new_node->y_coordinate = parent->y_coordinate;

			new_node->left_wire_len = parent->left_wire_len;
			new_node->right_wire_len = parent->right_wire_len;

			parent->left_wire_len = 0.0;
			parent->right_wire_len = -1.0;

			new_node->num_node_inv = 1;

			new_node->total_cap = parent->total_cap;
			parent->total_cap = inv_cin;

			new_node->next = parent;

			parent->left = new_node;
			parent->right = NULL;
		}
	} else if(parent->num_left_inv % 2 == 1 && parent->num_right_inv % 2 == 0) {
		if(parent->left->node_num == -1) {
			new_node = create_internal_node(-1, parent->left, NULL);

			new_node->x_coordinate = parent->left->x_coordinate;
			new_node->y_coordinate = parent->left->y_coordinate;

			new_node->left_wire_len = 0.0;
			new_node->right_wire_len = -1.0;

			new_node->num_node_inv = 1;

			parent->num_left_inv += 1;

			new_node->total_cap = inv_cin;

			new_node->next = parent->left->next;
			parent->left->next = new_node;

			parent->left = new_node;
		} else {
			new_node = create_internal_node(-1, parent->left, NULL);

			find_inv_loc(parent, parent->left_wire_len, parent->left_wire_len / 2, parent->left, new_node);

			new_node->left_wire_len = parent->left_wire_len / 2;
			new_node->right_wire_len = -1.0;

			parent->left_wire_len = parent->left_wire_len / 2;

			new_node->num_node_inv = 1;

			parent->num_left_inv += 1;

			new_node->total_cap = parent->left->total_cap + c * new_node->left_wire_len;

			if(parent->right != NULL) {
				if(parent->right->node_num != -1) {
					parent->total_cap = inv_cin + c * parent->left_wire_len + parent->right->total_cap + c * parent->right_wire_len;
				} else {
					parent->total_cap = inv_cin + c * parent->left_wire_len + inv_cin + c * parent->right_wire_len;
				}
			} else {
				parent->total_cap = inv_cin + c * parent->left_wire_len;
			}

			new_node->next = parent->left->next;
			parent->left->next = new_node;

			parent->left = new_node;
		}
	} else if(parent->num_left_inv % 2 == 0 && parent->num_right_inv % 2 == 1) {
		if(parent->right->node_num == -1) {
			new_node = create_internal_node(-1, parent->right, NULL);

			new_node->x_coordinate = parent->right->x_coordinate;
			new_node->y_coordinate = parent->right->y_coordinate;

			new_node->left_wire_len = 0.0;
			new_node->right_wire_len = -1.0;

			new_node->num_node_inv = 1;

			parent->num_right_inv += 1;

			new_node->total_cap = inv_cin;

			new_node->next = parent->right->next;
			parent->right->next = new_node;

			parent->right = new_node;
		} else {
			new_node = create_internal_node(-1, parent->right, NULL);

			find_inv_loc(parent, parent->right_wire_len, parent->right_wire_len / 2, parent->right, new_node);

			new_node->left_wire_len = parent->right_wire_len / 2;
			new_node->right_wire_len = -1.0;

			parent->right_wire_len = parent->right_wire_len / 2;

			new_node->num_node_inv = 1;

			parent->num_right_inv += 1;

			new_node->total_cap = parent->right->total_cap + c * new_node->left_wire_len;

			if(parent->left->node_num != -1) {
				parent->total_cap = parent->left->total_cap + c * parent->left_wire_len + inv_cin + c * parent->right_wire_len;
			} else {
				parent->total_cap = inv_cin + c * parent->left_wire_len + inv_cin + c * parent->right_wire_len;
			}

			new_node->next = parent->right->next;
			parent->right->next = new_node;

			parent->right = new_node;
		}
	}
}
