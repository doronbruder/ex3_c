#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "RBTree.h"

/**
 * enum created identify easily if a Node is the right \ left child of its parent.
 */
typedef enum Direction
{
    LEFT, RIGHT
} Direction;


/**
 * constructs a new RBTree with the given CompareFunc.
 * comp: a function two compare two variables.
 */
RBTree *newRBTree(CompareFunc compFunc, FreeFunc freeFunc)
{
    RBTree * tree = NULL;
    tree = (RBTree *) malloc (sizeof(RBTree));
    if (tree == NULL)
    {
        return NULL;
    }
    tree->root = NULL;
    tree->compFunc = compFunc;
    tree->freeFunc = freeFunc;
    tree->size = 0;
    return tree;
}


/**
 * find node that contian data in it if exist, if not the parent of such node in a BST similar to our tree.
 * @param tree: RBTree to search for data in.
 * @param data: data to search for in tree.
 * @return node that contian data in it if exist, if not the parent of such node in a BST similar to our tree.
 */
Node * findNodeLocation(const RBTree * tree, const void * data)
{
    Node * node = tree->root;
    while(node != NULL)
    {
        int result = tree->compFunc(node->data, data);
        if (result > 0)
        {
            if (node->left == NULL)
            {
                break;
            }
            node = node->left;
            continue;
        }
        if (result < 0)
        {
            if (node->right == NULL)
            {
                break;
            }
            node = node->right;
            continue;
        }
        return node;
    }
    return node;
}


/**
 * find location for data in tree.
 * @param tree: RBTree to place node in.
 * @param node: node to place in tree.
 * @return: false if the data is already in the tree, true otherwise.
 */
bool placeNode(RBTree * tree, Node * node)
{
    Node * parent = findNodeLocation(tree, node->data);
    int result = tree->compFunc(parent->data, node->data);
    if (result == 0)
    {
        return false;
    }
    if (result > 0) { // todo go over with lab support help (switching to ? :)
        parent->left = node;
    } else {
        parent->right = node;
    }
    node->parent = parent;
    tree->size++;
    return true;
}


/**
 * check the direction of node compared to its parent.
 * @param node: the node its direction required.
 * @return: LEFT if node is the left child of its parent, RIGHT otherwise.
 */
Direction nodeDirection(Node *node)
{
    return node->data == node->parent->left->data ? LEFT : RIGHT;
}


/**
 * check the color of the node sibling.
 * @param node: the sibling of the node, its color required.
 * @return: RED if the color of node sibling is RED, BLACK otherwise.
 */
Color siblingColor(Node *node)
{
    Node *sibling = nodeDirection(node) == RIGHT ? node->parent->left : node->parent->right;
    return (sibling != NULL && sibling->color == RED) ? RED : BLACK;
}


/**
 * recolor the parent and the uncle and the grandparent of node if the node, its parent and its uncle are RED.
 * @param node: a red Node with a red parent which require recoloring.
 * @return: NULL if rotation isn't required, otherwise the Node require rotation.
 */
Node * recolor(Node *node)
{
    while (node->parent != NULL)
    {
        node->color = RED;
        if (node->parent->color == BLACK)
        {
            return NULL;
        }
        if (siblingColor(node->parent) == BLACK)
        {
            return node;
        }
        node = node->parent->parent;
        node->left->color = BLACK;
        node->right->color = BLACK;
    }
    return NULL;
}


/**
 * set newParent as the parent of node.
 * @param node: the Node require resetting its parent.
 * @param newParent: the new parent of node.
 */
void setParent(Node *node, Node * newParent)
{
    node->parent = newParent;
}


 /**
  * set newChild as the left child of node.
  * @param node: the Node require resetting its left child.
  * @param newChild: the new left child of node.
  */
void setLeftChild(Node * node, Node *newChild)
{
    node->left = newChild;
}


/**
 * set newChild as the right child of node.
 * @param node: the Node require resetting its right child.
 * @param newChild: the new right child of node.
 */
void setRightChild(Node * node, Node *newChild)
{
    node->right = newChild;
}


/**
 * set node as the new root of tree.
 * @param tree: the RBTree require resetting its root.
 * @param node: the new root of tree.
 */
void setRoot(RBTree *tree, Node *node)
{
    setParent(node, NULL);
    node->color = BLACK;
    tree->root = node;
}


/**
 * switching the colors of node1 and node2.
 * @param node1: a Node object.
 * @param node2: a Node object.
 */
void switchColors(Node *node1, Node * node2)
{
    Color temp = node1->color;
    node1->color = node2->color;
    node2->color = temp;
}


/**
 * rotate parent to the direction in rotationDirection.
 * @param tree: the RBTree containing the Nodes node and parent.
 * @param parent: the node required rotation.
 * @param node: a child of the node required rotation in the opposite direction then rotationDirection.
 * @param rotationDirection: the direction of the rotation.
 */
void rotate(RBTree *tree, Node *parent, Node *node, Direction rotationDirection)
{
    if (parent->parent == NULL)
    {
        tree->root = node;
    }
    else
    {
        nodeDirection(parent) == LEFT ? setLeftChild(parent->parent, node) : setRightChild(parent->parent, node);
    }
    setParent(node, parent->parent);

    Node * child = rotationDirection == LEFT ? node->left : node->right;
    rotationDirection == LEFT ? setRightChild(parent, child) : setLeftChild(parent, child);
    if (child != NULL)
    {
        setParent(child, parent);
    }

    setParent(parent, node);
    rotationDirection == LEFT ? setLeftChild(node, parent) : setRightChild(node, parent);
}


/**
 * implement the right rotation by the directions of node and its parent.
 * @param tree: a tree containing node.
 * @param node: the Node required rotation.
 */
void rotation(RBTree *tree, Node *node)
{
    if (nodeDirection(node) == LEFT && nodeDirection(node->parent) == LEFT) // Left Left Case
    {
        node = node->parent;
        rotate(tree, node->parent, node, RIGHT);
        switchColors(node, node->right);
    }
    else if (nodeDirection(node) == LEFT) // Right Left Case
    {
        rotate(tree, node->parent, node, RIGHT);
        rotate(tree, node->parent, node, LEFT);
        switchColors(node, node->left);
    }
    else if (nodeDirection(node) == RIGHT && nodeDirection(node->parent) == RIGHT) // Right Right Case
    {
        node = node->parent;
        rotate(tree, node->parent, node, LEFT);
        switchColors(node, node->left);
    }
    else if (nodeDirection(node) == RIGHT) // Left Right Case
    {
        rotate(tree, node->parent, node, LEFT);
        rotate(tree, node->parent, node, RIGHT);
        switchColors(node, node->right);
    }
}


Node * getNewNode(void *data)
{
    Node * node = NULL;
    node = (Node *) malloc(sizeof(Node));
    if (node == NULL)
    {
        free(node);
        return NULL;
    }
    node->data = data;
    node->parent = NULL;
    node->left = NULL;
    node->right = NULL;
    return node;
}


/**
 * add an item to the tree
 * @param tree: the tree to add an item to.
 * @param data: item to add to the tree.
 * @return: 0 on failure, other on success. (if the item is already in the tree - failure).
 */
int insertToRBTree(RBTree *tree, void *data)
{
    if (tree == NULL || data == NULL || tree->compFunc == NULL || (tree->root != NULL && tree->size != sizeof(*data)))
    {
        return false;
    }
    Node * node = getNewNode(data);
    if (node == NULL)
    {
        return false;
    }
    if (tree->root == NULL) // empty tree
    {
        setRoot(tree, node);
        tree->size++;
        return true;
    }
    node->color = RED;
    if (!placeNode(tree, node))
    {
        return false;
    }
    if (node->parent->color == BLACK) // black parent
    {
        return true;
    }
    if ((node = recolor(node)) == NULL) // black parent after recolor
    {
        return true;
    }
    rotation(tree, node); // rotating the subtree
    return true;
}


/**
 * remove a red Node with at most one child from the tree by linking its parent to its child.
 * @param node: the node to remove from the tree.
 * @param child: the child of node if one exist, if not null.
 */
void removeRedNode(Node *node, Node *child)
{
    nodeDirection(node) == LEFT ? setLeftChild(node->parent, child) : setRightChild(node->parent, child);
    if (child != NULL)
    {
        setParent(child, node->parent);
    }
}


/**
 * handle case where one of the Nodes in an RBTree is a double black node.
 * @param tree: a RBTree that contain a double black Node.
 * @param node: the double black node in tree.
 * @param direction: the direction from node to one of is existing children.
 */
void doubleBlackNode(RBTree *tree, Node *node, Direction direction)
{
    Node * child = direction == LEFT ? node->left : node->right;
    if (child->color == RED)
    {
        rotate(tree, node, child, !direction);
        doubleBlackNode(tree, node, direction);
    }
    else if ((direction == LEFT ? child->right : child->left) != NULL)
    {
        child->color = node->color;
        node->color = BLACK;
        (direction == LEFT ? child->left : child->right)->color = BLACK;
        rotate(tree, node, child, !direction);
    }
    else if ((direction == LEFT ? child->left : child->right) != NULL)
    {
        rotate(tree, node, child, direction);
        doubleBlackNode(tree, node, direction);
    }
    else if (node->color == RED)
    {
        node->color = BLACK;
        child->color = RED;
    }
}



/**
 * remove a black Node with at most one child and a parent from the tree by linking its parent to its child.
 * @param tree: a RBTree that contain node and child.
 * @param node: the node to remove from the tree.
 * @param child: a child of node if exist one, if not null.
 */
void removeBlackNode(RBTree *tree, Node *node, Node *child)
{
    Direction direction = nodeDirection(node);
    direction == LEFT ? setLeftChild(node->parent, child) : setRightChild(node->parent, child);
    if (child != NULL)
    {
        setParent(child, node->parent);
    }
    doubleBlackNode(tree, node->parent, direction);
}


/**
 * remove the node from the RBTree tree and delete it.
 * @param tree: a RBTree tree containing node.
 * @param node: a Node contained in tree to remove from it and than delete it.
 */
void deleteNode(RBTree *tree, Node * node)
{
    Node * successor = node;
    if (node->left != NULL && node->right != NULL)
    {
        successor = node->right;
        while (successor->left != NULL)
        {
            successor = successor->left;
        }
        node->data = successor->data;
    }
    Node * child = node->left == NULL ? node->right : node->left;
    if (successor->color == RED)
    {
        removeRedNode(successor, child);
    }
    else if (successor->parent == NULL)
    {
        child == NULL ? tree->root = NULL : setRoot(tree, child);
    }
    else if (child->color == RED)
    {
        child->color = BLACK;
        removeRedNode(successor, child);
    }
    else
    {
        removeBlackNode(tree, successor, child);
    }
    free(successor);
}


/**
 * remove an item from the tree
 * @param tree: the tree to remove an item from.
 * @param data: item to remove from the tree.
 * @return: 0 on failure, other on success. (if data is not in the tree - failure).
 */
int deleteFromRBTree(RBTree *tree, void *data)
{
    if (tree == NULL || tree->root == NULL || tree->compFunc == NULL || tree->freeFunc == NULL || data == NULL)
    {
        return false;
    }
    Node * node = findNodeLocation(tree, data);
    if (tree->compFunc(node->data, data) != 0)
    {
        return false;
    }
    tree->freeFunc(node->data);
    deleteNode(tree, node);
    tree->size--;
    return true;
}


/**
 * check whether the tree RBTree Contains this item.
 * @param tree: the tree to check an item in.
 * @param data: item to check.
 * @return: 0 if the item is not in the tree, other if it is.
 */
int RBTreeContains(const RBTree *tree, const void *data)
{
    if (tree == NULL || tree->compFunc == NULL || data == NULL)
    {
        return false;
    }
    return tree->compFunc(findNodeLocation(tree, data)->data, data) == 0;
}


/**
 * doing operation on the data in a sub tree by going over it recursively,
 * first the left child then the node itself and finely the right child.
 * @param node: the root of the sub tree.
 * @param func: the operation to do on the data in the tree.
 * @param args: an argument that func might need to operate right.
 * @return: true if the operation was successful, false otherwise.
 */
bool forEachElementInTree(const Node *node, forEachFunc func, void *args)
{
    if (node == NULL)
    {
        return true;
    }
    if (!forEachElementInTree(node->left, func, args))
    {
        return false;
    }
    if (!func(node->data, args))
    {
        return false;
    }
    if (!forEachElementInTree(node->right, func, args))
    {
        return false;
    }
    return true;
}


/**
 * Activate a function on each item of the tree. the order is an ascending order. if one of the activations of the
 * function returns 0, the process stops.
 * @param tree: the tree with all the items.
 * @param func: the function to activate on all items.
 * @param args: more optional arguments to the function (may be null if the given function support it).
 * @return: 0 on failure, other on success.
 */
int forEachRBTree(const RBTree *tree, forEachFunc func, void *args)
{
    if (tree == NULL || func == NULL)
    {
        return false;
    }
    return forEachElementInTree(tree->root, func, args);
}


/**
 * this function free a single node.
 * @param node: a Node object to free.
 * @param func: the function to free the node data.
 */
void freeNode(Node **node, FreeFunc func)
{
    func((*node)->data);
    (*node)->data = NULL; (*node)->parent = NULL; (*node)->left = NULL; (*node)->right = NULL;
    free(*node);
    (*node) = NULL;
}


/**
 * free a sub tree by going over it recursively, first its children and the node itself.
 * @param node: the root of the sub tree.
 * @param func: the function to free the nodes data.
 */
void deleteEachElementInTree(Node **node, FreeFunc func)
{
    if ((*node) == NULL)
    {
        return;
    }
    deleteEachElementInTree(&(*node)->left, func);
    deleteEachElementInTree(&(*node)->right, func);
    freeNode(node, func);
}


/**
 * free all memory of the data structure.
 * @param tree: pointer to the tree to free.
 */
void freeRBTree(RBTree **tree)
{
    if (tree == NULL || (*tree) == NULL || (*tree)->freeFunc == NULL)
    {
        return;
    }
    deleteEachElementInTree(&(*tree)->root, (*tree)->freeFunc);
    (*tree)->root = NULL;
    free((*tree));
}
