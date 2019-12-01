#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstring>

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
    void initialize(vector<string> *nodeData);
    Node* insert(int v);
    void remove(int v);
    bool search(int v);

};

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
//        cout << endl;
    }
    cout << endl;
}
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
/**
 * Helper method to contruct tree recursively
 * @param nodes
 * @param start
 * @param end
 * @return
 */
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
    return node;
}
/**
 * Initializes tree with predefined nodes if any
 * @param nodes
 */
void RedBlackTree::initialize(vector<string> *nodes) {
    root = construct(*nodes, 0, nodes->size()-1);
}
/**
 * Rotates subtree left to maintain Red-Black tree properties
 * @param node
 */
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
/**
 * Rotates subtree right to maintain Red-Black tree properties
 * @param node
 */
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

/**
 * Restructure via recoloring and/or rotating tree to fit Red-Black tree properties.
 * @param node
 */
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

/**
 * Recursive helper method to insert node into BST
 * @param root
 * @param node
 * @return root node
 */
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
/**
 * Insert node into BST
 * @param v
 * @return
 */
Node* RedBlackTree::insert(int v) {
    Node *node = new Node(v,RED);
    root = insertBST(root,node);
    rbTreeify(node);
    printLevelOrder(root);
}

/**
 * Splits string into array based on delimiter
 * @param str
 * @param sep
 * @return
 */
vector<string> split(string str,string del){
    char* cstr=const_cast<char*>(str.c_str());
    char* current;
    std::vector<std::string> arr;
    current=strtok(cstr,del.c_str());
    while(current!=NULL){
        arr.push_back(current);
        current=strtok(NULL,del.c_str());
    }
    return arr;
}
/**
 * Trims white space from string
 * @param str
 * @return
 */
string trim(string str)
{
    size_t first = str.find_first_not_of(' ');
    if (string::npos == first)
    {
        return str;
    }
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

bool recSearch(Node* root, int key){
    if(root == NULL){
        cout << "False: could not find "<< key << endl;
        return false;
    }
    if(root->key == key){
        cout << "True: found node " << root->key << " on thread #" << endl;
        return true;
    }
    if(root->key < key){
        return recSearch(root->right,key);
    }
    return recSearch(root->left,key);
}

bool RedBlackTree::search(int v) {
    return recSearch(root,v);
}

int main(int argc, char **argv) {
    std::cout << argv[1] << std::endl;
    vector<string> nodeInputs;
    int searchThreads;
    int modThreads;
    vector<string> ops;

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
                getline(ss,token);
                nodeInputs = split(token,",");
                if(nodeInputs.size()==0){
                    cout << "empty"<<endl;
                }
//                nodeInputs.push_back(NULL);
                cout << nodeInputs.size() << endl;
            }
            // get number of search threads
            if (line == 2) {
                string token;
                getline(ss,token);
                searchThreads = stoi(token.substr(token.find(':')+2));
            }
            // get number of modification threads
            if (line == 3) {
                string token;
                getline(ss,token);
                modThreads = stoi(token.substr(token.find(':')+2));
            }
            // get operations data
            if (line > 4) {
                string token;
                getline(ss,token);
                vector<string> lineOps = split(token,"||");
                for(string op: lineOps){
                    ops.push_back(trim(op));
                }
            }

            line++;
        }
        infile.close();

        RedBlackTree *rbtree = new RedBlackTree();
        rbtree->initialize(&nodeInputs);
        if(rbtree->root){
            cout << "root: " << rbtree->root->key << endl;
        }

        for(string op: ops){
            char command = op.front();
            int key = stoi(op.substr(op.find('(')+1,op.find(')')-1));
            if(command=='i'){
                rbtree->insert(key);
            }
            if(command=='s'){
                rbtree->search(key);
            }
            if(command=='d'){

            }
        }

//        printLevelOrder(rbtree->root);
    }
    return 0;
}

/*
 *   for(Node* n: allNodes){
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
 * */

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