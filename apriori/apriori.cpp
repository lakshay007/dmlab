#include <bits/stdc++.h>
using namespace std;

map<set<string>, int> getFrequentItemsets(vector<set<string>> &transactions, int min_support) {
    map<set<string>, int> freq_itemsets;
    map<set<string>, int> candidates;
    for (auto &transaction : transactions) {
        for (auto &item : transaction) {
            candidates[{item}]++;
        }
    }

   
    for (auto &c : candidates) {
        if (c.second >= min_support)
            freq_itemsets[c.first] = c.second;
    }

  
    int k = 2;
    while (true) {
        map<set<string>, int> new_candidates;
        vector<set<string>> prev_freq_sets;
        for (auto &it : freq_itemsets) {
            if ((int)it.first.size() == k - 1)
                prev_freq_sets.push_back(it.first);
        }

     
        for (int i = 0; i < (int)prev_freq_sets.size(); i++) {
            for (int j = i + 1; j < (int)prev_freq_sets.size(); j++) {
                set<string> combined;
                set_union(prev_freq_sets[i].begin(), prev_freq_sets[i].end(),
                          prev_freq_sets[j].begin(), prev_freq_sets[j].end(),
                          inserter(combined, combined.begin()));
                if ((int)combined.size() == k) {
                  
                    int count = 0;
                    for (auto &transaction : transactions) {
                        if (includes(transaction.begin(), transaction.end(), combined.begin(), combined.end()))
                            count++;
                    }
                    if (count >= min_support)
                        new_candidates[combined] = count;
                }
            }
        }

        if (new_candidates.empty())
            break;

        freq_itemsets.insert(new_candidates.begin(), new_candidates.end());
        k++;
    }

    return freq_itemsets;
}

void generateAssociationRules(map<set<string>, int> &freq_itemsets, vector<set<string>> &transactions, double min_confidence) {
    cout << "\nAssociation Rules (Confidence >= " << min_confidence * 100 << "%):\n";
    for (auto &itemset : freq_itemsets) {
        if (itemset.first.size() < 2) continue;
        vector<string> items(itemset.first.begin(), itemset.first.end());
        int itemset_count = itemset.second;

        int n = (int)items.size();
        for (int i = 1; i < (1 << n); i++) {
            set<string> left, right;
            for (int j = 0; j < n; j++) {
                if (i & (1 << j)) left.insert(items[j]);
                else right.insert(items[j]);
            }
            if (left.empty() || right.empty()) continue;


            int left_count = freq_itemsets[left];
            double confidence = (double)itemset_count / left_count;
            if (confidence >= min_confidence) {
                cout << "{ ";
                for (auto &l : left) cout << l << " ";
                cout << "} => { ";
                for (auto &r : right) cout << r << " ";
                cout << "} (Confidence: " << confidence * 100 << "%)\n";
            }
        }
    }
}

int main() {

    vector<set<string>> transactions = {
        {"milk", "bread", "butter"},
        {"beer", "bread"},
        {"milk", "bread", "butter", "beer"},
        {"milk", "bread"},
        {"butter", "beer"}
    };

    int min_support;
    double min_confidence;

    cout << "Enter minimum support (as count): ";
    cin >> min_support;
    cout << "Enter minimum confidence (as decimal, e.g., 0.6 for 60%): ";
    cin >> min_confidence;

    map<set<string>, int> freq_itemsets = getFrequentItemsets(transactions, min_support);

    cout << "\nFrequent Itemsets:\n";
    for (auto &itemset : freq_itemsets) {
        cout << "{ ";
        for (auto &item : itemset.first)
            cout << item << " ";
        cout << "} - Support: " << itemset.second << endl;
    }

    generateAssociationRules(freq_itemsets, transactions, min_confidence);

    return 0;
}
