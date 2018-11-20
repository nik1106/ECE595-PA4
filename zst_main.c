/*
 * zst_main.c
 *
 *  Created on: Oct 25, 2018
 *      Author: nchhabri
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "zst.h"

int main(int argc, char *argv[])
{
	if(argc != 6) {
		printf("Usage: %s input_filename1 input_filename2 input_filename3 output_filename1 output_filename2\n", argv[0]);
		return 1;
	}

	node *sink_node_head;
	node *binary_tree_head;
	node *preorder_traversal_head;
	node *exact_placement_head;
	node *temp;
	FILE *fp;
	FILE *binary_fp;

	/* Read the file input_filename1 */
	fp = fopen(argv[1], "r");
	if(fp == NULL) {
		printf("Error opening file %s for reading\n", argv[1]);
	}
	fscanf(fp, "%le %le %le\n", &inv_cin, &inv_cout, &inv_rout);
	fclose(fp);

	/* Read the file input_filename2 */
	fp = fopen(argv[2], "r");
	if(fp == NULL) {
		printf("Error opening file %s for reading\n", argv[2]);
	}
	fscanf(fp, "%le %le\n", &r, &c);
	fclose(fp);

	/* Read the file input_filename3 */
	sink_node_head = read_sink_input_file(argv[3]);
	if(sink_node_head == NULL) {
		printf("Error opening file %s for reading\n", argv[1]);
		return 1;
	}

	binary_tree_head = find_center(sink_node_head);

	modify_binary_tree(binary_tree_head);

	preorder_traversal_head = preorder_traversal(binary_tree_head);

	exact_placement_head = find_exact_placements(preorder_traversal_head);

	/*Convert bst into linked list in post order*/
	int num_nodes = find_num_elements(preorder_traversal_head);
	int index = 0;
	node** list = malloc(sizeof(node) * num_nodes);
	bst_to_list(exact_placement_head, list, &index);
	node* linked_list_head = list[0];
	list[num_nodes - 1]->next = NULL;
	free(list);

	fp = fopen(argv[4], "w");
	if(fp == NULL) {
		printf("Error opening file %s for writing\n", argv[2]);
		return 1;
	}
	binary_fp = fopen(argv[5], "w");
	if(binary_fp == NULL) {
		printf("Error opening file %s for writing\n", argv[3]);
		return 1;
	}
	postorder_traversal(exact_placement_head, fp, binary_fp);
	fclose(fp);
	fclose(binary_fp);

	while(preorder_traversal_head != NULL) {
		temp = preorder_traversal_head;
		preorder_traversal_head = preorder_traversal_head->next;

		free(temp->trr->core);
		free(temp->trr);
		free(temp);
	}

	free_binary_tree(exact_placement_head);

	return 0;
}
