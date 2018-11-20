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

		new_node->num_node_inv = 1;
		new_node->num_left_inv = parent->left->num_left_inv + parent->left->num_right_inv + 1;
		parent->num_left_inv = new_node->num_left_inv;

		/* Recalculate total_cap for parent */

		parent->left = new_node;

		insert_inv_left(new_node);
	} else if(temp_wire_len >= parent->left_wire_len + parent->left->left_wire_len || temp_wire_len >= parent->left_wire_len + parent->left->right_wire_len) {
		parent->left->num_node_inv = 1;
		parent->left->num_left_inv += 1;
		parent->left->num_right_inv += 1;

		parent->num_left_inv += 1;
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

		new_node->num_node_inv = 1;
		new_node->num_left_inv = parent->right->num_left_inv + parent->right->num_right_inv + 1;
		parent->num_right_inv = new_node->num_left_inv;

		/* Recalculate total_cap for parent */

		parent->right = new_node;

		insert_inv_right(new_node);
	} else if(parent->node_num == -1 && temp_wire_len <= parent->left_wire_len) {
		new_node = create_internal_node(-1, parent->left, NULL);

		/* Find x_coordinate and y_coordinate of new_node */

		new_node->left_wire_len = parent->left_wire_len - temp_wire_len;
		new_node->right_wire_len = -1.0;

		new_node->num_node_inv = 1;
		new_node->num_left_inv = parent->left->num_left_inv + parent->left->num_right_inv + 1;
		parent->num_left_inv = new_node->num_left_inv;

		/* Recalculate total_cap for parent */

		parent->left = new_node;

		insert_inv_right(new_node);
	} else if(parent->node_num != -1 &&
			(temp_wire_len >= parent->right_wire_len + parent->right->left_wire_len || temp_wire_len >= parent->right_wire_len + parent->right->right_wire_len)) {
		parent->right->num_node_inv = 1;
		parent->right->num_left_inv += 1;
		parent->right->num_right_inv += 1;

		parent->num_right_inv += 1;
	} else if(parent->node_num == -1 &&
			(temp_wire_len >= parent->left_wire_len + parent->left->left_wire_len || temp_wire_len >= parent->left_wire_len + parent->left->right_wire_len)) {
		parent->left->num_node_inv = 1;
		parent->left->num_left_inv += 1;
		parent->left->num_right_inv += 1;

		parent->num_left_inv += 1;
	}

	return 0;
}
