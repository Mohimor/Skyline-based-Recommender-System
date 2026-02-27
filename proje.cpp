#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <unordered_map>
#include <limits>
using namespace std;

//=================== Point ===================
struct Point {
    vector<int> values;
    int id = -1;

    bool dominate(const Point &other) const {
        const int D = (int)values.size();
        bool strictlyBetter = false;
        for (int i = 0; i < D; ++i) {
            if (values[i] > other.values[i]) return false;     // بزرگتر = بدتر
            if (values[i] < other.values[i]) strictlyBetter = true;
        }
        return strictlyBetter;
    }
};

//=================== Node ====================
struct Node {
    Node* parent = nullptr;
    vector<Node*> children;
    Point point;

    // برای Lazy & Balancing (Algorithm 3)
    int depth = 0;          // عمق از ریشه
    size_t subsize = 1;     // اندازهٔ زیر درخت = خودش + همهٔ نوادگان

    void addChild(Node* node) {
        children.push_back(node);
        node->parent = this;
        node->depth  = this->depth + 1;

        // subsize اجداد را افزایش بده
        Node* p = node->parent;
        size_t delta = node->subsize;
        while (p) {
            p->subsize += delta;
            p = p->parent;
        }
    }

    void removeChild(Node *child) {
        for (size_t i = 0; i < children.size(); ++i) {
            if (children[i] == child) {  // مقایسه با اشاره‌گر
                // subsize اجداد را کاهش بده
                Node* p = this;
                size_t delta = child->subsize;
                while (p) {
                    p->subsize -= delta;
                    p = p->parent;
                }
                children.erase(children.begin() + (long long)i);
                child->parent = nullptr;
                return;
            }
        }
    }
};

//=================== BJRTree =================
struct BJRTree {
    Node* root = new Node();
    unordered_map<int, Node*> nodesById;   // id -> Node*
    int lazyDepth = 3;                     // آستانهٔ عمق برای لیزی

    BJRTree() {
        root->depth = 0;
        root->subsize = 1;
    }

    // ---------- ابزارهای کمکی ----------
    void reparent(Node* newParent, Node* child) {
        if (child->parent) child->parent->removeChild(child);
        newParent->addChild(child);
    }

    Node* bestDominatingChild(Node* parent, const Point& p) {
        Node* best = nullptr;
        size_t bestDesc = numeric_limits<size_t>::max();
        for (Node* c : parent->children) {
            if (c->point.dominate(p)) {
                if (c->subsize < bestDesc) {
                    bestDesc = c->subsize;
                    best = c;
                }
            }
        }
        return best;
    }

    // ---------- تزریق عادی (Algorithm 1) ----------
    void inject(Node* current, Node* newNode) {
        // اگر یکی از بچه‌ها newNode را dominate کند، به همان بچه رکورس بزن
        for (Node* child : current->children) {
            if (child->point.dominate(newNode->point)) {
                inject(child, newNode);
                return;
            }
        }

        // بچه‌های مغلوب current را جمع کن
        vector<Node*> toReparent;
        for (Node* child : current->children) {
            if (newNode->point.dominate(child->point)) {
                toReparent.push_back(child);
            }
        }

        // ابتدا newNode را به current وصل کن
        current->addChild(newNode);

        // سپس جابجایی مغلوب‌ها زیر newNode
        for (Node* child : toReparent) {
            reparent(newNode, child);
        }
    }

    // ---------- تزریق تنبل (Algorithm 3) ----------
    void injectLazy(Node* current, Node* newNode, int curDepth = 0) {
        // تا عمق d: بهترین بچهٔ غالب با کمترین subsize را انتخاب کن
        if (current == root || curDepth < lazyDepth) {
            if (Node* t = bestDominatingChild(current, newNode->point)) {
                injectLazy(t, newNode, curDepth + 1);
                return;
            }
        }

        // در غیر این‌صورت: چسباندن تنبل زیر current
        current->addChild(newNode);

        // فقط در ریشه یا عمق‌های کم، بچه‌های مغلوب current را زیر newNode منتقل کن
        if (current == root || curDepth < lazyDepth) {
            vector<Node*> toMove;
            for (Node* c : current->children) {
                if (c != newNode && newNode->point.dominate(c->point)) {
                    toMove.push_back(c);
                }
            }
            for (Node* c : toMove) {
                reparent(newNode, c);
            }
        }
    }

    // ---------- Ejection (Algorithm 2) ----------
    // اگر useLazy=true باشد، کودکان با injectLazy دوباره تزریق می‌شوند
    void eject(Node* node, bool useLazy) {
        Node* parent = node->parent;
        if (!parent) return;

        parent->removeChild(node);
        // بچه‌ها را دوباره وارد کن
        vector<Node*> kids = node->children;  // کپی لیست
        for (Node* child : kids) {
            // detach صِرف: removeChild در reparent انجام می‌شود
            if (useLazy) injectLazy(parent, child, parent->depth);
            else         inject(parent, child);
        }
        node->children.clear();
        nodesById.erase(node->point.id);
        delete node;
    }

    // ---------- آی‌دی‌های اسکای‌لاین ----------
    vector<int> getSkylineIds() const {
        vector<int> ids;
        ids.reserve(root->children.size());
        for (Node* n : root->children) ids.push_back(n->point.id);
        sort(ids.begin(), ids.end());
        return ids;
    }
};

//=================== برنامهٔ اصلی =================
int main(int argc, char* argv[]) {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    if (argc < 2) {
        cerr << "Usage: ./skyline <prefix>\n";
        return 1;
    }
    string prefix = argv[1];

    // --- خواندن setup ---
    ifstream setupFile(prefix + ".setup");
    if (!setupFile) { cerr << "Cannot open " << prefix << ".setup\n"; return 1; }
    int nPoints, dim, timeSteps;
    string size, type;
    setupFile >> nPoints >> dim >> timeSteps >> size >> type;
    setupFile.close();

    // --- خواندن input (مختصات) ---
    vector<Point> allPoints(nPoints);
    ifstream inputFile(prefix + ".input");
    if (!inputFile) { cerr << "Cannot open " << prefix << ".input\n"; return 1; }
    for (int i = 0; i < nPoints; ++i) {
        allPoints[i].id = i;
        allPoints[i].values.reserve(dim);
        for (int d = 0; d < dim; ++d) {
            int val; inputFile >> val;
            allPoints[i].values.push_back(val);
        }
    }
    inputFile.close();

    // --- خواندن بازه‌های زمانی ---
    vector<pair<int,int>> intervals(nPoints);
    ifstream timeFile(prefix + ".times");
    if (!timeFile) { cerr << "Cannot open " << prefix << ".times\n"; return 1; }
    for (int i = 0; i < nPoints; ++i) {
        int s, e; timeFile >> s >> e;
        intervals[i] = {s, e};
    }
    timeFile.close();

    // --- دو درخت: استاندارد و لیزی ---
    BJRTree treeStd;      // inject/eject معمولی
    BJRTree treeLazy;     // injectLazy/eject با useLazy=true
    treeLazy.lazyDepth = 3; // در صورت نیاز تغییر بده

    ofstream outStd(prefix + ".out");
    ofstream outLazy(prefix + ".lazyout");
    if (!outStd || !outLazy) {
        cerr << "Cannot open output files\n";
        return 1;
    }

    // --- پردازش زمان ---
    for (int t = 0; t < timeSteps; ++t) {
        // حذف نقاط منقضی‌شده: استاندارد
        {
            vector<int> toRemove;
            toRemove.reserve(treeStd.nodesById.size());
            for (auto &kv : treeStd.nodesById) {
                int id = kv.first;
                if (intervals[id].second <= t) toRemove.push_back(id);
            }
            for (int id : toRemove) {
                auto it = treeStd.nodesById.find(id);
                if (it != treeStd.nodesById.end()) treeStd.eject(it->second, /*useLazy=*/false);
            }
        }
        // حذف نقاط منقضی‌شده: لیزی
        {
            vector<int> toRemove;
            toRemove.reserve(treeLazy.nodesById.size());
            for (auto &kv : treeLazy.nodesById) {
                int id = kv.first;
                if (intervals[id].second <= t) toRemove.push_back(id);
            }
            for (int id : toRemove) {
                auto it = treeLazy.nodesById.find(id);
                if (it != treeLazy.nodesById.end()) treeLazy.eject(it->second, /*useLazy=*/true);
            }
        }

        // افزودن نقاطِ فعال در بازه: هر دو حالت
        for (int i = 0; i < nPoints; ++i) {
            if (intervals[i].first <= t && t < intervals[i].second) {
                // استاندارد
                if (treeStd.nodesById.count(i) == 0) {
                    Node* ns = new Node();
                    ns->point = allPoints[i];
                    treeStd.nodesById[i] = ns;
                    treeStd.inject(treeStd.root, ns);
                }
                // لیزی
                if (treeLazy.nodesById.count(i) == 0) {
                    Node* nl = new Node();
                    nl->point = allPoints[i];
                    treeLazy.nodesById[i] = nl;
                    treeLazy.injectLazy(treeLazy.root, nl, 0);
                }
            }
        }

        // --- خروجی اسکای‌لاینِ هر دو ---
        auto skyStd  = treeStd.getSkylineIds();
        auto skyLazy = treeLazy.getSkylineIds();

        for (size_t i = 0; i < skyStd.size(); ++i) {
            if (i) outStd << ' ';
            outStd << skyStd[i];
        }
        outStd << '\n';

        for (size_t i = 0; i < skyLazy.size(); ++i) {
            if (i) outLazy << ' ';
            outLazy << skyLazy[i];
        }
        outLazy << '\n';
    }

    outStd.close();
    outLazy.close();

    // --- مقایسهٔ خروجی استاندارد با refout ---
    ifstream refFile(prefix + ".refout");
    ifstream outFileRead(prefix + ".out");

    if (!refFile) {
        cerr << "Cannot open " << prefix << ".refout\n";
        return 1;
    }
    if (!outFileRead) {
        cerr << "Cannot open " << prefix << ".out\n";
        return 1;
    }

    auto rtrim = [](string& s) {
        size_t pos = s.find_last_not_of(" \t\r\n");
        if (pos == string::npos) s.clear();
        else s.erase(pos + 1);
    };

    string refLine, outLine;
    int lineNum = 1;
    bool match = true;

    while (true) {
        bool r1 = static_cast<bool>(getline(refFile, refLine));
        bool r2 = static_cast<bool>(getline(outFileRead, outLine));

        if (!r1 || !r2) {
            if (r1 == r2) break;
            cout << "❌ Line count differs.\n";
            match = false;
            break;
        }

        rtrim(refLine);
        rtrim(outLine);

        if (refLine != outLine) {
            cout << "❌ Mismatch at line " << lineNum << ":\n";
            cout << "Expected: \"" << refLine << "\"\n";
            cout << "Got     : \"" << outLine << "\"\n";
            match = false;
            break;
        }
        ++lineNum;
    }

    if (match) cout << "✅ Output matches reference!\n";

    return 0;
}
