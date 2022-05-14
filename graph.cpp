#include <bits/stdc++.h>
using namespace std;
#define endl '\n'

static int gid = 0;
class Node{
    public:
    int id;
    int state;
    int value;
    Node():id(gid++), state(0), value(0){}
    Node(int v):id(gid++), state(0), value(v){}
    friend bool operator==(Node a, Node b){
        return a.id == b.id && a.state == b.state && a.value == b.value;
    }
};

class Graph{
    map<
        int, pair<Node, list<Node>>
    > LIST; // adj list of graph
    public:
    void insertNode(int value);
    void insertNode(Node &node);
    void connect(Node &a, Node &b); // create an edge between a & b
    void print_all();
    void removeNode(Node &node);
    void removeEdge(Node &a, Node &b); // delete the edge between a & b
    void DFS(Node &a);
};

void Graph::insertNode(int value){
    Node node = Node(value);
    list<Node> l;
    // instead of "insert", use "emplace", it avoids implicit creation of temporary element w.r.t. container.
    // "emplace" use the parameters to create element
    const auto p = LIST.emplace(node.id, make_pair(node, l)); // i.e. use node.id, make_pair(node, l) to create element in map
    // Returns a pair consisting of an iterator to the inserted element, or the already-existing element if no insertion happened, 
    // and a bool denoting whether the insertion took place.
    if (!p.second) {
        cout << "insert failed\n";
    }
}

void Graph::insertNode(Node &node){
    list<Node> l;
    const auto p = LIST.emplace(node.id, make_pair(node, l));
    if (!p.second) {
        cout << "insert failed\n";
    }
}

void Graph::connect(Node &a, Node &b){
    int foundA = (LIST.find(a.id) != LIST.end());
    int foundB = (LIST.find(b.id) != LIST.end());
    if (foundA && foundB){
        LIST[a.id].second.emplace_back(b);
        LIST[b.id].second.emplace_back(a);
    }
    else{
        if (!foundA)
            cout << "-\nnode id_" << a.id << " not found\n";
        if (!foundB)
            cout << "-\nnode id_" << b.id << " not found\n";
    }
}

void Graph::print_all(){
    cout << "-\n";
    for (auto i: LIST){
        cout << "id: " << i.second.first.id << ", value: " << i.second.first.value << endl;
        for (auto j: i.second.second){
            cout << "\tadj_id: " << j.id << ", adj_value: " << j.value << endl;
        }
    }
}

void Graph::removeNode(Node &node){
    // remove node and it's all adjacent edge from adj_list
    auto iter = LIST.find(node.id);
    if (iter == LIST.end()) return;
    for (auto &n: (*iter).second.second){ // iterate through node's all adj nodes
        // NOTE: if not simple graph, remember to remove all edges.
        auto tmp = find(LIST[n.id].second.begin(), LIST[n.id].second.end(), node); // remove node's adj edge
        if (tmp != LIST[n.id].second.end()){
            LIST[n.id].second.erase(tmp);
        }
        else{
            cout << "-\nedge(" << n.id << ", " << node.id << ") not found\n";
        }
    }
    LIST.erase(iter); // what's the content after erase ?
}

void Graph::removeEdge(Node &a, Node &b){
    if (!LIST.count(a.id)) return;
    if (!LIST.count(b.id)) return; 
    // NOTE: if LIST[id] is called, and id is not a key in map, cpp would implicitly create an element and insert it to map, and might cause bugs.
    // remove node's adj edge
    auto ab = find(LIST[a.id].second.begin(), LIST[a.id].second.end(), b); // edge(a, b)
    auto ba = find(LIST[b.id].second.begin(), LIST[b.id].second.end(), a); // edge(b, a)
    if (ab != LIST[a.id].second.end() && ba != LIST[b.id].second.end()){
        LIST[a.id].second.erase(ab);
        LIST[b.id].second.erase(ba);
    }
    else{
        cout << "-\nedge(" << a.id << ", " << b.id << ") not found\n";
    }
}

int main(){
    Node a = Node(5);
    Node b = Node(10);
    Node c = Node(15);
    Graph g = Graph();
    g.insertNode(a);
    g.insertNode(b);
    g.insertNode(c);
    g.connect(a, b);
    g.connect(b, c);
    g.connect(c, a);
    g.print_all();
    g.removeNode(b);
    g.removeEdge(a, b);
    Node d = Node(20);
    g.insertNode(d);
    g.connect(a, d);
    g.print_all();
    g.removeEdge(a, d);
    g.print_all();
    g.removeEdge(a, d);
    g.removeNode(b);
}