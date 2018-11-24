#include "zst.h"
void test_insert(node* post_order_traversal_head) {
    while(post_order_traversal_head != NULL) {
        if(post_order_traversal_head->leaf_node_label == -1) {
            insert_inv_left(post_order_traversal_head, post_order_traversal_head->left);
            if(post_order_traversal_head->right != NULL) {
                insert_inv_right(post_order_traversal_head, post_order_traversal_head->right);
            }
        }
        post_order_traversal_head = post_order_traversal_head->next;
    }
}
