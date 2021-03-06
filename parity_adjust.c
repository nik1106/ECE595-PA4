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
	node *temp;

	if(parent->num_left_inv % 2 == 1 && parent->num_right_inv % 2 == 1) {
		if(parent->num_node_inv == 0) {
			parent->num_node_inv = 1;
			parent->num_left_inv += 1;
			parent->num_right_inv += 1;
		} else {
			new_node = create_internal_node(parent->node_num, parent->left, parent->right);

			new_node->x_coordinate = parent->x_coordinate;
			new_node->y_coordinate = parent->y_coordinate;

			new_node->left_wire_len = parent->left_wire_len;
			new_node->right_wire_len = parent->right_wire_len;

			parent->node_num = -1;

			parent->left_wire_len = 0.0;
			parent->right_wire_len = -1.0;

			new_node->num_node_inv = parent->num_node_inv;
			parent->num_node_inv = 1;

			new_node->num_left_inv = parent->num_left_inv;
			new_node->num_right_inv = parent->num_right_inv;

			parent->num_left_inv = 1;
			parent->num_right_inv = 0;

			new_node->total_cap = parent->total_cap;
			parent->total_cap = parent->num_node_inv * inv_cout + new_node->num_node_inv * inv_cin;

			if(parent->next != NULL) {
				if(parent->next->node_num != -1) {
					if(parent->next->right == parent) {
						if(parent->next->left->node_num != -1 && parent->next->left->num_node_inv == 0) {
							parent->next->total_cap = parent->next->num_node_inv * inv_cout + parent->next->left->total_cap
									+ c * parent->next->left_wire_len + parent->num_node_inv * inv_cin + c * parent->right_wire_len;
						} else {
							parent->next->total_cap = parent->next->num_node_inv * inv_cout + parent->next->left->num_node_inv * inv_cin
									+ c * parent->next->left_wire_len + parent->num_node_inv * inv_cin + c * parent->right_wire_len;
						}
					} else {
						temp = parent->next;
						while(temp->left != parent) {
							temp = temp->next;
						}

						if(temp->right->node_num != -1 && temp->right->num_node_inv == 0) {
							temp->total_cap = temp->num_node_inv * inv_cout + parent->num_node_inv * inv_cin
									+ c * temp->left_wire_len + temp->right->total_cap + c * temp->right_wire_len;
						} else {
							temp->total_cap = temp->num_node_inv * inv_cout + parent->num_node_inv * inv_cin
									+ c * temp->left_wire_len + temp->right->num_node_inv * inv_cin + c * temp->right_wire_len;
						}
					}
				} else {
					parent->next->total_cap = parent->next->num_node_inv * inv_cout + parent->num_node_inv * inv_cin
							+ c * parent->next->left_wire_len;
				}
			}

			new_node->next = parent;
			parent->right->next = new_node;

			parent->left = new_node;
			parent->right = NULL;
		}
	} else if(parent->num_left_inv % 2 == 1 && parent->num_right_inv % 2 == 0) {
		new_node = create_internal_node(-1, parent->left, NULL);

		new_node->x_coordinate = parent->left->x_coordinate;
		new_node->y_coordinate = parent->left->y_coordinate;

		new_node->left_wire_len = 0.0;
		new_node->right_wire_len = -1.0;

		new_node->num_node_inv = 1;

		parent->num_left_inv += 1;

		if(parent->left->node_num != -1 && parent->left->num_node_inv == 0) {
			new_node->total_cap = new_node->num_node_inv * inv_cout + parent->left->total_cap;
		} else {
			new_node->total_cap = new_node->num_node_inv * inv_cout + parent->left->num_node_inv * inv_cin;
		}

		if(parent->right != NULL) {
			if(parent->right->node_num != -1 && parent->right->num_node_inv == 0) {
				parent->total_cap = parent->num_node_inv * inv_cout + new_node->num_node_inv * inv_cin
						+ c * parent->left_wire_len + parent->right->total_cap + c * parent->right_wire_len;
			} else {
				parent->total_cap = parent->num_node_inv * inv_cout + new_node->num_node_inv * inv_cin
						+ c * parent->left_wire_len + parent->right->num_node_inv * inv_cin + c * parent->right_wire_len;
			}
		} else {
			parent->total_cap = parent->num_node_inv * inv_cout + new_node->num_node_inv * inv_cin + c * parent->left_wire_len;
		}

		new_node->next = parent->left->next;
		parent->left->next = new_node;

		parent->left = new_node;
	} else if(parent->num_left_inv % 2 == 0 && parent->num_right_inv % 2 == 1) {
		new_node = create_internal_node(-1, parent->right, NULL);

		new_node->x_coordinate = parent->right->x_coordinate;
		new_node->y_coordinate = parent->right->y_coordinate;

		new_node->left_wire_len = 0.0;
		new_node->right_wire_len = -1.0;

		new_node->num_node_inv = 1;

		parent->num_right_inv += 1;

		if(parent->right->node_num != -1 && parent->right->num_node_inv == 0) {
			new_node->total_cap = new_node->num_node_inv * inv_cout + parent->right->total_cap;
		} else {
			new_node->total_cap = new_node->num_node_inv * inv_cout + parent->right->num_node_inv * inv_cin;
		}

		if(parent->left->node_num != -1 && parent->left->num_node_inv == 0) {
			parent->total_cap = parent->num_node_inv * inv_cout + parent->left->total_cap
					+ c * parent->left_wire_len + new_node->num_node_inv * inv_cin + c * parent->right_wire_len;
		} else {
			parent->total_cap = parent->num_node_inv * inv_cout + parent->left->num_node_inv * inv_cin
					+ c * parent->left_wire_len + new_node->num_node_inv * inv_cin + c * parent->right_wire_len;
		}

		new_node->next = parent->right->next;
		parent->right->next = new_node;

		parent->right = new_node;
	}
}
