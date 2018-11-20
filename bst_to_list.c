/*
 * bst_to_list.c
 *
 *  Created on: Nov 20, 2018
 *      Author: wei100
 */
#include "zst.h" 
void bst_to_list(node* root, node** list, int* index) {
    if(root == NULL) {
        return;
    }
    bst_to_list(root->left, list, index);
    bst_to_list(root->right, list, index);
    list[*index] = root;
    list[*index]->next = NULL;
    if(*index != 0) {
        list[*index - 1]->next = list[*index];
    }
    (*index)++;
}