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
    public:
    map<
        int, pair<Node, list<Node>>
    > LIST; // adj list of graph
    Graph(){}
    void insertNode(int value);
    void insertNode(Node &node);
    void connect(Node &a, Node &b); // create an edge between a & b
    void disconnect(Node &a, Node &b); // delete the edge between a & b
    void print_all();
    void removeNode(Node &node);
    void removeEdge(Node &a, Node &b);
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
    LIST[a.id].second.emplace_back(b);
    LIST[b.id].second.emplace_back(a);
}

void Graph::print_all(){
    for (auto i: LIST){
        cout << "id: " << i.second.first.id << endl;
        for (auto j: i.second.second){
            cout << "\tadj_id: " << j.id << ", adj_value: " << j.value << endl;
        }
    }
}

void Graph::removeNode(Node &node){
    // remove node and it's all adjacent edge from adj_list
    auto iter = LIST.find(node.id);
    for (auto &n: (*iter).second.second){ // iterate through node's all adj nodes
        // NOTE: if not simple graph, remember to remove all edges.
        auto tmp = find(LIST[n.id].second.begin(), LIST[n.id].second.end(), node); // remove node's adj edge
        if (tmp != LIST[n.id].second.end()){
            LIST[n.id].second.erase(tmp);
        }
        else{
            cout << "not found\n";
        }
    }
    LIST.erase(iter);
}

void Graph::removeEdge(Node &a, Node &b){
    auto iter1 = LIST.find(a.id);
    auto iter2 = LIST.find(b.id);
    // "find" return b's iterator in adj_list of node_a
    auto tmp1 = find(LIST[a.id].second.begin(), LIST[a.id].second.end(), b); // remove node's adj edge
    auto tmp2 = find(LIST[b.id].second.begin(), LIST[b.id].second.end(), a); 
    if (tmp1 != LIST[a.id].second.end()){
        LIST[a.id].second.erase(tmp1);
    }
    else{
        cout << "node a not found\n";
    }
    if (tmp2 != LIST[b.id].second.end()){
        LIST[b.id].second.erase(tmp2);
    }
    else{
        cout << "node b not found\n";
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
    g.connect(a, c);
    g.print_all();
    // g.removeNode(a);
    g.removeEdge(a, b);
    g.print_all();
}