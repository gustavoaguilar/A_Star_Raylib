#ifndef LIST_H
#define LIST_H

#include<stdio.h>
#include<stdlib.h>
#include"raylib.h"

typedef struct t_Node{
    void* data;
    struct t_Node* next;
    struct t_Node* previous;
}t_Node;

typedef struct t_List{
    int size;
    t_Node* head;
    t_Node* tail;
}t_List;

t_List* list_create();
int list_push(t_List* list, void* data);
void* list_pop(t_List* list);
int list_add_first(t_List* list, void* data);
int list_remove_first(t_List* list);
int list_remove(t_List* list, void* data);
void list_print(t_List* list);
void list_erase(t_List* list);
void* list_get(t_List* list, int index);
int list_find(t_List* list, void* data);

#endif