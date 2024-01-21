#include <bits/stdc++.h>
using namespace std;

#define HOUR_LIMIT 2

struct SegmentTree {

    struct Node {
        int mx, tag;
    };
    vector<Node> a;
    int size;

    void init(int num_of_elements) {
        size = num_of_elements + 1; // length
        a.resize(size * 4);
    }

    void pushdown(int x) {
        int v = a[x].tag;
        if (!v) return;
        a[x << 1].tag += v;
        a[x << 1].mx += v;
        a[x << 1 | 1].tag += v;
        a[x << 1 | 1].mx += v;
        a[x].tag = 0;
    }

    void update(int x, int l, int r, int ll, int rr) {
        if (l >= ll && r <= rr) { // node included
            a[x].tag++;
            a[x].mx++;
            return;
        }
        if (l < r) {
            pushdown(x);
        }
        int mid = (l + r) >> 1;
        if (ll <= mid) update(x << 1, l, mid, ll, rr);
        if (rr > mid) update(x << 1 | 1, mid + 1, r, ll, rr);
        a[x].mx = max(a[x << 1].mx, a[x << 1 | 1].mx);
    }

    int query(int x, int l, int r, int ll, int rr) {
        if (l >= ll && r <= rr) {
            return a[x].mx;
        }
        if (l < r) {
            pushdown(x);
        }
        int mid = (l + r) >> 1;
        int ret = 0;
        if (ll <= mid) ret = max(ret, query(x << 1, l, mid, ll, rr));
        if (rr > mid) ret = max(ret, query(x << 1 | 1, mid + 1, r, ll, rr));
        return ret;
    }

};

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


int get_hour(int ts) {
    return (ts % (3600 * 24)) / 3600;
}

void solve_by_sliding_window(int X, int Y, int num_of_buckets, vector<int>& vec_elements) {
    vector<int> bucket(num_of_buckets, -1);
    int n = vec_elements.size();
    int bkt_in_range = Y * 24 / HOUR_LIMIT;
    int sec_in_range = Y * 3600 * 24;
    int cnt_seleted = 0;
    int cnt_drop_hour = 0;
    int cnt_drop_range = 0;
    for (int i = 0; i < n; i++) {
        int ts = vec_elements[i];
        int bkt_id = ts / (3600 * HOUR_LIMIT);

        // hour window
        if (bucket[bkt_id] != -1) {
            cnt_drop_hour++;
            continue;
        }

        if (bkt_id > 0 && bucket[bkt_id - 1] != -1 && 
            ts - bucket[bkt_id - 1] <= 3600 * HOUR_LIMIT) {
            cnt_drop_hour++;
            continue;
        }

        if (bkt_id + 1 < num_of_buckets && bucket[bkt_id + 1] != -1 && 
            bucket[bkt_id + 1] - ts <= 3600 * HOUR_LIMIT) {
            cnt_drop_hour++;
            continue;
        }

        // sliding window for X in Y days
        int start = max(0, bkt_id - bkt_in_range + 1);
        int cnt = 0;
        bool flag = true;
        bucket[bkt_id] = ts;
        for (int j = start; j < bkt_id; j++) {
            if (bucket[j] != -1) cnt++;
        }
        for (int j = bkt_id; j < min(num_of_buckets, bkt_id + bkt_in_range); j++) {
            if (bucket[j] == -1) continue;
            cnt++;
            while (start < j && (bucket[start] == -1 || j - start >= bkt_in_range)) {
                cnt -= (bucket[start] != -1);
                start++;
            }
            if (cnt > X) {
                flag = false;
                break;
            }
        }
        if (!flag) {
            bucket[bkt_id] = -1;
            cnt_drop_range++;
        }
        else {
            cnt_seleted++;
        }
    }
    std::cout << "solve_by_sliding_window : cnt_seleted " << cnt_seleted 
        << " cnt_drop_hour " << cnt_drop_hour
        << " cnt_drop_range " << cnt_drop_range << endl;

}

void solve_by_seg_tree(int X, int Y, int num_of_buckets, vector<int>& vec_elements) {
    vector<int> bucket(num_of_buckets, -1);
    int n = vec_elements.size();
    int bkt_in_range = Y * 24 / HOUR_LIMIT;
    int sec_in_range = Y * 3600 * 24;
    int cnt_seleted = 0;
    int cnt_drop_hour = 0;
    int cnt_drop_range = 0;
    SegmentTree sgt;
    sgt.init(num_of_buckets - bkt_in_range + 1);

    for (int i = 0; i < n; i++) {
        int ts = vec_elements[i];
        int bkt_id = ts / (3600 * HOUR_LIMIT);

        // hour window
        if (bucket[bkt_id] != -1) {
            cnt_drop_hour++;
            continue;
        }

        if (bkt_id > 0 && bucket[bkt_id - 1] != -1 && 
            ts - bucket[bkt_id - 1] <= 3600 * HOUR_LIMIT) {
            cnt_drop_hour++;
            continue;
        }

        if (bkt_id + 1 < num_of_buckets && bucket[bkt_id + 1] != -1 && 
            bucket[bkt_id + 1] - ts <= 3600 * HOUR_LIMIT) {
            cnt_drop_hour++;
            continue;
        }

        // for X in Y days
        int mx = sgt.query(1, 1, sgt.size, max(0, bkt_id - bkt_in_range + 1) + 1, bkt_id + 1);
        if (mx >= X) {
            cnt_drop_range++;
        }
        else {
            cnt_seleted++;
            bucket[bkt_id] = ts;
            sgt.update(1, 1, sgt.size, max(0, bkt_id - bkt_in_range + 1) + 1, bkt_id + 1);
        }
    }

    std::cout << "solve_by_sgt_tree : cnt_seleted " << cnt_seleted 
        << " cnt_drop_hour " << cnt_drop_hour
        << " cnt_drop_range " << cnt_drop_range << endl;
}

void solve_by_zkw_seg_tree(int X, int Y, int num_of_buckets, vector<int>& vec_elements) {
    vector<int> bucket(num_of_buckets, -1);
    int n = vec_elements.size();
    int bkt_in_range = Y * 24 / HOUR_LIMIT;
    int sec_in_range = Y * 3600 * 24;
    int cnt_seleted = 0;
    int cnt_drop_hour = 0;
    int cnt_drop_range = 0;
    ZkwSgtTree sgt;
    sgt.init(num_of_buckets - bkt_in_range + 1);

    for (int i = 0; i < n; i++) {
        int ts = vec_elements[i];
        int bkt_id = ts / (3600 * HOUR_LIMIT);

        // hour window
        if (bucket[bkt_id] != -1) {
            cnt_drop_hour++;
            continue;
        }

        if (bkt_id > 0 && bucket[bkt_id - 1] != -1 && 
            ts - bucket[bkt_id - 1] <= 3600 * HOUR_LIMIT) {
            cnt_drop_hour++;
            continue;
        }

        if (bkt_id + 1 < num_of_buckets && bucket[bkt_id + 1] != -1 && 
            bucket[bkt_id + 1] - ts <= 3600 * HOUR_LIMIT) {
            cnt_drop_hour++;
            continue;
        }

        // for X in Y days
        int mx = sgt.query(max(0, bkt_id - bkt_in_range + 1) + 1, bkt_id + 1);
        if (mx >= X) {
            cnt_drop_range++;
        }
        else {
            cnt_seleted++;
            bucket[bkt_id] = ts;
            sgt.update(max(0, bkt_id - bkt_in_range + 1) + 1, bkt_id + 1, 1);
        }
    }

    std::cout << "solve_by_sgt_tree : cnt_seleted " << cnt_seleted 
        << " cnt_drop_hour " << cnt_drop_hour
        << " cnt_drop_range " << cnt_drop_range << endl;
}

int main() {
    int X, Y; // no more than X in Y days;
    int num_of_days;  // total days

    X = 4; Y = 4;
    num_of_days = 90;

    int num_of_buckets = num_of_days * 24 / HOUR_LIMIT;
    
    int n = 10000000; // number of elements
    std::cout << "number of elements : " << n << endl;
    vector<int> vec_elements;
    vec_elements.resize(n);
    std::mt19937 seed(std::random_device{}());
    std::uniform_int_distribution<int> rnd(0, 24 * 3600 * num_of_days);
    for (int i = 0; i < n; i++) {
        vec_elements[i] = rnd(seed);
    }

    clock_t time_start, time_end;

    time_start = clock();
    solve_by_sliding_window(X, Y, num_of_buckets, vec_elements);
    time_end = clock();
    std::cout<<"solve_by_sliding_window time = "<<double(time_end-time_start)/CLOCKS_PER_SEC<<"s"<<endl;

    std::cout << endl;

    time_start = clock();
    solve_by_seg_tree(X, Y, num_of_buckets, vec_elements);
    time_end = clock();
    std::cout<<"solve_by_seg_tree time = "<<double(time_end-time_start)/CLOCKS_PER_SEC<<"s"<<endl;

    std::cout << endl;

    time_start = clock();
    solve_by_zkw_seg_tree(X, Y, num_of_buckets, vec_elements);
    time_end = clock();
    std::cout<<"solve_by_skw_seg_tree time = "<<double(time_end-time_start)/CLOCKS_PER_SEC<<"s"<<endl;


    return 0;
}