#include <iostream>
#include <set>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm> // For includes
#include <iterator> // For set_union

using namespace std;

// Function to split a string into a set of attributes
set<string> splitAttributes(const string& str) {
    set<string> attributes;
    stringstream ss(str);
    string attribute;
    while (getline(ss, attribute, ',')) {
        attributes.insert(attribute);
    }
    return attributes;
}

// Function to parse functional dependencies
vector<pair<set<string>, set<string>>> parseFDs(const set<string>& FDs) {
    vector<pair<set<string>, set<string>>> parsedFDs;
    for (const string& fd : FDs) {
        size_t arrowPos = fd.find("->");
        if (arrowPos != string::npos) {
            set<string> lhs = splitAttributes(fd.substr(0, arrowPos));
            set<string> rhs = splitAttributes(fd.substr(arrowPos + 2));
            parsedFDs.push_back(make_pair(lhs, rhs));
        }
    }
    return parsedFDs;
}

// Function to compute closure using iterative approach
set<string> closure_iterative(const vector<pair<set<string>, set<string>>>& FDs, const set<string>& X) {
    set<string> closure = X;
    bool changed = true;
    while (changed) {
        changed = false;
        for (const auto& fd : FDs) {
            // Check if LHS is a subset of the closure
            if (includes(closure.begin(), closure.end(), fd.first.begin(), fd.first.end())) {
                // Add RHS attributes to closure if not already present
                for (const string& attr : fd.second) {
                    if (closure.count(attr) == 0) {
                        closure.insert(attr);
                        changed = true;
                    }
                }
            }
        }
    }
    return closure;
}

// Function to identify candidate keys
set<set<string>> findCandidateKeys(const set<string>& FDs) {
    // Parse functional dependencies
    vector<pair<set<string>, set<string>>> parsedFDs = parseFDs(FDs);

    set<set<string>> candidateKeys;
    set<string> allAttributes;
    // Extract all attributes
    for (const auto& fd : parsedFDs) {
        allAttributes.insert(fd.first.begin(), fd.first.end());
        allAttributes.insert(fd.second.begin(), fd.second.end());
    }
    // Iterate through all possible attribute sets
    for (int i = 1; i <= allAttributes.size(); ++i) {
        // Generate all possible subsets of attributes of size i
        vector<string> attributesVector(allAttributes.begin(), allAttributes.end());
        vector<bool> bitmask(allAttributes.size(), false);
        fill(bitmask.begin(), bitmask.begin() + i, true);
        do {
            set<string> currentKey;
            for (int j = 0; j < allAttributes.size(); ++j) {
                if (bitmask[j]) {
                    currentKey.insert(attributesVector[j]);
                }
            }
            // Compute the closure of the current attribute set
            set<string> closure = closure_iterative(parsedFDs, currentKey);
            // Check if the closure contains all attributes
            if (closure == allAttributes) {
                candidateKeys.insert(currentKey);
            }
        } while (prev_permutation(bitmask.begin(), bitmask.end()));
    }
    return candidateKeys;
}

int main() {
    // Sample FDs
    set<string> FDs = {"A->B", "B->C", "CD->E"};

    // Find candidate keys
    set<set<string>> candidateKeys = findCandidateKeys(FDs);

    // Output candidate keys
    cout << "Candidate keys found:\n";
    if (candidateKeys.empty()) {
        cout << "None\n";
    } else {
        for (const auto& key : candidateKeys) {
            for (const auto& attr : key) {
                cout << attr << ",";
            }
            cout << endl;
        }
    }

    return 0;
}

