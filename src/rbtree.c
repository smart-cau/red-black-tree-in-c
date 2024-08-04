#include "rbtree.h"

#include <stdlib.h>

node_t sentinel = {
  .color = RBTREE_BLACK,
  .key = 0,
  .parent = &sentinel,
  .left = &sentinel,
  .right = &sentinel
};

void left_rotate(rbtree* t, node_t* x){
  node_t* y = x->right;

  // 1. y의 left subtree를 x의 right subtree로 옮김
  x->right = y->left;
  if (y->left != t->nil)
    y->left->parent = x;

  // 2. 부모 작업
  y->parent = x->parent;
  if (x->parent == t->nil){
    t->root = y;
    y->parent = t->nil;
  }    
  if (x == x->parent->left)
    x->parent->left = y;
  else
    x->parent->right = y;

  // 3. x를 y의 left subtree로 편입
  y->left = x;
  x->parent = y;
}

void right_rotate(rbtree* t, node_t* y) {
  node_t* x = y->left;

  // 1. y의 왼쪽 자식으로 x의 오른쪽 자식을 붙임
  y->left = x->right;
  if (x->right != t->nil)
    x->right->parent = y;
  
  // 2. x의 부모를 y의 부모로 교체
  x->parent = y->parent;
  if (y->parent == t->nil){
      t->root = x;
      x->parent = t->nil;
    }
  if (y == y->parent->right)
    y->parent->right = x;
  else
    y->parent->left = x;

  // 3. x의 오른쪽 자식으로 y
  x->right = y;
  y->parent = x;
}

rbtree *new_rbtree(void) {
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
  // TODO: initialize struct if needed
  if (p == NULL)
    return NULL;

  p->nil = &sentinel;
  p->root = p->nil;

  return p;
}

void delete_rbtree_helper(rbtree *t, node_t* node) {
  if (node == t->nil) 
    return;

  delete_rbtree_helper(t, node->left);
  delete_rbtree_helper(t, node->right);
  free(node);
}

void delete_rbtree(rbtree *t) {
  // TODO: reclaim the tree nodes's memory
  node_t* root = t->root;
  if (root != t->nil)
    delete_rbtree_helper(t, root);
  free(t);
}

int is_left_child_of_p(node_t* child, node_t* parent) {
  if (child == parent->left)
    return 1;
  return 0;
}

int is_right_child_of_p(node_t* child, node_t* parent) {
  if (child == parent->right)
    return 1;
  return 0;
}

void rbtree_insert_fixup(rbtree *t, node_t* check_n) {
  // check_n에 double red 현상이 있는 경우
  while (check_n->parent->color == RBTREE_RED) {
    node_t* _parent = check_n->parent;
    node_t* uncle;
    if (is_left_child_of_p(_parent, _parent->parent))
      uncle = _parent->parent->right;
    else
      uncle = _parent->parent->left;
    /* case 1 */
    if (uncle->color == RBTREE_RED) {
      _parent->color = RBTREE_BLACK;
      uncle->color = RBTREE_BLACK;
      _parent->parent->color = RBTREE_RED;
      check_n = _parent->parent;
      continue;
    }
    /* case 2 & 3. uncle->color == RBTREE_BLACK */
    if (is_left_child_of_p(check_n->parent, check_n->parent->parent)) {
        // case 2
        if (is_right_child_of_p(check_n, check_n->parent)) {   
          check_n = check_n->parent;
          left_rotate(t, check_n);
        }
        // case 3
        if (is_left_child_of_p(check_n, check_n->parent)) {
          check_n->parent->color = RBTREE_BLACK;
          check_n->parent->parent->color = RBTREE_RED;
          right_rotate(t, check_n->parent->parent);
        }
        continue;
      }

      // check의 부모가 조부모의 오른쪽 자녀
      // case 3
      if (is_right_child_of_p(check_n, check_n->parent)) {   
        check_n->parent->color = RBTREE_BLACK;
        check_n->parent->parent->color = RBTREE_RED;
        left_rotate(t, check_n->parent->parent);
      }
      // case 2
      if (is_left_child_of_p(check_n, check_n->parent)) {
        check_n = check_n->parent;
        right_rotate(t, check_n);
      }
  }

  t->root->color = RBTREE_BLACK;
}

node_t *rbtree_insert(rbtree *t, const key_t key) {
  // TODO: implement insert0
  node_t* target = t->root;

  while (target != t->nil) {
    if (target->key > key){
      if (target->left == t->nil)
        break;
      target = target->left;
    }
    if (target->key <= key){
      if (target->right == t->nil)
        break;
      target = target->right;
    }
  }

  node_t* new_n = (node_t*)calloc(1, sizeof(node_t));
  new_n->key = key;
  new_n->color = RBTREE_RED;
  new_n->left = t->nil;
  new_n->right = t->nil;

  new_n->parent = target;
  if (target == t->nil)
    t->root = new_n;
  if (target != t->nil) {
    if (target->key > key)
      target->left = new_n;
    if (target->key <= key)
      target->right = new_n;
  }

  rbtree_insert_fixup(t, new_n);

  return new_n;
}

node_t *rbtree_find(const rbtree *t, const key_t key) {
  // TODO: implement find  
  node_t* target = t->root;
  while (target != t->nil && target->key != key){
    if (target->key > key)
      target = target->left;
    if (target->key < key)
      target = target->right;
  }
  
  if (target == t->nil)
    return NULL;
  
  return target;
}

node_t *rbtree_min(const rbtree *t) {
  // TODO: implement find
  node_t* min = t->root;

  if (min != t->nil)
    min = tree_find_min_max(t, min, FIND_MIN);
  return min;
}

node_t *rbtree_max(const rbtree *t) {
  // TODO: implement find
  node_t* max = t->root;

  if (max != t->nil)
    max = tree_find_min_max(t, max, FIND_MAX);
  return max;
}

node_t* tree_find_min_max(const rbtree *t, node_t* node, find_t findType) {
  node_t* target = node;

  while (findType == FIND_MIN && target->left) {
    if (target->left == t->nil)
      break;
    target = target->left;
  }
    
  
  while (findType == FIND_MAX && target->right) {
    if (target->right == t->nil)
      break;
    target = target->right;
  }    

  return target;
}

void transplant(rbtree* t, node_t* delete, node_t* successor) {
  if (delete->parent == t->nil)
    t->root = successor;
  if (delete == delete->parent->left)
    delete->parent->left = successor;
  if (delete == delete->parent->right)
    delete->parent->right = successor;

  successor->parent = delete->parent;
}

int rbtree_erase(rbtree *t, node_t *p) {
  // TODO: implement erase
  

  return 0;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  // TODO: implement to_array
  return 0;
}
