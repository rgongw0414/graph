#include <bits/stdc++.h>
using namespace std;
#define endl '\n'
#define WHITE 0
#define IN_QUEUE 1.5 // in queue, but not visited yet.
#define GRAY 1 // visited
#define BLACK 2 // finish

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
    Node():id(gid++), color(WHITE), TIME(make_pair(0, 0)), pred(NULL){}
    Node(T v):id(gid++), color(WHITE), value(v), TIME(make_pair(0, 0)), pred(NULL){}
    friend bool operator==(const Node<T> a, const Node<T> b){
        return a.id == b.id && a.color == b.color && a.value == b.value;
    }
};

template<class T>
class Graph{
    map<
        int, pair<const Node<T>*, list<const Node<T>*>>
    > LIST; // adj list of graph
    bool Acyclic; // default acyclic, once find back edge, set to cyclic.
    bool Directed;
    public:
    Graph():Acyclic(true), Directed(false){}
    bool acyclic() const;
    bool directed() const;
    void insertNode(const T value);
    void insertNode(const Node<T> *node);
    void connect(const Node<T> *a, const Node<T> *b); // create an edge between a & b
    void connect_d(const Node<T> *a, const Node<T> *b); // create an edge between a & b
    void removeNode(const Node<T> *node);
    void removeNode_d(const Node<T> *node);
    void removeEdge(const Node<T> *a, const Node<T> *b); // delete the edge between a & b
    void removeEdge_d(const Node<T> *a, const Node<T> *b); // delete the edge between a & b
    void DFS_traverse(const Node<T> *a);
    void DFS(const Node<T> *a);
    void BFS_traverse(list<const Node<T>*> &queue);
    void BFS(const Node<T> *a);
    void print_all() const;
    void color_reset() const;
};

template<class T>
bool Graph<T>::acyclic() const{
    return this->Acyclic;
}

template<class T>
bool Graph<T>::directed() const{
    return this->Directed;
}

template<class T>
void Graph<T>::insertNode(const T value){
    const Node<T> *node = new Node<T>(value);
    list<const Node<T>*> l;
    // instead of "insert", use "emplace", it avoids implicit creation of temporary element w.r.t. container.
    // "emplace" use the parameters to create element
    const auto p = LIST.emplace(node->id, make_pair(node, l)); // i.e. use node->id, make_pair(node, l) to create element in map
    // Returns a pair consisting of an iterator to the inserted element, or the already-existing element if no insertion happened, 
    // and a bool denoting whether the insertion took place.
    if (!p.second) cout << "insert failed\n";
}

template<class T>
void Graph<T>::insertNode(const Node<T> *node){
    list<const Node<T>*> l;
    const auto p = LIST.emplace(node->id, make_pair(node, l));
    if (!p.second) cout << "insert failed\n";
}

template<class T>
void Graph<T>::connect(const Node<T> *a, const Node<T> *b){
    // create edge(a, b) and edge(b, a)
    const bool foundA = (LIST.find(a->id) != LIST.end());
    const bool foundB = (LIST.find(b->id) != LIST.end());
    if (foundA && foundB){
        LIST[a->id].second.emplace_back(b);
        LIST[b->id].second.emplace_back(a);
    }
    else{
        if (!foundA) cout << "-\nnode id_" << a->id << " not found\n";
        if (!foundB) cout << "-\nnode id_" << b->id << " not found\n";
    }
}

template<class T>
void Graph<T>::connect_d(const Node<T> *a, const Node<T> *b){ 
    if (!directed()) Directed = true;
    // create edge(a, b)
    const bool foundA = (LIST.find(a->id) != LIST.end());
    const bool foundB = (LIST.find(b->id) != LIST.end());
    if (foundA && foundB){
        (const_cast<Node<T>*>(b))->ptrBy.emplace_back(a->id); // save id of a, for the use of removeNode
        LIST[a->id].second.emplace_back(b);
    }
    else{
        if (!foundA) cout << "-\nnode id_" << a->id << " not found\n";
        if (!foundB) cout << "-\nnode id_" << b->id << " not found\n";
    }
}

template<class T>
void Graph<T>::print_all() const{
    cout << "-\n";
    if (acyclic()) cout << "acyclic, ";
    if (directed()) cout << "directed graph\n";
    else cout << "undirected graph\n";
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
void Graph<T>::removeNode(const Node<T> *node){
    // remove node and it's all adjacent edge from adj_list
    const auto a = LIST.find(node->id);
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
void Graph<T>::removeNode_d(const Node<T> *node){
    if (!directed()) Directed = true;
    // remove all edges to node_a, or edges from node_a to other nodes
    const auto a = LIST.find(node->id);
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
}

template<class T>
void Graph<T>::removeEdge(const Node<T> *a, const Node<T> *b){
    if (!LIST.count(a->id)) return;
    if (!LIST.count(b->id)) return; 
    // NOTE: if LIST[id] is called, and id is not a key in map, cpp would implicitly create an element and insert it to map, and might cause bugs.
    // remove node's adj edge
    const auto ab = find(LIST[a->id].second.begin(), LIST[a->id].second.end(), b); // edge(a, b)
    const auto ba = find(LIST[b->id].second.begin(), LIST[b->id].second.end(), a); // edge(b, a)
    if (ab != LIST[a->id].second.end() && ba != LIST[b->id].second.end()){
        LIST[a->id].second.erase(ab);
        LIST[b->id].second.erase(ba);
    }
    else{
        cout << "-\nedge(" << a->id << ", " << b->id << ") not found\n";
    }
}

template<class T>
void Graph<T>::removeEdge_d(const Node<T> *a, const Node<T> *b){
    if (!directed()) Directed = true;
    if (!LIST.count(a->id)) return;
    if (!LIST.count(b->id)) return; 
    // NOTE: if LIST[id] is called, and id is not a key in map, cpp would implicitly create an element and insert it to map, and might cause bugs.
    // remove node's adj edge
    const auto ab = find(LIST[a->id].second.begin(), LIST[a->id].second.end(), b); // edge(a, b)
    if (ab != LIST[a->id].second.end()){
        LIST[a->id].second.erase(ab);
    }
    else{
        cout << "-\nedge(" << a->id << ", " << b->id << ") not found\n";
    }
}

template<class T>
void Graph<T>::color_reset() const{
    // set all nodes' color to white, i.e. set to 0
    for (auto &l: LIST){
        (const_cast<Node<T>*>(l.second.first))->color = WHITE;
    }
}

template<class T>
void Graph<T>::DFS_traverse(const Node<T> *node){
    auto a = const_cast<Node<T>*>(node);
    if (LIST.find(a->id) == LIST.end()) return;
    a->color = GRAY; // set node to gray
    a->TIME.first = ::TIME++;
    cout << "\tid_" << a->id << ", value: " << a->value << endl;
    for (auto &b: LIST[a->id].second){
        // NOTE: undirected graph only have either Tree edge or Back edge
        if (b->color == WHITE){
            // cout << "tree edge: (" << a->value << ", " << b->value << ")\n";
            (const_cast<Node<T>*>(b))->pred = a;
            DFS_traverse(b);
        }
        else if (b->color == WHITE){
            // cout << "back edge: (" << a->value << ", " << b->value << ")\n";
            if (directed() && acyclic()) this->Acyclic = false;
            else if (!directed() && acyclic()){
                // undirected graph cycle detection
                if (a->pred->color == WHITE && b != a->pred){
                    // cycle detected
                    this->Acyclic = false;
                }
            }
        }
        else{ // compare discover time
            if (a->TIME.first < b->TIME.first){
                // cout << "forward edge: (" << a->value << ", " << b->value << ")\n";
            }
            else if (a->TIME.first > b->TIME.first){
                // cout << "cross edge: (" << a->value << ", " << b->value << ")\n";
            }
        }
    }
    a->color = BLACK; // set node to black
    a->TIME.second = ::TIME++;
}

template<class T>
void Graph<T>::DFS(const Node<T> *node){
    cout << "-\nDFS:\n";
    DFS_traverse(node);
    // cout << "-\ndo DFS to nodes that haven't been visited: \n";
    for (auto l: LIST){
        if (l.second.first->color == WHITE){
            DFS_traverse(l.second.first);
        }
    }
    color_reset();
    ::TIME = 0;
}

template<class T>
void Graph<T>::BFS_traverse(list<const Node<T>*> &queue){
    if (queue.empty()) return;
    auto node = const_cast<Node<T>*>(queue.front());
    queue.pop_front();
    node->color = GRAY;
    // node->TIME.first = ::TIME++;
    cout << "\tid_" << node->id << ", value: " << node->value << endl;
    const auto adj_nodes = (*LIST.find(node->id)).second.second;
    for (auto adj_node: adj_nodes){
        if (adj_node->color == IN_QUEUE) continue;
        else if (adj_node->color == WHITE) {
            (const_cast<Node<T>*>(adj_node))->pred = node;
            (const_cast<Node<T>*>(adj_node))->color = IN_QUEUE;
            queue.emplace_back(adj_node);
        }
    }
    BFS_traverse(queue);
    node->color = BLACK;
    // node->TIME.second = ::TIME++;
}

template<class T>
void Graph<T>::BFS(const Node<T> *node){
    const auto n = LIST.find(node->id);
    if (n == LIST.end()){
        cout << "node not found\n";
        return;
    }
    list<const Node<T>*> queue; // FIFO
    if (node->color == WHITE){
        queue.emplace_back(node);
        BFS_traverse(queue);
    }
    // cout << "-\ndo BFS to nodes that haven't been visited: \n";
    for (auto l: LIST){
        if (l.second.first->color == WHITE){
            queue.emplace_back(l.second.first);
            BFS_traverse(queue);
        }
    }
    color_reset();
    ::TIME = 0;
}

int main(){
    // Graph<char> graph = Graph<char>();
    Node<char> *a = new Node<char>('A'); Node<char> *b = new Node<char>('B'); Node<char> *c = new Node<char>('C'); Node<char> *d = new Node<char>('D'); 
    Node<char> *e = new Node<char>('E'); Node<char> *f = new Node<char>('F'); Node<char> *g = new Node<char>('G'); Node<char> *h = new Node<char>('H');
    Node<char> *i = new Node<char>('I');
    // graph.insertNode(a); graph.insertNode(b); graph.insertNode(c); graph.insertNode(d);
    // graph.insertNode(e); graph.insertNode(f); graph.insertNode(g); graph.insertNode(h);
    // graph.connect_d(a, b); graph.connect_d(a, c); graph.connect_d(b, d); graph.connect_d(c, b); graph.connect_d(c, f); graph.connect_d(d, e); 
    // graph.connect_d(d, f); graph.connect_d(f, b); graph.connect_d(g, e); graph.connect_d(g, h); graph.connect_d(h, g);
    // graph.print_all();
    // graph.removeNode_d(b);
    // graph.removeEdge_d(a, c);
    // graph.DFS(a);
    // graph.BFS(a);
    // graph.print_all();

    Graph<char> graph3 = Graph<char>();
    graph3.insertNode(a); graph3.insertNode(b); graph3.insertNode(c); graph3.insertNode(d);
    graph3.insertNode(e); graph3.insertNode(f); graph3.insertNode(g); graph3.insertNode(h);
    graph3.insertNode(i);
    graph3.connect(a, b); graph3.connect(a, c); graph3.connect(a, d); graph3.connect(b, e); 
    graph3.connect(c, e); graph3.connect(c, f); graph3.connect(c, g); graph3.connect(c, h); 
    graph3.connect(d, h); graph3.connect(h, g); graph3.connect(e, f); graph3.connect(f, i); graph3.connect(g, i); 
    graph3.BFS(a);
    graph3.print_all();

    // Graph<int> graph2 = Graph<int>();
    // Node<int> *zero = new Node<int>(0);  Node<int> *one = new Node<int>(1); Node<int> *two = new Node<int>(2); Node<int> *three = new Node<int>(3); 
    // graph2.insertNode(zero); graph2.insertNode(one); graph2.insertNode(two);
    // graph2.insertNode(three); 
    // graph2.connect(zero, one); graph2.connect(one, two);
    // graph2.connect(two, three); graph2.connect(zero, three); 
    // graph2.DFS(zero);
    // graph2.print_all();
}