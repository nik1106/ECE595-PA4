#include "zst.h"
void test_parity_adjust(node* curr, node* parent) {
    if(curr == NULL) {
        return;
    }
    curr->num_up_inv += curr->num_node_inv + parent->num_up_inv;
    if(curr->leaf_node_label != -1) {
        if(curr->num_up_inv % 2 == 1) {
            node* new_node = create_internal_node(-1, parent->left, parent->right);
            new_node->x_coordinate = parent->x_coordinate;
            new_node->y_coordinate = parent->y_coordinate;
            new_node->left_wire_len = parent->left_wire_len;
            parent->left_wire_len = 0.0;
            new_node->right_wire_len = parent->right_wire_len;
            parent->right_wire_len = -1.0;
            new_node->num_node_inv = 1;
            new_node->total_cap = parent->total_cap;
            parent->total_cap = inv_cin;
            if(parent->node_num == -1){
                new_node->next = parent;
                curr->next = new_node;
            }
            else{
                while(curr->next != parent) {
                    curr = curr->next;
                }
                curr->next = new_node;
                new_node->next = parent;
            }
            parent->left = new_node;
            parent->right = NULL;
            new_node->num_up_inv = new_node->num_node_inv + parent->num_up_inv;
            curr->num_up_inv++;

        }
    }
    test_parity_adjust(curr->left, curr);
    test_parity_adjust(curr->right, curr);
}