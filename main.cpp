#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

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
public:
    Node *root;
    RedBlackTree() {
        root = NULL;
    }

    Node *initialize(vector<string> *nodeData);

    void insert(int v);

    void rotateLeft(Node *);

    void rotateRight(Node *);

    void remove(int v);

    void search(int v);

};

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
    if (start > end) { return NULL; }
    Node *node = strToNode(nodes[start]);

    int i;
    for(i=start; i<= end; i++){
        if(strToNode(nodes[i])->key > node->key){
            break;
        }
    }
    node->left = construct(nodes,start+1,i-1);
    node->right = construct(nodes,i,end);
    return node;
}

Node *RedBlackTree::initialize(vector<string> *nodes) {
    root = construct(*nodes, 0, nodes->size()-1);
}

void RedBlackTree::insert(int v) {
}
int height(Node* node)
{
    if (node == NULL)
        return 0;
    else
    {
        /* compute the height of each subtree */
        int lheight = height(node->left);
        int rheight = height(node->right);

        /* use the larger one */
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
        printLevelOrder(rbtree->root);
    }
    return 0;
}