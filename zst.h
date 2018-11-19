/*
 * zst.h
 *
 *  Created on: Oct 25, 2018
 *      Author: nchhabri
 */

#ifndef ZST_H
#define ZST_H
#define EPSILON 0.00001

#define SKEW_BOUND 3.0e-12
#define TRANS_TIME_BOUND 80.0e-12

double inv_cin, inv_cout, inv_rout;
double r, c;
double square_edge1_y_intercept, square_edge2_y_intercept;
int global_node_num;

typedef struct manhattan_arc {
	double x1_coordinate;
	double y1_coordinate;
	double x2_coordinate;
	double y2_coordinate;
	int slope;
	int row_num1;
	int col_num1;
	int bucket_num1;
	int row_num2;
	int col_num2;
	int bucket_num2;
} manhattan_arc;

typedef struct tilted_rect_reg {
	manhattan_arc *core;
	double radius;
} tilted_rect_reg;

typedef struct node {
	int leaf_node_label;
	double sink_node_cap;
	double x_coordinate;
	double y_coordinate;
	double left_wire_len;
	double right_wire_len;
	double total_cap;
	double delay;
	double max_delay;
	double min_delay;
	int num_node_inv;
	int num_left_inv;
	int num_right_inv;
	int node_num;
	tilted_rect_reg *trr;
	bool is_left_done;
	bool is_right_done;

	struct node *nearest_neighbor;
	double nearest_neighbor_dist;

	struct node *left;
	struct node *right;

	struct node *next;
	struct node *bucket_next;
} node;

typedef struct bucket {
	node *head;
	bool visited;
} bucket;

typedef struct edge {
	node *start_node;
	node *end_node;
	double weight;

	struct edge *next;
} edge;

/* Function declarations */
node *read_sink_input_file(char filename[]);
node *insert_end(node *head, int leaf_node_label, double sink_node_cap, double x_coordinate,
		double y_coordinate, int node_num);
node *insert_bucket_list(node *head, node *new_node);
node *construct_nearest_neighbor_graph(node *head, bucket *bucket_array, edge **edge_head_ptr, int d);
node *calculate_edge_lengths(node *internal_node, double shortest_dist);
node *find_center(node *head);
node *create_internal_node(int node_num, node *left_child, node *right_child);
node *remove_node(node *head, node *old_node);
node *modify_binary_tree(node *root);
node *preorder_traversal(node *root);
node *copy_node(node *node1);
node *find_exact_placements(node *head);
bucket *create_bucket_array(node *head, int num_buckets, int d, double bucket_size);
edge *sorted_merge(edge *a, edge *b);
edge *find_independent_set(edge *head);
manhattan_arc *zero_skew_merge(manhattan_arc *man_arc_merge, tilted_rect_reg *trr1, tilted_rect_reg *trr2);
double s(double x, double y, double z);
double find_diameter(node *head);
double calculate_bucket_size(double diameter, double num_elements, int *d);
double shortest_distance(manhattan_arc *man_arc1, manhattan_arc *man_arc2);
double calculate_triangle_area(double x1, double y1, double x2, double y2, double x3, double y3);
bool is_point_on_line_segment(double p_x, double p_y, double q_x, double q_y, double r_x, double r_y);
bool do_intersect(double p1_x, double p1_y, double q1_x, double q1_y, double p2_x, double p2_y, double q2_x, double q2_y);
bool is_point_inside_trr(double x, double y, double trr_x1, double trr_y1, double trr_x2, double trr_y2,
		double trr_x3, double trr_y3, double trr_x4, double trr_y4);
bool almost_equal_relative(double dbl1, double dbl2);
int orientation(double x1, double y1, double x2, double y2, double x3, double y3);
int find_num_elements(node *head);
int merge_sort(edge **head_ptr);
int front_back_split(edge *source, edge **front_ptr, edge **back_ptr);
int postorder_traversal(node *root, FILE *fp, FILE *binary_fp);
int free_binary_tree(node *root);

#endif /* ZST_H */
