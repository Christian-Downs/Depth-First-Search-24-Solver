
/*
    Christian Downs
    This code takes about half a second to run.

    What it does is take in 4 numbers and creates
    expression trees for all combinations of these 4 numbers
    it also incorperates ( and ) by having them layered in the expression tree


*/

#include <string>
#include <stdlib.h>
#include <vector>
#include <cmath>
#include <initializer_list>
#include <iostream>
#include <string>

using namespace std;

class Node // used to make and hold expression tree
{
public:
    string item;
    Node *right;
    Node *left;

    Node() {};

    Node(string item)
    {
        this->item = item;
        this->right = nullptr;
        this->left = nullptr;
    };
    Node(string item, Node *right, Node *left)
    {
        this->item = item;
        this->right = right;
        this->left = left;
    };

    void addToRight(Node &node, string item) // recursively adds items to the bottom right
    {
        if (node.right == NULL)
        {
            node.right = new Node(item);
            return;
        }

        addToRight(*node.right, item);
    }

    int height(Node *node) // get's the height of the tree
    {
        if (node == NULL)
        {
            return 0;
        }
        return max(height(node->right) + 1, height(node->left) + 1);
    }

    static void applyOp(stack<Node *> &nodes, stack<char> &ops) //
    {
        char op = ops.top();
        ops.pop();
        Node *right = nodes.top();
        nodes.pop();
        Node *left = nodes.top();
        nodes.pop();
        string opStr(1, op);
        nodes.push(new Node(opStr, right, left));
    }

    static int precedence(char op)
    {
        if (op == '+' || op == '-')
            return 1;
        if (op == '*' || op == '/')
            return 2;
        return 0;
    }

    friend ostream &operator<<(ostream &os, const Node &node)
    {
        printTree(&node, os, 0);
        return os;
    }

    static void printTree(const Node *node, ostream &os, int depth)
    {
        if (!node)
            return;

        printTree(node->right, os, depth + 1);

        os << string(depth * 4, ' ') << node->item << endl;

        printTree(node->left, os, depth + 1);
    }

    double evaluateTree(const Node *node)
    {
        if (!node->left && !node->right)
        {
            return stod(node->item); // It's a number (leaf)
        }

        double leftVal = evaluateTree(node->left);
        double rightVal = evaluateTree(node->right);

        if (node->item == "+")
            return leftVal + rightVal;
        if (node->item == "-")
            return leftVal - rightVal;
        if (node->item == "*")
            return leftVal * rightVal;
        if (node->item == "/")
            return rightVal != 0 ? leftVal / rightVal : NAN;

        throw runtime_error("Unknown operator: " + node->item);
    }

    static string toInfix(const Node *node, bool isRoot = true)
    {
        if (!node)
            return "";

        // Leaf: just the value
        if (!node->left && !node->right)
        {
            return node->item;
        }

        // Otherwise, parenthesize the subtrees
        string L = toInfix(node->left, false);
        string R = toInfix(node->right, false);
        string expr = "(" + L + node->item + R + ")";

        // If this is the very top call, strip the outer ()
        if (isRoot && expr.size() >= 2)
        {
            return expr.substr(1, expr.size() - 2);
        }
        return expr;
    }
};

class Solution
{
public:
    // vector<vector<string>> solutions = {};
    vector<char> operations = {'+', '-', '*', '/'};

    void solve24All(vector<Node *> nodes, vector<string> &solutions)
    {

        int size = nodes.size();

        if (size == 1)
        { // base case
            double value = nodes[0]->evaluateTree(nodes[0]);
            if (fabs(value - 24) <= 1e-6)
            {
                solutions.push_back(Node::toInfix(nodes[0])); // adds the tree to the solution pile
            }
            return;
        }

        for (int i = 0; i < size; i++)
        {
            for (int j = 0; j < size; j++)
            {
                if (i == j)
                    continue;
                Node *A = nodes[i];
                Node *B = nodes[j];
                double va = A->evaluateTree(A); // get's current value of tree A
                double vb = B->evaluateTree(B); // get's current value of tree B

                // build the rest vector without A,B
                vector<Node *> rest; // gets the rest of the nodes that aren't A and B
                rest.reserve(size - 1);
                for (int k = 0; k < size; ++k)
                    if (k != i && k != j)
                        rest.push_back(nodes[k]);

                // A + B
                {
                    Node *p = new Node("+", A, B);
                    rest.push_back(p);
                    solve24All(rest, solutions);
                    rest.pop_back();
                    delete p;
                }
                // A * B
                {
                    Node *p = new Node("*", A, B);
                    rest.push_back(p);
                    solve24All(rest, solutions);
                    rest.pop_back();
                    delete p;
                }
                {
                    // A - B
                    Node *p1 = new Node("-", A, B);
                    rest.push_back(p1);
                    solve24All(rest, solutions);
                    rest.pop_back();
                    delete p1;
                }
                if (fabs(vb) > 1e-6)
                {
                    // A / B
                    Node *p2 = new Node("/", A, B);
                    rest.push_back(p2);
                    solve24All(rest, solutions);
                    rest.pop_back();
                    delete p2;
                }
            }
        }
    }
};

int main()
{
    Solution solution;

    vector<double> inputs(4);
    cout << "Enter 4 numbers with spaces: ";
    for (int i = 0; i < 4; ++i)
    {
        cin >> inputs[i];
    }
    vector<Node *> leaves;
    for (double v : inputs)
        leaves.push_back(new Node(to_string((int)v)));

    vector<string> solutions;
    solution.solve24All(leaves, solutions);

    sort(solutions.begin(), solutions.end());
    solutions.erase(unique(solutions.begin(), solutions.end()), solutions.end());

    if (solutions.size() > 0)
        for (auto &expr : solutions)
            cout << expr << " = 24\n";
    else
        cout << "No solutions!\n";
}