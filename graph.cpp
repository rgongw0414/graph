#include <bits/stdc++.h>
using namespace std;

class Node{
    public:
    int id;
    int state;
    int value;
    Node():id(gid++), state(0), value(0){};
    Node(int v):id(gid++), state(0), value(v){};
    friend operator==(Node a, Node b){
        return a.state == b.state && a.value == b.value;
    }
};
static int gid = 0;

class Graph{
    public:
    vector<
        pair<Node, list<int>>
    > graph;
    // vector<list<int>> graph;
    void insertNode(int value);
    void connect(Node a, Node b); // create an edge between a & b
    void disconnect(Node a, Node b); // delete the edge between a & b
};

Graph::insertNode(int value){
    Node node = Node(value);
    list<int> List;
    graph.push_back(make_pair(node, List));
}

Graph::connect(Node a, Node b){
    //find(graph.begin(), graph.end(), a)
}

int main(){
    Node a = Node(1);
    Node b = Node(2);
    int c = (a == b);
    cout << c << endl;
    // m[a] = a.value;
    // m[b] = b.value;
    // cout << m[a] << endl;
    // cout << m[b] << endl;
}