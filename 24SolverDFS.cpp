#include <string>
#include <stdlib.h>
#include <vector>
#include <cmath>
#include <initializer_list>
#include <iostream>
#include <string>

using namespace std;

class Node
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

    void addToRight(Node &node, string item)
    {
        if (node.right == NULL)
        {
            node.right = new Node(item);
            return;
        }

        addToRight(*node.right, item);
    }

    int height(Node *node)
    {
        if (node == NULL)
        {
            return 0;
        }
        return max(height(node->right) + 1, height(node->left) + 1);
    }

    void balance(Node *&node)
    {
        int rightHeight = height(node->right);
        int leftHeight = height(node->left);
        if (abs(leftHeight - rightHeight) <= 1)
        {
            return;
        }
        if (leftHeight > rightHeight + 1)
        {
            // Left is too heavy
            rotateRight(node);
        }
        else if (rightHeight > leftHeight + 1)
        {
            // Right is too heavy
            rotateLeft(node);
        }
    }

    void rotateLeft(Node *&root)
    {
        Node *newRoot = root->right;
        root->right = newRoot->left;
        newRoot->left = root;
        root = newRoot;
    }

    void rotateRight(Node *&root)
    {
        Node *newRoot = root->left;
        root->left = newRoot->right;
        newRoot->right = root;
        root = newRoot;
    }

    static void applyOp(stack<Node *> &nodes, stack<char> &ops)
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

    static Node *buildTree(string exp)
    {
        stack<Node *> nodes;
        stack<char> ops;

        for (int i = 0; i < exp.size(); i++)
        {
            char c = exp[i];
            if (isspace(c))
                continue; // ignore spaces;
            if (isdigit(c))
            {
                string num;
                while (i < exp.size() && isdigit(exp[i]))
                {
                    num += exp[i++]; // add all the numbers to the num string
                }
                i--; // so the for loop will continue correctly
                     // 6598
                nodes.push(new Node(num));
            }
            else if (c == '(')
            {
                ops.push(c);
            }
            else if (c == ')')
            {
                while (!ops.empty() && ops.top() != '(')
                {
                    applyOp(nodes, ops);
                }
                if (!ops.empty())
                    ops.pop(); // Remove the last (
            }
            else if (c == '+' || c == '-' || c == '*' || c == '/')
            {
                while (!ops.empty() && ops.top() != '(' && precedence(ops.top()) >= precedence(c))
                { // check if it's the right layer
                    applyOp(nodes, ops);
                }
                ops.push(c);
            }
        }

        while (!ops.empty())
        {
            applyOp(nodes, ops);
        }

        return nodes.top();
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

    static string toInfix(const Node* node, bool isRoot = true) {
        if (!node) return "";

        // Leaf: just the value
        if (!node->left && !node->right) {
            return node->item;
        }
    
        // Otherwise, parenthesize the subtrees
        string L = toInfix(node->left,  false);
        string R = toInfix(node->right, false);
        string expr = "(" + L + node->item + R + ")";
    
        // If this is the very top call, strip the outer ()
        if (isRoot && expr.size() >= 2) {
            return expr.substr(1, expr.size() - 2);
        }
        return expr;
    }
};

class Solution
{
public:
    vector<vector<string>> solutions = {};
    vector<char> operations = {'+', '-', '*', '/'};

    void solve24All(vector<Node *> nodes, vector<string> &out)
    {

        int size = nodes.size();

        if (size == 1)
        { // base case
            double value = nodes[0]->evaluateTree(nodes[0]);
            if (fabs(value - 24) <= 1e-6)
            {
                out.push_back(Node::toInfix(nodes[0]));
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
                double va = A->evaluateTree(A);
                double vb = B->evaluateTree(B);

                // build the rest vector without A,B
                vector<Node *> rest;
                rest.reserve(size - 1);
                for (int k = 0; k < size; ++k)
                    if (k != i && k != j)
                        rest.push_back(nodes[k]);

                // 1) COMMUTATIVE OPS: only do them when i<j
                if (i < j)
                {
                    // A + B
                    {
                        Node *p = new Node("+", A, B);
                        rest.push_back(p);
                        solve24All(rest, out);
                        rest.pop_back();
                        delete p;
                    }
                    // A * B
                    {
                        Node *p = new Node("*", A, B);
                        rest.push_back(p);
                        solve24All(rest, out);
                        rest.pop_back();
                        delete p;
                    }
                }

                // 2) NON-COMMUTATIVE OPS: do A–B and A/B for every (i,j)
                // — the reversed pair (j,i) will cover B–A and B/A
                {
                    // A - B
                    Node *p1 = new Node("-", A, B);
                    rest.push_back(p1);
                    solve24All(rest, out);
                    rest.pop_back();
                    delete p1;
                }
                if (fabs(vb) > 1e-6)
                {
                    // A / B
                    Node *p2 = new Node("/", A, B);
                    rest.push_back(p2);
                    solve24All(rest, out);
                    rest.pop_back();
                    delete p2;
                }
            }
        }
    }

    double evaluate(vector<string> equation)
    {

        double value = 0.0;

        Node root = Node();
        bool rootBool = true;

        string prev = "";

        for (string item : equation)
        {
            if (item.empty())
                continue;
            if (isspace(item[0]))
                continue;
            if (isdigit(item[0]))
            {                            // number
                double num = stod(item); // pulls number as double
                if (rootBool)
                {
                    root.item = num;
                    rootBool = false;
                    continue;
                }

                root.addToRight(root, item);
            }
            else
            { // not num
                if (rootBool)
                {
                    root.item = item;
                    rootBool = false;
                    continue;
                }
                root.addToRight(root, item);
            }
        }
        return 5;
    }

    bool solve(vector<double> v, vector<string> current)
    {

        return true;
    }
};

int main()
{
    Solution solution;

    vector<double> inputs(4);
    cout << "Enter 4 numbers: ";
    for (int i = 0; i < 4; ++i) {
        cin >> inputs[i];
    }
    vector<Node*> leaves;
    for (double v : inputs)
      leaves.push_back(new Node(to_string((int)v)));
  
    vector<string> solutions;
    solution.solve24All(leaves, solutions);
  
    sort(solutions.begin(), solutions.end());
    solutions.erase(unique(solutions.begin(), solutions.end()), solutions.end());
  
    for (auto &expr : solutions)
      cout << expr << " = 24\n";
  
}