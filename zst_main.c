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
	if(argc != 4) {
		printf("Usage: %s input_filename output_filename1 output_filename2\n", argv[0]);
		return 1;
	}

	node *sink_node_head;
	node *binary_tree_head;
	node *preorder_traversal_head;
	node *exact_placement_head;
	node *temp;
	FILE *fp;
	FILE *binary_fp;

	/* Read the file input_filename */
	sink_node_head = read_input_file(argv[1]);
	if(sink_node_head == NULL) {
		printf("Error opening file %s for reading\n", argv[1]);
		return 1;
	}

	binary_tree_head = find_center(sink_node_head);

	modify_binary_tree(binary_tree_head);

	preorder_traversal_head = preorder_traversal(binary_tree_head);

	exact_placement_head = find_exact_placements(preorder_traversal_head);

	fp = fopen(argv[2], "w");
	if(fp == NULL) {
		printf("Error opening file %s for writing\n", argv[2]);
		return 1;
	}
	binary_fp = fopen(argv[3], "w");
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
