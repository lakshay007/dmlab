#include <bits/stdc++.h>
using namespace std;

int main() {
    srand(time(0));
    vector<int>p = {2, 4, 10, 12, 3, 20, 30, 11, 25};  
    vector<float> c = {(float)p[1], (float)p[p.size() - 2]}; 
    vector<int> l(p.size(), -1); 
    bool ch = true;
    int iteration = 0;
    while (ch) {
        cout << "Iteration " << ++iteration << ":" << endl;
        ch = false;
        vector<vector<int>>cl(2); 
        for (int i = 0; i < p.size(); ++i) {
            int d0 = abs(p[i] - c[0]), d1 = abs(p[i] - c[1]);
            int lbl = (d0 < d1) ? 0 : 1;
            if (lbl != l[i]) {
                l[i] = lbl;
                ch = true;
            }
            cl[lbl].push_back(p[i]);
        }
        for (int i = 0; i < 2; ++i) {
            int sum = 0;
            for (int x : cl[i]) sum += x;
            c[i] = (float)sum / (float)cl[i].size(); 
        }

        for (int i = 0; i < p.size(); ++i) {
            cout << "P " << p[i] << " -> C " << l[i] + 1 << endl;
        }
        cout << "Centroids: C1 = " << c[0] << ", C2 = " << c[1] << endl << endl;
    }
    return 0;
}
