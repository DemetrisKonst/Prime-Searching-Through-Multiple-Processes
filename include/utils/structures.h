#pragma once

#include <cstdio>
#include <cmath>

#include "../core/prime_item.h"

/*
The following code is an implementation of a Priority Queue of PrimeItems.
*/

typedef struct QueueNode* node_ptr;

struct QueueNode{
  PrimeItem item;
  node_ptr next;
  node_ptr prev;

  QueueNode (PrimeItem i) {
    item = i;
    next = NULL;
    prev = NULL;
  }

  ~QueueNode () {

  }

  void insertNext (node_ptr node) {
    node->next = next;
    if (next)
      next->prev = node;

    next = node;
    next->prev = this;
  }

  void insertPrev (node_ptr node) {
    node->prev = prev;
    if (prev)
      prev->next = node;

    prev = node;
    prev->next = this;
  }
};


class PriorityQueue{
private:
  node_ptr head;
  node_ptr tail;
  int items;

public:
  PriorityQueue () {
    head = NULL;
    tail = NULL;
    items = 0;
  }

  ~PriorityQueue () {
    node_ptr current = head;

    while (current){
        node_ptr next_node = current->next;
        delete current;
        current = next_node;
    }
  }

  int compare (PrimeItem x, PrimeItem y) {
    if (x.number < y.number)
      return -1;
    else if (x.number == y.number)
      return 0;
    else
      return 1;
  }

  void push (PrimeItem item) {
    node_ptr node = new QueueNode(item);
    // item->print();
    // If this is the first item to be added, initialize the head & tail as the new item
    if (head == NULL) {
      head = node;
      tail = head;
      items++;
      return;
    }

    node_ptr current = head;

    while (current->next != NULL) {
      if (compare(node->item, current->item) == -1){
        current->insertPrev(node);

        if (current == head)
          head = node;

        items++;
        return;
      }
      current = current->next;
    }

    if (compare(node->item, current->item) == -1){
      current->insertPrev(node);
      if (current == head)
        head = node;
    }else{
      current->insertNext(node);
      tail = node;
    }

    items++;
  }

  PrimeItem pop () {
    if (head == NULL)
      return PrimeItem();

    PrimeItem item = head->item;
    node_ptr temp = head;

    if (head->next)
      head->next->prev = NULL;
    head = head->next;

    delete temp;
    items--;
    return item;
  }

  void remove (int index) {
    node_ptr current = head;

    for (int i = 0; i < index; i++) {
      current = current->next;
    }

    // If it is the only node, set the head & tail to NULL
    if (items == 1) {
      head = NULL;
      tail = NULL;
    // If it is the last node, set the tail equal to its previous
    }else if (current == tail){
      tail = current->prev;
      tail->next = NULL;
    // If it is the first node, set the head equal to its next
    }else if (current == head){
      head = current->next;
      head->prev = NULL;
    // If it is in the middle, unlink it from both ends
    }else{
      current->prev->next = current->next;
      current->next->prev = current->prev;
    }

    // Finally delete the node and decrease the item counter
    delete current;
    items--;
  }

  int size () {
    return items;
  }

  bool empty () {
    return items == 0;
  }

  void print () {
    node_ptr current = head;

    while (current != NULL) {
      current->item.print();
      current = current->next;
    }
  }


  PrimeItem** split_in_batches (int batch_size) {
    int batch_amount = ceil(items*1.0/batch_size);

    PrimeItem** batch_arr = new PrimeItem*[batch_amount];

    for (int i = 0; i < batch_amount; i++){
      batch_arr[i] = new PrimeItem[batch_size];
      for (int j = 0; j < batch_size; j++){
        if (items == 0)
          return batch_arr;

        PrimeItem tmp = pop();
        batch_arr[i][j] = tmp;
      }
    }

    return batch_arr;
  }
};
