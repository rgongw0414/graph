#include <bits/stdc++.h>
using namespace std;

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
    > LIST;
    Graph(){}
    void insertNode(int value);
    void insertNode(Node &node);
    void connect(Node &a, Node &b); // create an edge between a & b
    void disconnect(Node &a, Node &b); // delete the edge between a & b
};

void Graph::insertNode(int value){
    Node node = Node(value);
    list<Node> l;
    // instead of "insert", use "emplace", it avoids implicit creation of temporary element w.r.t. container.
    // "emplace" use the parameters to create element
    LIST.emplace(node.id, make_pair(node, l));  // i.e. use node.id, make_pair(node, l) to create element in map
}

void Graph::insertNode(Node &node){
    list<Node> l;
    LIST.emplace(node.id, make_pair(node, l));
}

void Graph::connect(Node &a, Node &b){
    //find(LIST.begin(), LIST.end(), a)
    LIST[a.id].push_back(b);
    LIST[b.id].push_back(a);
}

int main(){
    Node a = Node(1);
    Graph g = Graph();
    g.insertNode(a);
    cout << g.LIST[0].first.id << endl;
    g.insertNode(2);
    cout << g.LIST[1].first.id << endl;
}