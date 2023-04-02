#ifndef QUEUE_H
#define QUEUE_H


typedef struct node {
	void *data;
	struct node *next;
} node;


node *node_create();
node *queue_add(node *head, void *data);
void queue_insert_after(node *pos, void *data);
void queue_insert_before(node **head, node *pos, void *data);
void *queue_peek(node *head);
void *queue_pop(node **head);
void *priority_queue_peek(node *h1, node *h2, node *h3);
void *priority_queue_pop(node **h1, node **h2, node **h3);
int queue_len(node *head);

#endif
