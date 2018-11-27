/*
 * test_insert.c
 *
 *  Created on: Nov 23, 2018
 *      Author: wei100
 */

#include "zst.h"

void test_insert(node* postorder_traversal_head) {
    while(postorder_traversal_head != NULL) {
        if(postorder_traversal_head->leaf_node_label == -1) {
            insert_inv_left(postorder_traversal_head, postorder_traversal_head->left);
            if(postorder_traversal_head->right != NULL) {
                insert_inv_right(postorder_traversal_head, postorder_traversal_head->right);
            }
            parity_adjust(postorder_traversal_head);
        }
        postorder_traversal_head = postorder_traversal_head->next;
    }
}
