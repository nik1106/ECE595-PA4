/*
 * zst.c
 *
 *  Created on: Oct 25, 2018
 *      Author: nchhabri
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <float.h>
#include "zst.h"

node *read_sink_input_file(char filename[])
{
	FILE *fp;
	char first_char;
	int leaf_node_label;
	double sink_node_cap, x_coordinate, y_coordinate;
	int node_num = 0;
	node *head = NULL;

	fp = fopen(filename, "r");
	if(fp == NULL) {
		return head;
	}

	first_char = fgetc(fp);
	while(first_char != EOF) {
		/* Move the file pointer back by 1 character */
		fseek(fp, -1L, SEEK_CUR);

		fscanf(fp, "%d(%le %le %le)\n", &leaf_node_label, &sink_node_cap, &x_coordinate, &y_coordinate);

		node_num += 1;

		head = insert_end(head, leaf_node_label, sink_node_cap, x_coordinate, y_coordinate, node_num);

		first_char = fgetc(fp);
	}

	global_node_num = node_num;

	fclose(fp);

	return head;
}

node *insert_end(node *head, int leaf_node_label, double sink_node_cap, double x_coordinate, double y_coordinate, int node_num)
{
	static node *tail;

	if(head == NULL) {
		head = malloc(sizeof(node));
		head->trr = malloc(sizeof(tilted_rect_reg));
		head->trr->core = malloc(sizeof(manhattan_arc));

		head->leaf_node_label = leaf_node_label;
		head->sink_node_cap = sink_node_cap;
		head->x_coordinate = x_coordinate;
		head->y_coordinate = y_coordinate;
		head->left_wire_len = 0.0;
		head->right_wire_len = 0.0;
		head->total_cap = sink_node_cap;
		head->delay = 0.0;
		head->max_delay = 0.0;
		head->min_delay = 0.0;
		head->num_node_inv = 0;
		head->num_left_inv = 0;
		head->num_right_inv = 0;
		head->node_num = node_num;

		head->trr->core->x1_coordinate = x_coordinate;
		head->trr->core->y1_coordinate = y_coordinate;
		head->trr->core->x2_coordinate = x_coordinate;
		head->trr->core->y2_coordinate = y_coordinate;
		head->trr->core->slope = 0;
		head->trr->core->row_num1 = 0;
		head->trr->core->col_num1 = 0;
		head->trr->core->bucket_num1 = 0;
		head->trr->core->row_num2 = 0;
		head->trr->core->col_num2 = 0;
		head->trr->core->bucket_num2 = 0;
		head->trr->radius = 0.0;

		head->is_left_done = true;
		head->is_right_done = true;

		head->nearest_neighbor = NULL;
		head->nearest_neighbor_dist = 0.0;

		head->left = NULL;
		head->right = NULL;

		head->next = NULL;
		head->bucket_next = NULL;

		tail = head;

		return head;
	}

	tail->next = malloc(sizeof(node));
	tail->next->trr = malloc(sizeof(tilted_rect_reg));
	tail->next->trr->core = malloc(sizeof(manhattan_arc));

	tail->next->leaf_node_label = leaf_node_label;
	tail->next->sink_node_cap = sink_node_cap;
	tail->next->x_coordinate = x_coordinate;
	tail->next->y_coordinate = y_coordinate;
	tail->next->left_wire_len = 0.0;
	tail->next->right_wire_len = 0.0;
	tail->next->total_cap = sink_node_cap;
	tail->next->delay = 0.0;
	tail->next->max_delay = 0.0;
	tail->next->min_delay = 0.0;
	tail->next->num_node_inv = 0;
	tail->next->num_left_inv = 0;
	tail->next->num_right_inv = 0;
	tail->next->node_num = node_num;

	tail->next->trr->core->x1_coordinate = x_coordinate;
	tail->next->trr->core->y1_coordinate = y_coordinate;
	tail->next->trr->core->x2_coordinate = x_coordinate;
	tail->next->trr->core->y2_coordinate = y_coordinate;
	tail->next->trr->core->slope = 0;
	tail->next->trr->core->row_num1 = 0;
	tail->next->trr->core->col_num1 = 0;
	tail->next->trr->core->bucket_num1 = 0;
	tail->next->trr->core->row_num2 = 0;
	tail->next->trr->core->col_num2 = 0;
	tail->next->trr->core->bucket_num2 = 0;
	tail->next->trr->radius = 0.0;

	tail->next->is_left_done = true;
	tail->next->is_right_done = true;

	tail->next->nearest_neighbor = NULL;
	tail->next->nearest_neighbor_dist = 0.0;

	tail->next->left = NULL;
	tail->next->right = NULL;

	tail->next->next = NULL;
	tail->next->bucket_next = NULL;

	tail = tail->next;

	return head;
}

double s(double x, double y, double z)
{
	if(x >= y) {
		return x;
	} else if(x < y && y < z) {
		return y;
	} else {
		return z;
	}
}

double find_diameter(node *head)
{
	/* max1 = max(x[i] + y[i]), max2 = max(x[i] - y[i]), max3 = max(-x[i] + y[i]), max4 = max(-x[i] - y[i]) */
	double max1 = -DBL_MAX, max2 = -DBL_MAX, max3 = -DBL_MAX, max4 = -DBL_MAX;
	double max1_x = -DBL_MAX, max1_y = -DBL_MAX, max2_x = -DBL_MAX, max2_y = -DBL_MAX, max3_x = -DBL_MAX, max3_y = -DBL_MAX,
			max4_x = -DBL_MAX, max4_y = -DBL_MAX;
	double temp_distance;
	double diameter = -DBL_MAX;
	node *current = head;

	while(current != NULL) {
		if(current->x_coordinate + current->y_coordinate > max1) {
			max1 = current->x_coordinate + current->y_coordinate;
			max1_x = current->x_coordinate;
			max1_y = current->y_coordinate;
		}

		if(current->x_coordinate - current->y_coordinate > max2) {
			max2 = current->x_coordinate - current->y_coordinate;
			max2_x = current->x_coordinate;
			max2_y = current->y_coordinate;
		}

		if(-current->x_coordinate + current->y_coordinate > max3) {
			max3 = -current->x_coordinate + current->y_coordinate;
			max3_x = current->x_coordinate;
			max3_y = current->y_coordinate;
		}

		if(-current->x_coordinate - current->y_coordinate > max4) {
			max4 = -current->x_coordinate - current->y_coordinate;
			max4_x = current->x_coordinate;
			max4_y = current->y_coordinate;
		}

		current = current->next;
	}

	current = head;

	while(current != NULL) {
		temp_distance = fabs(current->x_coordinate - max1_x) + fabs(current->y_coordinate - max1_y);
		if(temp_distance > diameter) {
			diameter = temp_distance;
		}

		temp_distance = fabs(current->x_coordinate - max2_x) + fabs(current->y_coordinate - max2_y);
		if(temp_distance > diameter) {
			diameter = temp_distance;
		}

		temp_distance = fabs(current->x_coordinate - max3_x) + fabs(current->y_coordinate - max3_y);
		if(temp_distance > diameter) {
			diameter = temp_distance;
		}

		temp_distance = fabs(current->x_coordinate - max4_x) + fabs(current->y_coordinate - max4_y);
		if(temp_distance > diameter) {
			diameter = temp_distance;
		}

		current = current->next;
	}

	square_edge1_y_intercept = max4_x + max4_y;
	square_edge2_y_intercept = max3_y - max3_x;

	return diameter;
}

double calculate_bucket_size(double diameter, double num_elements, int *d)
{
	*d = ceil(sqrt(s(2.0, 0.5 * num_elements + 1, num_elements)) - 1);

	return diameter / *d;
}

int find_num_elements(node *head)
{
	int num_elements = 0;

	while(head != NULL) {
		num_elements += 1;
		head = head->next;
	}

	return num_elements;
}

bucket *create_bucket_array(node *head, int num_buckets, int d, double bucket_size)
{
	bucket *bucket_array = calloc(num_buckets, sizeof(bucket));
	double dist1, dist2;
	int row_num, col_num;

	while(head != NULL) {
		dist1 = fabs(-head->trr->core->x1_coordinate - head->trr->core->y1_coordinate
				+ square_edge1_y_intercept) / sqrt(2);
		dist2 = fabs(head->trr->core->x1_coordinate - head->trr->core->y1_coordinate
				+ square_edge2_y_intercept) / sqrt(2);

		row_num = ceil(dist2 / bucket_size);
		col_num = ceil(dist1 / bucket_size);

		if(row_num == 0 && col_num == 0) {
			head->trr->core->row_num1 = 1;
			head->trr->core->col_num1 = 1;
			head->trr->core->bucket_num1 = 1;
		} else if(row_num == 0 && col_num != 0) {
			head->trr->core->row_num1 = 1;
			head->trr->core->col_num1 = col_num;
			head->trr->core->bucket_num1 = col_num;
		} else if(row_num != 0 && col_num == 0) {
			head->trr->core->row_num1 = row_num;
			head->trr->core->col_num1 = 1;
			head->trr->core->bucket_num1 = (row_num - 1) * d + 1;
		} else {
			head->trr->core->row_num1 = row_num;
			head->trr->core->col_num1 = col_num;
			head->trr->core->bucket_num1 = (row_num - 1) * d + col_num;
		}

		bucket_array[head->trr->core->bucket_num1 - 1].head = insert_bucket_list(bucket_array[head->trr->core->bucket_num1 - 1].head, head);

		dist1 = fabs(-head->trr->core->x2_coordinate - head->trr->core->y2_coordinate
				+ square_edge1_y_intercept) / sqrt(2);
		dist2 = fabs(head->trr->core->x2_coordinate - head->trr->core->y2_coordinate
				+ square_edge2_y_intercept) / sqrt(2);

		row_num = ceil(dist2 / bucket_size);
		col_num = ceil(dist1 / bucket_size);

		if(row_num == 0 && col_num == 0) {
			head->trr->core->row_num2 = 1;
			head->trr->core->col_num2 = 1;
			head->trr->core->bucket_num2 = 1;
		} else if(row_num == 0 && col_num != 0) {
			head->trr->core->row_num2 = 1;
			head->trr->core->col_num2 = col_num;
			head->trr->core->bucket_num2 = col_num;
		} else if(row_num != 0 && col_num == 0) {
			head->trr->core->row_num2 = row_num;
			head->trr->core->col_num2 = 1;
			head->trr->core->bucket_num2 = (row_num - 1) * d + 1;
		} else {
			head->trr->core->row_num2 = row_num;
			head->trr->core->col_num2 = col_num;
			head->trr->core->bucket_num2 = (row_num - 1) * d + col_num;
		}

		if(head->trr->core->bucket_num2 != head->trr->core->bucket_num1) {
			bucket_array[head->trr->core->bucket_num2 - 1].head = insert_bucket_list(bucket_array[head->trr->core->bucket_num2 - 1].head,
					head);
		}

		head = head->next;
	}

	return bucket_array;
}

node *insert_bucket_list(node *head, node *new_node)
{
		if(head == NULL) {
			head = new_node;
			head->bucket_next = NULL;
			return head;
		}

		node *current = head;

		while(current->bucket_next != NULL) {
			current = current->bucket_next;
		}

		current->bucket_next = new_node;
		current->bucket_next->bucket_next = NULL;

		return head;
}

double shortest_distance(manhattan_arc *man_arc1, manhattan_arc *man_arc2)
{
	double shortest_dist;
	double temp_shortest_dist;
	double temp_y_intercept;
	double temp_x_coordinate;

	if(man_arc1->slope != man_arc2->slope && man_arc1->slope != 0 && man_arc2->slope != 0) {
		if(do_intersect(man_arc1->x1_coordinate, man_arc1->y1_coordinate, man_arc1->x2_coordinate, man_arc1->y2_coordinate,
				man_arc2->x1_coordinate, man_arc2->y1_coordinate, man_arc2->x2_coordinate, man_arc2->y2_coordinate)) {
			shortest_dist = 0.0;
			return shortest_dist;
		} else if((man_arc1->y1_coordinate > man_arc2->y2_coordinate && man_arc1->y1_coordinate < man_arc2->y1_coordinate) ||
					(man_arc1->y1_coordinate > man_arc2->y1_coordinate && man_arc1->y1_coordinate < man_arc2->y2_coordinate) ||
					(man_arc1->y2_coordinate > man_arc2->y2_coordinate && man_arc1->y2_coordinate < man_arc2->y1_coordinate) ||
					(man_arc1->y2_coordinate > man_arc2->y1_coordinate && man_arc1->y2_coordinate < man_arc2->y2_coordinate) ||
					(man_arc2->y1_coordinate > man_arc1->y1_coordinate && man_arc2->y1_coordinate < man_arc1->y2_coordinate) ||
					(man_arc2->y2_coordinate > man_arc1->y1_coordinate && man_arc2->y2_coordinate < man_arc1->y2_coordinate) ||
					(man_arc2->y1_coordinate > man_arc1->y2_coordinate && man_arc2->y1_coordinate < man_arc1->y1_coordinate) ||
					(man_arc2->y2_coordinate > man_arc1->y2_coordinate && man_arc2->y2_coordinate < man_arc1->y1_coordinate)) {
			temp_y_intercept = man_arc1->y1_coordinate - man_arc1->slope * man_arc1->x1_coordinate;

			temp_x_coordinate = (man_arc2->y1_coordinate - temp_y_intercept) / man_arc1->slope;
			temp_shortest_dist = fabs(man_arc2->x1_coordinate - temp_x_coordinate);
			if(is_point_on_line_segment(temp_x_coordinate, man_arc2->y1_coordinate,
					man_arc1->x1_coordinate, man_arc1->y1_coordinate, man_arc1->x2_coordinate, man_arc1->y2_coordinate)) {
				shortest_dist = temp_shortest_dist;
			} else {
				shortest_dist = DBL_MAX;
			}

			temp_x_coordinate = (man_arc2->y2_coordinate - temp_y_intercept) / man_arc1->slope;
			temp_shortest_dist = fabs(man_arc2->x2_coordinate - temp_x_coordinate);
			if(is_point_on_line_segment(temp_x_coordinate, man_arc2->y2_coordinate,
					man_arc1->x1_coordinate, man_arc1->y1_coordinate, man_arc1->x2_coordinate, man_arc1->y2_coordinate) &&
					temp_shortest_dist < shortest_dist) {
				shortest_dist = temp_shortest_dist;
			}

			temp_y_intercept = man_arc2->y1_coordinate - man_arc2->slope * man_arc2->x1_coordinate;

			temp_x_coordinate = (man_arc1->y1_coordinate - temp_y_intercept) / man_arc2->slope;
			temp_shortest_dist = fabs(man_arc1->x1_coordinate - temp_x_coordinate);
			if(is_point_on_line_segment(temp_x_coordinate, man_arc1->y1_coordinate,
					man_arc2->x1_coordinate, man_arc2->y1_coordinate, man_arc2->x2_coordinate, man_arc2->y2_coordinate) &&
					temp_shortest_dist < shortest_dist) {
				shortest_dist = temp_shortest_dist;
			}

			temp_x_coordinate = (man_arc1->y2_coordinate - temp_y_intercept) / man_arc2->slope;
			temp_shortest_dist = fabs(man_arc1->x2_coordinate - temp_x_coordinate);
			if(is_point_on_line_segment(temp_x_coordinate, man_arc1->y2_coordinate,
					man_arc2->x1_coordinate, man_arc2->y1_coordinate, man_arc2->x2_coordinate, man_arc2->y2_coordinate) &&
					temp_shortest_dist < shortest_dist) {
				shortest_dist = temp_shortest_dist;
			}

			return shortest_dist;
		}
	}

	else if(man_arc1->slope == 0 && (man_arc2->slope == 1 || man_arc2->slope == -1)) {
		if((man_arc1->y1_coordinate < man_arc2->y2_coordinate && man_arc1->y1_coordinate > man_arc2->y1_coordinate) ||
				(man_arc1->y1_coordinate < man_arc2->y1_coordinate && man_arc1->y1_coordinate > man_arc2->y2_coordinate)) {
			temp_y_intercept = man_arc2->y1_coordinate - man_arc2->slope * man_arc2->x1_coordinate;
			temp_x_coordinate = (man_arc1->y1_coordinate - temp_y_intercept) / man_arc2->slope;
			shortest_dist = fabs(man_arc1->x1_coordinate - temp_x_coordinate);
			return shortest_dist;
		}
	}

	else if(man_arc2->slope == 0 && (man_arc1->slope == 1 || man_arc1->slope == -1)) {
		if((man_arc2->y1_coordinate < man_arc1->y2_coordinate && man_arc2->y1_coordinate > man_arc1->y1_coordinate) ||
				(man_arc2->y1_coordinate < man_arc1->y1_coordinate && man_arc2->y1_coordinate > man_arc1->y2_coordinate)) {
			temp_y_intercept = man_arc1->y1_coordinate - man_arc1->slope * man_arc1->x1_coordinate;
			temp_x_coordinate = (man_arc2->y1_coordinate - temp_y_intercept) / man_arc1->slope;
			shortest_dist = fabs(man_arc2->x1_coordinate - temp_x_coordinate);
			return shortest_dist;
		}
	}

	else if(man_arc1->slope == 1 && man_arc2->slope == 1) {
		if(man_arc1->x1_coordinate < man_arc1->x2_coordinate && man_arc2->x1_coordinate < man_arc2->x2_coordinate) {
			if(man_arc1->y2_coordinate < man_arc2->y2_coordinate && man_arc1->y2_coordinate > man_arc2->y1_coordinate) {
				temp_y_intercept = man_arc2->y1_coordinate - man_arc2->x1_coordinate;
				temp_x_coordinate = man_arc1->y2_coordinate - temp_y_intercept;
				shortest_dist = fabs(man_arc1->x2_coordinate - temp_x_coordinate);
				return shortest_dist;
			}

			else if(man_arc1->y2_coordinate > man_arc2->y2_coordinate && man_arc2->y2_coordinate > man_arc1->y1_coordinate) {
				temp_y_intercept = man_arc1->y1_coordinate - man_arc1->x1_coordinate;
				temp_x_coordinate = man_arc2->y2_coordinate - temp_y_intercept;
				shortest_dist = fabs(man_arc2->x2_coordinate - temp_x_coordinate);
				return shortest_dist;
			}
		} else if(man_arc1->x1_coordinate < man_arc1->x2_coordinate && man_arc2->x1_coordinate > man_arc2->x2_coordinate) {
			if(man_arc1->y2_coordinate < man_arc2->y1_coordinate && man_arc1->y2_coordinate > man_arc2->y2_coordinate) {
				temp_y_intercept = man_arc2->y1_coordinate - man_arc2->x1_coordinate;
				temp_x_coordinate = man_arc1->y2_coordinate - temp_y_intercept;
				shortest_dist = fabs(man_arc1->x2_coordinate - temp_x_coordinate);
				return shortest_dist;
			}

			else if(man_arc1->y2_coordinate > man_arc2->y1_coordinate && man_arc2->y1_coordinate > man_arc1->y1_coordinate) {
				temp_y_intercept = man_arc1->y1_coordinate - man_arc1->x1_coordinate;
				temp_x_coordinate = man_arc2->y1_coordinate - temp_y_intercept;
				shortest_dist = fabs(man_arc2->x1_coordinate - temp_x_coordinate);
				return shortest_dist;
			}
		} else if(man_arc1->x1_coordinate > man_arc1->x2_coordinate && man_arc2->x1_coordinate < man_arc2->x2_coordinate) {
			if(man_arc1->y1_coordinate < man_arc2->y2_coordinate && man_arc1->y1_coordinate > man_arc2->y1_coordinate) {
				temp_y_intercept = man_arc2->y1_coordinate - man_arc2->x1_coordinate;
				temp_x_coordinate = man_arc1->y1_coordinate - temp_y_intercept;
				shortest_dist = fabs(man_arc1->x1_coordinate - temp_x_coordinate);
				return shortest_dist;
			}

			else if(man_arc1->y1_coordinate > man_arc2->y2_coordinate && man_arc2->y2_coordinate > man_arc1->y2_coordinate) {
				temp_y_intercept = man_arc1->y1_coordinate - man_arc1->x1_coordinate;
				temp_x_coordinate = man_arc2->y2_coordinate - temp_y_intercept;
				shortest_dist = fabs(man_arc2->x2_coordinate - temp_x_coordinate);
				return shortest_dist;
			}
		} else if(man_arc1->x1_coordinate > man_arc1->x2_coordinate && man_arc2->x1_coordinate > man_arc2->x2_coordinate) {
			if(man_arc1->y1_coordinate < man_arc2->y1_coordinate && man_arc1->y1_coordinate > man_arc2->y2_coordinate) {
				temp_y_intercept = man_arc2->y1_coordinate - man_arc2->x1_coordinate;
				temp_x_coordinate = man_arc1->y1_coordinate - temp_y_intercept;
				shortest_dist = fabs(man_arc1->x1_coordinate - temp_x_coordinate);
				return shortest_dist;
			}

			else if(man_arc1->y1_coordinate > man_arc2->y1_coordinate && man_arc2->y1_coordinate > man_arc1->y2_coordinate) {
				temp_y_intercept = man_arc1->y1_coordinate - man_arc1->x1_coordinate;
				temp_x_coordinate = man_arc2->y1_coordinate - temp_y_intercept;
				shortest_dist = fabs(man_arc2->x1_coordinate - temp_x_coordinate);
				return shortest_dist;
			}
		}
	}

	else if(man_arc1->slope == -1 && man_arc2->slope == -1) {
		if(man_arc1->x1_coordinate < man_arc1->x2_coordinate && man_arc2->x1_coordinate < man_arc2->x2_coordinate) {
			if(man_arc1->y1_coordinate < man_arc2->y1_coordinate && man_arc1->y1_coordinate > man_arc2->y2_coordinate) {
				temp_y_intercept = man_arc2->x1_coordinate + man_arc2->y1_coordinate;
				temp_x_coordinate = temp_y_intercept - man_arc1->y1_coordinate;
				shortest_dist = fabs(man_arc1->x1_coordinate - temp_x_coordinate);
				return shortest_dist;
			}

			else if(man_arc1->y1_coordinate > man_arc2->y1_coordinate && man_arc2->y1_coordinate > man_arc1->y2_coordinate) {
				temp_y_intercept = man_arc1->x1_coordinate + man_arc1->y1_coordinate;
				temp_x_coordinate = temp_y_intercept - man_arc2->y1_coordinate;
				shortest_dist = fabs(man_arc2->x1_coordinate - temp_x_coordinate);
				return shortest_dist;
			}
		} else if(man_arc1->x1_coordinate < man_arc1->x2_coordinate && man_arc2->x1_coordinate > man_arc2->x2_coordinate) {
			if(man_arc1->y1_coordinate < man_arc2->y2_coordinate && man_arc1->y1_coordinate > man_arc2->y1_coordinate) {
				temp_y_intercept = man_arc2->x1_coordinate + man_arc2->y1_coordinate;
				temp_x_coordinate = temp_y_intercept - man_arc1->y1_coordinate;
				shortest_dist = fabs(man_arc1->x1_coordinate - temp_x_coordinate);
				return shortest_dist;
			}

			else if(man_arc1->y1_coordinate > man_arc2->y2_coordinate && man_arc2->y2_coordinate > man_arc1->y2_coordinate) {
				temp_y_intercept = man_arc1->x1_coordinate + man_arc1->y1_coordinate;
				temp_x_coordinate = temp_y_intercept - man_arc2->y2_coordinate;
				shortest_dist = fabs(man_arc2->x2_coordinate - temp_x_coordinate);
				return shortest_dist;
			}
		} else if(man_arc1->x1_coordinate > man_arc1->x2_coordinate && man_arc2->x1_coordinate < man_arc2->x2_coordinate) {
			if(man_arc1->y2_coordinate < man_arc2->y1_coordinate && man_arc1->y2_coordinate > man_arc2->y2_coordinate) {
				temp_y_intercept = man_arc2->x1_coordinate + man_arc2->y1_coordinate;
				temp_x_coordinate = temp_y_intercept - man_arc1->y2_coordinate;
				shortest_dist = fabs(man_arc1->x2_coordinate - temp_x_coordinate);
				return shortest_dist;
			}

			else if(man_arc1->y2_coordinate > man_arc2->y1_coordinate && man_arc2->y1_coordinate > man_arc1->y1_coordinate) {
				temp_y_intercept = man_arc1->x1_coordinate + man_arc1->y1_coordinate;
				temp_x_coordinate = temp_y_intercept - man_arc2->y1_coordinate;
				shortest_dist = fabs(man_arc2->x1_coordinate - temp_x_coordinate);
				return shortest_dist;
			}
		} else if(man_arc1->x1_coordinate > man_arc1->x2_coordinate && man_arc2->x1_coordinate > man_arc2->x2_coordinate) {
			if(man_arc1->y2_coordinate < man_arc2->y2_coordinate && man_arc1->y2_coordinate > man_arc2->y1_coordinate) {
				temp_y_intercept = man_arc2->x1_coordinate + man_arc2->y1_coordinate;
				temp_x_coordinate = temp_y_intercept - man_arc1->y2_coordinate;
				shortest_dist = fabs(man_arc1->x2_coordinate - temp_x_coordinate);
				return shortest_dist;
			}

			else if(man_arc1->y2_coordinate > man_arc2->y2_coordinate && man_arc2->y2_coordinate > man_arc1->y1_coordinate) {
				temp_y_intercept = man_arc1->x1_coordinate + man_arc1->y1_coordinate;
				temp_x_coordinate = temp_y_intercept - man_arc2->y2_coordinate;
				shortest_dist = fabs(man_arc2->x2_coordinate - temp_x_coordinate);
				return shortest_dist;
			}
		}
	}

	temp_shortest_dist = fabs(man_arc1->x1_coordinate - man_arc2->x1_coordinate) + fabs(man_arc1->y1_coordinate - man_arc2->y1_coordinate);
	shortest_dist = temp_shortest_dist;

	temp_shortest_dist = fabs(man_arc1->x1_coordinate - man_arc2->x2_coordinate) + fabs(man_arc1->y1_coordinate - man_arc2->y2_coordinate);
	if(temp_shortest_dist < shortest_dist) {
		shortest_dist = temp_shortest_dist;
	}

	temp_shortest_dist = fabs(man_arc1->x2_coordinate - man_arc2->x1_coordinate) + fabs(man_arc1->y2_coordinate - man_arc2->y1_coordinate);
	if(temp_shortest_dist < shortest_dist) {
		shortest_dist = temp_shortest_dist;
	}

	temp_shortest_dist = fabs(man_arc1->x2_coordinate - man_arc2->x2_coordinate) + fabs(man_arc1->y2_coordinate - man_arc2->y2_coordinate);
	if(temp_shortest_dist < shortest_dist) {
		shortest_dist = temp_shortest_dist;
	}

	return shortest_dist;
}

node *calculate_edge_lengths(node *internal_node, double shortest_dist)
{
	double x = (internal_node->right->delay - internal_node->left->delay + r * shortest_dist * (internal_node->right->total_cap +
			0.5 * c * shortest_dist)) / (r * (internal_node->left->total_cap + internal_node->right->total_cap + c * shortest_dist));

	if(x >= 0.0 && x <= shortest_dist) {
		internal_node->left_wire_len = x;
		internal_node->right_wire_len = shortest_dist - x;
	} else if(x < 0.0) {
		internal_node->left_wire_len = 0.0;
		internal_node->right_wire_len = (sqrt(pow(r * internal_node->right->total_cap, 2) + 2.0 * r * c * (internal_node->left->delay -
				internal_node->right->delay)) - r * internal_node->right->total_cap) / (r * c);
	} else {
		internal_node->left_wire_len = (sqrt(pow(r * internal_node->left->total_cap, 2) + 2.0 * r * c * (internal_node->right->delay -
				internal_node->left->delay)) - r * internal_node->left->total_cap) / (r * c);
		internal_node->right_wire_len = 0.0;
	}

	return internal_node;
}

/* Given three collinear points p, q, and r, this utility function checks whether point p lies on line segment 'qr' */
bool is_point_on_line_segment(double p_x, double p_y, double q_x, double q_y, double r_x, double r_y)
{
	double left, right, top, bottom;

	/* For left and right of bounding box */
	if(q_x < r_x) {
		left = q_x;
		right = r_x;
	} else {
		left = r_x;
		right = q_x;
	}

	/* For top and bottom of bounding box */
	if(q_y < r_y) {
		top = r_y;
		bottom = q_y;
	} else {
		top = q_y;
		bottom = r_y;
	}

	if(p_x >= left && p_x<= right && p_y <= top && p_y >= bottom) {
		return true;
	} else {
		return false;
	}
}

/* A utility function to check whether two line segments 'p1q1' and 'p2q2' intersect */
bool do_intersect(double p1_x, double p1_y, double q1_x, double q1_y, double p2_x, double p2_y, double q2_x, double q2_y)
{
	int orient1 = orientation(p1_x, p1_y, q1_x, q1_y, p2_x, p2_y);
	int orient2 = orientation(p1_x, p1_y, q1_x, q1_y, q2_x, q2_y);
	int orient3 = orientation(p2_x, p2_y, q2_x, q2_y, p1_x, p1_y);
	int orient4 = orientation(p2_x, p2_y, q2_x, q2_y, q1_x, q1_y);

	if(orient1 != orient2 && orient3 != orient4) {
		return true;
	} else {
		return false;
	}
}

int orientation(double x1, double y1, double x2, double y2, double x3, double y3)
{
	double val = (y2 - y1) * (x3 - x2) - (y3 - y2) * (x2 - x1);

	/* If fabs(val) < EPSILON, return 0 => points (x1, y1), (x2, y2) and (x3, y3) are collinear
	 * Else if val > 0.0, return 1 => points (x1, y1), (x2, y2) and (x3, y3) are clockwise
	 * Else, return 2 => points (x1, y1), (x2, y2) and (x3, y3) are counterclockwise */
	if(fabs(val) < EPSILON) {
		return 0;
	} else if(val > 0.0) {
		return 1;
	} else {
		return 2;
	}
}

node *construct_nearest_neighbor_graph(node *head, bucket *bucket_array, edge **edge_head_ptr, int d)
{
	node *current = head;
	node *temp;
	edge *edge_head = *edge_head_ptr;
	edge *edge_tail = NULL;
	edge *new_edge;
	int row_num, col_num, bucket_num1, bucket_num2, bucket_num3, bucket_num4, bucket_num5, bucket_num6;
	int min_row_num, min_col_num, max_row_num, max_col_num;
	int i;
	double temp_shortest_dist;

	while(current != NULL) {
		if(current->trr->core->row_num1 < current->trr->core->row_num2) {
			min_row_num = current->trr->core->row_num1;
			max_row_num = current->trr->core->row_num2;
		} else {
			min_row_num = current->trr->core->row_num2;
			max_row_num = current->trr->core->row_num1;
		}

		if(current->trr->core->col_num1 < current->trr->core->col_num2) {
			min_col_num = current->trr->core->col_num1;
			max_col_num = current->trr->core->col_num2;
		} else {
			min_col_num = current->trr->core->col_num2;
			max_col_num = current->trr->core->col_num1;
		}

		if(current->trr->core->slope == 0 || current->trr->core->slope == 1) {
			row_num = min_row_num - 1;
			if(row_num == 0) {
				row_num = min_row_num;
			}

			col_num = min_col_num - 1;
			if(col_num == 0) {
				col_num = min_col_num;
			}
		} else {
			row_num = min_row_num - 1;
			if(row_num == 0) {
				row_num = min_row_num;
			}

			col_num = min_col_num + 1;
			if(col_num > d) {
				col_num = min_col_num;
			}
		}

		bucket_num1 = (row_num - 1) * d + col_num;

		if(current->trr->core->slope == 0 || current->trr->core->slope == 1) {
			row_num = max_row_num - 1;
			if(row_num == 0) {
				row_num = max_row_num;
			}

			col_num = max_col_num + 1;
			if(col_num > d) {
				col_num = max_col_num;
			}
		} else {
			row_num = max_row_num + 1;
			if(row_num > d) {
				row_num = max_row_num;
			}

			col_num = max_col_num + 1;
			if(col_num > d) {
				col_num = max_col_num;
			}
		}

		bucket_num2 = (row_num - 1) * d + col_num;

		if(current->trr->core->slope == 0 || current->trr->core->slope == 1) {
			row_num = min_row_num;

			col_num = min_col_num - 1;
			if(col_num == 0) {
				col_num = min_col_num;
			}
		} else {
			row_num = min_row_num - 1;
			if(row_num == 0) {
				row_num = min_row_num;
			}

			col_num = min_col_num;
		}

		bucket_num3 = (row_num - 1) * d + col_num;

		if(current->trr->core->slope == 0 || current->trr->core->slope == 1) {
			row_num = max_row_num;

			col_num = max_col_num + 1;
			if(col_num > d) {
				col_num = max_col_num;
			}
		} else {
			row_num = max_row_num + 1;
			if(row_num > d) {
				row_num = max_row_num;
			}

			col_num = max_col_num;
		}

		bucket_num4 = (row_num - 1) * d + col_num;

		if(current->trr->core->slope == 0 || current->trr->core->slope == 1) {
			row_num = min_row_num + 1;
			if(row_num > d) {
				row_num = min_row_num;
			}

			col_num = min_col_num - 1;
			if(col_num == 0) {
				col_num = min_col_num;
			}
		} else {
			row_num = min_row_num - 1;
			if(row_num == 0) {
				row_num = min_row_num;
			}

			col_num = min_col_num - 1;
			if(col_num == 0) {
				col_num = min_col_num;
			}
		}

		bucket_num5 = (row_num - 1) * d + col_num;

		if(current->trr->core->slope == 0 || current->trr->core->slope == 1) {
			row_num = max_row_num + 1;
			if(row_num > d) {
				row_num = max_row_num;
			}

			col_num = max_col_num + 1;
			if(col_num > d) {
				col_num = max_col_num;
			}
		} else {
			row_num = max_row_num + 1;
			if(row_num > d) {
				row_num = max_row_num;
			}

			col_num = max_col_num - 1;
			if(col_num == 0) {
				col_num = max_col_num;
			}
		}

		bucket_num6 = (row_num - 1) * d + col_num;

		current->nearest_neighbor_dist = DBL_MAX;

		if(current->trr->core->slope == 0 || current->trr->core->slope == 1) {
			for(i = bucket_num1; i <= bucket_num2; i++) {
				if(bucket_array[i - 1].visited == false) {
					bucket_array[i - 1].visited = true;
					temp = bucket_array[i - 1].head;

					while(temp != NULL) {
						if(temp->node_num != current->node_num) {
							temp_shortest_dist = shortest_distance(current->trr->core, temp->trr->core);
							if(temp_shortest_dist < current->nearest_neighbor_dist) {
								current->nearest_neighbor = temp;
								current->nearest_neighbor_dist = temp_shortest_dist;
							}
						}

						temp = temp->bucket_next;
					}
				}
			}

			for(i = bucket_num3; i <= bucket_num4; i++) {
				if(bucket_array[i - 1].visited == false) {
					bucket_array[i - 1].visited = true;
					temp = bucket_array[i - 1].head;

					while(temp != NULL) {
						if(temp->node_num != current->node_num) {
							temp_shortest_dist = shortest_distance(current->trr->core, temp->trr->core);
							if(temp_shortest_dist < current->nearest_neighbor_dist) {
								current->nearest_neighbor = temp;
								current->nearest_neighbor_dist = temp_shortest_dist;
							}
						}

						temp = temp->bucket_next;
					}
				}
			}

			for(i = bucket_num5; i <= bucket_num6; i++) {
				if(bucket_array[i - 1].visited == false) {
					bucket_array[i - 1].visited = true;
					temp = bucket_array[i - 1].head;

					while(temp != NULL) {
						if(temp->node_num != current->node_num) {
							temp_shortest_dist = shortest_distance(current->trr->core, temp->trr->core);
							if(temp_shortest_dist < current->nearest_neighbor_dist) {
								current->nearest_neighbor = temp;
								current->nearest_neighbor_dist = temp_shortest_dist;
							}
						}

						temp = temp->bucket_next;
					}
				}
			}

			for(i = bucket_num1; i <= bucket_num2; i++) {
				bucket_array[i - 1].visited = false;
			}

			for(i = bucket_num3; i <= bucket_num4; i++) {
				bucket_array[i - 1].visited = false;
			}

			for(i = bucket_num5; i <= bucket_num6; i++) {
				bucket_array[i - 1].visited = false;
			}
		} else {
			for(i = bucket_num1; i <= bucket_num2; i += d) {
				if(bucket_array[i - 1].visited == false) {
					bucket_array[i - 1].visited = true;
					temp = bucket_array[i - 1].head;

					while(temp != NULL) {
						if(temp->node_num != current->node_num) {
							temp_shortest_dist = shortest_distance(current->trr->core, temp->trr->core);
							if(temp_shortest_dist < current->nearest_neighbor_dist) {
								current->nearest_neighbor = temp;
								current->nearest_neighbor_dist = temp_shortest_dist;
							}
						}

						temp = temp->bucket_next;
					}
				}
			}

			for(i = bucket_num3; i <= bucket_num4; i += d) {
				if(bucket_array[i - 1].visited == false) {
					bucket_array[i - 1].visited = true;
					temp = bucket_array[i - 1].head;

					while(temp != NULL) {
						if(temp->node_num != current->node_num) {
							temp_shortest_dist = shortest_distance(current->trr->core, temp->trr->core);
							if(temp_shortest_dist < current->nearest_neighbor_dist) {
								current->nearest_neighbor = temp;
								current->nearest_neighbor_dist = temp_shortest_dist;
							}
						}

						temp = temp->bucket_next;
					}
				}
			}

			for(i = bucket_num5; i <= bucket_num6; i += d) {
				if(bucket_array[i - 1].visited == false) {
					bucket_array[i - 1].visited = true;
					temp = bucket_array[i - 1].head;

					while(temp != NULL) {
						if(temp->node_num != current->node_num) {
							temp_shortest_dist = shortest_distance(current->trr->core, temp->trr->core);
							if(temp_shortest_dist < current->nearest_neighbor_dist) {
								current->nearest_neighbor = temp;
								current->nearest_neighbor_dist = temp_shortest_dist;
							}
						}

						temp = temp->bucket_next;
					}
				}
			}

			for(i = bucket_num1; i <= bucket_num2; i += d) {
				bucket_array[i - 1].visited = false;
			}

			for(i = bucket_num3; i <= bucket_num4; i += d) {
				bucket_array[i - 1].visited = false;
			}

			for(i = bucket_num5; i <= bucket_num6; i += d) {
				bucket_array[i - 1].visited = false;
			}
		}

		new_edge = malloc(sizeof(edge));

		new_edge->start_node = current;
		new_edge->end_node = current->nearest_neighbor;
		new_edge->weight = current->nearest_neighbor_dist;

		if(edge_head == NULL) {
			edge_head = new_edge;
			edge_head->next = NULL;
			edge_tail = edge_head;
		} else {
			edge_tail->next = new_edge;
			edge_tail = edge_tail->next;
			edge_tail->next = NULL;
		}

		current = current->next;
	}

	*edge_head_ptr = edge_head;

	return head;
}

int merge_sort(edge **head_ptr)
{
	edge *head = *head_ptr;
	edge *a;
	edge *b;

	if(head == NULL || head->next == NULL) {
		return 0;
	}

	front_back_split(head, &a, &b);

	merge_sort(&a);
	merge_sort(&b);

	*head_ptr = sorted_merge(a, b);

	return 0;
}

int front_back_split(edge *source, edge **front_ptr, edge **back_ptr)
{
	edge *slow;
	edge *fast;

	slow = source;
	fast = source->next;

	while(fast != NULL) {
		fast = fast->next;
		if(fast != NULL) {
			slow = slow->next;
			fast = fast->next;
		}
	}

	*front_ptr = source;
	*back_ptr = slow->next;
	slow->next = NULL;

	return 0;
}

edge *sorted_merge(edge *a, edge *b)
{
	edge *result = NULL;

	if(a == NULL) {
		return b;
	} else if(b == NULL) {
		return a;
	}

	if(a->weight <= b->weight) {
		result = a;
		result->next = sorted_merge(a->next, b);
	} else {
		result = b;
		result->next = sorted_merge(a, b->next);
	}

	return result;
}

edge *find_independent_set(edge *head)
{
	edge *indep_set_head = NULL;
	edge *indep_set_tail = NULL;
	edge *new_edge;
	edge *temp;

	while(head != NULL) {
		temp = indep_set_head;

		while(temp != NULL) {
			if(head->start_node->node_num != temp->end_node->node_num &&
				head->end_node->node_num != temp->start_node->node_num &&
				head->end_node->node_num != temp->end_node->node_num) {
				temp = temp->next;
			} else {
				break;
			}
		}

		if(temp == NULL) {
			new_edge = malloc(sizeof(edge));

			new_edge->start_node = head->start_node;
			new_edge->end_node = head->end_node;
			new_edge->weight = head->weight;

			if(indep_set_head == NULL) {
				indep_set_head = new_edge;
				indep_set_head->next = NULL;
				indep_set_tail = indep_set_head;
			} else {
				indep_set_tail->next = new_edge;
				indep_set_tail = indep_set_tail->next;
				indep_set_tail->next = NULL;
			}
		}

		head = head->next;
	}

	return indep_set_head;
}

/* A utility function to calculate area of triangle formed by points (x1, y1), (x2, y2) and (x3, y3) */
double calculate_triangle_area(double x1, double y1, double x2, double y2, double x3, double y3)
{
	return fabs((x1 * (y2 - y3) + x2 * (y3 - y1) +  x3 * (y1 - y2)) / 2.0);
}

/* A utility function to check whether point P(x, y) lies inside the tilted rectangular region (trr) formed by A(x1, y1), B(x2, y2),
 * C(x3, y3) and D(x4, y4) */
bool is_point_inside_trr(double x, double y, double trr_x1, double trr_y1, double trr_x2, double trr_y2,
		double trr_x3, double trr_y3, double trr_x4, double trr_y4)
{
	/* Calculate area of trr ABCD */
	double A = calculate_triangle_area(trr_x1, trr_y1, trr_x2, trr_y2, trr_x3, trr_y3) +
			calculate_triangle_area(trr_x2, trr_y2, trr_x3, trr_y3, trr_x4, trr_y4);

	/* Calculate area of triangle PAB */
	double A1 = calculate_triangle_area(x, y, trr_x1, trr_y1, trr_x2, trr_y2);

	/* Calculate area of triangle PBD */
	double A2 = calculate_triangle_area(x, y, trr_x2, trr_y2, trr_x4, trr_y4);

	/* Calculate area of triangle PCD */
	double A3 = calculate_triangle_area(x, y, trr_x3, trr_y3, trr_x4, trr_y4);

	/* Calculate area of triangle PAC */
	double A4 = calculate_triangle_area(x, y, trr_x1, trr_y1, trr_x3, trr_y3);

	if(almost_equal_relative(A, A1 + A2 + A3 + A4) ||
			(fabs(A1) < EPSILON && x >= trr_x1 && x <= trr_x2) ||
			(fabs(A2) < EPSILON && x >= trr_x4 && x <= trr_x2) ||
			(fabs(A3) < EPSILON && x >= trr_x3 && x <= trr_x4) ||
			(fabs(A4) < EPSILON && x >= trr_x3 && x <= trr_x1)) {
		return true;
	} else {
		return false;
	}
}

bool almost_equal_relative(double dbl1, double dbl2)
{
	double diff = fabs(dbl1 - dbl2);
	double largest;

	dbl1 = fabs(dbl1);
	dbl2 = fabs(dbl2);

	if(dbl1 > dbl2) {
		largest = dbl1;
	} else {
		largest = dbl2;
	}

	if(diff <= largest * EPSILON) {
		return true;
	} else {
		return false;
	}
}

manhattan_arc *zero_skew_merge(manhattan_arc *man_arc_merge, tilted_rect_reg *trr1, tilted_rect_reg *trr2)
{
	double trr1_x1, trr1_y1, trr1_x2, trr1_y2, trr1_x3, trr1_y3, trr1_x4, trr1_y4;
	double trr2_x1, trr2_y1, trr2_x2, trr2_y2, trr2_x3, trr2_y3, trr2_x4, trr2_y4;
	int trr1_edge1_slope, trr1_edge2_slope, trr1_edge3_slope, trr1_edge4_slope;
	double trr1_edge1_y_intercept, trr1_edge2_y_intercept, trr1_edge3_y_intercept, trr1_edge4_y_intercept;
	int trr2_edge1_slope, trr2_edge2_slope, trr2_edge3_slope, trr2_edge4_slope;
	double trr2_edge1_y_intercept, trr2_edge2_y_intercept, trr2_edge3_y_intercept, trr2_edge4_y_intercept;
	double trr1_core_y_intercept, trr2_core_y_intercept;
	bool single_intersect_flag, double_intersect_flag;

	if(almost_equal_relative(trr1->core->x1_coordinate, trr1->core->x2_coordinate) &&
			almost_equal_relative(trr1->core->y1_coordinate, trr1->core->y2_coordinate)) {
		trr1_x1 = trr1->core->x1_coordinate;
		trr1_y1 = trr1->core->y1_coordinate + trr1->radius;

		trr1_x2 = trr1->core->x1_coordinate + trr1->radius;
		trr1_y2 = trr1->core->y1_coordinate;

		trr1_x3 = trr1->core->x1_coordinate - trr1->radius;
		trr1_y3 = trr1->core->y1_coordinate;

		trr1_x4 = trr1->core->x1_coordinate;
		trr1_y4 = trr1->core->y1_coordinate - trr1->radius;
	} else {
		if(trr1->core->slope == 1) {
			if(trr1->core->x1_coordinate > trr1->core->x2_coordinate) {
				trr1_x1 = trr1->core->x1_coordinate + trr1->radius;
				trr1_y1 = trr1->core->y1_coordinate;

				trr1_x2 = trr1->core->x2_coordinate;
				trr1_y2 = trr1->core->y2_coordinate - trr1->radius;

				trr1_x3 = trr1->core->x1_coordinate;
				trr1_y3 = trr1->core->y1_coordinate + trr1->radius;

				trr1_x4 = trr1->core->x2_coordinate - trr1->radius;
				trr1_y4 = trr1->core->y2_coordinate;
			} else {
				trr1_x1 = trr1->core->x2_coordinate + trr1->radius;
				trr1_y1 = trr1->core->y2_coordinate;

				trr1_x2 = trr1->core->x1_coordinate;
				trr1_y2 = trr1->core->y1_coordinate - trr1->radius;

				trr1_x3 = trr1->core->x2_coordinate;
				trr1_y3 = trr1->core->y2_coordinate + trr1->radius;

				trr1_x4 = trr1->core->x1_coordinate - trr1->radius;
				trr1_y4 = trr1->core->y1_coordinate;
			}
		} else {
			if(trr1->core->x1_coordinate < trr1->core->x2_coordinate) {
				trr1_x1 = trr1->core->x1_coordinate;
				trr1_y1 = trr1->core->y1_coordinate + trr1->radius;

				trr1_x2 = trr1->core->x2_coordinate + trr1->radius;
				trr1_y2 = trr1->core->y2_coordinate;

				trr1_x3 = trr1->core->x1_coordinate - trr1->radius;
				trr1_y3 = trr1->core->y1_coordinate;

				trr1_x4 = trr1->core->x2_coordinate;
				trr1_y4 = trr1->core->y2_coordinate - trr1->radius;
			} else {
				trr1_x1 = trr1->core->x2_coordinate;
				trr1_y1 = trr1->core->y2_coordinate + trr1->radius;

				trr1_x2 = trr1->core->x1_coordinate + trr1->radius;
				trr1_y2 = trr1->core->y1_coordinate;

				trr1_x3 = trr1->core->x2_coordinate - trr1->radius;
				trr1_y3 = trr1->core->y2_coordinate;

				trr1_x4 = trr1->core->x1_coordinate;
				trr1_y4 = trr1->core->y1_coordinate - trr1->radius;
			}
		}
	}

	if(almost_equal_relative(trr2->core->x1_coordinate, trr2->core->x2_coordinate) &&
			almost_equal_relative(trr2->core->y1_coordinate, trr2->core->y2_coordinate)) {
		trr2_x1 = trr2->core->x1_coordinate;
		trr2_y1 = trr2->core->y1_coordinate + trr2->radius;

		trr2_x2 = trr2->core->x1_coordinate + trr2->radius;
		trr2_y2 = trr2->core->y1_coordinate;

		trr2_x3 = trr2->core->x1_coordinate - trr2->radius;
		trr2_y3 = trr2->core->y1_coordinate;

		trr2_x4 = trr2->core->x1_coordinate;
		trr2_y4 = trr2->core->y1_coordinate - trr2->radius;
	} else {
		if(trr2->core->slope == 1) {
			if(trr2->core->x1_coordinate > trr2->core->x2_coordinate) {
				trr2_x1 = trr2->core->x1_coordinate + trr2->radius;
				trr2_y1 = trr2->core->y1_coordinate;

				trr2_x2 = trr2->core->x2_coordinate;
				trr2_y2 = trr2->core->y2_coordinate - trr2->radius;

				trr2_x3 = trr2->core->x1_coordinate;
				trr2_y3 = trr2->core->y1_coordinate + trr2->radius;

				trr2_x4 = trr2->core->x2_coordinate - trr2->radius;
				trr2_y4 = trr2->core->y2_coordinate;
			} else {
				trr2_x1 = trr2->core->x2_coordinate + trr2->radius;
				trr2_y1 = trr2->core->y2_coordinate;

				trr2_x2 = trr2->core->x1_coordinate;
				trr2_y2 = trr2->core->y1_coordinate - trr2->radius;

				trr2_x3 = trr2->core->x2_coordinate;
				trr2_y3 = trr2->core->y2_coordinate + trr2->radius;

				trr2_x4 = trr2->core->x1_coordinate - trr2->radius;
				trr2_y4 = trr2->core->y1_coordinate;
			}
		} else {
			if(trr2->core->x1_coordinate < trr2->core->x2_coordinate) {
				trr2_x1 = trr2->core->x1_coordinate;
				trr2_y1 = trr2->core->y1_coordinate + trr2->radius;

				trr2_x2 = trr2->core->x2_coordinate + trr2->radius;
				trr2_y2 = trr2->core->y2_coordinate;

				trr2_x3 = trr2->core->x1_coordinate - trr2->radius;
				trr2_y3 = trr2->core->y1_coordinate;

				trr2_x4 = trr2->core->x2_coordinate;
				trr2_y4 = trr2->core->y2_coordinate - trr2->radius;
			} else {
				trr2_x1 = trr2->core->x2_coordinate;
				trr2_y1 = trr2->core->y2_coordinate + trr2->radius;

				trr2_x2 = trr2->core->x1_coordinate + trr2->radius;
				trr2_y2 = trr2->core->y1_coordinate;

				trr2_x3 = trr2->core->x2_coordinate - trr2->radius;
				trr2_y3 = trr2->core->y2_coordinate;

				trr2_x4 = trr2->core->x1_coordinate;
				trr2_y4 = trr2->core->y1_coordinate - trr2->radius;
			}
		}
	}

	if(fabs(trr1->radius) >= EPSILON && fabs(trr2->radius) >= EPSILON) {
		trr1_edge1_slope = round((trr1_y2 - trr1_y1) / (trr1_x2 - trr1_x1));
		trr1_edge1_y_intercept = trr1_y1 - trr1_edge1_slope * trr1_x1;

		trr1_edge2_slope = round((trr1_y4 - trr1_y2) / (trr1_x4 - trr1_x2));
		trr1_edge2_y_intercept = trr1_y2 - trr1_edge2_slope * trr1_x2;

		trr1_edge3_slope = round((trr1_y4 - trr1_y3) / (trr1_x4 - trr1_x3));
		trr1_edge3_y_intercept = trr1_y3 - trr1_edge3_slope * trr1_x3;

		trr1_edge4_slope = round((trr1_y3 - trr1_y1) / (trr1_x3 - trr1_x1));
		trr1_edge4_y_intercept = trr1_y1 - trr1_edge4_slope * trr1_x1;

		trr2_edge1_slope = round((trr2_y2 - trr2_y1) / (trr2_x2 - trr2_x1));
		trr2_edge1_y_intercept = trr2_y1 - trr2_edge1_slope * trr2_x1;

		trr2_edge2_slope = round((trr2_y4 - trr2_y2) / (trr2_x4 - trr2_x2));
		trr2_edge2_y_intercept = trr2_y2 - trr2_edge2_slope * trr2_x2;

		trr2_edge3_slope = round((trr2_y4 - trr2_y3) / (trr2_x4 - trr2_x3));
		trr2_edge3_y_intercept = trr2_y3 - trr2_edge3_slope * trr2_x3;

		trr2_edge4_slope = round((trr2_y3 - trr2_y1) / (trr2_x3 - trr2_x1));
		trr2_edge4_y_intercept = trr2_y1 - trr2_edge4_slope * trr2_x1;

		if(trr1_edge1_slope == trr2_edge2_slope && almost_equal_relative(trr1_edge1_y_intercept, trr2_edge2_y_intercept)) {
			if(trr1_x1 < trr2_x4) {
				man_arc_merge->x1_coordinate = trr2_x4;
				man_arc_merge->y1_coordinate = trr2_y4;
			} else {
				man_arc_merge->x1_coordinate = trr1_x1;
				man_arc_merge->y1_coordinate = trr1_y1;
			}

			if(trr1_x2 < trr2_x2) {
				man_arc_merge->x2_coordinate = trr1_x2;
				man_arc_merge->y2_coordinate = trr1_y2;
			} else {
				man_arc_merge->x2_coordinate = trr2_x2;
				man_arc_merge->y2_coordinate = trr2_y2;
			}
		}

		else if(trr1_edge1_slope == trr2_edge3_slope && almost_equal_relative(trr1_edge1_y_intercept, trr2_edge3_y_intercept)) {
			if(trr1_y1 < trr2_y3) {
				man_arc_merge->x1_coordinate = trr1_x1;
				man_arc_merge->y1_coordinate = trr1_y1;
			} else {
				man_arc_merge->x1_coordinate = trr2_x3;
				man_arc_merge->y1_coordinate = trr2_y3;
			}

			if(trr1_y2 < trr2_y4) {
				man_arc_merge->x2_coordinate = trr2_x4;
				man_arc_merge->y2_coordinate = trr2_y4;
			} else {
				man_arc_merge->x2_coordinate = trr1_x2;
				man_arc_merge->y2_coordinate = trr1_y2;
			}
		}

		else if(trr1_edge1_slope == trr2_edge4_slope && almost_equal_relative(trr1_edge1_y_intercept, trr2_edge4_y_intercept)) {
			if(trr1_x1 < trr2_x1) {
				man_arc_merge->x1_coordinate = trr1_x1;
				man_arc_merge->y1_coordinate = trr1_y1;
			} else {
				man_arc_merge->x1_coordinate = trr2_x1;
				man_arc_merge->y1_coordinate = trr2_y1;
			}

			if(trr1_x2 < trr2_x3) {
				man_arc_merge->x2_coordinate = trr2_x3;
				man_arc_merge->y2_coordinate = trr2_y3;
			} else {
				man_arc_merge->x2_coordinate = trr1_x2;
				man_arc_merge->y2_coordinate = trr1_y2;
			}
		}

		else if(trr1_edge2_slope == trr2_edge3_slope && almost_equal_relative(trr1_edge2_y_intercept, trr2_edge3_y_intercept)) {
			if(trr1_x2 < trr2_x3) {
				man_arc_merge->x1_coordinate = trr1_x2;
				man_arc_merge->y1_coordinate = trr1_y2;
			} else {
				man_arc_merge->x1_coordinate = trr2_x3;
				man_arc_merge->y1_coordinate = trr2_y3;
			}

			if(trr1_x4 < trr2_x4) {
				man_arc_merge->x2_coordinate = trr2_x4;
				man_arc_merge->y2_coordinate = trr2_y4;
			} else {
				man_arc_merge->x2_coordinate = trr1_x4;
				man_arc_merge->y2_coordinate = trr1_y4;
			}
		}

		else if(trr1_edge2_slope == trr2_edge4_slope && almost_equal_relative(trr1_edge2_y_intercept, trr2_edge4_y_intercept)) {
			if(trr1_x2 < trr2_x1) {
				man_arc_merge->x1_coordinate = trr1_x2;
				man_arc_merge->y1_coordinate = trr1_y2;
			} else {
				man_arc_merge->x1_coordinate = trr2_x1;
				man_arc_merge->y1_coordinate = trr2_y1;
			}

			if(trr1_x4 < trr2_x3) {
				man_arc_merge->x2_coordinate = trr2_x3;
				man_arc_merge->y2_coordinate = trr2_y3;
			} else {
				man_arc_merge->x2_coordinate = trr1_x4;
				man_arc_merge->y2_coordinate = trr1_y4;
			}
		}

		else if(trr1_edge2_slope == trr2_edge1_slope && almost_equal_relative(trr1_edge2_y_intercept, trr2_edge1_y_intercept)) {
			if(trr1_x4 < trr2_x1) {
				man_arc_merge->x1_coordinate = trr2_x1;
				man_arc_merge->y1_coordinate = trr2_y1;
			} else {
				man_arc_merge->x1_coordinate = trr1_x4;
				man_arc_merge->y1_coordinate = trr1_y4;
			}

			if(trr1_x2 < trr2_x2) {
				man_arc_merge->x2_coordinate = trr1_x2;
				man_arc_merge->y2_coordinate = trr1_y2;
			} else {
				man_arc_merge->x2_coordinate = trr2_x2;
				man_arc_merge->y2_coordinate = trr2_y2;
			}
		}

		else if(trr1_edge3_slope == trr2_edge4_slope && almost_equal_relative(trr1_edge3_y_intercept, trr2_edge4_y_intercept)) {
			if(trr1_x3 < trr2_x3) {
				man_arc_merge->x1_coordinate = trr2_x3;
				man_arc_merge->y1_coordinate = trr2_y3;
			} else {
				man_arc_merge->x1_coordinate = trr1_x3;
				man_arc_merge->y1_coordinate = trr1_y3;
			}

			if(trr1_x4 < trr2_x1) {
				man_arc_merge->x2_coordinate = trr1_x4;
				man_arc_merge->y2_coordinate = trr1_y4;
			} else {
				man_arc_merge->x2_coordinate = trr2_x1;
				man_arc_merge->y2_coordinate = trr2_y1;
			}
		}

		else if(trr1_edge3_slope == trr2_edge1_slope && almost_equal_relative(trr1_edge3_y_intercept, trr2_edge1_y_intercept)) {
			if(trr1_y3 < trr2_y1) {
				man_arc_merge->x1_coordinate = trr1_x3;
				man_arc_merge->y1_coordinate = trr1_y3;
			} else {
				man_arc_merge->x1_coordinate = trr2_x1;
				man_arc_merge->y1_coordinate = trr2_y1;
			}

			if(trr1_y4 < trr2_y2) {
				man_arc_merge->x2_coordinate = trr2_x2;
				man_arc_merge->y2_coordinate = trr2_y2;
			} else {
				man_arc_merge->x2_coordinate = trr1_x4;
				man_arc_merge->y2_coordinate = trr1_y4;
			}
		}

		else if(trr1_edge3_slope == trr2_edge2_slope && almost_equal_relative(trr1_edge3_y_intercept, trr2_edge2_y_intercept)) {
			if(trr1_x3 < trr2_x2) {
				man_arc_merge->x1_coordinate = trr1_x3;
				man_arc_merge->y1_coordinate = trr1_y3;
			} else {
				man_arc_merge->x1_coordinate = trr2_x2;
				man_arc_merge->y1_coordinate = trr2_y2;
			}

			if(trr1_x4 < trr2_x4) {
				man_arc_merge->x2_coordinate = trr2_x4;
				man_arc_merge->y2_coordinate = trr2_y4;
			} else {
				man_arc_merge->x2_coordinate = trr1_x4;
				man_arc_merge->y2_coordinate = trr1_y4;
			}
		}

		else if(trr1_edge4_slope == trr2_edge1_slope && almost_equal_relative(trr1_edge4_y_intercept, trr2_edge1_y_intercept)) {
			if(trr1_x1 < trr2_x1) {
				man_arc_merge->x1_coordinate = trr1_x1;
				man_arc_merge->y1_coordinate = trr1_y1;
			} else {
				man_arc_merge->x1_coordinate = trr2_x1;
				man_arc_merge->y1_coordinate = trr2_y1;
			}

			if(trr1_x3 < trr2_x2) {
				man_arc_merge->x2_coordinate = trr2_x2;
				man_arc_merge->y2_coordinate = trr2_y2;
			} else {
				man_arc_merge->x2_coordinate = trr1_x3;
				man_arc_merge->y2_coordinate = trr1_y3;
			}
		}

		else if(trr1_edge4_slope == trr2_edge2_slope && almost_equal_relative(trr1_edge4_y_intercept, trr2_edge2_y_intercept)) {
			if(trr1_x1 < trr2_x2) {
				man_arc_merge->x1_coordinate = trr1_x1;
				man_arc_merge->y1_coordinate = trr1_y1;
			} else {
				man_arc_merge->x1_coordinate = trr2_x2;
				man_arc_merge->y1_coordinate = trr2_y2;
			}

			if(trr1_x3 < trr2_x4) {
				man_arc_merge->x2_coordinate = trr2_x4;
				man_arc_merge->y2_coordinate = trr2_y4;
			} else {
				man_arc_merge->x2_coordinate = trr1_x3;
				man_arc_merge->y2_coordinate = trr1_y3;
			}
		}

		else if(trr1_edge4_slope == trr2_edge3_slope && almost_equal_relative(trr1_edge4_y_intercept, trr2_edge3_y_intercept)) {
			if(trr1_x3 < trr2_x3) {
				man_arc_merge->x1_coordinate = trr2_x3;
				man_arc_merge->y1_coordinate = trr2_y3;
			} else {
				man_arc_merge->x1_coordinate = trr1_x3;
				man_arc_merge->y1_coordinate = trr1_y3;
			}

			if(trr1_x1 < trr2_x4) {
				man_arc_merge->x2_coordinate = trr1_x1;
				man_arc_merge->y2_coordinate = trr1_y1;
			} else {
				man_arc_merge->x2_coordinate = trr2_x4;
				man_arc_merge->y2_coordinate = trr2_y4;
			}
		}
	} else if(fabs(trr1->radius) < EPSILON && fabs(trr2->radius) >= EPSILON) {
		if(!almost_equal_relative(trr1->core->x1_coordinate, trr1->core->x2_coordinate) &&
				!almost_equal_relative(trr1->core->y1_coordinate, trr1->core->y2_coordinate)) {
			trr1_core_y_intercept = trr1->core->y1_coordinate - trr1->core->slope * trr1->core->x1_coordinate;

			if(do_intersect(trr1->core->x1_coordinate, trr1->core->y1_coordinate, trr1->core->x2_coordinate, trr1->core->y2_coordinate,
					trr2_x1, trr2_y1, trr2_x2, trr2_y2)) {
				trr2_edge1_slope = round((trr2_y2 - trr2_y1) / (trr2_x2 - trr2_x1));
				trr2_edge1_y_intercept = trr2_y1 - trr2_edge1_slope * trr2_x1;

				man_arc_merge->x1_coordinate = (trr2_edge1_y_intercept - trr1_core_y_intercept) / (trr1->core->slope - trr2_edge1_slope);
				man_arc_merge->y1_coordinate = trr1->core->slope * man_arc_merge->x1_coordinate + trr1_core_y_intercept;

				single_intersect_flag = true;

				if(do_intersect(trr1->core->x1_coordinate, trr1->core->y1_coordinate, trr1->core->x2_coordinate, trr1->core->y2_coordinate,
						trr2_x3, trr2_y3, trr2_x4, trr2_y4)) {
					trr2_edge3_slope = round((trr2_y4 - trr2_y3) / (trr2_x4 - trr2_x3));
					trr2_edge3_y_intercept = trr2_y3 - trr2_edge3_slope * trr2_x3;

					man_arc_merge->x2_coordinate = (trr2_edge3_y_intercept - trr1_core_y_intercept) / (trr1->core->slope - trr2_edge3_slope);
					man_arc_merge->y2_coordinate = trr1->core->slope * man_arc_merge->x2_coordinate + trr1_core_y_intercept;

					double_intersect_flag = true;
				} else {
					double_intersect_flag = false;
				}
			}

			else if(do_intersect(trr1->core->x1_coordinate, trr1->core->y1_coordinate, trr1->core->x2_coordinate, trr1->core->y2_coordinate,
						trr2_x2, trr2_y2, trr2_x4, trr2_y4)) {
				trr2_edge2_slope = round((trr2_y4 - trr2_y2) / (trr2_x4 - trr2_x2));
				trr2_edge2_y_intercept = trr2_y2 - trr2_edge2_slope * trr2_x2;

				man_arc_merge->x1_coordinate = (trr2_edge2_y_intercept - trr1_core_y_intercept) / (trr1->core->slope - trr2_edge2_slope);
				man_arc_merge->y1_coordinate = trr1->core->slope * man_arc_merge->x1_coordinate + trr1_core_y_intercept;

				single_intersect_flag = true;

				if(do_intersect(trr1->core->x1_coordinate, trr1->core->y1_coordinate, trr1->core->x2_coordinate, trr1->core->y2_coordinate,
						trr2_x1, trr2_y1, trr2_x3, trr2_y3)) {
					trr2_edge4_slope = round((trr2_y3 - trr2_y1) / (trr2_x3 - trr2_x1));
					trr2_edge4_y_intercept = trr2_y1 - trr2_edge4_slope * trr2_x1;

					man_arc_merge->x2_coordinate = (trr2_edge4_y_intercept - trr1_core_y_intercept) / (trr1->core->slope - trr2_edge4_slope);
					man_arc_merge->y2_coordinate = trr1->core->slope * man_arc_merge->x2_coordinate + trr1_core_y_intercept;

					double_intersect_flag = true;
				} else {
					double_intersect_flag = false;
				}
			}

			else if(do_intersect(trr1->core->x1_coordinate, trr1->core->y1_coordinate, trr1->core->x2_coordinate, trr1->core->y2_coordinate,
						trr2_x3, trr2_y3, trr2_x4, trr2_y4)) {
				trr2_edge3_slope = round((trr2_y4 - trr2_y3) / (trr2_x4 - trr2_x3));
				trr2_edge3_y_intercept = trr2_y3 - trr2_edge3_slope * trr2_x3;

				man_arc_merge->x1_coordinate = (trr2_edge3_y_intercept - trr1_core_y_intercept) / (trr1->core->slope - trr2_edge3_slope);
				man_arc_merge->y1_coordinate = trr1->core->slope * man_arc_merge->x1_coordinate + trr1_core_y_intercept;

				single_intersect_flag = true;
				double_intersect_flag = false;
			}

			else if(do_intersect(trr1->core->x1_coordinate, trr1->core->y1_coordinate, trr1->core->x2_coordinate, trr1->core->y2_coordinate,
						trr2_x1, trr2_y1, trr2_x3, trr2_y3)) {
				trr2_edge4_slope = round((trr2_y3 - trr2_y1) / (trr2_x3 - trr2_x1));
				trr2_edge4_y_intercept = trr2_y1 - trr2_edge4_slope * trr2_x1;

				man_arc_merge->x1_coordinate = (trr2_edge4_y_intercept - trr1_core_y_intercept) / (trr1->core->slope - trr2_edge4_slope);
				man_arc_merge->y1_coordinate = trr1->core->slope * man_arc_merge->x1_coordinate + trr1_core_y_intercept;

				single_intersect_flag = true;
				double_intersect_flag = false;
			}

			else if(orientation(trr2_x1, trr2_y1, trr2_x2, trr2_y2, trr1->core->x1_coordinate, trr1->core->y1_coordinate) == 0 &&
						is_point_on_line_segment(trr1->core->x1_coordinate, trr1->core->y1_coordinate, trr2_x1, trr2_y1, trr2_x2, trr2_y2)) {
				man_arc_merge->x1_coordinate = trr1->core->x1_coordinate;
				man_arc_merge->y1_coordinate = trr1->core->y1_coordinate;

				if(trr1->core->y2_coordinate > trr2_y1) {
					man_arc_merge->x2_coordinate = trr2_x1;
					man_arc_merge->y2_coordinate = trr2_y1;
				} else if(trr1->core->y2_coordinate < trr2_y2) {
					man_arc_merge->x2_coordinate = trr2_x2;
					man_arc_merge->y2_coordinate = trr2_y2;
				} else {
					man_arc_merge->x2_coordinate = trr1->core->x2_coordinate;
					man_arc_merge->y2_coordinate = trr1->core->y2_coordinate;
				}

				single_intersect_flag = true;
				double_intersect_flag = true;
			}

			else if(orientation(trr2_x1, trr2_y1, trr2_x2, trr2_y2, trr1->core->x2_coordinate, trr1->core->y2_coordinate) == 0 &&
						is_point_on_line_segment(trr1->core->x2_coordinate, trr1->core->y2_coordinate, trr2_x1, trr2_y1, trr2_x2, trr2_y2)) {
				man_arc_merge->x1_coordinate = trr1->core->x2_coordinate;
				man_arc_merge->y1_coordinate = trr1->core->y2_coordinate;

				if(trr1->core->y1_coordinate > trr2_y1) {
					man_arc_merge->x2_coordinate = trr2_x1;
					man_arc_merge->y2_coordinate = trr2_y1;
				} else if(trr1->core->y1_coordinate < trr2_y2) {
					man_arc_merge->x2_coordinate = trr2_x2;
					man_arc_merge->y2_coordinate = trr2_y2;
				} else {
					man_arc_merge->x2_coordinate = trr1->core->x1_coordinate;
					man_arc_merge->y2_coordinate = trr1->core->y1_coordinate;
				}

				single_intersect_flag = true;
				double_intersect_flag = true;
			}

			else if(orientation(trr2_x2, trr2_y2, trr2_x4, trr2_y4, trr1->core->x1_coordinate, trr1->core->y1_coordinate) == 0 &&
						is_point_on_line_segment(trr1->core->x1_coordinate, trr1->core->y1_coordinate, trr2_x2, trr2_y2, trr2_x4, trr2_y4)) {
				man_arc_merge->x1_coordinate = trr1->core->x1_coordinate;
				man_arc_merge->y1_coordinate = trr1->core->y1_coordinate;

				if(trr1->core->x2_coordinate > trr2_x2) {
					man_arc_merge->x2_coordinate = trr2_x2;
					man_arc_merge->y2_coordinate = trr2_y2;
				} else if(trr1->core->x2_coordinate < trr2_x4) {
					man_arc_merge->x2_coordinate = trr2_x4;
					man_arc_merge->y2_coordinate = trr2_y4;
				} else {
					man_arc_merge->x2_coordinate = trr1->core->x2_coordinate;
					man_arc_merge->y2_coordinate = trr1->core->y2_coordinate;
				}

				single_intersect_flag = true;
				double_intersect_flag = true;
			}

			else if(orientation(trr2_x2, trr2_y2, trr2_x4, trr2_y4, trr1->core->x2_coordinate, trr1->core->y2_coordinate) == 0 &&
						is_point_on_line_segment(trr1->core->x2_coordinate, trr1->core->y2_coordinate, trr2_x2, trr2_y2, trr2_x4, trr2_y4)) {
				man_arc_merge->x1_coordinate = trr1->core->x2_coordinate;
				man_arc_merge->y1_coordinate = trr1->core->y2_coordinate;

				if(trr1->core->x1_coordinate > trr2_x2) {
					man_arc_merge->x2_coordinate = trr2_x2;
					man_arc_merge->y2_coordinate = trr2_y2;
				} else if(trr1->core->x1_coordinate < trr2_x4) {
					man_arc_merge->x2_coordinate = trr2_x4;
					man_arc_merge->y2_coordinate = trr2_y4;
				} else {
					man_arc_merge->x2_coordinate = trr1->core->x1_coordinate;
					man_arc_merge->y2_coordinate = trr1->core->y1_coordinate;
				}

				single_intersect_flag = true;
				double_intersect_flag = true;
			}

			else if(orientation(trr2_x3, trr2_y3, trr2_x4, trr2_y4, trr1->core->x1_coordinate, trr1->core->y1_coordinate) == 0 &&
						is_point_on_line_segment(trr1->core->x1_coordinate, trr1->core->y1_coordinate, trr2_x3, trr2_y3, trr2_x4, trr2_y4)) {
				man_arc_merge->x1_coordinate = trr1->core->x1_coordinate;
				man_arc_merge->y1_coordinate = trr1->core->y1_coordinate;

				if(trr1->core->y2_coordinate > trr2_y3) {
					man_arc_merge->x2_coordinate = trr2_x3;
					man_arc_merge->y2_coordinate = trr2_y3;
				} else if(trr1->core->y2_coordinate < trr2_y4) {
					man_arc_merge->x2_coordinate = trr2_x4;
					man_arc_merge->y2_coordinate = trr2_y4;
				} else {
					man_arc_merge->x2_coordinate = trr1->core->x2_coordinate;
					man_arc_merge->y2_coordinate = trr1->core->y2_coordinate;
				}

				single_intersect_flag = true;
				double_intersect_flag = true;
			}

			else if(orientation(trr2_x3, trr2_y3, trr2_x4, trr2_y4, trr1->core->x2_coordinate, trr1->core->y2_coordinate) == 0 &&
						is_point_on_line_segment(trr1->core->x2_coordinate, trr1->core->y2_coordinate, trr2_x3, trr2_y3, trr2_x4, trr2_y4)) {
				man_arc_merge->x1_coordinate = trr1->core->x2_coordinate;
				man_arc_merge->y1_coordinate = trr1->core->y2_coordinate;

				if(trr1->core->y1_coordinate > trr2_y3) {
					man_arc_merge->x2_coordinate = trr2_x3;
					man_arc_merge->y2_coordinate = trr2_y3;
				} else if(trr1->core->y1_coordinate < trr2_y4) {
					man_arc_merge->x2_coordinate = trr2_x4;
					man_arc_merge->y2_coordinate = trr2_y4;
				} else {
					man_arc_merge->x2_coordinate = trr1->core->x1_coordinate;
					man_arc_merge->y2_coordinate = trr1->core->y1_coordinate;
				}

				single_intersect_flag = true;
				double_intersect_flag = true;
			}

			else if(orientation(trr2_x1, trr2_y1, trr2_x3, trr2_y3, trr1->core->x1_coordinate, trr1->core->y1_coordinate) == 0 &&
						is_point_on_line_segment(trr1->core->x1_coordinate, trr1->core->y1_coordinate, trr2_x1, trr2_y1, trr2_x3, trr2_y3)) {
				man_arc_merge->x1_coordinate = trr1->core->x1_coordinate;
				man_arc_merge->y1_coordinate = trr1->core->y1_coordinate;

				if(trr1->core->x2_coordinate > trr2_x1) {
					man_arc_merge->x2_coordinate = trr2_x1;
					man_arc_merge->y2_coordinate = trr2_y1;
				} else if(trr1->core->x2_coordinate < trr2_x3) {
					man_arc_merge->x2_coordinate = trr2_x3;
					man_arc_merge->y2_coordinate = trr2_y3;
				} else {
					man_arc_merge->x2_coordinate = trr1->core->x2_coordinate;
					man_arc_merge->y2_coordinate = trr1->core->y2_coordinate;
				}

				single_intersect_flag = true;
				double_intersect_flag = true;
			}

			else if(orientation(trr2_x1, trr2_y1, trr2_x3, trr2_y3, trr1->core->x2_coordinate, trr1->core->y2_coordinate) == 0 &&
						is_point_on_line_segment(trr1->core->x2_coordinate, trr1->core->y2_coordinate, trr2_x1, trr2_y1, trr2_x3, trr2_y3)) {
				man_arc_merge->x1_coordinate = trr1->core->x2_coordinate;
				man_arc_merge->y1_coordinate = trr1->core->y2_coordinate;

				if(trr1->core->x1_coordinate > trr2_x1) {
					man_arc_merge->x2_coordinate = trr2_x1;
					man_arc_merge->y2_coordinate = trr2_y1;
				} else if(trr1->core->x1_coordinate < trr2_x3) {
					man_arc_merge->x2_coordinate = trr2_x3;
					man_arc_merge->y2_coordinate = trr2_y3;
				} else {
					man_arc_merge->x2_coordinate = trr1->core->x1_coordinate;
					man_arc_merge->y2_coordinate = trr1->core->y1_coordinate;
				}

				single_intersect_flag = true;
				double_intersect_flag = true;
			}

			else {
				man_arc_merge->x1_coordinate = trr1->core->x1_coordinate;
				man_arc_merge->y1_coordinate = trr1->core->y1_coordinate;

				single_intersect_flag = false;
				double_intersect_flag = false;
			}

			if(single_intersect_flag && !double_intersect_flag && is_point_inside_trr(trr1->core->x1_coordinate, trr1->core->y1_coordinate,
					trr2_x1, trr2_y1, trr2_x2, trr2_y2, trr2_x3, trr2_y3, trr2_x4, trr2_y4)) {
				man_arc_merge->x2_coordinate = trr1->core->x1_coordinate;
				man_arc_merge->y2_coordinate = trr1->core->y1_coordinate;
			} else if((single_intersect_flag && !double_intersect_flag && is_point_inside_trr(trr1->core->x2_coordinate,
						trr1->core->y2_coordinate, trr2_x1, trr2_y1, trr2_x2, trr2_y2, trr2_x3, trr2_y3, trr2_x4, trr2_y4)) ||
						(!single_intersect_flag && !double_intersect_flag)) {
				man_arc_merge->x2_coordinate = trr1->core->x2_coordinate;
				man_arc_merge->y2_coordinate = trr1->core->y2_coordinate;
			}
		} else {
			man_arc_merge->x1_coordinate = trr1->core->x1_coordinate;
			man_arc_merge->y1_coordinate = trr1->core->y1_coordinate;

			man_arc_merge->x2_coordinate = trr1->core->x2_coordinate;
			man_arc_merge->y2_coordinate = trr1->core->y2_coordinate;
		}
	} else if(fabs(trr1->radius) >= EPSILON && fabs(trr2->radius) < EPSILON) {
		if(!almost_equal_relative(trr2->core->x1_coordinate, trr2->core->x2_coordinate) &&
				!almost_equal_relative(trr2->core->y1_coordinate, trr2->core->y2_coordinate)) {
			trr2_core_y_intercept = trr2->core->y1_coordinate - trr2->core->slope * trr2->core->x1_coordinate;

			if(do_intersect(trr2->core->x1_coordinate, trr2->core->y1_coordinate, trr2->core->x2_coordinate, trr2->core->y2_coordinate,
					trr1_x1, trr1_y1, trr1_x2, trr1_y2)) {
				trr1_edge1_slope = round((trr1_y2 - trr1_y1) / (trr1_x2 - trr1_x1));
				trr1_edge1_y_intercept = trr1_y1 - trr1_edge1_slope * trr1_x1;

				man_arc_merge->x1_coordinate = (trr1_edge1_y_intercept - trr2_core_y_intercept) / (trr2->core->slope - trr1_edge1_slope);
				man_arc_merge->y1_coordinate = trr2->core->slope * man_arc_merge->x1_coordinate + trr2_core_y_intercept;

				single_intersect_flag = true;

				if(do_intersect(trr2->core->x1_coordinate, trr2->core->y1_coordinate, trr2->core->x2_coordinate, trr2->core->y2_coordinate,
						trr1_x3, trr1_y3, trr1_x4, trr1_y4)) {
					trr1_edge3_slope = round((trr1_y4 - trr1_y3) / (trr1_x4 - trr1_x3));
					trr1_edge3_y_intercept = trr1_y3 - trr1_edge3_slope * trr1_x3;

					man_arc_merge->x2_coordinate = (trr1_edge3_y_intercept - trr2_core_y_intercept) / (trr2->core->slope - trr1_edge3_slope);
					man_arc_merge->y2_coordinate = trr2->core->slope * man_arc_merge->x2_coordinate + trr2_core_y_intercept;

					double_intersect_flag = true;
				} else {
					double_intersect_flag = false;
				}
			}

			else if(do_intersect(trr2->core->x1_coordinate, trr2->core->y1_coordinate, trr2->core->x2_coordinate, trr2->core->y2_coordinate,
						trr1_x2, trr1_y2, trr1_x4, trr1_y4)) {
				trr1_edge2_slope = round((trr1_y4 - trr1_y2) / (trr1_x4 - trr1_x2));
				trr1_edge2_y_intercept = trr1_y2 - trr1_edge2_slope * trr1_x2;

				man_arc_merge->x1_coordinate = (trr1_edge2_y_intercept - trr2_core_y_intercept) / (trr2->core->slope - trr1_edge2_slope);
				man_arc_merge->y1_coordinate = trr2->core->slope * man_arc_merge->x1_coordinate + trr2_core_y_intercept;

				single_intersect_flag = true;

				if(do_intersect(trr2->core->x1_coordinate, trr2->core->y1_coordinate, trr2->core->x2_coordinate, trr2->core->y2_coordinate,
						trr1_x1, trr1_y1, trr1_x3, trr1_y3)) {
					trr1_edge4_slope = round((trr1_y3 - trr1_y1) / (trr1_x3 - trr1_x1));
					trr1_edge4_y_intercept = trr1_y1 - trr1_edge4_slope * trr1_x1;

					man_arc_merge->x2_coordinate = (trr1_edge4_y_intercept - trr2_core_y_intercept) / (trr2->core->slope - trr1_edge4_slope);
					man_arc_merge->y2_coordinate = trr2->core->slope * man_arc_merge->x2_coordinate + trr2_core_y_intercept;

					double_intersect_flag = true;
				} else {
					double_intersect_flag = false;
				}
			}

			else if(do_intersect(trr2->core->x1_coordinate, trr2->core->y1_coordinate, trr2->core->x2_coordinate, trr2->core->y2_coordinate,
						trr1_x3, trr1_y3, trr1_x4, trr1_y4)) {
				trr1_edge3_slope = round((trr1_y4 - trr1_y3) / (trr1_x4 - trr1_x3));
				trr1_edge3_y_intercept = trr1_y3 - trr1_edge3_slope * trr1_x3;

				man_arc_merge->x1_coordinate = (trr1_edge3_y_intercept - trr2_core_y_intercept) / (trr2->core->slope - trr1_edge3_slope);
				man_arc_merge->y1_coordinate = trr2->core->slope * man_arc_merge->x1_coordinate + trr2_core_y_intercept;

				single_intersect_flag = true;
				double_intersect_flag = false;
			}

			else if(do_intersect(trr2->core->x1_coordinate, trr2->core->y1_coordinate, trr2->core->x2_coordinate, trr2->core->y2_coordinate,
						trr1_x1, trr1_y1, trr1_x3, trr1_y3)) {
				trr1_edge4_slope = round((trr1_y3 - trr1_y1) / (trr1_x3 - trr1_x1));
				trr1_edge4_y_intercept = trr1_y1 - trr1_edge4_slope * trr1_x1;

				man_arc_merge->x1_coordinate = (trr1_edge4_y_intercept - trr2_core_y_intercept) / (trr2->core->slope - trr1_edge4_slope);
				man_arc_merge->y1_coordinate = trr2->core->slope * man_arc_merge->x1_coordinate + trr2_core_y_intercept;

				single_intersect_flag = true;
				double_intersect_flag = false;
			}

			else if(orientation(trr1_x1, trr1_y1, trr1_x2, trr1_y2, trr2->core->x1_coordinate, trr2->core->y1_coordinate) == 0 &&
						is_point_on_line_segment(trr2->core->x1_coordinate, trr2->core->y1_coordinate, trr1_x1, trr1_y1, trr1_x2, trr1_y2)) {
				man_arc_merge->x1_coordinate = trr2->core->x1_coordinate;
				man_arc_merge->y1_coordinate = trr2->core->y1_coordinate;

				if(trr2->core->y2_coordinate > trr1_y1) {
					man_arc_merge->x2_coordinate = trr1_x1;
					man_arc_merge->y2_coordinate = trr1_y1;
				} else if(trr2->core->y2_coordinate < trr1_y2) {
					man_arc_merge->x2_coordinate = trr1_x2;
					man_arc_merge->y2_coordinate = trr1_y2;
				} else {
					man_arc_merge->x2_coordinate = trr2->core->x2_coordinate;
					man_arc_merge->y2_coordinate = trr2->core->y2_coordinate;
				}

				single_intersect_flag = true;
				double_intersect_flag = true;
			}

			else if(orientation(trr1_x1, trr1_y1, trr1_x2, trr1_y2, trr2->core->x2_coordinate, trr2->core->y2_coordinate) == 0 &&
						is_point_on_line_segment(trr2->core->x2_coordinate, trr2->core->y2_coordinate, trr1_x1, trr1_y1, trr1_x2, trr1_y2)) {
				man_arc_merge->x1_coordinate = trr2->core->x2_coordinate;
				man_arc_merge->y1_coordinate = trr2->core->y2_coordinate;

				if(trr2->core->y1_coordinate > trr1_y1) {
					man_arc_merge->x2_coordinate = trr1_x1;
					man_arc_merge->y2_coordinate = trr1_y1;
				} else if(trr2->core->y1_coordinate < trr1_y2) {
					man_arc_merge->x2_coordinate = trr1_x2;
					man_arc_merge->y2_coordinate = trr1_y2;
				} else {
					man_arc_merge->x2_coordinate = trr2->core->x1_coordinate;
					man_arc_merge->y2_coordinate = trr2->core->y1_coordinate;
				}

				single_intersect_flag = true;
				double_intersect_flag = true;
			}

			else if(orientation(trr1_x2, trr1_y2, trr1_x4, trr1_y4, trr2->core->x1_coordinate, trr2->core->y1_coordinate) == 0 &&
						is_point_on_line_segment(trr2->core->x1_coordinate, trr2->core->y1_coordinate, trr1_x2, trr1_y2, trr1_x4, trr1_y4)) {
				man_arc_merge->x1_coordinate = trr2->core->x1_coordinate;
				man_arc_merge->y1_coordinate = trr2->core->y1_coordinate;

				if(trr2->core->x2_coordinate > trr1_x2) {
					man_arc_merge->x2_coordinate = trr1_x2;
					man_arc_merge->y2_coordinate = trr1_y2;
				} else if(trr2->core->x2_coordinate < trr1_x4) {
					man_arc_merge->x2_coordinate = trr1_x4;
					man_arc_merge->y2_coordinate = trr1_y4;
				} else {
					man_arc_merge->x2_coordinate = trr2->core->x2_coordinate;
					man_arc_merge->y2_coordinate = trr2->core->y2_coordinate;
				}

				single_intersect_flag = true;
				double_intersect_flag = true;
			}

			else if(orientation(trr1_x2, trr1_y2, trr1_x4, trr1_y4, trr2->core->x2_coordinate, trr2->core->y2_coordinate) == 0 &&
						is_point_on_line_segment(trr2->core->x2_coordinate, trr2->core->y2_coordinate, trr1_x2, trr1_y2, trr1_x4, trr1_y4)) {
				man_arc_merge->x1_coordinate = trr2->core->x2_coordinate;
				man_arc_merge->y1_coordinate = trr2->core->y2_coordinate;

				if(trr2->core->x1_coordinate > trr1_x2) {
					man_arc_merge->x2_coordinate = trr1_x2;
					man_arc_merge->y2_coordinate = trr1_y2;
				} else if(trr2->core->x1_coordinate < trr1_x4) {
					man_arc_merge->x2_coordinate = trr1_x4;
					man_arc_merge->y2_coordinate = trr1_y4;
				} else {
					man_arc_merge->x2_coordinate = trr2->core->x1_coordinate;
					man_arc_merge->y2_coordinate = trr2->core->y1_coordinate;
				}

				single_intersect_flag = true;
				double_intersect_flag = true;
			}

			else if(orientation(trr1_x3, trr1_y3, trr1_x4, trr1_y4, trr2->core->x1_coordinate, trr2->core->y1_coordinate) == 0 &&
						is_point_on_line_segment(trr2->core->x1_coordinate, trr2->core->y1_coordinate, trr1_x3, trr1_y3, trr1_x4, trr1_y4)) {
				man_arc_merge->x1_coordinate = trr2->core->x1_coordinate;
				man_arc_merge->y1_coordinate = trr2->core->y1_coordinate;

				if(trr2->core->y2_coordinate > trr1_y3) {
					man_arc_merge->x2_coordinate = trr1_x3;
					man_arc_merge->y2_coordinate = trr1_y3;
				} else if(trr2->core->y2_coordinate < trr1_y4) {
					man_arc_merge->x2_coordinate = trr1_x4;
					man_arc_merge->y2_coordinate = trr1_y4;
				} else {
					man_arc_merge->x2_coordinate = trr2->core->x2_coordinate;
					man_arc_merge->y2_coordinate = trr2->core->y2_coordinate;
				}

				single_intersect_flag = true;
				double_intersect_flag = true;
			}

			else if(orientation(trr1_x3, trr1_y3, trr1_x4, trr1_y4, trr2->core->x2_coordinate, trr2->core->y2_coordinate) == 0 &&
						is_point_on_line_segment(trr2->core->x2_coordinate, trr2->core->y2_coordinate, trr1_x3, trr1_y3, trr1_x4, trr1_y4)) {
				man_arc_merge->x1_coordinate = trr2->core->x2_coordinate;
				man_arc_merge->y1_coordinate = trr2->core->y2_coordinate;

				if(trr2->core->y1_coordinate > trr1_y3) {
					man_arc_merge->x2_coordinate = trr1_x3;
					man_arc_merge->y2_coordinate = trr1_y3;
				} else if(trr2->core->y1_coordinate < trr1_y4) {
					man_arc_merge->x2_coordinate = trr1_x4;
					man_arc_merge->y2_coordinate = trr1_y4;
				} else {
					man_arc_merge->x2_coordinate = trr2->core->x1_coordinate;
					man_arc_merge->y2_coordinate = trr2->core->y1_coordinate;
				}

				single_intersect_flag = true;
				double_intersect_flag = true;
			}

			else if(orientation(trr1_x1, trr1_y1, trr1_x3, trr1_y3, trr2->core->x1_coordinate, trr2->core->y1_coordinate) == 0 &&
						is_point_on_line_segment(trr2->core->x1_coordinate, trr2->core->y1_coordinate, trr1_x1, trr1_y1, trr1_x3, trr1_y3)) {
				man_arc_merge->x1_coordinate = trr2->core->x1_coordinate;
				man_arc_merge->y1_coordinate = trr2->core->y1_coordinate;

				if(trr2->core->x2_coordinate > trr1_x1) {
					man_arc_merge->x2_coordinate = trr1_x1;
					man_arc_merge->y2_coordinate = trr1_y1;
				} else if(trr2->core->x2_coordinate < trr1_x3) {
					man_arc_merge->x2_coordinate = trr1_x3;
					man_arc_merge->y2_coordinate = trr1_y3;
				} else {
					man_arc_merge->x2_coordinate = trr2->core->x2_coordinate;
					man_arc_merge->y2_coordinate = trr2->core->y2_coordinate;
				}

				single_intersect_flag = true;
				double_intersect_flag = true;
			}

			else if(orientation(trr1_x1, trr1_y1, trr1_x3, trr1_y3, trr2->core->x2_coordinate, trr2->core->y2_coordinate) == 0 &&
						is_point_on_line_segment(trr2->core->x2_coordinate, trr2->core->y2_coordinate, trr1_x1, trr1_y1, trr1_x3, trr1_y3)) {
				man_arc_merge->x1_coordinate = trr2->core->x2_coordinate;
				man_arc_merge->y1_coordinate = trr2->core->y2_coordinate;

				if(trr2->core->x1_coordinate > trr1_x1) {
					man_arc_merge->x2_coordinate = trr1_x1;
					man_arc_merge->y2_coordinate = trr1_y1;
				} else if(trr2->core->x1_coordinate < trr1_x3) {
					man_arc_merge->x2_coordinate = trr1_x3;
					man_arc_merge->y2_coordinate = trr1_y3;
				} else {
					man_arc_merge->x2_coordinate = trr2->core->x1_coordinate;
					man_arc_merge->y2_coordinate = trr2->core->y1_coordinate;
				}

				single_intersect_flag = true;
				double_intersect_flag = true;
			}

			else {
				man_arc_merge->x1_coordinate = trr2->core->x1_coordinate;
				man_arc_merge->y1_coordinate = trr2->core->y1_coordinate;

				single_intersect_flag = false;
				double_intersect_flag = false;
			}

			if(single_intersect_flag && !double_intersect_flag && is_point_inside_trr(trr2->core->x1_coordinate, trr2->core->y1_coordinate,
					trr1_x1, trr1_y1, trr1_x2, trr1_y2, trr1_x3, trr1_y3, trr1_x4, trr1_y4)) {
				man_arc_merge->x2_coordinate = trr2->core->x1_coordinate;
				man_arc_merge->y2_coordinate = trr2->core->y1_coordinate;
			} else if((single_intersect_flag && !double_intersect_flag && is_point_inside_trr(trr2->core->x2_coordinate,
						trr2->core->y2_coordinate, trr1_x1, trr1_y1, trr1_x2, trr1_y2, trr1_x3, trr1_y3, trr1_x4, trr1_y4)) ||
						(!single_intersect_flag && !double_intersect_flag)) {
				man_arc_merge->x2_coordinate = trr2->core->x2_coordinate;
				man_arc_merge->y2_coordinate = trr2->core->y2_coordinate;
			}
		} else {
			man_arc_merge->x1_coordinate = trr2->core->x1_coordinate;
			man_arc_merge->y1_coordinate = trr2->core->y1_coordinate;

			man_arc_merge->x2_coordinate = trr2->core->x2_coordinate;
			man_arc_merge->y2_coordinate = trr2->core->y2_coordinate;
		}
	} else {
		trr1_core_y_intercept = trr1->core->y1_coordinate - trr1->core->slope * trr1->core->x1_coordinate;
		trr2_core_y_intercept = trr2->core->y1_coordinate - trr2->core->slope * trr2->core->x1_coordinate;

		man_arc_merge->x1_coordinate = (trr2_core_y_intercept - trr1_core_y_intercept) / (trr1->core->slope - trr2->core->slope);
		man_arc_merge->y1_coordinate = trr1->core->slope * man_arc_merge->x1_coordinate + trr1_core_y_intercept;

		man_arc_merge->x2_coordinate = man_arc_merge->x1_coordinate;
		man_arc_merge->y2_coordinate = man_arc_merge->y1_coordinate;
	}

	if(!almost_equal_relative(man_arc_merge->x1_coordinate, man_arc_merge->x2_coordinate) &&
			!almost_equal_relative(man_arc_merge->y1_coordinate, man_arc_merge->y2_coordinate)) {
		man_arc_merge->slope = round((man_arc_merge->y2_coordinate - man_arc_merge->y1_coordinate) /
				(man_arc_merge->x2_coordinate - man_arc_merge->x1_coordinate));
	}

	return man_arc_merge;
}

node *find_center(node *head)
{
	bucket *bucket_array;
	edge *edge_head = NULL;
	edge *indep_set_head;
	edge *temp;
	node *new_node;
	double diameter, bucket_size;
	int num_elements = find_num_elements(head);
	int d;

	diameter = find_diameter(head);

	while(num_elements != 1) {
		bucket_size = calculate_bucket_size(diameter, num_elements, &d);

		bucket_array = create_bucket_array(head, pow(d, 2), d, bucket_size);

		head = construct_nearest_neighbor_graph(head, bucket_array, &edge_head, d);

		merge_sort(&edge_head);

		indep_set_head = find_independent_set(edge_head);

		head = remove_node(head, indep_set_head->start_node);
		head = remove_node(head, indep_set_head->end_node);

		global_node_num += 1;

		new_node = create_internal_node(global_node_num, indep_set_head->start_node, indep_set_head->end_node);

		new_node = calculate_edge_lengths(new_node, indep_set_head->weight);

		new_node->left->trr->radius = new_node->left_wire_len;
		new_node->right->trr->radius = new_node->right_wire_len;

		new_node->trr->core = zero_skew_merge(new_node->trr->core, new_node->left->trr, new_node->right->trr);

		new_node->total_cap = new_node->left->total_cap + c * new_node->left_wire_len +
				new_node->right->total_cap + c * new_node->right_wire_len;
		new_node->delay = r * new_node->left_wire_len * (new_node->left->total_cap +
				0.5 * c * new_node->left_wire_len) + new_node->left->delay;

		new_node->next = head;
		head = new_node;

		temp = indep_set_head;
		indep_set_head = indep_set_head->next;
		free(temp);

		if(indep_set_head != NULL) {
			head = remove_node(head, indep_set_head->start_node);
			head = remove_node(head, indep_set_head->end_node);

			global_node_num += 1;

			new_node = create_internal_node(global_node_num, indep_set_head->start_node, indep_set_head->end_node);

			new_node = calculate_edge_lengths(new_node, indep_set_head->weight);

			new_node->left->trr->radius = new_node->left_wire_len;
			new_node->right->trr->radius = new_node->right_wire_len;

			new_node->trr->core = zero_skew_merge(new_node->trr->core, new_node->left->trr, new_node->right->trr);

			new_node->total_cap = new_node->left->total_cap + c * new_node->left_wire_len +
					new_node->right->total_cap + c * new_node->right_wire_len;
			new_node->delay = r * new_node->left_wire_len * (new_node->left->total_cap +
					0.5 * c * new_node->left_wire_len) + new_node->left->delay;

			new_node->next = head;
			head = new_node;

			temp = indep_set_head;
			indep_set_head = indep_set_head->next;
			free(temp);
		}

		free(bucket_array);

		while(edge_head != NULL) {
			temp = edge_head;
			edge_head = edge_head->next;
			free(temp);
		}

		while(indep_set_head != NULL) {
			temp = indep_set_head;
			indep_set_head = indep_set_head->next;
			free(temp);
		}

		num_elements = find_num_elements(head);
	}

	return head;
}

node *create_internal_node(int node_num, node *left_child, node *right_child)
{
	node *new_node;

	new_node = malloc(sizeof(node));
	new_node->trr = malloc(sizeof(tilted_rect_reg));
	new_node->trr->core = malloc(sizeof(manhattan_arc));

	new_node->leaf_node_label = -1;
	new_node->sink_node_cap = -1.0;
	new_node->x_coordinate = -1.0;
	new_node->y_coordinate = -1.0;
	new_node->left_wire_len = 0.0;
	new_node->right_wire_len = 0.0;
	new_node->total_cap = 0.0;
	new_node->delay = 0.0;
	new_node->max_delay = 0.0;
	new_node->min_delay = 0.0;
	new_node->num_node_inv = 0;
	new_node->num_left_inv = 0;
	new_node->num_right_inv = 0;
	new_node->node_num = node_num;

	new_node->trr->core->x1_coordinate = 0.0;
	new_node->trr->core->y1_coordinate = 0.0;
	new_node->trr->core->x2_coordinate = 0.0;
	new_node->trr->core->y2_coordinate = 0.0;
	new_node->trr->core->slope = 0;
	new_node->trr->core->row_num1 = 0;
	new_node->trr->core->col_num1 = 0;
	new_node->trr->core->bucket_num1 = 0;
	new_node->trr->core->row_num2 = 0;
	new_node->trr->core->col_num2 = 0;
	new_node->trr->core->bucket_num2 = 0;
	new_node->trr->radius = 0.0;

	new_node->is_left_done = false;
	new_node->is_right_done = false;

	new_node->nearest_neighbor = NULL;
	new_node->nearest_neighbor_dist = 0.0;

	new_node->left = left_child;
	new_node->right = right_child;

	new_node->next = NULL;
	new_node->bucket_next = NULL;

	return new_node;
}

node *remove_node(node *head, node *old_node)
{
	node *temp = head;
	node *prev;

	if(temp != NULL && temp->node_num == old_node->node_num) {
		head = temp->next;
		return head;
	}

	while(temp != NULL && temp->node_num != old_node->node_num) {
		prev = temp;
		temp = temp->next;
	}

	if(temp == NULL) {
		return head;
	}

	prev->next = temp->next;

	return head;
}

node *modify_binary_tree(node *root)
{
	if(root->left == NULL && root->right == NULL) {
		return root;
	}

	if(root->left == NULL) {
		return modify_binary_tree(root->right);
	}

	if(root->right == NULL) {
		root->right = root->left;
		root->left = NULL;
		return modify_binary_tree(root->right);
	}

	node *left = root->left;
	node *right = root->right;
	node *last_left = modify_binary_tree(left);

	root->right = left;
	root->left = NULL;
	last_left->right = right;

	return modify_binary_tree(right);
}

node *preorder_traversal(node *root)
{
	node *preorder_traversal_head = NULL;
	node *preorder_traversal_tail = NULL;

	while(root != NULL) {
		if(preorder_traversal_head == NULL) {
			preorder_traversal_head = root;
			preorder_traversal_head->next = NULL;
			preorder_traversal_tail = preorder_traversal_head;
		} else {
			preorder_traversal_tail->next = root;
			preorder_traversal_tail = preorder_traversal_tail->next;
			preorder_traversal_tail->next = NULL;
		}

		root = root->right;
	}

	return preorder_traversal_head;
}

node *copy_node(node *node1)
{
	node *node2;

	node2 = malloc(sizeof(node));
	node2->trr = malloc(sizeof(tilted_rect_reg));
	node2->trr->core = malloc(sizeof(manhattan_arc));

	node2->leaf_node_label = node1->leaf_node_label;
	node2->sink_node_cap = node1->sink_node_cap;
	node2->x_coordinate = node1->x_coordinate;
	node2->y_coordinate = node1->y_coordinate;
	node2->left_wire_len = node1->left_wire_len;
	node2->right_wire_len = node1->right_wire_len;
	node2->total_cap = node1->total_cap;
	node2->delay = node1->delay;
	node2->max_delay = node1->max_delay;
	node2->min_delay = node1->min_delay;
	node2->num_node_inv = node1->num_node_inv;
	node2->num_left_inv = node1->num_left_inv;
	node2->num_right_inv = node1->num_right_inv;
	node2->node_num = node1->node_num;

	node2->trr->core->x1_coordinate = node1->trr->core->x1_coordinate;
	node2->trr->core->y1_coordinate = node1->trr->core->y1_coordinate;
	node2->trr->core->x2_coordinate = node1->trr->core->x2_coordinate;
	node2->trr->core->y2_coordinate = node1->trr->core->y2_coordinate;
	node2->trr->core->slope = node1->trr->core->slope;
	node2->trr->core->row_num1 = node1->trr->core->row_num1;
	node2->trr->core->col_num1 = node1->trr->core->col_num1;
	node2->trr->core->bucket_num1 = node1->trr->core->bucket_num1;
	node2->trr->core->row_num2 = node1->trr->core->row_num2;
	node2->trr->core->col_num2 = node1->trr->core->col_num2;
	node2->trr->core->bucket_num2 = node1->trr->core->bucket_num2;
	node2->trr->radius = node1->trr->radius;

	node2->is_left_done = node1->is_left_done;
	node2->is_right_done = node1->is_right_done;

	node2->nearest_neighbor = NULL;
	node2->nearest_neighbor_dist = 0.0;

	node2->left = NULL;
	node2->right = NULL;

	node2->next = NULL;
	node2->bucket_next = NULL;

	return node2;
}

node *find_exact_placements(node *head)
{
	node *exact_placement_head = NULL;
	node *new_node;
	double trr_x1, trr_y1, trr_x2, trr_y2, trr_x3, trr_y3, trr_x4, trr_y4;
	bool is_point1_inside_trr, is_point2_inside_trr;
	double dist1, dist2;
	double temp_man_arc_y_intercept, temp_trr_edge1_y_intercept, temp_trr_edge2_y_intercept,
	temp_trr_edge3_y_intercept, temp_trr_edge4_y_intercept;

	while(head != NULL) {
		if(head->leaf_node_label != -1) {
			new_node = copy_node(head);

			new_node->left = NULL;
			new_node->right = NULL;

			new_node->is_left_done = true;
			new_node->is_right_done = true;

			if(!exact_placement_head->is_left_done) {
				exact_placement_head->left = new_node;
				exact_placement_head->is_left_done = true;
			} else {
				exact_placement_head->right = new_node;
				exact_placement_head->is_right_done = true;

				while(exact_placement_head->is_right_done && exact_placement_head->next != NULL) {
					exact_placement_head = exact_placement_head->next;
				}
			}
		} else {
			if(exact_placement_head == NULL) {
				exact_placement_head = copy_node(head);

				if(exact_placement_head->trr->core->x1_coordinate < exact_placement_head->trr->core->x2_coordinate) {
					exact_placement_head->x_coordinate = exact_placement_head->trr->core->x1_coordinate;
					exact_placement_head->y_coordinate = exact_placement_head->trr->core->y1_coordinate;
				} else {
					exact_placement_head->x_coordinate = exact_placement_head->trr->core->x2_coordinate;
					exact_placement_head->y_coordinate = exact_placement_head->trr->core->y2_coordinate;
				}

				exact_placement_head->next = NULL;
			} else {
				new_node = copy_node(head);

				if(!exact_placement_head->is_left_done) {
					exact_placement_head->left = new_node;
					exact_placement_head->is_left_done = true;

					trr_x1 = exact_placement_head->x_coordinate;
					trr_y1 = exact_placement_head->y_coordinate + exact_placement_head->left_wire_len;

					trr_x2 = exact_placement_head->x_coordinate + exact_placement_head->left_wire_len;
					trr_y2 = exact_placement_head->y_coordinate;

					trr_x3 = exact_placement_head->x_coordinate - exact_placement_head->left_wire_len;
					trr_y3 = exact_placement_head->y_coordinate;

					trr_x4 = exact_placement_head->x_coordinate;
					trr_y4 = exact_placement_head->y_coordinate - exact_placement_head->left_wire_len;
				} else {
					exact_placement_head->right = new_node;
					exact_placement_head->is_right_done = true;

					trr_x1 = exact_placement_head->x_coordinate;
					trr_y1 = exact_placement_head->y_coordinate + exact_placement_head->right_wire_len;

					trr_x2 = exact_placement_head->x_coordinate + exact_placement_head->right_wire_len;
					trr_y2 = exact_placement_head->y_coordinate;

					trr_x3 = exact_placement_head->x_coordinate - exact_placement_head->right_wire_len;
					trr_y3 = exact_placement_head->y_coordinate;

					trr_x4 = exact_placement_head->x_coordinate;
					trr_y4 = exact_placement_head->y_coordinate - exact_placement_head->right_wire_len;
				}

				is_point1_inside_trr = is_point_inside_trr(new_node->trr->core->x1_coordinate, new_node->trr->core->y1_coordinate,
						trr_x1, trr_y1, trr_x2, trr_y2, trr_x3, trr_y3, trr_x4, trr_y4);
				is_point2_inside_trr = is_point_inside_trr(new_node->trr->core->x2_coordinate, new_node->trr->core->y2_coordinate,
						trr_x1, trr_y1, trr_x2, trr_y2, trr_x3, trr_y3, trr_x4, trr_y4);
				if(is_point1_inside_trr && is_point2_inside_trr && !almost_equal_relative(trr_x1, trr_x2)) {
					dist1 = fabs(new_node->trr->core->x1_coordinate - exact_placement_head->x_coordinate) +
							fabs(new_node->trr->core->y1_coordinate - exact_placement_head->y_coordinate);
					dist2 = fabs(new_node->trr->core->x2_coordinate - exact_placement_head->x_coordinate) +
							fabs(new_node->trr->core->y2_coordinate - exact_placement_head->y_coordinate);
					if(dist1 < dist2) {
						new_node->x_coordinate = new_node->trr->core->x1_coordinate;
						new_node->y_coordinate = new_node->trr->core->y1_coordinate;
					} else {
						new_node->x_coordinate = new_node->trr->core->x2_coordinate;
						new_node->y_coordinate = new_node->trr->core->y2_coordinate;
					}
				} else if(is_point1_inside_trr && !almost_equal_relative(trr_x1, trr_x2)) {
					new_node->x_coordinate = new_node->trr->core->x1_coordinate;
					new_node->y_coordinate = new_node->trr->core->y1_coordinate;
				} else if(is_point2_inside_trr && !almost_equal_relative(trr_x1, trr_x2)) {
					new_node->x_coordinate = new_node->trr->core->x2_coordinate;
					new_node->y_coordinate = new_node->trr->core->y2_coordinate;
				} else if(!is_point1_inside_trr && !is_point2_inside_trr && !almost_equal_relative(trr_x1, trr_x2)) {
					temp_man_arc_y_intercept = new_node->trr->core->y1_coordinate -
							new_node->trr->core->slope * new_node->trr->core->x1_coordinate;

					temp_trr_edge1_y_intercept = trr_x1 + trr_y1;
					temp_trr_edge2_y_intercept = trr_y2 - trr_x2;
					temp_trr_edge3_y_intercept = trr_x3 + trr_y3;
					temp_trr_edge4_y_intercept = trr_y1 - trr_x1;

					if(new_node->trr->core->slope == -1 && almost_equal_relative(temp_man_arc_y_intercept, temp_trr_edge1_y_intercept)) {
						new_node->x_coordinate = trr_x1;
						new_node->y_coordinate = trr_y1;
					} else if(new_node->trr->core->slope == 1 && almost_equal_relative(temp_man_arc_y_intercept, temp_trr_edge2_y_intercept)) {
						new_node->x_coordinate = trr_x2;
						new_node->y_coordinate = trr_y2;
					} else if(new_node->trr->core->slope == -1 && almost_equal_relative(temp_man_arc_y_intercept, temp_trr_edge3_y_intercept)) {
						new_node->x_coordinate = trr_x4;
						new_node->y_coordinate = trr_y4;
					} else if(new_node->trr->core->slope == 1 && almost_equal_relative(temp_man_arc_y_intercept, temp_trr_edge4_y_intercept)) {
						new_node->x_coordinate = trr_x3;
						new_node->y_coordinate = trr_y3;
					} else {
						new_node->x_coordinate = (exact_placement_head->y_coordinate - temp_man_arc_y_intercept) / new_node->trr->core->slope;
						new_node->y_coordinate = exact_placement_head->y_coordinate;
					}
				} else {
					new_node->x_coordinate = exact_placement_head->x_coordinate;
					new_node->y_coordinate = exact_placement_head->y_coordinate;
				}

				new_node->next = exact_placement_head;
				exact_placement_head = new_node;
			}
		}

		head = head->next;
	}

	return exact_placement_head;
}

int postorder_traversal(node *root, FILE *fp, FILE *binary_fp)
{
	if(root != NULL) {
		postorder_traversal(root->left, fp, binary_fp);
		postorder_traversal(root->right, fp, binary_fp);

		if(root->leaf_node_label != -1) {
			fprintf(fp, "%d(%e %e %e)\n", root->leaf_node_label, root->sink_node_cap, root->x_coordinate, root->y_coordinate);

			fwrite(&root->leaf_node_label, sizeof(root->leaf_node_label), 1, binary_fp);
			fwrite(&root->sink_node_cap, sizeof(root->sink_node_cap), 1, binary_fp);
			fwrite(&root->x_coordinate, sizeof(root->x_coordinate), 1, binary_fp);
			fwrite(&root->y_coordinate, sizeof(root->y_coordinate), 1, binary_fp);
		} else {
			fprintf(fp, "(%e %e %e %e)\n", root->left_wire_len, root->right_wire_len, root->x_coordinate, root->y_coordinate);

			fwrite(&root->leaf_node_label, sizeof(root->leaf_node_label), 1, binary_fp);
			fwrite(&root->left_wire_len, sizeof(root->left_wire_len), 1, binary_fp);
			fwrite(&root->right_wire_len, sizeof(root->right_wire_len), 1, binary_fp);
			fwrite(&root->x_coordinate, sizeof(root->x_coordinate), 1, binary_fp);
			fwrite(&root->y_coordinate, sizeof(root->y_coordinate), 1, binary_fp);
		}
	}

	return 0;
}

int free_binary_tree(node *root)
{
	if(root != NULL) {
		free_binary_tree(root->left);
		free_binary_tree(root->right);

		free(root->trr->core);
		free(root->trr);
		free(root);
	}

	return 0;
}
