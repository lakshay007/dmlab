#include <bits/stdc++.h>
using namespace std;

struct Node {
    string attribute;
    unordered_map<string, Node*> children;
    string label;  
};

vector<vector<string>> dataset = {
    {"Sunny", "Hot", "High", "Weak", "No"},
    {"Sunny", "Hot", "High", "Strong", "No"},
    {"Overcast", "Hot", "High", "Weak", "Yes"},
    {"Rain", "Mild", "High", "Weak", "Yes"},
    {"Rain", "Cool", "Normal", "Weak", "Yes"},
    {"Rain", "Cool", "Normal", "Strong", "No"},
    {"Overcast", "Cool", "Normal", "Strong", "Yes"},
    {"Sunny", "Mild", "High", "Weak", "No"},
    {"Sunny", "Cool", "Normal", "Weak", "Yes"},
    {"Rain", "Mild", "Normal", "Weak", "Yes"},
    {"Sunny", "Mild", "Normal", "Strong", "Yes"},
    {"Overcast", "Mild", "High", "Strong", "Yes"},
    {"Overcast", "Hot", "Normal", "Weak", "Yes"},
    {"Rain", "Mild", "High", "Strong", "No"}
};

vector<string> attributes = {"Outlook", "Temperature", "Humidity", "Wind"};

double entropy(vector<vector<string>> &data) {
    unordered_map<string, int> label_count;
    for (auto &row : data)
        label_count[row.back()]++;
    double ent = 0.0;
    double total = data.size();
    for (auto &pair : label_count) {
        double p = pair.second / total;
        ent -= p * log2(p);
    }
    return ent;
}


double information_gain(vector<vector<string>> &data, int attr_index) {
    double total_entropy = entropy(data);
    unordered_map<string, vector<vector<string>>> subsets;
    for (auto &row : data)
        subsets[row[attr_index]].push_back(row);

    double weighted_entropy = 0.0;
    for (auto &pair : subsets)
        weighted_entropy += (pair.second.size() / (double)data.size()) * entropy(pair.second);

    return total_entropy - weighted_entropy;
}


int best_attribute(vector<vector<string>> &data, vector<int> &available_attrs) {
    int best_attr = -1;
    double max_gain = -1;
    for (int attr : available_attrs) {
        double gain = information_gain(data, attr);
        if (gain > max_gain) {
            max_gain = gain;
            best_attr = attr;
        }
    }
    return best_attr;
}


Node* build_tree(vector<vector<string>> data, vector<int> available_attrs) {
    Node* node = new Node();
    unordered_map<string, int> label_count;
    for (auto &row : data)
        label_count[row.back()]++;

    if (label_count.size() == 1) {
        node->label = data[0].back();
        return node;
    }

    if (available_attrs.empty()) {
        string majority_label = max_element(label_count.begin(), label_count.end(),
                                            [](auto &a, auto &b) { return a.second < b.second; })->first;
        node->label = majority_label;
        return node;
    }

    int best_attr = best_attribute(data, available_attrs);
    if (best_attr == -1) {
        node->label = "Yes"; 
        return node;
    }

    node->attribute = attributes[best_attr];
    unordered_map<string, vector<vector<string>>> subsets;
    for (auto &row : data)
        subsets[row[best_attr]].push_back(row);

    vector<int> new_attrs;
    for (int attr : available_attrs)
        if (attr != best_attr)
            new_attrs.push_back(attr);

    for (auto &pair : subsets)
        node->children[pair.first] = build_tree(pair.second, new_attrs);

    return node;
}


void print_tree(Node* node, string indent = "") {
    if (!node->label.empty()) {
        cout << indent << "Leaf: " << node->label << "\n";
        return;
    }
    cout << indent << "Attribute: " << node->attribute << "\n";
    for (auto &child : node->children) {
        cout << indent << "  - " << child.first << " -> ";
        print_tree(child.second, indent + "    ");
    }
}


string classify(Node* root, vector<string> sample) {
    if (!root->label.empty()) {
        return root->label;
    }
    int attr_index = -1;
    for (int i = 0; i < attributes.size(); i++) {
        if (attributes[i] == root->attribute) {
            attr_index = i;
            break;
        }
    }
    if (root->children.find(sample[attr_index]) != root->children.end())
        return classify(root->children[sample[attr_index]], sample);
    else
        return "Unknown";
}


void print_rules(Node* node, string rule = "") {
    if (!node->label.empty()) {
        cout << rule << " => " << node->label << endl;
        return;
    }
    for (auto &child : node->children) {
        string new_rule = rule + (rule.empty() ? "" : " AND ") + node->attribute + " = " + child.first;
        print_rules(child.second, new_rule);
    }
}

int main() {
    vector<int> available_attrs = {0, 1, 2, 3};
    Node* root = build_tree(dataset, available_attrs);

    cout << "---- Decision Tree ----\n";
    print_tree(root);

    cout << "\n---- Decision Rules Derived ----\n";
    print_rules(root);


    cout << "\n---- Test Your Own Sample ----\n";
    while (true) {
        vector<string> user_sample;
        cout << "\nEnter the following:\n";
        for (auto &attr : attributes) {
            cout << attr << ": ";
            string val; cin >> val;
            user_sample.push_back(val);
        }
        cout << "Prediction => Play Ball? " << classify(root, user_sample) << endl;
        cout << "Try another? (y/n): ";
        char ch; cin >> ch;
        if (ch == 'n' || ch == 'N') break;
    }

    return 0;
}
