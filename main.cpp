#include <bits/stdc++.h>
using namespace std;

template <typename T>
class DoublyLinkedList {
private:
    struct Node {
        T value;
        shared_ptr<Node> next;
        weak_ptr<Node> prev;
        explicit Node(const T& val) : value(val), next(nullptr) {}
    };

    shared_ptr<Node> head;
    weak_ptr<Node> tail;
    size_t length;

public:
    class iterator {
    private:
        shared_ptr<Node> current;

    public:
        explicit iterator(shared_ptr<Node> node) : current(std::move(node)) {}

        T& operator*() {
            return current->value;
        }

        iterator operator++(int) {
            iterator tmp = *this;
            if (current) current = current->next;
            return tmp;
        }

        bool operator==(const iterator& other) const {
            return current == other.current;
        }

        bool operator!=(const iterator& other) const {
            return !(*this == other);
        }
    };

    DoublyLinkedList() : head(nullptr), length(0) {}

    bool empty() const { return length == 0; }
    size_t size() const { return length; }

    void push_front(const T& value) {
        auto node = make_shared<Node>(value);
        node->next = head;
        if (head) {
            head->prev = node;
        } else {
            tail = node;
        }
        head = node;
        ++length;
    }

    void push_back(const T& value) {
        auto node = make_shared<Node>(value);
        auto t = tail.lock();
        if (!t) {
            head = node;
            tail = node;
        } else {
            t->next = node;
            node->prev = t;
            tail = node;
        }
        ++length;
    }

    optional<T> pop_front() {
        if (!head) return nullopt;
        auto old = head;
        auto res = old->value;
        head = old->next;
        if (head) {
            head->prev.reset();
        } else {
            tail.reset();
        }
        --length;
        return res;
    }

    optional<T> pop_back() {
        if (empty()) return nullopt;
        auto t = tail.lock();
        auto res = t->value;
        auto prev = t->prev.lock();
        if (prev) {
            prev->next.reset();
            tail = prev;
        } else {
            head.reset();
            tail.reset();
        }
        --length;
        return res;
    }

    iterator begin() { return iterator(head); }
    iterator end() { return iterator(nullptr); }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    // Command-based interface (robust to multiple aliases):
    // Supported commands (case-insensitive):
    // - PF x / PUSH_FRONT x
    // - PB x / PUSH_BACK x
    // - POF / POP_FRONT  -> prints value or NA
    // - POB / POP_BACK   -> prints value or NA
    // - SZ / SIZE        -> prints size
    // - IT / ITERATE     -> prints elements space-separated
    // If the first token is an integer N, exactly N commands follow.

    DoublyLinkedList<long long> dll;
    string cmd;
    long long firstInt;
    bool bounded = false;
    if (cin >> firstInt) {
        // Peek next char to see if line continues with non-command; if next token is non-integer word, treat firstInt as command name by rewinding
        // Simpler: if next char is newline or space, assume it's count; else treat as part of word
        // We'll set up a stream to re-insert if not bounded
        cin >> ws;
        if (cin.peek() == EOF) {
            // Single integer input; treat as count zero commands (noop)
            return 0;
        }
        // Assume it's bounded count; else we will treat it as part of command stream by pushing back into a stringstream
        // To distinguish, check if next is alpha char indicating a command; both cases acceptable.
        // We'll assume bounded if next is alpha.
        if (isalpha(cin.peek())) {
            bounded = true;
        } else {
            // Not clearly alpha, revert: put back by building a new stream that starts with the integer and the rest of cin
            // Easiest: create a string that begins with the integer and remaining stream and reparse.
            string rest;
            getline(cin, rest, '\0');
            string all = to_string(firstInt) + (rest.size() ? string(" ") + rest : string());
            cin.clear();
            cin.seekg(0, ios::beg); // may fail on stdin; we'll use stringstream instead
            stringstream ss(all);
            // Process from ss as unbounded
            auto process = [&](istream& in){
                string c; while (in >> c) {
                    string u=c; for (auto& ch:u) ch=toupper((unsigned char)ch);
                    if (u=="PF"||u=="PUSH_FRONT") { long long x; in>>x; dll.push_front(x);} \
                    else if (u=="PB"||u=="PUSH_BACK") { long long x; in>>x; dll.push_back(x);} \
                    else if (u=="POF"||u=="POP_FRONT") { auto v=dll.pop_front(); if(v) cout<<*v<<"\n"; else cout<<"NA\n";} \
                    else if (u=="POB"||u=="POP_BACK") { auto v=dll.pop_back(); if(v) cout<<*v<<"\n"; else cout<<"NA\n";} \
                    else if (u=="SZ"||u=="SIZE") { cout<<dll.size()<<"\n";} \
                    else if (u=="IT"||u=="ITERATE") { bool first=true; for(auto it=dll.begin(); it!=dll.end(); it++){ if(!first) cout<<' '; cout<<*it; first=false;} cout<<"\n";} \
                    else { /* ignore unknown */ }
                }
            };
            process(ss);
            return 0;
        }
    } else {
        cin.clear();
    }

    auto handle = [&](const string& c, istream& in){
        string u=c; for (auto& ch:u) ch=toupper((unsigned char)ch);
        if (u=="PF"||u=="PUSH_FRONT") { long long x; in>>x; dll.push_front(x); return true; }
        if (u=="PB"||u=="PUSH_BACK") { long long x; in>>x; dll.push_back(x); return true; }
        if (u=="POF"||u=="POP_FRONT") { auto v=dll.pop_front(); if(v) cout<<*v<<"\n"; else cout<<"NA\n"; return true; }
        if (u=="POB"||u=="POP_BACK") { auto v=dll.pop_back(); if(v) cout<<*v<<"\n"; else cout<<"NA\n"; return true; }
        if (u=="SZ"||u=="SIZE") { cout<<dll.size()<<"\n"; return true; }
        if (u=="IT"||u=="ITERATE") { bool first=true; for(auto it=dll.begin(); it!=dll.end(); it++){ if(!first) cout<<' '; cout<<*it; first=false;} cout<<"\n"; return true; }
        return false;
    };

    if (bounded) {
        long long n = firstInt;
        for (long long i = 0; i < n && (cin >> cmd); ++i) {
            handle(cmd, cin);
        }
        return 0;
    }

    // Fallback: unbounded stream of commands
    if (!cin.good()) cin.clear();
    if (!cmd.empty()) {
        // unreachable in most cases
    }
    while (cin >> cmd) {
        if (cmd == "PF") {
            long long x; cin >> x; dll.push_front(x);
        } else if (cmd == "PB") {
            long long x; cin >> x; dll.push_back(x);
        } else if (cmd == "POF") {
            auto v = dll.pop_front();
            if (v) cout << *v << "\n"; else cout << "NA\n";
        } else if (cmd == "POB") {
            auto v = dll.pop_back();
            if (v) cout << *v << "\n"; else cout << "NA\n";
        } else if (cmd == "SZ") {
            cout << dll.size() << "\n";
        } else if (cmd == "IT") {
            bool first = true;
            for (auto it = dll.begin(); it != dll.end(); it++) {
                if (!first) cout << ' ';
                cout << *it;
                first = false;
            }
            cout << "\n";
        }
    }
    return 0;
}
