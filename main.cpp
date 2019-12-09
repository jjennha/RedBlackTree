#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstring>
#include <deque>
#include <functional>
#include <queue>
#include <pthread.h>
#include <zconf.h>
#include <ctime>

# define COUNT 5 // for testing

using namespace std;


enum Color {
    BlACK, RED
};
queue<string> readers;
queue<string> writers;
int readLimit;
int writeLimit;

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
    void rotateLeft(Node *node);
    void rotateRight(Node *node);
public:
    Node *root;

    RedBlackTree() {
        root = NULL;
    }
    void rbTreeify(Node *node);
    void rbTreeifyDel(Node* node);
    void initialize(vector<string> *nodeData);
    void insert(int v);

    void remove(int v);

    Node *search(int v, int thread);

    Node *sibling(Node *n) {
        if (n == NULL || n->parent == NULL) {
            return n;
        }
        if (n->parent->right != NULL && n->parent->right == n) {
            return n->parent->left;
        } else if (n->parent->left != NULL) {
            return n->parent->right;
        }
        return n;
    }

    Node* successor(Node *p) {
        Node *y = NULL;
        if (p->left != NULL) {
            y = p->left;
            while (y->right != NULL)
                y = y->right;
        } else {
            y = p->right;
            while (y->left != NULL)
                y = y->left;
        }
        return y;
    }
};

RedBlackTree *rbtree = new RedBlackTree();

struct thread_data {
    int thread_id;
};

int height(Node *node) {
    if (node == NULL)
        return 0;
    else {
        int lheight = height(node->left);
        int rheight = height(node->right);

        if (lheight > rheight)
            return (lheight + 1);
        else return (rheight + 1);
    }
}

void printGivenLevel(Node *root, int level) {
    if (root == NULL) {
        return;
    }
    if (level == 1) {
        printf("(%i%c)", root->key, ((root->color==BlACK) ? 'b' : 'r'));
    } else if (level > 1) {
        printGivenLevel(root->left, level - 1);
        printGivenLevel(root->right, level - 1);
    }
}
/**
 * Preorder print
 * @param root
 */
void printPreorder(Node *root){
    if(root==NULL){
        printf("f ");
        return;
    }
    char color = (root->color==BlACK)? 'b':'r';
    printf("%i%c ",root->key,color);
    printPreorder(root->left);
    printPreorder(root->right);
}
void printLevelOrder(Node *root) {
    int h = height(root);
    int i;
    for (i = 1; i <= h; i++) {
        printGivenLevel(root, i);
//        cout << endl;
    }
    printf("\n");
}

void print2DUtil(Node *root, int space) {
    if (root == NULL)
        return;
    space += COUNT;
    print2DUtil(root->right, space);
    printf("\n");
    for (int i = COUNT; i < space; i++)
        printf(" ");
    printf("(%i%c)", root->key, ((root->color==BlACK) ? 'b' : 'r'));
    print2DUtil(root->left, space);
}

void print2D(Node *root) {
    print2DUtil(root, 0);
    printf("\n");
}
/**
 * Turns string to node
 * @param data
 * @return
 */
Node *strToNode(string data) {
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
    if (start > end || nodes[start] == "f") { return NULL; }
    Node *node = strToNode(nodes[start]);

    int i;
    for (i = start; i <= end; i++) {
        if (strToNode(nodes[i])->key > node->key) {
            break;
        }
    }
    node->left = construct(nodes, start + 1, i - 1);
    node->right = construct(nodes, i, end);
    if (node->left) {
        node->left->parent = node;
    }
    if (node->right) {
        node->right->parent = node;
    }
    return node;
}

/**
 * Initializes tree with predefined nodes if any
 * @param nodes
 */
void RedBlackTree::initialize(vector<string> *nodes) {
    root = construct(*nodes, 0, nodes->size() - 1);
}

/**
 * Rotates subtree left to maintain Red-Black tree properties
 * @param node
 */
void RedBlackTree::rotateLeft(Node *node) {
    if(node->right==NULL)
        return ;
    else
    {
        Node *y=node->right;
        if(y->left!=NULL)
        {
            node->right=y->left;
            y->left->parent=node;
        }
        else
            node->right=NULL;
        if(node->parent!=NULL)
            y->parent=node->parent;
        if(node->parent==NULL)
            root=y;
        else
        {
            if(node==node->parent->left)
                node->parent->left=y;
            else
                node->parent->right=y;
        }
        y->left=node;
        node->parent=y;
    }
}

/**
 * Rotates subtree right to maintain Red-Black tree properties
 * @param node
 */
void RedBlackTree::rotateRight(Node *node) {
    if(node->left==NULL) {return;}
    else{
        Node *left=node->left;
        if(left->right!=NULL){
            node->left=left->right;
            left->right->parent=node;
        }
        else{ node->left=NULL; }
        if(node->parent!= NULL){ left->parent = node->parent; }
        if(node->parent== NULL){ root = left; }
        else {
            if(node == node->parent->left){
                node->parent->left= left;
            } else{
                node->parent->right= left;
            }
        }
        left->right = node;
        node->parent = left;
    }
}

/**
 * Restructure via recoloring and/or rotating tree to fit Red-Black tree properties.
 * @param node
 */
void RedBlackTree::rbTreeify(Node *node) {
    Node *parentOfNode = NULL;
    Node *gParentOfNode = NULL;

    // while both node to insert and nodes parent are RED
    while ((node != root) && (node->color != BlACK) && (node->parent->color == RED)) {
        parentOfNode = node->parent;
        gParentOfNode = node->parent->parent;

        // (1) if parent of node is left child of grandparent of node
        if (parentOfNode == gParentOfNode->left) {
            Node *uncleOfNode = gParentOfNode->right;

            // (1.1) if uncle of node is red -> recolor
            if (uncleOfNode != NULL && uncleOfNode->color == RED) {
                gParentOfNode->color = RED;
                parentOfNode->color = BlACK;
                uncleOfNode->color = BlACK;
                node = gParentOfNode;
            } else {
                // (1.2) if node is right child of its parent -> rotate left
                if (node == parentOfNode->right) {
                    rotateLeft(parentOfNode);
                    node = parentOfNode;
                    parentOfNode = node->parent;
                }

                // (1.3) node is left child of its parent -> rotate right
                rotateRight(gParentOfNode);
                swap(parentOfNode->color, gParentOfNode->color);
                node = parentOfNode;
            }
        }
            // (2) if parent parent of node is right child of grandparent of node
        else {
            Node *uncleOfNode = gParentOfNode;

            // (2.1) if uncle of node is red -> recolor
            if ((uncleOfNode != NULL) && (uncleOfNode->color == RED)) {
                gParentOfNode->color = RED;
                parentOfNode->color = BlACK;
                uncleOfNode->color = BlACK;
                node = gParentOfNode;
            } else {
                // (2.2) if node is left child of parent -> rotate right
                if (node == parentOfNode->left) {
                    rotateRight(parentOfNode);
                    node = parentOfNode;
                    parentOfNode = node->parent;
                }
                // (2.3) node is right child of its parent -> rotate left
                rotateLeft(gParentOfNode);
                swap(parentOfNode->color, gParentOfNode->color);
                node = parentOfNode;
            }
        }
    }
    root->color = BlACK;
}

/**
 * Recursive helper method to insert node into BST
 * @param root
 * @param node
 * @return root node
 */
Node *insertBST(Node *root, Node *node) {
    if (root == NULL) { return node; }
    if (node->key < root->key) {
        root->left = insertBST(root->left, node);
        root->left->parent = root;
    } else if (node->key > root->key) {
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
void RedBlackTree::insert(int v) {
    Node *node = new Node(v, RED);
    root = insertBST(root, node);
    rbTreeify(node);
}

/**
 * Splits string into array based on delimiter
 * @param str
 * @param sep
 * @return
 */
vector<string> split(string str, string del) {
    char *cstr = const_cast<char *>(str.c_str());
    char *current;
    std::vector<std::string> arr;
    current = strtok(cstr, del.c_str());
    while (current != NULL) {
        arr.push_back(current);
        current = strtok(NULL, del.c_str());
    }
    return arr;
}

/**
 * Trims white space from string
 * @param str
 * @return
 */
string trim(string str) {
    size_t first = str.find_first_not_of(' ');
    if (string::npos == first) {
        return str;
    }
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}
/**
 * Searches for node with value key
 * @param root
 * @param key
 * @param thread
 * @return
 */
Node *recSearch(Node *root, int key, int thread) {
    if (root == NULL) {
        if (thread != -1) {
            printf("search(%i)-> false, performed by thread %i \n", key, thread);
        }
        return root;
    }
    if (root->key == key) {
        if (thread != -1) {
            printf("search(%i)-> true, performed by thread %i \n", root->key, thread);
        }
        return root;
    }
    if (root->key < key) {
        return recSearch(root->right, key, thread);
    }
    return recSearch(root->left, key, thread);
}

Node *RedBlackTree::search(int v, int thread) {
    return recSearch(root, v, thread);
}
/**
 * Fix tree from deletions
 * @param p
 */
void RedBlackTree::rbTreeifyDel(Node *p) {
    Node *s;
    while(p!=root&&p->color==BlACK)
    {
        if(p->parent->left==p)
        {
            s=p->parent->right;
            if(s->color==RED)
            {
                s->color=BlACK;
                p->parent->color=RED;
                rotateLeft(p->parent);
                s=p->parent->right;
            }
            if(s->right->color==BlACK&&s->left->color==BlACK)
            {
                s->color=RED;
                p=p->parent;
            }
            else
            {
                if(s->right->color==BlACK)
                {
                    s->left->color==BlACK;
                    s->color=RED;
                    rotateRight(s);
                    s=p->parent->right;
                }
                s->color=p->parent->color;
                p->parent->color=BlACK;
                s->right->color=BlACK;
                rotateLeft(p->parent);
                p=root;
            }
        }
        else
        {
            s=p->parent->left;
            if(s->color==RED)
            {
                s->color=BlACK;
                p->parent->color=RED;
                rotateRight(p->parent);
                s=p->parent->left;
            }
            if(s->left->color==BlACK&&s->right->color==BlACK)
            {
                s->color=RED;
                p=p->parent;
            }
            else
            {
                if(s->left->color==BlACK)
                {
                    s->right->color=BlACK;
                    s->color=RED;
                    rotateLeft(s);
                    s=p->parent->left;
                }
                s->color=p->parent->color;
                p->parent->color=BlACK;
                s->left->color=BlACK;
                rotateRight(p->parent);
                p=root;
            }
        }
        p->color=BlACK;
        root->color=BlACK;
    }
}
/**
 * // creds to geeks for geeks for helping with deletion
 * Remove node with value v
 * @param v
 */
void RedBlackTree::remove(int v) {
    Node *node = root;
    Node *y=NULL;
    Node *qode=NULL;
    int found=0;
    Node *vNode = search(v, -1);
    if(vNode!=NULL){
        node = vNode;
        found=1;
    }
    if(found==0)
    {
        cout<<"\nElement Not Found.\n";
        return ;
    }
    else {
        if(node->left==NULL||node->right==NULL){ y=node; }
        else{ y=successor(node); }
        if(y->left!=NULL){qode=y->left;}
        else {
            if(y->right!=NULL){ qode=y->right; }
            else{ qode=NULL; }
        }
        if(qode!=NULL){ qode->parent=y->parent; }
        if(y->parent==NULL){ root=qode; }
        else{
            if(y==y->parent->left) { y->parent->left=qode; }
            else{ y->parent->right=qode; }
        }
        if(y!=node) {
            node->color=y->color;
            node->key=y->key;
        }
        if(y->color==BlACK){ rbTreeifyDel(qode); }
    }
}

class monitor {
private:
    int rcnt; // # readers reading
    int wcnt; // # writers writing
    int waitr; // # reader waiting
    int waitw; // # writers waiting
    pthread_cond_t canread;
    pthread_cond_t canwrite;
    pthread_mutex_t condlock;
public:
    monitor() {
        rcnt = 0;
        wcnt = 0;
        waitr = 0;
        waitw = 0;

        pthread_cond_init(&canread, NULL);
        pthread_cond_init(&canwrite, NULL);
        pthread_mutex_init(&condlock, NULL);
    }

    // mutex provide synchronisation so that no other thread
    // can change the value of data
    void beginread(int i, int key) {
        pthread_mutex_lock(&condlock);

        // wait to read if writers are writing or waiting or if the limit for # of readers has been hit
        if (wcnt == 1 || waitw > 0 || rcnt >= readLimit) {
            waitr++;
            pthread_cond_wait(&canread, &condlock);
            waitr--;
        }

        rcnt++;
        rbtree->search(key, i);
        pthread_mutex_unlock(&condlock);
        pthread_cond_broadcast(&canread);
    }

    void endread(int i) {
        pthread_mutex_lock(&condlock);
        if (--rcnt == 0) {
            pthread_cond_signal(&canwrite);
        }
        pthread_mutex_unlock(&condlock);
    }

    void beginwrite(int i, string op, int key) {
        pthread_mutex_lock(&condlock);

        // wait to write if there is a writer writing / if there are readers waiting / if the number of mod threads has been hit
        if (wcnt == 1 || rcnt > 0 || wcnt >= writeLimit) {
            ++waitw;
            pthread_cond_wait(&canwrite, &condlock);
            --waitw;
        }
        wcnt = 1;
        if (op.front() == 'i') {
            rbtree->insert(key);
        } else {
            rbtree->remove(key);
        }
//        printf(" - AFTER: %s -\n", op.c_str());
        pthread_mutex_unlock(&condlock);
    }

    void endwrite(int i) {
        pthread_mutex_lock(&condlock);
        wcnt = 0;
        // if any readers are waiting, signal unblocked
        if (waitr > 0) {
            pthread_cond_signal(&canread);
        } else {
            pthread_cond_signal(&canwrite);
        }
        pthread_mutex_unlock(&condlock);
    }

}

M;
/**
 * @param td
 * @return
 */
void *reader(void *td) {
    struct thread_data *data;
    data = (struct thread_data *) td;
    int i = (int) data->thread_id;
    while (!readers.empty()) {
        string op = readers.front();
        readers.pop();
        int key = stoi(op.substr(op.find('(') + 1, op.find(')') - 1));
        usleep(1);
        M.beginread(i, key);
        M.endread(i);
    }
}
/**
 * @param td
 * @return
 */
void *writer(void *td) {
    struct thread_data *data;
    data = (struct thread_data *) td;
    int i = (int) data->thread_id;
    while (!writers.empty()) {
        string op = writers.front();
        writers.pop();
        if (!op.empty()) {
            int key = stoi(op.substr(op.find('(') + 1, op.find(')') - 1));
            usleep(1);
            M.beginwrite(i, op, key);
            M.endwrite(i);
        }
    }
}
/**
 * set read and write thread count limits
 * @param r
 * @param w
 */
void setLimits(int r, int w) {
    readLimit = r;
    writeLimit = w;
}

int main(int argc, char **argv) {
    vector<string> nodeInputs;
    int searchThreads;
    int modThreads;
    vector<string> ops;
    clock_t begin = clock();

    if (argv[1]) {
        string contents;
        ifstream infile;
        infile.open(argv[1]);
        int line = 0;
        while (!infile.eof()) {
            getline(infile, contents);
            stringstream ss(contents);
            string token;
            getline(ss, token);
            // get existing Node data
            if (line == 0) {
                nodeInputs = split(token, ",");
                if (nodeInputs.size() == 0) {
                    cout << "empty" << endl;
                }
            }
            // get number of search threads
            if (line == 2) {
                searchThreads = stoi(token.substr(token.find(':') + 2));
            }
            // get number of modification threads
            if (line == 3) {
                modThreads = stoi(token.substr(token.find(':') + 2));
            }
            // get operations data
            if (line > 4) {
                vector<string> lineOps = split(token, "||");
                for (string op: lineOps) {
                    ops.push_back(trim(op));
                }
            }

            line++;
        }
        infile.close();
        setLimits(searchThreads, modThreads);
        rbtree->initialize(&nodeInputs);

        for (string op: ops) {
            char command = op.front();
            if (command == 'i') {
                writers.push(op);
            }
            if (command == 's') {
                readers.push(op);
            }
            if (command == 'd') {
                writers.push(op);
            }
        }
        int rqSize = readers.size();
        int wqSize = writers.size();
        pthread_t r[searchThreads], w[modThreads];
        struct thread_data tdr[searchThreads];
        struct thread_data tdw[modThreads];

        for (int i = 0; i < searchThreads && !readers.empty(); i++) {
            tdr[i].thread_id = i;
            pthread_create(&r[i], NULL, &reader, &tdr[i]);
        }
        for (int i = 0; i < modThreads && !writers.empty(); i++) {
            tdw[i].thread_id = i;
            pthread_create(&w[i], NULL, &writer, &tdw[i]);
        }
        for (int i = 0; i < searchThreads && i < rqSize; i++) {
            pthread_join(r[i], NULL);
        }
        for (int i = 0; i < modThreads && i < wqSize; i++) {
            pthread_join(w[i], NULL);
        }
    }
//    printLevelOrder(rbtree->root);
//    rbtree->rbTreeify(rbtree->root);
//    print2D(rbtree->root);
    clock_t end = clock();
    printPreorder(rbtree->root);

    double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
    printf("\nTime Elapsed: %f seconds",elapsed_secs);
    printf("\n");

    return 0;
}

