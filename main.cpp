#include <iostream>

struct node {
    int key;
    node *parent;
    char color;
    node *left;
    node *right;
};
class RedBlackTree {
    node *root;
    public:
        RedBlackTree(){
            root=NULL;
        }
        void insert(int v);
        void rotateLeft(node*);
        void rotateRight(node*);
        void remove(int v);
        void search(int v);

};
void RedBlackTree::insert(int v) {
    node *n = new node();
    n->key=v;

}
int main(int argc, char** argv) {
    std::cout << "Hello, World!" << std::endl;
    return 0;
}