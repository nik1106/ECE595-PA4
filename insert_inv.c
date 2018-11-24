/*
 * insert_inv.c
 *
 *  Created on: Nov 20, 2018
 *      Author: nchhabri
 */

#include "zst.h"

int insert_inv_left(node *parent, node *child)
{
	double temp_wire_len;
	node *new_node;
	double x, y, z;

	x = 1.1 * r * c;

	if(child->node_num != -1) {
		y = 2.2 * r * child->total_cap;
	} else {
		y = 2.2 * r * inv_cin;
	}

	z = -TRANS_TIME_BOUND;

	temp_wire_len = (-y + sqrt(pow(y, 2) - 4.0 * x * z)) / (2.0 * x);

	if(temp_wire_len < parent->left_wire_len) {
		new_node = create_internal_node(-1, child, NULL);

		find_inv_loc(parent, parent->left_wire_len, temp_wire_len, child, new_node);

		new_node->left_wire_len = temp_wire_len;
		new_node->right_wire_len = -1.0;

		parent->left_wire_len -= temp_wire_len;

		new_node->num_node_inv = 1;

		if(parent->right != NULL) {
			parent->total_cap = inv_cin + c * parent->left_wire_len + parent->right->total_cap + c * parent->right_wire_len;
		} else {
			parent->total_cap = inv_cin + c * parent->left_wire_len;
		}

		new_node->next = child->next;
		child->next = new_node;

		parent->left = new_node;

		insert_inv_left(parent, new_node);
	}

	return 0;
}

int insert_inv_right(node *parent, node *child)
{
	double temp_wire_len;
	node *new_node;
	double x, y, z;

	x = 1.1 * r * c;

	if(child->node_num != -1) {
		y = 2.2 * r * child->total_cap;
	} else {
		y = 2.2 * r * inv_cin;
	}

	z = -TRANS_TIME_BOUND;

	temp_wire_len = (-y + sqrt(pow(y, 2) - 4.0 * x * z)) / (2.0 * x);

	if(temp_wire_len < parent->right_wire_len) {
		new_node = create_internal_node(-1, child, NULL);

		find_inv_loc(parent, parent->right_wire_len, temp_wire_len, child, new_node);

		new_node->left_wire_len = temp_wire_len;
		new_node->right_wire_len = -1.0;

		parent->right_wire_len -= temp_wire_len;

		new_node->num_node_inv = 1;

		if(parent->left->node_num != -1) {
			parent->total_cap = parent->left->total_cap + c * parent->left_wire_len + inv_cin + c * parent->right_wire_len;
		} else {
			parent->total_cap = inv_cin + c * parent->left_wire_len + inv_cin + c * parent->right_wire_len;
		}

		new_node->next = child->next;
		child->next = new_node;

		parent->right = new_node;

		insert_inv_right(parent, new_node);
	}

	return 0;
}
