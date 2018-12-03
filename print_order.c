#include "zst.h"
void print_t(FILE* fptr2, node* head) {
    if(head == NULL) {
        return;
    }
    print_t(fptr2, head->left); 
    print_t(fptr2, head->right); 
    fprintf(fptr2, "%p\n", head);
}
void print_order(node* post_order, node* head_order){ 
    FILE* fptr1 = fopen("Post.txt", "w");
    while(post_order != NULL) {
        fprintf(fptr1, "%p\n", post_order);
        post_order = post_order->next;
    }
    fclose(fptr1);
    FILE* fptr2 = fopen("Tree.txt", "w");
    print_t(fptr2,head_order);
    fclose(fptr2);
    return;

}