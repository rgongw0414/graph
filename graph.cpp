#include <bits/stdc++.h>
using namespace std;
#define endl '\n'
#define WHITE 0 // not found yet
#define GRAY 1  // visited
#define BLACK 2 // finish

static int gid = 0, TIME = 0;

template<class T>
class Node{
    public:
    const int id;
    int color; // 0: white, 1: gray, 2: black
    T value;
    pair<int, int> TIME; //  discover, finish
    Node* pred; // predecessor
    Node():id(gid++), color(WHITE), TIME(make_pair(-1, -1)), pred(NULL){}
    Node(T v):id(gid++), color(WHITE), value(v), TIME(make_pair(-1, -1)), pred(NULL){}
};

template<class T>
bool operator==(const Node<T> a, const Node<T> b){ return a.id == b.id && a.color == b.color && a.value == b.value; }

// template<class T>
// struct Node_comp {
//     bool operator()(const Node<T>* lhs, const Node<T>* rhs) const { 
//         return lhs->id < rhs->id; // NB. ignores y on purpose
//     }
// };

template<class T>
bool Node_comp(const Node<T>* a, const Node<T>* b){ return a->id < b->id; }

template<class T>
class Graph{
    public:
    multimap<
        const Node<T>*, pair<const Node<T>*, float>
                        , bool(*)(const Node<T>*, const Node<T>*)
    > LIST; // adj list of graph

    bool Acyclic; // default acyclic, once find back edge, set to cyclic.
    bool Directed;
    bool Weighted;
    Graph():LIST(Node_comp<T>), Acyclic(true), Directed(false), Weighted(false){}
    bool acyclic() const;
    bool directed() const;
    bool weighted() const;
    const Node<T> * insertNode(const T value); // return the inserted elem in LIST
    void insertNode(const Node<T> *node);
    void connect(const Node<T> *a, const Node<T> *b, float weight = 0); // create weighted undirected edge
    void connect_d(const Node<T> *a, const Node<T> *b, float weight = 0); // create weighted directed edge
    void removeNode(const Node<T> *node);
    void removeNode_d(const Node<T> *node);
    void removeEdge(const Node<T> *a, const Node<T> *b); // delete the edge between a & b
    void removeEdge_d(const Node<T> *a, const Node<T> *b); // delete the edge between a & b
    void DFS_traverse(const Node<T> *a);
    void DFS(const Node<T> *start);
    void DFS(const Node<T> *start, list<const Node<T>*> &nodes); // do DFS in customized order, e.g. in descending order of finish time
    void BFS_traverse(list<const Node<T>*> &queue);
    void BFS(const Node<T> *a);
    void SetCollapsing(const Node<T>* node);
    void CCDFS(); // find connected components in "undirected graph", with SetCollapsing.
    void CCDFS2(); // find CC only by predecessors
    void CCBFS(); 
    void transpose();
    void SCCDFS(const Node<T> *start); //  find strongly connected components in "directed graph", by two DFS, transpose, and sort by finish time.
    void topological_sort();
    void topological_sort(const Node<T>* start);
    void MST_Kruskal(const Node<T>* start);
    void MST_Prim();
    void print_CC(map<int, list<const Node<T>*>> &CC);
    void print_all() const;
    void pred_reset();
    void color_reset();
    void reset();
};

template<class T>
bool Graph<T>::acyclic() const{ return this->Acyclic; }

template<class T>
bool Graph<T>::directed() const{ return this->Directed; }

template<class T>
bool Graph<T>::weighted() const{ return this->Weighted; }

template<class T>
const Node<T> * Graph<T>::insertNode(const T value){
    const Node<T> *node = new Node<T>(value);
    const Node<T>* NuLL = NULL;
    float weight = 0;
    LIST.emplace(make_pair(node, make_pair(NuLL, weight))); // "emplace" returns the iter of the inserted elem
    // NOTE: instead of "insert", use "emplace", it avoids implicit creation of temporary element w.r.t. container.
    //       "emplace" use the parameters to create element
    return node;
}

template<class T>
void Graph<T>::insertNode(const Node<T> *node){
    const Node<T>* NuLL = NULL;
    float weight = 0;
    auto elem = LIST.emplace(make_pair(node, make_pair(NuLL, weight)));
}

template<class T>
void Graph<T>::connect(const Node<T> *a, const Node<T> *b, float weight){
    // create edge(a, b) and edge(b, a)
    auto iterA = LIST.find(a); const bool foundA = (iterA != LIST.end());
    auto iterB = LIST.find(b); const bool foundB = (iterB != LIST.end());
    if (foundA && foundB){
        LIST.emplace(make_pair(a, make_pair(b, weight)));
        LIST.emplace(make_pair(b, make_pair(a, weight)));
    }
    else{
        if (!foundA) cout << "-\nnode id_" << a->id << " not found\n";
        if (!foundB) cout << "-\nnode id_" << b->id << " not found\n";
    }
}

template<class T>
void Graph<T>::connect_d(const Node<T> *a, const Node<T> *b, float weight){ 
    if (!directed()) Directed = true;
    // create edge(a, b)
    const bool foundA = (LIST.find(a) != LIST.end());
    const bool foundB = (LIST.find(b) != LIST.end());
    if (foundA && foundB){
        if (!weighted()) this->Weighted = true;
        LIST.emplace(make_pair(a, make_pair(b, weight)));
    }
    else{
        if (!foundA) cout << "-\nnode id_" << a->id << " not found\n";
        if (!foundB) cout << "-\nnode id_" << b->id << " not found\n";
    }
}

template<class T>
void Graph<T>::print_all() const{
    cout << "-\n";
    if (acyclic()) cout << "acyclic, "; // detect cycle while DFS
    if (directed()) cout << "directed graph\n";
    else cout << "undirected graph\n";
    for (auto elem: LIST){
        auto a = elem.first;
        auto b = elem.second.first;
        if (b == NULL){
            int pred = (a->pred) ? a->pred->id : -1; // if predecessor is NULL, then print as -1
            cout << "id: " << a->id << ", value: " << a->value << ", color: " << a->color 
            << ", time: <" << a->TIME.first << ", " << a->TIME.second << ">, pred: id(" << pred << ")\n";
        }
        else{
            cout << "\tadj_id: " << b->id << ", adj_value: " << b->value << ", adj_color: " << b->color;
            if (weighted()) cout << ", weight: " << elem.second.second;
            cout << endl;
        }
    }
}

template<class T>
void Graph<T>::removeNode(const Node<T> *node){
    // remove node and it's all adjacent edge from adj_list
    auto num_deleted = LIST.erase(node);
    if (num_deleted == 0){
        cout << "node not in graph\n";
        return;
    }
    for (auto iter = LIST.begin(); iter != LIST.end();){ // iterate through node's all adj nodes
        auto b = (*iter).second.first;
        auto tmp = iter;
        iter++;
        if (b == node) LIST.erase(tmp);
    }
    delete(node);
}

template<class T>
void Graph<T>::removeNode_d(const Node<T> *node){
    if (!directed()) Directed = true;
    removeNode(node);
}

template<class T>
void Graph<T>::removeEdge(const Node<T> *a, const Node<T> *b){
    if (a == NULL || b == NULL) return;
    auto pa = LIST.equal_range(a); auto ab = LIST.end();
    for (auto it = pa.first; it != pa.second; it++){
        if (it->second.first == b){
            ab = it;
            break;
        }
    }
    auto pb = LIST.equal_range(b); auto ba = LIST.end();
    for (auto it = pb.first; it != pb.second; it++){
        if (it->second.first == b){
            ba = it;
            break;
        }
    }
    if (ab != LIST.end() && ba != LIST.end()){
        LIST.erase(ab);
        LIST.erase(ba);
    }
    else cout << "-\nedge(" << a->id << ", " << b->id << ") not found\n";
}

template<class T>
void Graph<T>::removeEdge_d(const Node<T> *a, const Node<T> *b){
    if (a == NULL || b == NULL) return;
    if (!directed()) Directed = true;
    auto pa = LIST.equal_range(a); auto ab = LIST.end();
    for (auto it = pa.first; it != pa.second; it++){
        if (it->second.first == b) { ab = it; break; }
    }
    if (ab != LIST.end()) LIST.erase(ab);
    else cout << "-\nedge(" << a->id << ", " << b->id << ") not found\n";
}

template<class T>
void Graph<T>::color_reset(){
    // set all nodes' color to white, i.e. set to 0
    for (auto &l: LIST) if (l.first->color != WHITE) (const_cast<Node<T>*>(l.first))->color = WHITE;
}

template<class T>
void Graph<T>::pred_reset(){
    // reset predecessor, for multiple times of DFS.
    for (auto &l: LIST) if (l.first->pred != NULL) const_cast<Node<T>*>(l.first)->pred = NULL; 
}

template<class T>
void Graph<T>::reset(){
    color_reset();
    pred_reset();
    for (auto &l: LIST) if (l.first->TIME.first != 0 || l.first->TIME.second != 0) l.first->TIME = make_pair(0, 0);
}

template<class T>
void Graph<T>::DFS_traverse(const Node<T> *node){
    auto a = const_cast<Node<T>*>(node);
    // if (LIST.find(a) == LIST.end()) return;
    a->color = GRAY; // set node to gray
    a->TIME.first = ::TIME++;
    cout << "\tid_" << a->id << ", value: " << a->value << endl;
    auto p = LIST.equal_range(node);
    for (auto it = p.first; it != p.second; it++){
        auto b = it->second.first;
        if (b == NULL) continue; // if b is a iteself, i.e. b: <a, <NULL, 0>>, then skip.
        // NOTE: undirected graph only have either Tree edge or Back edge
        if (b->color == WHITE){
            // cout << "tree edge: (" << a->value << ", " << b->value << ")\n";
            (const_cast<Node<T>*>(b))->pred = a;
            DFS_traverse(b);
        }
        else if (b->color == GRAY){
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
        else if (b->color == BLACK){ // compare discover time
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
void Graph<T>::DFS(const Node<T> *start){
    pred_reset();
    cout << "-\nDFS:\n";
    DFS_traverse(start);
    // cout << "-\ndo DFS to nodes that haven't been visited: \n";
    for (auto &l: LIST) if (l.first->color == WHITE) DFS_traverse(l.first);
    color_reset();
    ::TIME = 0;
}

template<class T>
void Graph<T>::DFS(const Node<T> *start, list<const Node<T>*> &nodes){ // do DFS in customized order, e.g. in descending order of finish time
    pred_reset();
    cout << "-\nDFS:\n";
    DFS_traverse(start);
    // cout << "-\ndo DFS to nodes that haven't been visited: \n";
    for (auto &n: nodes) if (n->color == WHITE) DFS_traverse(n);
    color_reset();
    ::TIME = 0;
}

template<class T>
void Graph<T>::BFS_traverse(list<const Node<T>*> &queue){
    if (queue.empty()) return;
    // cout << "queue: \n(front) <- "; for (auto n: queue) cout << n->value << " "; cout << "<- (back)" << endl;
    auto node = const_cast<Node<T>*>(queue.front());
    queue.pop_front();
    node->color = BLACK;
    node->TIME.second = ::TIME++;
    cout << "\tid_" << node->id << ", value: " << node->value << endl;
    auto p = LIST.equal_range(node);
    for (auto it = p.first; it != p.second; it++){
        auto adj_node = (const_cast<Node<T>*>(it->second.first)) ;
        if (adj_node != NULL && adj_node->color == WHITE) {
            adj_node->color = GRAY;
            adj_node->TIME.first = ::TIME++;
            adj_node->pred = node;
            queue.emplace_back(it->second.first);
        }
    }
    BFS_traverse(queue);
}

template<class T>
void Graph<T>::BFS(const Node<T> *start){
    if (LIST.find(start) == LIST.end()){ cout << "node not found\n"; return; }
    pred_reset();
    cout << "-\nBFS:\n";
    list<const Node<T>*> queue; // FIFO
    if (start->color == WHITE){
        (const_cast<Node<T>*>(start))->color = GRAY;
        (const_cast<Node<T>*>(start))->TIME.first = ::TIME++;
        queue.emplace_back(start);
        BFS_traverse(queue);
    }
    // cout << "-\ndo BFS to nodes that haven't been visited: \n";
    for (auto &l: LIST){
        if (l.first->color == WHITE){
            (const_cast<Node<T>*>(l.first))->color = GRAY;
            (const_cast<Node<T>*>(l.first))->TIME.first = ::TIME++;
            queue.emplace_back(l.first);
            BFS_traverse(queue);
        }
    }
    color_reset();
    ::TIME = 0;
}


template<class T>
void Graph<T>::SetCollapsing(const Node<T>* node){
    auto current = const_cast<Node<T>*>(node);
    auto root = current;
    while (root->pred != NULL) root = root->pred;
    while (current != root){
        auto parent = current->pred;
        current->pred = root;    
        current = parent;
    }
}

template<class T>
void Graph<T>::CCDFS(){
    if (this->directed()) return;
    auto start = (*LIST.begin()).first;
    this->DFS(start); // DFS first, in order to build pred on each node
    map<int, list<const Node<T>*>> CC;
    Node<T>* prev = NULL; // to avoid visit the same node
    for (auto &l: LIST){
        if (prev != l.first){
            if (l.first->pred == NULL) CC[l.first->id].emplace_back(l.first);
            else this->SetCollapsing(l.first);
        }
        prev = const_cast<Node<T>*>(l.first);
    }
    prev = NULL;
    for (auto &l: LIST){
        if (prev != l.first){
            if (l.first->pred != NULL && CC.find(l.first->pred->id) != CC.end())
                CC[l.first->pred->id].emplace_back(l.first);
        }
        prev = const_cast<Node<T>*>(l.first);
    }
    print_CC(CC);
}

template<class T>
void Graph<T>::CCDFS2(){
    if (this->directed()) return;
    auto start = (*LIST.begin()).first;
    this->DFS(start); // DFS first, in order to build pred on each node
    map<int, list<const Node<T>*>> CC;
    Node<T>* prev = NULL; // to avoid visit the same node
    for (auto &l: LIST){
        if (prev != l.first){
            if (l.first->pred == NULL)
                if (CC.find(l.first->id) == CC.end()) CC[l.first->id].emplace_back(l.first);
            else{
                auto current = const_cast<Node<T>*>(l.first);
                while (current->pred->pred != NULL) current = current->pred;
                if (CC.find(current->pred->id) == CC.end()) CC[current->pred->id].emplace_back(current->pred);
                CC[current->pred->id].emplace_back(l.first);
            }
        }
        prev = const_cast<Node<T>*>(l.first);
    }
    print_CC(CC);
}

template<class T>
void Graph<T>::transpose(){ 
    // after transpose, SCCs remain, other edges are reversed;
    // by doing so, it prevents the second DFS from traversing all components.
    if (!this->directed()) return;
    // reverse the direction of every edge
    
    // TODO: iterate through container and erase elem at the same loop safely.
    auto newLIST = LIST;
    for (auto it = newLIST.begin(); it != newLIST.end();){ // remove original edges
        auto tmp = it; it++;
        if (tmp->second.first == NULL) continue;
        else newLIST.erase(tmp);
    }
    for (auto it = LIST.begin(); it != LIST.end();){ // insert the reverse edges to new LIST
        auto tmp = it; it++;
        if (tmp->second.first == NULL) continue; // O(E), E: the num of edges
        else newLIST.emplace(make_pair(tmp->second.first, make_pair(tmp->first, tmp->second.second)));
    }
    LIST = newLIST;
}

template<class T>
bool comp_finish_time(const Node<T>* a, const Node<T>* b){ return a->TIME.second > b->TIME.second; }

template<class T>
void Graph<T>::SCCDFS(const Node<T> *start){
    if (!this->directed()) return;
    this->DFS(start); // DFS_1, to build finish time of nodes. O(V)
    this->transpose(); // O(E)
    list<const Node<T>*> nodes;
    for (auto &l: LIST) if (l.first != nodes.back()) nodes.emplace_back(l.first);
    nodes.sort(comp_finish_time<T>); // O(N*log(N))
    this->DFS(nodes.front(), nodes); // NOTE: only works for the component, which has minimal finish time in G^T (maximal finish time in G).
    map<int, list<const Node<T>*>> CC; 
    Node<T>* prev = NULL; // to avoid visit the same node
    for (auto &l: LIST){
        if (prev != l.first){
            if (l.first->pred == NULL) CC[l.first->id].emplace_back(l.first);
            else this->SetCollapsing(l.first);
        }
        prev = const_cast<Node<T>*>(l.first);
    }
    prev = NULL; // to avoid visit the same node
    for (auto &l: LIST){
        if (prev != l.first){
            if (l.first->pred != NULL && CC.find(l.first->pred->id) != CC.end())
                CC[l.first->pred->id].emplace_back(l.first);
        }
        prev = const_cast<Node<T>*>(l.first);
    }
    print_CC(CC);
    this->transpose(); // resume the graph's adj_list
}

template<class T>
void Graph<T>::topological_sort(){
    auto start = (*LIST.begin()).first;
    this->DFS(start); // DFS first, in order to build finish time on each node
    if (!acyclic()) return;
    list<const Node<T>*> topology;
    for (auto &l: LIST) if (l.first != topology.back()) topology.emplace_back(l.first);
    topology.sort(comp_finish_time<T>); // O(N*log(N))
    cout << "-\nTopological Sort: ";
    for (auto &t: topology) cout << t->value << " ";
    cout << endl;
}

template<class T>
void Graph<T>::topological_sort(const Node<T>* start){ // only for DAG (directed acyclic graph)
    this->DFS(start); // DFS first, in order to build finish time on each node, and check whether acyclic or not.
    if (!acyclic()) return;
    list<const Node<T>*> topology;
    for (auto &l: LIST) if (l.first != topology.back()) topology.emplace_back(l.first);
    topology.sort(comp_finish_time<T>); // O(N*log(N))
    cout << "-\nTopological Sort: ";
    for (auto &t: topology) cout << t->value << " ";
    cout << endl;
}

template<class T>
void Graph<T>::CCBFS(){
    if (this->directed()) return;
    auto start = (*LIST.begin()).first;
    this->BFS(start); // BFS first, in order to build pred on each node
    map<int, list<const Node<T>*>> CC;
    Node<T>* prev = NULL; // to avoid visit the same node
    for (auto &l: LIST){
        if (prev != l.first){
            if (l.first->pred == NULL) 
                CC[l.first->id].emplace_back(l.first);
            else this->SetCollapsing(l.first);
        }
        prev = const_cast<Node<T>*>(l.first);
    }
    for (auto &l: LIST){
        if (prev != l.first){
            if (l.first->pred != NULL && CC.find(l.first->pred->id) != CC.end())
                CC[l.first->pred->id].emplace_back(l.first);
        }
        prev = const_cast<Node<T>*>(l.first);
    }
    print_CC(CC);
}

template<class T>
void Graph<T>::MST_Kruskal(const Node<T>* start){
    // while avoid creating cycle, select start_node's adj nodes with min-weight edges.
    if (directed()) return;
}

template<class T>
bool comp_weight(const Node<T>* a, const Node<T>* b){ // sort weight in ascending order
    return a->weight < b->weight; 
}

template<class T>
void Graph<T>::MST_Prim(){
    // while avoid creating cycle, select min-weight edges in the graph.
    if (directed()) return;

}

template<class T>
void Graph<T>::print_CC(std::map<int, list<const Node<T>*>> &CC){
    cout << "-\nthe number of connected component: " << CC.size() << endl;
    int i = 0;
    for (auto &cc: CC){
        cout << "Component_" << 1 + i++ << ": ";
        for (auto &n: cc.second) cout << n->value << " ";
        cout << endl;
    }
}

int main(){
    Graph<int> graph2 = Graph<int>();
    Node<int> *zero = new Node<int>(0); Node<int> *one = new Node<int>(1); Node<int> *two = new Node<int>(2); Node<int> *three = new Node<int>(3); 
    Node<int> *four = new Node<int>(4); Node<int> *five = new Node<int>(5); Node<int> *six = new Node<int>(6); Node<int> *seven = new Node<int>(7); Node<int> *eight = new Node<int>(8); 
    Node<int> *nine = new Node<int>(9); Node<int> *ten = new Node<int>(10); Node<int> *eleven = new Node<int>(11); Node<int> *twelve = new Node<int>(12); Node<int> *thirteen = new Node<int>(13); 
    Node<int> *fourteen = new Node<int>(14); 

    graph2.insertNode(zero); graph2.insertNode(one); graph2.insertNode(two); graph2.insertNode(three); graph2.insertNode(four); graph2.insertNode(five); 
    graph2.insertNode(six); graph2.insertNode(seven); graph2.insertNode(eight); 
    // graph2.connect(zero, one, 5); graph2.connect(one, two, 10); graph2.connect(zero, five, 3); graph2.connect(one, four, 1); graph2.connect(one, six, 4); graph2.connect(two, six, 8); 
    // graph2.connect(two, three, 5); graph2.connect(six, four, 2); graph2.connect(six, three, 9); graph2.connect(five, four, 6); graph2.connect(four, three, 7); 
    graph2.connect_d(zero, one, 5); graph2.connect_d(one, two, 10); graph2.connect_d(two, three, 15); 
    graph2.connect_d(two, zero, 2); graph2.connect_d(three, two, 6); graph2.connect_d(two, five, 6); graph2.connect_d(one, four, 6); 
    graph2.connect_d(five, four, 6); graph2.connect_d(four, five, 6); graph2.connect_d(four, six, 6); graph2.connect_d(five, six, 6); 
    graph2.connect_d(five, seven, 6); graph2.connect_d(six, seven, 6); graph2.connect_d(seven, eight, 6); graph2.connect_d(eight, six, 6); 
    graph2.SCCDFS(zero);
    graph2.print_all();

    // std::multimap<int,char> map = {{1,'a'},{1,'b'},{1,'d'},{2,'b'},{1,'e'}};
    // auto range = map.equal_range(1);
    // for (auto it = range.first; it != range.second; ++it) {
    //     std::cout << it->first << ' ' << it->second << '\n';
    // }
    // auto r = map.emplace(make_pair(555, 'z'));
    // auto p = map.equal_range(2);
    // for (auto it = p.first; it != p.second; it++){
    //     cout << it->first << endl;
    // }
    // cout << p.first - p.second << endl;
    // cout << p.second << endl;
}