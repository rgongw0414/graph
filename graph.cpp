#include <bits/stdc++.h>
using namespace std;
#define endl '\n'

static int gid = 0, TIME = 0;

template<class T>
class Node{
    public:
    int id;
    int color; // 0: white, 1: gray, 2: black
    T value;
    vector<int> ptrBy; // ids, for removeNode in directed graph, e.g. consider edge(A, B), B is pointed by A, hence push A's id to ptrBy of B.
    pair<int, int> TIME; //  discover, finish
    Node* pred; // predecessor
    Node():id(gid++), color(0), TIME(make_pair(0, 0)), pred(NULL){}
    Node(T v):id(gid++), color(0), value(v), TIME(make_pair(0, 0)), pred(NULL){}
    friend bool operator==(Node<T> a, Node<T> b){
        return a.id == b.id && a.color == b.color && a.value == b.value;
    }
};

template<class T>
class Graph{
    map<
        int, pair<Node<T>*, list<Node<T>*>>
    > LIST; // adj list of graph
    public:
    void insertNode(T value);
    void insertNode(Node<T> *node);
    void connect(Node<T> *a, Node<T> *b); // create an edge between a & b
    void connect_d(Node<T> *a, Node<T> *b); // create an edge between a & b
    void removeNode(Node<T> *node);
    void removeNode_d(Node<T> *node);
    void removeEdge(Node<T> *a, Node<T> *b); // delete the edge between a & b
    void removeEdge_d(Node<T> *a, Node<T> *b); // delete the edge between a & b
    void DFS_traverse(Node<T> *a);
    void DFS(Node<T> *a);
    void print_all();
    void color_reset();
};

template<class T>
void Graph<T>::insertNode(T value){
    Node<T> *node = new Node<T>(value);
    list<Node<T>*> l;
    // instead of "insert", use "emplace", it avoids implicit creation of temporary element w.r.t. container.
    // "emplace" use the parameters to create element
    const auto p = LIST.emplace(node->id, make_pair(node, l)); // i.e. use node->id, make_pair(node, l) to create element in map
    // Returns a pair consisting of an iterator to the inserted element, or the already-existing element if no insertion happened, 
    // and a bool denoting whether the insertion took place.
    if (!p.second) {
        cout << "insert failed\n";
    }
}

template<class T>
void Graph<T>::insertNode(Node<T> *node){
    list<Node<T>*> l;
    const auto p = LIST.emplace(node->id, make_pair(node, l));
    if (!p.second) {
        cout << "insert failed\n";
    }
}

template<class T>
void Graph<T>::connect(Node<T> *a, Node<T> *b){
    // create edge(a, b) and edge(b, a)
    bool foundA = (LIST.find(a->id) != LIST.end());
    bool foundB = (LIST.find(b->id) != LIST.end());
    if (foundA && foundB){
        LIST[a->id].second.emplace_back(b);
        LIST[b->id].second.emplace_back(a);
    }
    else{
        if (!foundA)
            cout << "-\nnode id_" << a->id << " not found\n";
        if (!foundB)
            cout << "-\nnode id_" << b->id << " not found\n";
    }
}

template<class T>
void Graph<T>::connect_d(Node<T> *a, Node<T> *b){ 
    // create edge(a, b)
    bool foundA = (LIST.find(a->id) != LIST.end());
    bool foundB = (LIST.find(b->id) != LIST.end());
    if (foundA && foundB){
        b->ptrBy.emplace_back(a->id); // save id of a, for the use of removeNode
        LIST[a->id].second.emplace_back(b);
    }
    else{
        if (!foundA)
            cout << "-\nnode id_" << a->id << " not found\n";
        if (!foundB)
            cout << "-\nnode id_" << b->id << " not found\n";
    }
}

template<class T>
void Graph<T>::print_all(){
    cout << "-\n";
    for (auto i: LIST){
        int pred = (i.second.first->pred) ? i.second.first->pred->id : -1; // if predecessor is NULL, then print as -1
        cout << "id: " << i.second.first->id << ", value: " << i.second.first->value << ", color: " << i.second.first->color 
        << ", time: <" << i.second.first->TIME.first << ", " << i.second.first->TIME.second << ">, pred: id(" << pred << ")\n";
        for (auto j: i.second.second){
            cout << "\tadj_id: " << j->id << ", adj_value: " << j->value << ", adj_color: " << j->color << endl;
        }
        cout << endl;
    }
}

template<class T>
void Graph<T>::removeNode(Node<T> *node){
    // remove node and it's all adjacent edge from adj_list
    auto a = LIST.find(node->id);
    if (a == LIST.end()) return;
    for (auto &n: (*a).second.second){ // iterate through node's all adj nodes
        // NOTE: if not simple graph, remember to remove all edges.
        auto b = find(LIST[n->id].second.begin(), LIST[n->id].second.end(), node); // remove node's adj edge
        if (b != LIST[n->id].second.end()){
            LIST[n->id].second.erase(b);
        }
        else{
            cout << "-\nedge(" << n->id << ", " << node->id << ") not found\n";
        }
    }
    LIST.erase(a); // what's the content after erase ?
    delete(node);
}

template<class T>
void Graph<T>::removeNode_d(Node<T> *node){
    // remove all edges to node_a
    auto a = LIST.find(node->id);
    if (a == LIST.end()) return;
    for (int id: node->ptrBy){
        auto b = find(LIST[id].second.begin(), LIST[id].second.end(), node);
        if (b != LIST[id].second.end()){
            LIST[id].second.erase(b);
        }
        else{
            cout << "-\nedge(" << id << ", " << node->id << ") not found\n";
        }
    }
    LIST.erase(a);
    delete(node);

    // for (auto &n: (*iter).second.second){ // iterate through node's all adj nodes
    //     // NOTE: if not simple graph, remember to remove all edges.
    //     auto tmp = find(LIST[n->id].second.begin(), LIST[n->id].second.end(), node); // remove node's adj edge
    //     if (tmp != LIST[n->id].second.end()){
    //         LIST[n->id].second.erase(tmp);
    //     }
    //     else{
    //         cout << "-\nedge(" << n->id << ", " << node->id << ") not found\n";
    //     }
    // }
}

template<class T>
void Graph<T>::removeEdge(Node<T> *a, Node<T> *b){
    if (!LIST.count(a->id)) return;
    if (!LIST.count(b->id)) return; 
    // NOTE: if LIST[id] is called, and id is not a key in map, cpp would implicitly create an element and insert it to map, and might cause bugs.
    // remove node's adj edge
    auto ab = find(LIST[a->id].second.begin(), LIST[a->id].second.end(), b); // edge(a, b)
    auto ba = find(LIST[b->id].second.begin(), LIST[b->id].second.end(), a); // edge(b, a)
    if (ab != LIST[a->id].second.end() && ba != LIST[b->id].second.end()){
        LIST[a->id].second.erase(ab);
        LIST[b->id].second.erase(ba);
    }
    else{
        cout << "-\nedge(" << a->id << ", " << b->id << ") not found\n";
    }
}

template<class T>
void Graph<T>::removeEdge_d(Node<T> *a, Node<T> *b){
    if (!LIST.count(a->id)) return;
    if (!LIST.count(b->id)) return; 
    // NOTE: if LIST[id] is called, and id is not a key in map, cpp would implicitly create an element and insert it to map, and might cause bugs.
    // remove node's adj edge
    auto ab = find(LIST[a->id].second.begin(), LIST[a->id].second.end(), b); // edge(a, b)
    if (ab != LIST[a->id].second.end()){
        LIST[a->id].second.erase(ab);
    }
    else{
        cout << "-\nedge(" << a->id << ", " << b->id << ") not found\n";
    }
}

template<class T>
void Graph<T>::color_reset(){
    // set all nodes' color to white, i.e. set to 0
    for (auto l: LIST){
        l.second.first->color = 0;
    }
}

template<class T>
void Graph<T>::DFS_traverse(Node<T> *node){
    if (LIST.find(node->id) == LIST.end()) return;
    node->color = 1; // set node to gray
    node->TIME.first = ::TIME++;
    cout << "id_" << node->id << ", value: " << node->value << endl;
    for (auto &n: LIST[node->id].second){
        if (n->color == 0){
            n->pred = node;
            DFS_traverse(n);
        }
    }
    node->color = 2; // set node to black
    node->TIME.second = ::TIME++;
}

template<class T>
void Graph<T>::DFS(Node<T> *node){
    cout << "-\nDFS:\n";
    DFS_traverse(node);
    cout << "-\ndo DFS to nodes that haven't been visited: \n";
    for (auto l: LIST){
        if (l.second.first->color == 0){
            DFS_traverse(l.second.first);
            // cout << "id_" << l.second.first->id << ", value: " << l.second.first->value << " color: " << l.second.first->color << endl;
        }
    }
    color_reset();
}

int main(){
    Graph<char> graph = Graph<char>();
    Node<char> *a = new Node<char>('A'); Node<char> *b = new Node<char>('B'); Node<char> *c = new Node<char>('C'); Node<char> *d = new Node<char>('D'); 
    Node<char> *e = new Node<char>('E'); Node<char> *f = new Node<char>('F'); Node<char> *g = new Node<char>('G'); Node<char> *h = new Node<char>('H');
    graph.insertNode(a); graph.insertNode(b); graph.insertNode(c); graph.insertNode(d);
    graph.insertNode(e); graph.insertNode(f); graph.insertNode(g); graph.insertNode(h);
    graph.connect_d(a, b); graph.connect_d(a, c);
    graph.connect_d(b, d);
    graph.connect_d(c, b); graph.connect_d(c, f);
    graph.connect_d(d, e); graph.connect_d(d, f);
    graph.connect_d(f, b);
    graph.connect_d(g, e); graph.connect_d(g, h);
    graph.connect_d(h, g);
    
    graph.print_all();
    graph.removeNode_d(b);
    graph.removeEdge_d(a, c);
    graph.DFS(a);
    graph.print_all();
}