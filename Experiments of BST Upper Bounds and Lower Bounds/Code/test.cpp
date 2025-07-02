#include <iostream>
#include <vector>
#include <chrono>
#include <fstream>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>

using namespace std;

struct BSTMetrics
{
    int total_comparisons = 0;
    int total_rotations = 0;
    double total_time = 0.0;
};

struct hash_pair
{
    size_t operator()(const pair<int, int> &p) const
    {

        return hash<int>()(p.first) ^ hash<int>()(p.second);
    }
};

class BST
{
public:
    BSTMetrics metrics;
    virtual void insert(int key) = 0;
    virtual bool search(int key) = 0;
    virtual void remove(int key) = 0;
    virtual ~BST() {}
};

class SplayTree : public BST
{
private:
    struct Node
    {
        int key;
        Node *left;
        Node *right;
        Node(int k) : key(k), left(nullptr), right(nullptr) {}
    };

    Node *root;

    Node *rotateRight(Node *x)
    {
        Node *y = x->left;
        x->left = y->right;
        y->right = x;
        metrics.total_rotations++;
        return y;
    }

    Node *rotateLeft(Node *x)
    {
        Node *y = x->right;
        x->right = y->left;
        y->left = x;
        metrics.total_rotations++;
        return y;
    }

    Node *splay(Node *node, int key)
    {
        if (!node || node->key == key)
        {
            return node;
        }

        metrics.total_comparisons++;

        if (key < node->key)
        {
            if (!node->left)
                return node;

            metrics.total_comparisons++;
            if (key < node->left->key)
            {

                node->left->left = splay(node->left->left, key);
                node = rotateRight(node);
            }
            else if (key > node->left->key)
            {

                node->left->right = splay(node->left->right, key);
                if (node->left->right)
                    node->left = rotateLeft(node->left);
            }
            return (node->left) ? rotateRight(node) : node;
        }
        else
        {
            if (!node->right)
                return node;

            metrics.total_comparisons++;
            if (key > node->right->key)
            {

                node->right->right = splay(node->right->right, key);
                node = rotateLeft(node);
            }
            else if (key < node->right->key)
            {

                node->right->left = splay(node->right->left, key);
                if (node->right->left)
                    node->right = rotateRight(node->right);
            }
            return (node->right) ? rotateLeft(node) : node;
        }
    }

public:
    SplayTree() : root(nullptr) {}

    void insert(int key) override
    {
        if (!root)
        {
            root = new Node(key);
            return;
        }
        root = splay(root, key);
        if (key == root->key)
            return;

        Node *newNode = new Node(key);
        if (key < root->key)
        {
            newNode->right = root;
            newNode->left = root->left;
            root->left = nullptr;
        }
        else
        {
            newNode->left = root;
            newNode->right = root->right;
            root->right = nullptr;
        }
        root = newNode;
    }

    bool search(int key) override
    {
        root = splay(root, key);
        return (root && root->key == key);
    }

    void remove(int key) override {}
};

class BasicBST : public BST
{
private:
    struct Node
    {
        int key;
        Node *left;
        Node *right;
        Node(int k) : key(k), left(nullptr), right(nullptr) {}
    };

    Node *root;

    Node *insert(Node *node, int key)
    {
        if (!node)
            return new Node(key);
        metrics.total_comparisons++;
        if (key < node->key)
            node->left = insert(node->left, key);
        else
            node->right = insert(node->right, key);
        return node;
    }

    bool search(Node *node, int key)
    {
        if (!node)
            return false;
        metrics.total_comparisons++;
        if (key == node->key)
            return true;
        if (key < node->key)
            return search(node->left, key);
        return search(node->right, key);
    }

public:
    BasicBST() : root(nullptr) {}

    void insert(int key) override
    {
        root = insert(root, key);
    }

    bool search(int key) override
    {
        return search(root, key);
    }

    void remove(int key) override {}
};

struct ReferenceNode
{
    int key;
    ReferenceNode *left;
    ReferenceNode *right;
    ReferenceNode *parent;
    bool isPreferred;

    ReferenceNode(int k) : key(k), left(nullptr), right(nullptr), parent(nullptr), isPreferred(false) {}
};

class ReferenceTree
{
public:
    ReferenceNode *root;
    BSTMetrics metrics;

    ReferenceTree() : root(nullptr) {}

    void insert(int key)
    {
        if (!root)
        {
            root = new ReferenceNode(key);
            return;
        }
        ReferenceNode *curr = root;
        ReferenceNode *prev = nullptr;
        while (curr)
        {
            prev = curr;
            if (key < curr->key)
                curr = curr->left;
            else
                curr = curr->right;
        }
        ReferenceNode *newNode = new ReferenceNode(key);
        if (key < prev->key)
            prev->left = newNode;
        else
            prev->right = newNode;
        newNode->parent = prev;
    }

    vector<ReferenceNode *> findPath(int key)
    {
        vector<ReferenceNode *> path;
        ReferenceNode *curr = root;
        while (curr)
        {
            metrics.total_comparisons++;
            path.push_back(curr);
            if (key == curr->key)
                break;
            curr = (key < curr->key) ? curr->left : curr->right;
        }
        return path;
    }

    void inorderTraversal(ReferenceNode *node)
    {
        if (!node)
            return;
        inorderTraversal(node->left);
        cout << node->key << " ";
        inorderTraversal(node->right);
    }

    void printInorder()
    {
        inorderTraversal(root);
        cout << endl;
    }
};

class TangoTree : public BST
{
private:
    ReferenceTree refTree;

public:
    void insert(int key) override
    {
        refTree.insert(key);
    }

    bool search(int key) override
    {

        refTree.metrics = BSTMetrics();
        auto path = refTree.findPath(key);
        if (path.empty())
            return false;

        ReferenceNode *preferredRoot = nullptr;
        for (ReferenceNode *node : path)
        {
            if (node->isPreferred)
            {
                preferredRoot = node;
                break;
            }
        }

        if (!preferredRoot)
            preferredRoot = path.front();

        bool needRebuild = false;
        for (ReferenceNode *node : path)
        {
            if (!node->isPreferred)
            {
                node->isPreferred = true;
                needRebuild = true;
            }
        }
        if (needRebuild)
        {
            refTree.metrics.total_rotations++;
        }

        metrics.total_comparisons += refTree.metrics.total_comparisons;
        metrics.total_rotations += refTree.metrics.total_rotations;

        return !path.empty() && (path.back()->key == key);
    }

    void remove(int key) override {}
};

class WilberLowerBound
{
private:
    ReferenceTree *tree;
    unordered_set<pair<int, int>, hash_pair> last_path_edges;

public:
    WilberLowerBound(ReferenceTree *ref) : tree(ref) {}

    void extractEdges(ReferenceNode *node, unordered_set<pair<int, int>, hash_pair> &edges)
    {
        while (node->parent)
        {
            int u = node->parent->key;
            int v = node->key;
            edges.insert({min(u, v), max(u, v)});
            node = node->parent;
        }
    }

    int computeTurnings(int key)
    {
        unordered_set<pair<int, int>, hash_pair> current_edges;
        ReferenceNode *node = tree->root;

        while (node && node->key != key)
        {
            if (key < node->key)
                node = node->left;
            else
                node = node->right;
        }
        if (!node)
            return 0;

        extractEdges(node, current_edges);

        int turning_points = 0;
        for (const auto &e : current_edges)
        {
            if (last_path_edges.find(e) == last_path_edges.end())
            {
                turning_points++;
            }
        }

        last_path_edges = std::move(current_edges);
        return turning_points;
    }
};

vector<int> loadSequenceFromFile(const string &filename)
{
    vector<int> sequence;
    ifstream file(filename);
    int key;
    while (file >> key)
    {
        sequence.push_back(key);
    }
    return sequence;
}

void runExperiment(BST *tree, const vector<int> &insert_sequence, const vector<int> &access_sequence)
{
    tree->metrics = BSTMetrics();

    for (int key : insert_sequence)
        tree->insert(key);

    auto start = chrono::high_resolution_clock::now();
    for (int key : access_sequence)
        tree->search(key);
    auto end = chrono::high_resolution_clock::now();

    tree->metrics.total_time = chrono::duration_cast<chrono::milliseconds>(end - start).count();

    cout << "Comparisons: " << tree->metrics.total_comparisons << endl;
    cout << "Rotations: " << tree->metrics.total_rotations << endl;
    cout << "Execution Time: " << tree->metrics.total_time << " ms" << endl;
}

void saveResultsToCSV(const string &filename, const string &algorithm_name, const BSTMetrics &result)
{
    ofstream file(filename);
    file << "Algorithm,Comparisons,Rotations,ExecutionTime\n";
    file << algorithm_name << "," << result.total_comparisons << "," << result.total_rotations << "," << result.total_time << "\n";
}

int main()
{

    vector<int> insert_sequence = loadSequenceFromFile("insert.txt");
    vector<int> access_sequence = loadSequenceFromFile("sequence.txt");

    struct Experiment
    {
        string name;
        BST *tree;
    };

    vector<Experiment> experiments = {
        {"BasicBST", new BasicBST()},
        {"SplayTree", new SplayTree()},
        {"TangoTree", new TangoTree()}};

    cout << "==================== BST Upper Bounds ====================\n";
    for (auto &exp : experiments)
    {
        cout << "\n[Run] " << exp.name << "\n";
        runExperiment(exp.tree, insert_sequence, access_sequence);
        string filename = "results_" + exp.name + ".csv";
        saveResultsToCSV(filename, exp.name, exp.tree->metrics);
    }

    ReferenceTree ref_tree;
    for (int key : insert_sequence)
        ref_tree.insert(key);

    WilberLowerBound wilber(&ref_tree);
    int total_wilber1 = 0;
    for (int key : access_sequence)
    {
        total_wilber1 += wilber.computeTurnings(key);
    }

    cout << "\n==================== Wilber I Lower Bound ====================\n";
    cout << "Estimated Wilber I Lower Bound: " << total_wilber1 << "\n";

    ofstream wilber_file("results_wilber.csv");
    wilber_file << "LowerBoundType,Value\n";
    wilber_file << "Wilber1," << total_wilber1 << "\n";
    wilber_file.close();

    cout << "\n==================== Summary Report ====================\n";
    printf("%-12s | %12s | %10s | %10s | %10s | %10s\n",
           "Algorithm", "Comparisons", "Rotations", "Time(ms)", "C/Wilber", "R/C (%)");
    cout << string(70, '-') << "\n";

    for (const auto &exp : experiments)
    {
        const auto &m = exp.tree->metrics;
        double ratio_c = (total_wilber1 > 0) ? static_cast<double>(m.total_comparisons) / total_wilber1 : 0.0;
        double ratio_r = (m.total_comparisons > 0) ? static_cast<double>(m.total_rotations) * 100.0 / m.total_comparisons : 0.0;

        printf("%-12s | %12d | %10d | %10.2f | %10.2f | %9.2f%%\n",
               exp.name.c_str(),
               m.total_comparisons,
               m.total_rotations,
               m.total_time,
               ratio_c,
               ratio_r);
    }

    for (auto &exp : experiments)
    {
        delete exp.tree;
    }

    cout << "\nAll results saved to CSV.\n";
    return 0;
}
