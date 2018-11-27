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
	node *new_node = NULL;
	double x, y, z;
	double left_res, left_cap, right_res, right_cap;
	double left_trans_time = 0.0, right_trans_time = 0.0;

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

		parent->num_left_inv += 1;

		if(child->node_num != -1) {
			new_node->total_cap = child->total_cap + c * new_node->left_wire_len;
		} else {
			new_node->total_cap = inv_cin + c * new_node->left_wire_len;
		}

		if(parent->right != NULL) {
			if(parent->right->node_num != -1) {
				parent->total_cap = inv_cin + c * parent->left_wire_len + parent->right->total_cap + c * parent->right_wire_len;
			} else {
				parent->total_cap = inv_cin + c * parent->left_wire_len + inv_cin + c * parent->right_wire_len;
			}
		} else {
			parent->total_cap = inv_cin + c * parent->left_wire_len;
		}

		new_node->next = child->next;
		child->next = new_node;

		parent->left = new_node;

		insert_inv_left(parent, new_node);
	}

	if(child->node_num != -1 && child->num_node_inv == 0) {
		if(child->left != NULL) {
			if(new_node != NULL) {
				left_res = r * (new_node->left_wire_len + child->left_wire_len);

				if(child->left->node_num != -1) {
					left_cap = child->left->total_cap + 0.5 * c * (new_node->left_wire_len + child->left_wire_len);
				} else {
					left_cap = inv_cin + 0.5 * c * (new_node->left_wire_len + child->left_wire_len);
				}
			} else {
				left_res = r * (parent->left_wire_len + child->left_wire_len);

				if(child->left->node_num != -1) {
					left_cap = child->left->total_cap + 0.5 * c * (parent->left_wire_len + child->left_wire_len);
				} else {
					left_cap = inv_cin + 0.5 * c * (parent->left_wire_len + child->left_wire_len);
				}
			}

			left_trans_time = 2.2 * left_res * left_cap;
		}

		if(child->right != NULL) {
			if(new_node != NULL) {
				right_res = r * (new_node->left_wire_len + child->right_wire_len);

				if(child->right->node_num != -1) {
					right_cap = child->right->total_cap + 0.5 * c * (new_node->left_wire_len + child->right_wire_len);
				} else {
					right_cap = inv_cin + 0.5 * c * (new_node->left_wire_len + child->right_wire_len);
				}
			} else {
				right_res = r * (parent->left_wire_len + child->right_wire_len);

				if(child->right->node_num != -1) {
					right_cap = child->right->total_cap + 0.5 * c * (parent->left_wire_len + child->right_wire_len);
				} else {
					right_cap = inv_cin + 0.5 * c * (parent->left_wire_len + child->right_wire_len);
				}
			}

			right_trans_time = 2.2 * right_res * right_cap;
		}

		if(left_trans_time >= TRANS_TIME_BOUND || right_trans_time >= TRANS_TIME_BOUND) {
			child->num_node_inv = 1;
			child->num_left_inv += 1;
			child->num_right_inv += 1;

			parity_adjust(child);

			if(new_node != NULL) {
				new_node->total_cap = inv_cin + c * new_node->left_wire_len;
			} else {
				if(parent->right != NULL) {
					if(parent->right->node_num != -1) {
						parent->total_cap = inv_cin + c * parent->left_wire_len + parent->right->total_cap + c * parent->right_wire_len;
					} else {
						parent->total_cap = inv_cin + c * parent->left_wire_len + inv_cin + c * parent->right_wire_len;
					}
				} else {
					parent->total_cap = inv_cin + c * parent->left_wire_len;
				}
			}
		}
	}

	return 0;
}

int insert_inv_right(node *parent, node *child)
{
	double temp_wire_len;
	node *new_node = NULL;
	double x, y, z;
	double left_res, left_cap, right_res, right_cap;
	double left_trans_time = 0.0, right_trans_time = 0.0;

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

		parent->num_right_inv += 1;

		if(child->node_num != -1) {
			new_node->total_cap = child->total_cap + c * new_node->left_wire_len;
		} else {
			new_node->total_cap = inv_cin + c * new_node->left_wire_len;
		}

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

	if(child->node_num != -1 && child->num_node_inv == 0) {
		if(child->left != NULL) {
			if(new_node != NULL) {
				left_res = r * (new_node->left_wire_len + child->left_wire_len);

				if(child->left->node_num != -1) {
					left_cap = child->left->total_cap + 0.5 * c * (new_node->left_wire_len + child->left_wire_len);
				} else {
					left_cap = inv_cin + 0.5 * c * (new_node->left_wire_len + child->left_wire_len);
				}
			} else {
				left_res = r * (parent->right_wire_len + child->left_wire_len);

				if(child->left->node_num != -1) {
					left_cap = child->left->total_cap + 0.5 * c * (parent->right_wire_len + child->left_wire_len);
				} else {
					left_cap = inv_cin + 0.5 * c * (parent->right_wire_len + child->left_wire_len);
				}
			}

			left_trans_time = 2.2 * left_res * left_cap;
		}

		if(child->right != NULL) {
			if(new_node != NULL) {
				right_res = r * (new_node->left_wire_len + child->right_wire_len);

				if(child->right->node_num != -1) {
					right_cap = child->right->total_cap + 0.5 * c * (new_node->left_wire_len + child->right_wire_len);
				} else {
					right_cap = inv_cin + 0.5 * c * (new_node->left_wire_len + child->right_wire_len);
				}
			} else {
				right_res = r * (parent->right_wire_len + child->right_wire_len);

				if(child->right->node_num != -1) {
					right_cap = child->right->total_cap + 0.5 * c * (parent->right_wire_len + child->right_wire_len);
				} else {
					right_cap = inv_cin + 0.5 * c * (parent->right_wire_len + child->right_wire_len);
				}
			}

			right_trans_time = 2.2 * right_res * right_cap;
		}

		if(left_trans_time >= TRANS_TIME_BOUND || right_trans_time >= TRANS_TIME_BOUND) {
			child->num_node_inv = 1;
			child->num_left_inv += 1;
			child->num_right_inv += 1;

			parity_adjust(child);

			if(new_node != NULL) {
				new_node->total_cap = inv_cin + c * new_node->left_wire_len;
			} else {
				if(parent->left->node_num != -1) {
					parent->total_cap = parent->left->total_cap + c * parent->left_wire_len + inv_cin + c * parent->right_wire_len;
				} else {
					parent->total_cap = inv_cin + c * parent->left_wire_len + inv_cin + c * parent->right_wire_len;
				}
			}
		}
	}

	return 0;
}
