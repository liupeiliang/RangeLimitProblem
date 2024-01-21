// non-recursive segment tree
#include <bits/stdc++.h>
using namespace std;

struct ZkwSgtTree {

    int N;
    vector<int> tree;

    void init(int n) {
        N = 1 << (__lg(n + 5) + 1);
        tree.resize(N << 1);
        std::fill(tree.begin(), tree.end(), 0);
    }

    void update(int l, int r, int d) {
        int A;
        for (l += N - 1, r += N + 1; l ^ r ^ 1; l >>= 1, r >>= 1) {
            if (~l & 1) tree[l ^ 1] += d;
            if (r & 1) tree[r ^ 1] += d;
            A = max(tree[l], tree[l ^ 1]);
            tree[l] -= A, tree[l ^ 1] -= A, tree[l >> 1] += A;
            A = max(tree[r], tree[r ^ 1]);
            tree[r] -= A, tree[r ^ 1] -= A, tree[r >> 1] += A;
        }
        for (; l != 1; l >>= 1) {
            A = max(tree[l], tree[l ^ 1]);
            tree[l] -= A, tree[l ^ 1] -= A, tree[l >> 1] += A;
        }
    }

    int query(int l, int r) {
        int maxl = 0, maxr = 0;
        l += N, r += N;
        if (l != r) {
            for (; l ^ r ^ 1; l >>= 1, r >>= 1) {
                maxl += tree[l], maxr += tree[r];
                if (~l & 1) maxl = max(maxl, tree[l ^ 1]);
                if (r & 1) maxr = max(maxr, tree[r ^ 1]);
            }
        }
        int ans = max(maxl + tree[l], maxr + tree[r]);
        while (l > 1) {
            ans += tree[l>>=1];
        }
        return ans;
    };

};

int main() {

    return 0;
}