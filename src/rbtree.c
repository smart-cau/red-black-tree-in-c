#include "rbtree.h"

#include <stdlib.h>

node_t sentinel = {
  .color = RBTREE_BLACK,
  .key = 0,
  .parent = &sentinel,
  .left = &sentinel,
  .right = &sentinel
};

void left_rotate(rbtree* tree, node_t* pivot_node) {
    node_t* new_top = pivot_node->right;
    pivot_node->right = new_top->left;
    if (new_top->left != tree->nil)
        new_top->left->parent = pivot_node;
    new_top->parent = pivot_node->parent;
    if (pivot_node->parent == tree->nil)
        tree->root = new_top;
    else if (pivot_node == pivot_node->parent->left)
        pivot_node->parent->left = new_top;
    else
        pivot_node->parent->right = new_top;
    new_top->left = pivot_node;
    pivot_node->parent = new_top;
}

void right_rotate(rbtree* tree, node_t* pivot_node) {
    node_t* new_top = pivot_node->left;
    pivot_node->left = new_top->right;
    if (new_top->right != tree->nil)
        new_top->right->parent = pivot_node;
    new_top->parent = pivot_node->parent;
    if (pivot_node->parent == tree->nil)
        tree->root = new_top;
    else if (pivot_node == pivot_node->parent->right)
        pivot_node->parent->right = new_top;
    else
        pivot_node->parent->left = new_top;
    new_top->right = pivot_node;
    pivot_node->parent = new_top;
}

rbtree *new_rbtree(void) {
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
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

node_t *rbtree_insert(rbtree *tree, const key_t key) {
    node_t* parent = tree->nil;
    node_t* current = tree->root;
    while (current != tree->nil) {
        parent = current;
        if (key < current->key)
            current = current->left;
        else
            current = current->right;  // 중복된 키를 포함하여 모든 key >= current->key인 경우 오른쪽으로 이동
    }

    node_t* new_node = (node_t*)calloc(1, sizeof(node_t));
    if (new_node == NULL) return NULL;  // 메모리 할당 실패 처리
    new_node->key = key;
    new_node->color = RBTREE_RED;
    new_node->left = tree->nil;
    new_node->right = tree->nil;
    new_node->parent = parent;

    if (parent == tree->nil)
        tree->root = new_node;
    else if (key < parent->key)
        parent->left = new_node;
    else
        parent->right = new_node;  // 중복된 키를 포함하여 모든 key >= parent->key인 경우 오른쪽에 추가

    rbtree_insert_fixup(tree, new_node);
    return new_node;
}

node_t *rbtree_find(const rbtree *tree, const key_t key) {
    node_t* current = tree->root;
    while (current != tree->nil) {
        if (key == current->key) {
            return current;
        }
        if (key < current->key) {
            current = current->left;
        } else {
            current = current->right;
        }
    }
    return NULL;  // 키를 찾지 못함
}

node_t* tree_find_min_max(const rbtree *tree, node_t* node, find_t findType) {
    node_t* target = node;
    if (findType == FIND_MIN) {
        while (target->left != tree->nil)
            target = target->left;
    } else { // FIND_MAX
        while (target->right != tree->nil)
            target = target->right;
    }
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

void transplant(rbtree* tree, node_t* old_node, node_t* new_node) {
    if (old_node->parent == tree->nil)
        tree->root = new_node;
    else if (old_node == old_node->parent->left)
        old_node->parent->left = new_node;
    else
        old_node->parent->right = new_node;
    new_node->parent = old_node->parent;
}

void delete_fixup(rbtree* t, node_t* replace_node) {
  node_t* brother;
  while (replace_node != t->root && replace_node->color == RBTREE_BLACK) {    
    // brother-> color == BLACK
    if (is_left_child_of_p(replace_node, replace_node->parent)) {
      brother = replace_node->parent->right;
      /* Case 1 */
      if (brother->color == RBTREE_RED) {
        brother->color = RBTREE_BLACK;
        replace_node->parent->color = RBTREE_RED;
        left_rotate(t, replace_node->parent);
        brother = replace_node->parent->right;
      }
      /* Case 2 */
      if (brother->left->color == RBTREE_BLACK && brother->right->color == RBTREE_BLACK) {
        brother->color = RBTREE_RED;
        replace_node = replace_node->parent;
      }
      else {
        /* Case 3 */
        if (brother->left->color == RBTREE_RED && brother->right->color == RBTREE_BLACK) {
          brother->left->color = RBTREE_BLACK;
          brother->color = RBTREE_RED;
          right_rotate(t, brother);
          brother = replace_node->parent->right;
        }
        /* Case 4 */
        // brother->right == RED
        brother->color = replace_node->parent->color;
        replace_node->parent->color = RBTREE_BLACK;
        brother->right->color = RBTREE_BLACK;
        left_rotate(t, replace_node->parent);
        replace_node = t->root;
      }
      continue;
    }
    // is_right_child_of_p(should_check_node, should_check_node->parent)
    brother = replace_node->parent->left;
    /* Case 1 */
    if (brother->color == RBTREE_RED) {
      brother->color = RBTREE_BLACK;
      replace_node->parent->color = RBTREE_RED;
      right_rotate(t, replace_node->parent);
      brother = replace_node->parent->left;
    }
    /* Case 2 */
    if (brother->left->color == RBTREE_BLACK && brother->right->color == RBTREE_BLACK) {
      brother->color = RBTREE_RED;
      replace_node = replace_node->parent;
    }
    else {
      /* Case 3 */
      if (brother->right->color == RBTREE_RED && brother->left->color == RBTREE_BLACK) {
        brother->right->color = RBTREE_BLACK;
        brother->color = RBTREE_RED;
        left_rotate(t, brother);
        brother = replace_node->parent->left;
      }
      /* Case 4 */
      // brother-left == RED
      brother->color = replace_node->parent->color;
      replace_node->parent->color = RBTREE_BLACK;
      brother->left->color = RBTREE_BLACK;
      right_rotate(t, replace_node->parent);
      replace_node = t->root;
    }
  }
  
  replace_node->color = RBTREE_BLACK;
  t->root->color = RBTREE_BLACK;
}

int rbtree_erase(rbtree *tree, node_t *deleted) {
    if (tree->root == tree->nil)
        return 0;

    color_t deleted_color = deleted->color;
    node_t* replace_node;

    if (deleted->left == tree->nil) {
        replace_node = deleted->right;
        transplant(tree, deleted, replace_node);
    }
    else if (deleted->right == tree->nil) {
        replace_node = deleted->left;
        transplant(tree, deleted, replace_node);
    }
    else {
        node_t* predecessor = tree_find_min_max(tree, deleted->left, FIND_MAX);
        deleted_color = predecessor->color;
        replace_node = predecessor->left;

        if (predecessor->parent == deleted) {
            replace_node->parent = predecessor;
        } else {
            transplant(tree, predecessor, replace_node);
            predecessor->left = deleted->left;
            predecessor->left->parent = predecessor;
        }

        transplant(tree, deleted, predecessor);
        predecessor->right = deleted->right;
        predecessor->right->parent = predecessor;
        predecessor->color = deleted->color;
    }

    free(deleted);

    if (deleted_color == RBTREE_BLACK)
        delete_fixup(tree, replace_node);
  
    return 1;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  // TODO: implement to_array
  return 0;
}
