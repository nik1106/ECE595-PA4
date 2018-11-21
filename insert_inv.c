/*
 * insert_inv.c
 *
 *  Created on: Nov 20, 2018
 *      Author: nchhabri
 */

#include "zst.h"

int insert_inv_left(node *parent)
{
	double temp_wire_len;
	node *new_node;

	temp_wire_len = (2.2 * r * c * parent->left_wire_len + 1.1 * r * parent->left->total_cap + sqrt(pow(2.2 * r * c * parent->left_wire_len
			+ 1.1 * r * parent->left->total_cap, 2) - 8.8 * r * c * TRANS_TIME_BOUND)) / (4.4 * r * c);

	if(temp_wire_len <= parent->left_wire_len) {
		new_node = create_internal_node(-1, parent->left, NULL);

		/* Find x_coordinate and y_coordinate of new_node */

		new_node->left_wire_len = parent->left_wire_len - temp_wire_len;
		new_node->right_wire_len = -1.0;

		parent->left_wire_len = temp_wire_len;

		new_node->num_node_inv = 1;

		parent->total_cap = inv_cin + c * temp_wire_len + parent->right->total_cap + c * parent->right_wire_len;

		parent->left = new_node;

		insert_inv_left(new_node);
	} else if(temp_wire_len >= parent->left_wire_len + parent->left->left_wire_len || temp_wire_len >= parent->left_wire_len + parent->left->right_wire_len) {
		parent->left->num_node_inv = 1;

		parent->total_cap = inv_cin + c * parent->left_wire_len;
	}

	return 0;
}

int insert_inv_right(node *parent)
{
	double temp_wire_len;
	node *new_node;

	if(parent->node_num != -1) {
		temp_wire_len = (2.2 * r * c * parent->right_wire_len + 1.1 * r * parent->right->total_cap + sqrt(pow(2.2 * r * c * parent->right_wire_len
				+ 1.1 * r * parent->right->total_cap, 2) - 8.8 * r * c * TRANS_TIME_BOUND)) / (4.4 * r * c);
	} else {
		temp_wire_len = (2.2 * r * c * parent->left_wire_len + 1.1 * r * parent->left->total_cap + sqrt(pow(2.2 * r * c * parent->left_wire_len
				+ 1.1 * r * parent->left->total_cap, 2) - 8.8 * r * c * TRANS_TIME_BOUND)) / (4.4 * r * c);
	}

	if(parent->node_num != -1 && temp_wire_len <= parent->right_wire_len) {
		new_node = create_internal_node(-1, parent->right, NULL);

		/* Find x_coordinate and y_coordinate of new_node */

		new_node->left_wire_len = parent->right_wire_len - temp_wire_len;
		new_node->right_wire_len = -1.0;

		parent->right_wire_len = temp_wire_len;

		new_node->num_node_inv = 1;

		parent->total_cap = parent->left->total_cap + c * parent->left_wire_len + inv_cin + c * temp_wire_len;

		parent->right = new_node;

		insert_inv_right(new_node);
	} else if(parent->node_num == -1 && temp_wire_len <= parent->left_wire_len) {
		new_node = create_internal_node(-1, parent->left, NULL);

		/* Find x_coordinate and y_coordinate of new_node */

		new_node->left_wire_len = parent->left_wire_len - temp_wire_len;
		new_node->right_wire_len = -1.0;

		parent->left_wire_len = temp_wire_len;

		new_node->num_node_inv = 1;

		parent->total_cap = inv_cin + c * temp_wire_len;

		parent->left = new_node;

		insert_inv_right(new_node);
	} else if(parent->node_num != -1 &&
			(temp_wire_len >= parent->right_wire_len + parent->right->left_wire_len || temp_wire_len >= parent->right_wire_len + parent->right->right_wire_len)) {
		parent->right->num_node_inv = 1;

		parent->total_cap = inv_cin + c * parent->right_wire_len;
	} else if(parent->node_num == -1 &&
			(temp_wire_len >= parent->left_wire_len + parent->left->left_wire_len || temp_wire_len >= parent->left_wire_len + parent->left->right_wire_len)) {
		parent->left->num_node_inv = 1;

		parent->total_cap = inv_cin + c * parent->left_wire_len;
	}

	return 0;
}
