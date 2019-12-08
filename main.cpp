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

# define COUNT 5
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

void print2DUtil(Node *root, int space)
{
    // Base case
    if (root == NULL)
        return;

    // Increase distance between levels
    space += COUNT;

    // Process right child first
    print2DUtil(root->right, space);

    // Print current node after space
    // count
    cout<<endl;
    for (int i = COUNT; i < space; i++)
        cout<<" ";
    char color = ((root->color) ? 'b' : 'r');
    cout << "(" << root->key << color << ")" << " ";
//    cout<<root->key<<color<<"\n";

    // Process left child
    print2DUtil(root->left, space);
}
// Wrapper over print2DUtil()
void print2D(Node *root)
{
    // Pass initial space count as 0
    print2DUtil(root, 0);
    cout << endl;
}
class RedBlackTree {
private:
    void rotateLeft(Node* node);
    void rotateRight(Node* node);
    void rbTreeify(Node* node);

    void remove_1(Node* node);
    void remove_2(Node* node);
    void remove_3(Node* node);
    void remove_4(Node* node);
    void remove_5(Node* node);
    void remove_6(Node* node);
    void replace(Node* o, Node* n);
public:
    Node *root;

    RedBlackTree() {
        root = NULL;
    }
    void initialize(vector<string> *nodeData);
    void insert(int v);
    void remove(int v);
    Node* search(int v,int thread);
    Node* sibling(Node* n) {
        if(n == NULL || n->parent == NULL){
            return n;
        }
        if(n->parent->right != NULL &&n->parent->right == n){
            return n->parent->left;
        }else if(n->parent->left != NULL){
            return n->parent->right;
        }
        return n;
    }
    Node* uncle(Node* n){
        if(n == NULL || n->parent == NULL || n->parent->parent == NULL){
            return n;
        }
        return sibling(n->parent);
    }
    Node* grandparent(Node* n){
        if(n == NULL || n->parent == NULL || n->parent->parent != NULL){
            return n;
        }
        return n->parent->parent;
    }
    Node* predecessor(Node* n){
        if(n==NULL){return n;}
        while(n->right != NULL){
            n = n->right;
        }
        return n;
    }
};
RedBlackTree* rbtree = new RedBlackTree();
struct thread_data {
    int  thread_id;
    char *message;
    int key;
//    RedBlackTree* rbtree;
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
void RedBlackTree::insert(int v) {
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

Node* recSearch(Node* root, int key, int thread){
    if(root == NULL){
        printf("False: could not find node %i on thread %i \n",key,thread);
        return root;
    }
    if(root->key == key){
        printf("True: found node %i on thread %i \n",root->key,thread);
        return root;
    }
    if(root->key < key){
        return recSearch(root->right,key,thread);
    }
    return recSearch(root->left,key,thread);
}

Node* RedBlackTree::search(int v, int thread) {
    return recSearch(root,v,thread);
}

void RedBlackTree::replace(Node* oldn, Node* newn)
{
    if (oldn->parent == NULL)
    {
        root = newn;
    }
    else
    {
        if (oldn == oldn->parent->left)
            oldn->parent->left = newn;
        else
            oldn->parent->right = newn;
    }
    if (newn != NULL)
    {
        newn->parent = oldn->parent;
    }
}
void RedBlackTree::remove(int v){
    Node* vNode = search(v,-1);
    if(vNode == NULL){return;}
    if(vNode->left != NULL && vNode->right != NULL){
        Node* pred = predecessor(vNode->left);
        vNode->key = pred->key;
        vNode = pred;
    }
    Node* child;
    if(vNode->left != NULL || vNode->right != NULL){return;}
    child = (vNode->right == NULL)? vNode->left : vNode->right;
    if(vNode->color == BlACK){
        vNode->color = child->color;
        remove_1(vNode);
    }
    replace(vNode,child);
    rbTreeify(root);
}
void RedBlackTree::remove_1(Node* n){
    if(n->parent == NULL){
        return;
    }
    remove_2(n);
}
void RedBlackTree::remove_2(Node* n){
    if(sibling(n)->color == RED){
        n->parent->color = RED;
        sibling(n)->color = BlACK;
        if(n==n->parent->left){
            rotateLeft(n->parent);
        }else{
            rotateRight(n->parent);
        }
    }
    remove_3(n);
}
void RedBlackTree::remove_3(Node *n) {
    if (n->parent->color == BlACK && sibling(n)->color == BlACK
        && sibling(n)->left->color == BlACK && sibling(n)->right->color == BlACK){
        sibling(n)->color = RED;
        remove_1(n->parent);
    }
    else
        remove_4(n);
}
void RedBlackTree::remove_4(Node *n) {
    if (n->parent->color == RED && sibling(n)->color == BlACK &&
        sibling(n)->left->color == BlACK && sibling(n)->right->color == BlACK)
    {
        sibling(n)->color = RED;
        n->parent->color = BlACK;
    }
    else
        remove_5(n);
}
void RedBlackTree::remove_5(Node *n) {
    if (n == n->parent->left && sibling(n)->color == BlACK &&
        sibling(n)->left->color == RED && sibling(n)->right->color == BlACK)
    {
        sibling(n)->color = RED;
        sibling(n)->left->color = BlACK;
        rotateRight(sibling(n));
    }
    else if (n == n->parent->right && sibling(n)->color == BlACK &&
             sibling(n)->right->color == RED && sibling(n)->left->color == BlACK)
    {
        sibling(n)->color = RED;
        sibling(n)->right->color = BlACK;
        rotateLeft(sibling(n));
    }
    remove_6(n);
}
void RedBlackTree::remove_6(Node *n) {
    sibling(n)->color = n->parent->color;
    n->parent->color = BlACK;
    if (n == n->parent->left)
    {
        if(sibling(n)->right->color != RED){return;}
        sibling(n)->right->color = BlACK;
        rotateLeft(n->parent);
    }
    else
    {
        if(sibling(n)->left->color != RED){return;}
        sibling(n)->left->color = BlACK;
        rotateRight(n->parent);
    }
}
void* search(void* threadarg){

    struct thread_data *data;
    data = (struct thread_data *) threadarg;
//    RedBlackTree* rbtree = data->rbtree;
    int key = data->key;
    sleep(1);
    rbtree->search(key, data->thread_id);
//    printLevelOrder(rbtree->root);
    pthread_exit(NULL);
}

class monitor {
private:
    // no. of readers
    int rcnt;

    // no. of writers
    int wcnt;

    // no. of readers waiting
    int waitr;

    // no. of writers waiting
    int waitw;

    // condition variable to check whether reader can read
    pthread_cond_t canread;

    // condition variable to check whether writer can write
    pthread_cond_t canwrite;

    // mutex for synchronisation
    pthread_mutex_t condlock;

public:
    monitor()
    {
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
    void beginread(int i,int key)
    {
        pthread_mutex_lock(&condlock);

        // if there are active or waiting writers
        if (wcnt == 1 || waitw > 0) {
            // incrementing waiting readers
            waitr++;

            // reader suspended
            pthread_cond_wait(&canread, &condlock);
            waitr--;
        }

        // else reader reads the resource
        rcnt++;
        rbtree->search(key,i);
        pthread_mutex_unlock(&condlock);
        pthread_cond_broadcast(&canread);
    }

    void endread(int i)
    {

        // if there are no readers left then writer enters monitor
        pthread_mutex_lock(&condlock);

        if (--rcnt == 0)
            pthread_cond_signal(&canwrite);

        pthread_mutex_unlock(&condlock);
    }

    void beginwrite(int i)
    {
        pthread_mutex_lock(&condlock);

        // a writer can enter when there are no active
        // or waiting readers or other writer
        if (wcnt == 1 || rcnt > 0) {
            ++waitw;
            pthread_cond_wait(&canwrite, &condlock);
            --waitw;
        }
        wcnt = 1;
        cout << "writer " << i << " is writing\n";
        pthread_mutex_unlock(&condlock);
    }

    void endwrite(int i)
    {
        pthread_mutex_lock(&condlock);
        wcnt = 0;

        // if any readers are waiting, threads are unblocked
        if (waitr > 0)
            pthread_cond_signal(&canread);
        else
            pthread_cond_signal(&canwrite);
        pthread_mutex_unlock(&condlock);
    }

}

// global object of monitor class
        M;

void* reader(void* td)
{
    struct thread_data *data;
    data = (struct thread_data *) td;
    int c = 0;
    int i = (int) data->thread_id;
    int key = data->key;
    // each reader attempts to read 5 times
    while (c < 2) {
        usleep(1);
        M.beginread(i,key);
//        cout << "reading form thread: " << i << endl;

        M.endread(i);
        c++;
    }
}

void* writer(void* id)
{
    int c = 0;
    int i = *(int*)id;

    // each writer attempts to write 5 times
    while (c < 5) {
        usleep(1);
        M.beginwrite(i);
        M.endwrite(i);
        c++;
    }
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
            string token;
            getline(ss,token);
            // get existing Node data
            if (line == 0) {
                nodeInputs = split(token,",");
                if(nodeInputs.size()==0){
                    cout << "empty"<<endl;
                }
                cout << nodeInputs.size() << endl;
            }
            // get number of search threads
            if (line == 2) {
                searchThreads = stoi(token.substr(token.find(':')+2));
            }
            // get number of modification threads
            if (line == 3) {
                modThreads = stoi(token.substr(token.find(':')+2));
            }
            // get operations data
            if (line > 4) {
                vector<string> lineOps = split(token,"||");
                for(string op: lineOps){
                    ops.push_back(trim(op));
                }
            }

            line++;
        }
        infile.close();

        rbtree->initialize(&nodeInputs);
        if(rbtree->root){
            cout << "root: " << rbtree->root->key << endl;
        }
        queue<string> readers;
        queue<string> writers;
        for(string op: ops){
            char command = op.front();
            if(command=='i'){
                writers.push(op);
            }
            if(command=='s'){
                readers.push(op);
            }
            if(command=='d'){
                writers.push(op);
            }
        }
        int size = readers.size();
        pthread_t r[size], w[size];
        struct thread_data td[searchThreads];
        int i = 0;

        while(!readers.empty() ){
            string op = readers.front();
            readers.pop();
            int key = stoi(op.substr(op.find('(')+1,op.find(')')-1));
            td[i].thread_id = i;
            td[i].key = key;

            // creating threads which execute reader function
            pthread_create(&r[i], NULL, &reader, &td[i]);

            // creating threads which execute writer function
//            pthread_create(&w[i], NULL, &writer, &td[i]);

            i++;
        }

        for (int i = 0; i < size; i++) {
            pthread_join(r[i], NULL);
        }
//        for (int i = 0; i < size; i++) {
//            pthread_join(w[i], NULL);
//        }



        // synchronized execution

//        for(string op: ops){
//            char command = op.front();
//            int key = stoi(op.substr(op.find('(')+1,op.find(')')-1));
//            if(command=='i'){
//                rbtree->insert(key);
//            }
//            if(command=='s'){
//                rbtree->search(key,-1);
//            }
//            if(command=='d'){
//                rbtree->remove(key);
//            }
//        }

        printLevelOrder(rbtree->root);
        print2D(rbtree->root);


    }
    return 0;
}


/*
 *        int rc;
        int i;
        pthread_t threads[searchThreads];
        pthread_attr_t attr;
        void *status;
        struct thread_data td[searchThreads];

        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

        while(!readers.empty() ){
            if(!readers.empty()){
                string op = readers.front();
                int key = stoi(op.substr(op.find('(')+1,op.find(')')-1));



                for( i = 0; i < searchThreads; i++ ) {
                    cout <<"main() : creating thread, " << i << endl;
                    td[i].thread_id = i;
                    td[i].key = key;
                    td[i].rbtree = rbtree;
                    rc = pthread_create(&threads[i], &attr, search, (void *)&td[i]);
                    if (rc) {
                        cout << "Error:unable to create thread," << rc << endl;
                        exit(-1);
                    }
                }
                pthread_attr_destroy(&attr);
                for( i = 0; i < searchThreads; i++ ) {
                    rc = pthread_join(threads[i], &status);
                    if (rc) {
                        cout << "Error:unable to join," << rc << endl;
                        exit(-1);
                    }
                    cout << "Main: completed thread id :" << i ;
                    cout << "  exiting with status :" << status << endl;
                }
                pthread_exit(NULL);
                readers.pop();

                i++;
            }
            if(!writers.empty()){
//                writers.pop();
            }
        }
 */