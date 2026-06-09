#include <cmath>
#include <iostream>
#include <fstream>
#include "SimpleGraph.h"

using namespace std;

const double kPi = 3.14159265358979323;
void Welcome();
SimpleGraph initGraph();
void printSimpleGraph(const SimpleGraph&);
std::ostream& operator<<(std::ostream&, const Node&);
std::ostream& operator<<(std::ostream&, const Edge&);

// Main method
int main() {
    Welcome();
    /* TODO: your implementation here */

    SimpleGraph g = initGraph();
    printSimpleGraph(g);

    return 0;
}

void printSimpleGraph(const SimpleGraph& g) {
    cout << "Nodes:\n";
    for (const auto& n : g.nodes) {
        cout << "\t" << n << "\n";
    }

    cout << "Edges:\n";
    for (const auto& e : g.edges) {
        cout << "\t" << e << "\n";
    }

    cout << endl;
}

SimpleGraph initGraph() {
    ifstream in("bull");
    if (!in) {
        throw std::runtime_error("Faild to open file: bull");
    }
    SimpleGraph g;
    size_t n;
    in >> n;
    g.nodes.reserve(n);
    for (size_t i = 0; i < n; ++i) {
        g.nodes.push_back(Node{
            .x = cos(2*kPi*i/n),
            .y = sin(2*kPi*i/n)
        });
    }
    size_t a, b;
    while (in >> a >> b) {
        if (a >= n || b >= n) {
            throw std::runtime_error("Invalid edge");
        }
        g.edges.push_back(Edge{a, b});
        g.edges.push_back(Edge{b, a});
    }

    return g;
}

std::ostream& operator<<(std::ostream& os, const Node& n) {
    return os << "Node(x=" << n.x << ",y=" << n.y << ")";
}

std::ostream& operator<<(std::ostream& os, const Edge& e) {
    return os << "Edge(start=" << e.start << ",end=" << e.end << ")";
}

/* Prints a message to the console welcoming the user and
 * describing the program. */
void Welcome() {
    cout << "Welcome to CS106L GraphViz!" << endl;
    cout << "This program uses a force-directed graph layout algorithm" << endl;
    cout << "to render sleek, snazzy pictures of various graphs." << endl;
    cout << endl;
}


