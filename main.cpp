#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

// https://www.geeksforgeeks.org/c-program-red-black-tree-insertion/
using namespace std;
enum Color {
    RED, BlACK
};

struct Node {
    int key;
    Node *parent;
    bool color;
    Node *left;
    Node *right;

    Node(int k, bool color) {
        this->key = k;
        this->color = color;
        left = right = parent = NULL;
    }

    Node(bool color) {
        this->key = NULL;
        this->color = color;
        left = right = parent = NULL;
    }

};

class RedBlackTree {
private:
    void rotateLeft(Node* node);
    void rotateRight(Node* node);
    void rbTreeify(Node* node);
public:
    Node *root;

    RedBlackTree() {
        root = NULL;
    }
    Node* initialize(vector<string> *nodeData);
    Node* insert(int v);
    void remove(int v);
    void search(int v);

};
vector<Node*> allNodes;
Node* strToNode(string data) {
    Node *n;
    char color = data.back();
    if (color == 'b') {
        color = BlACK;
    } else if (color == 'r') {
        color = RED;
    }
    if (color == 'f') {
        color = BlACK;
        n = new Node(color);
    } else {
        int val = stoi(data.substr(0, data.size() - 1));
        n = new Node(val, color);
    }
    return n;
}

Node *construct(vector<string> nodes, int start, int end) {
    if (start > end || nodes[start]=="f") { return NULL; }
    Node *node = strToNode(nodes[start]);

    int i;
    for(i=start; i<= end; i++){
        if(strToNode(nodes[i])->key > node->key){
            break;
        }
    }
    node->left = construct(nodes,start+1,i-1);
    node->right = construct(nodes,i,end);
    if(node->left){
        node->left->parent = node;
    }
    if(node->right){
        node->right->parent = node;
    }
    allNodes.push_back(node);
    return node;
}

Node *RedBlackTree::initialize(vector<string> *nodes) {
    root = construct(*nodes, 0, nodes->size()-1);
}

Node* insertBST(Node* root, Node* node){
    if(root == NULL){return node;}
    if(node->key < root->key){
        root->left = insertBST(root->left, node);
        root->left->parent = root;
    }
    else if(node->key > root->key){
        root->right = insertBST(root->right, node);
        root->right->parent = root;
    }
    return root;
}

Node* RedBlackTree::insert(int v) {
    Node *node = new Node(v,RED);
    root = insertBST(root,node);
    rbTreeify(node);
}

void RedBlackTree::rotateLeft(Node *node) {
    Node* right = node->right;
    node->right = right->left;
    if(node->right!=NULL){
        node->right->parent = node;
    }
    right->parent = node->parent;
    if(node->parent==NULL){
        root = right;
    }else if(node == node->parent->left){
        node->parent->left = right;
    }else{
        node->parent->right = right;
    }
    right->left = node;
    node->parent = right;
}
void RedBlackTree::rotateRight(Node *node) {
    Node* left = node->left;
    node->left = node->right;
    if(node->left!=NULL){
        node->left->parent = node;
    }
    left->parent = node->parent;
    if(node->parent==NULL){
        root = left;
    }else if(node == node->parent->left){
        node->parent->left = left;
    }else{
        node->parent->right = left;
    }
    left->right = node;
    node->parent = left;
}

void RedBlackTree::rbTreeify(Node *node) {
    Node *parentOfNode = NULL;
    Node *gParentOfNode = NULL;

    // while both node to insert and nodes parent are RED
    while((node!=root) && (node->color != BlACK) && (node->parent->color == RED)){
        parentOfNode = node->parent;
        gParentOfNode = node->parent->parent;

        // (1) if parent of node is left child of grandparent of node
        if(parentOfNode==gParentOfNode->left){
            Node* uncleOfNode = gParentOfNode->right;

            // (1.1) if uncle of node is red -> recolor
            if(uncleOfNode != NULL && uncleOfNode->color==RED){
                gParentOfNode->color = RED;
                parentOfNode->color = BlACK;
                uncleOfNode->color = BlACK;
                node = gParentOfNode;
            }else{
                // (1.2) if node is right child of its parent -> rotate left
                if(node == parentOfNode->right){
                    rotateLeft(parentOfNode);
                    node = parentOfNode;
                    parentOfNode = node->parent;
                }

                // (1.3) node is left child of its parent -> rotate right
                rotateRight(gParentOfNode);
                swap(parentOfNode->color,gParentOfNode->color);
                node = parentOfNode;
            }
        }
        // (2) if parent parent of node is right child of grandparent of node
        else{
            Node *uncleOfNode = gParentOfNode;

            // (2.1) if uncle of node is red -> recolor
            if((uncleOfNode!=NULL)&&(uncleOfNode->color==RED)){
                gParentOfNode->color = RED;
                parentOfNode->color = BlACK;
                uncleOfNode->color = BlACK;
                node = gParentOfNode;
            }else{
                // (2.2) if node is left child of parent -> rotate right
                if(node == parentOfNode->left){
                    rotateRight(parentOfNode);
                    node = parentOfNode;
                    parentOfNode = node->parent;
                }
                // (2.3) node is right child of its parent -> rotate left
                rotateLeft(gParentOfNode);
                swap(parentOfNode->color,gParentOfNode->color);
                node = parentOfNode;

            }
        }

    }
    root-> color = BlACK;
}
int main(int argc, char **argv) {
    std::cout << argv[1] << std::endl;
    vector<string> nodeInputs;
    int searchThreads;
    int modThreads;

    if (argv[1]) {
        string contents;
        ifstream infile;
        infile.open(argv[1]);
        int line = 0;
        while (!infile.eof()) {
            getline(infile, contents);
            stringstream ss(contents);

            // get existing Node data
            if (line == 0) {
                string token;
                while (getline(ss, token, ',')) {
                    nodeInputs.push_back(token);
                }
            }
            // get number of search threads
            if (line == 2) {

            }
            // get number of modification threads
            if (line == 3) {

            }
            // get operations data
            if (line > 4) {}

            line++;
        }
        infile.close();
        RedBlackTree *rbtree = new RedBlackTree();
        rbtree->initialize(&nodeInputs);
        cout << "root: " << rbtree->root->key << endl;
        rbtree->insert(1);
        for(Node* n: allNodes){
            char color = ((n->color) ? 'b' : 'r');
            cout << "(" << n->key << color << ")" << " ";
            if(n->left){
                char color = (n->left->color==0)? 'r':'b';
                cout << "lChild: " << n->left->key << color << " | ";
            }
            if(n->right){
                char color = (n->right->color==0)? 'r':'b';
                cout << "rChild: " << n->right->key << color;
            }
            cout << endl;

        }
    }
    return 0;
}



/*
 *
 *
int height(Node* node)
{
    if (node == NULL)
        return 0;
    else
    {
int lheight = height(node->left);
int rheight = height(node->right);

if (lheight > rheight)
return(lheight + 1);
else return(rheight + 1);
}
}
void printGivenLevel(Node* root, int level)
{
    if (root == NULL) {
        return;
    }
    if (level == 1) {
        char color = ((root->color) ? 'b' : 'r');
        cout << "(" << root->key << color << ")" << " ";
    }
    else if (level > 1)
    {
        printGivenLevel(root->left, level-1);
        printGivenLevel(root->right, level-1);
    }
}
void printLevelOrder(Node* root)
{
    int h = height(root);
    int i;
    for (i = 1; i <= h; i++) {
        printGivenLevel(root, i);
        cout << endl;
    }
}
 *
 *
 * */