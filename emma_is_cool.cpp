#include <iostream>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <map>
#include <algorithm>
#include <set>
#include <list>
#include <vector>

using namespace std;

int SUPPORT;
int CONFIDENCE;

/**
 * This is a map between the number we assign each function and the name
 * of that function.
 * This is filled in scanning and only used in printing.
 */
map<short, string> taylor_swift;

/**
 * This is a map between the functions we'll find in the bytecode file and
 * the number we assigned them.
 * This is filled for scanning and pretty much only used in scanning.
 */
map<string, short> scarlett_johansson;

/**
 * This is essentially a support mapping, it maps how many times we
 * have seen a function.
 */
map<short, short> mila_kunis;

/**
 * The simpliest adjacency list, it maps the function pairs.
 */
map<short, map<short, short> > kaley_cuoco;

/**
 * This is the list of functions we see in each function.
 * This is to be iterated later during analysis.
 */
set<pair<string, set<short> > > selena_gomez;

/**
 * This is a counter to make sure all of the shorts we assign each
 * function is unique.
 */
short ariana_grande = 0;

#define STR2INT(STR, INT) *INT = atoi(STR)
#define NEW_SCOPE "Call graph node f%'%'"
#define NEW_FUNC_CALL "%CS<%>%'%'"
#define MATCH_AT match.at(match_it)
#define LINE_AT line.at(line_it)
#define MATCH_ADD ++match_it;
#define LINE_ADD ++line_it;
#define MATCHES_PUSH_BACK(str) if (matches) { matches->push_back(str); }

/**
 * A quick regex library!
 * There is a wildcard, %, that matches everything
 * until the next character, note that this guy is extremely eager.
 * further more, if a vector is provided, then we'll populate it
 * with the results of each wildcard.
 * Usage: regex("abc", "abc") #=> true
 * Usage: regex("abcd", "d") #=> false
 * Usage: regex("abcde", "%") #=> true, ["abcde"]
 * Usage: regex("abcde", "%f") #=> false
 */
bool regex(const string line, const string match, vector<string> *matches) {
    unsigned int line_it = 0;
    unsigned int match_it = 0;
    char expecting;
    bool wildcarding = false;
    string wildcarded = "";
    while (match_it != match.size() && line_it != line.size()) {
        if (MATCH_AT == '%') {
            // We want to start wildcarding
            MATCH_ADD
            if (match_it == match.size()) {
                MATCHES_PUSH_BACK(line.substr(line_it))
                return true;
            }
            expecting = MATCH_AT;
            wildcarding = true;
        } else if (wildcarding && LINE_AT == expecting) {
            // This is the case we are wildcarding and now want to stop
            MATCH_ADD LINE_ADD
            MATCHES_PUSH_BACK(wildcarded);
            wildcarding = false;
            wildcarded = "";
        } else if (wildcarding) {
            // This is the case we are just grabbing more wildcarded stuff
            wildcarded += LINE_AT;
            LINE_ADD
        } else if (LINE_AT != MATCH_AT) {
            // Really bad, exact bad match, stop right here
            return false;
        } else {
            // The case is that LINE_AT == MATCH_AT
            MATCH_ADD LINE_ADD
        }
    }
    return match_it == match.size();
}

/**
 * Used for file state, used to determine if we should track functions
 */
enum FileState {
    TRACK_FUNC, DONT_TRACK 
};

short track_func_call(string &function) {
    short function_id;
    if (scarlett_johansson.find(function) != scarlett_johansson.end()) {
        function_id = scarlett_johansson.at(function);
        mila_kunis[function_id] += 1;
    } else {
        function_id = ariana_grande++;
        scarlett_johansson.insert(make_pair(function, function_id));
        taylor_swift.insert(make_pair(function_id, function));
        mila_kunis.insert(make_pair(function_id, 1));
    }
    return function_id;
}

/**
 * Iterate through every pair of currentFuncIds and add it to the adjacency list.
 */
void fill_in_adjacency(set<short> &currentFuncIds) {
    for (auto iIt = currentFuncIds.begin(); iIt != --currentFuncIds.end(); ++iIt) {
        for (auto jIt = iIt; ++jIt != currentFuncIds.end();) {
            // insure that all of the required keys are there.
            if (kaley_cuoco.find(*iIt) == kaley_cuoco.end()) {
                kaley_cuoco.insert(make_pair(*iIt, map<short, short>()));
                kaley_cuoco[*iIt][*jIt] = 1;
            } else if (kaley_cuoco[*iIt].find(*jIt) == kaley_cuoco[*iIt].end()) {
                kaley_cuoco[*iIt][*jIt] = 1;
            } else {
                ++kaley_cuoco[*iIt][*jIt];
            }
            if (kaley_cuoco.find(*jIt) == kaley_cuoco.end()) {
                kaley_cuoco.insert(make_pair(*jIt, map<short, short>()));
                kaley_cuoco[*jIt][*iIt] = 1;
            } else if (kaley_cuoco[*jIt].find(*iIt) == kaley_cuoco[*jIt].end()) {
                kaley_cuoco[*jIt][*iIt] = 1;
            } else {
                ++kaley_cuoco[*jIt][*iIt];
            }
        }
    }
}

/**
 * Scans the file based on regexes and fills in the global variables.
 * there are two regexes that are important, scanning for new scope and new function
 * When we see a new scope, then we can scan for new functions, otherwise,
 * we should ignore any function.
 * Then we fill in the vars ensuring uniqueness.
 * After this, selena_gomez, scarlett_johansson, mila_kunis, and taylor_swift
 * will be properly filled.
 */
void scan_file(char* inputFile) {
    ifstream filestream(inputFile);
    string line;
    FileState fState = DONT_TRACK;
    string currentScope;
    list<string> currentFuncs;

    while (getline(filestream, line)) {
        vector<string> *matches = new vector<string>();
        if (line.empty()) {
            if (fState == TRACK_FUNC && !currentFuncs.empty()) {
                // we need to dump all of the now tracked functions
                set<short> currentFuncIds;
                for (auto function : currentFuncs) {
                    currentFuncIds.insert(scarlett_johansson.at(function));
                }
                selena_gomez.insert(make_pair(
                    currentScope, currentFuncIds));
                fill_in_adjacency(currentFuncIds);
                currentFuncs.clear();
            }
            fState = DONT_TRACK;
        } else if (regex(line, NEW_SCOPE, matches)) {
            currentScope = matches->at(1);
            // start tracking functions
            fState = TRACK_FUNC;
        } else if (fState == TRACK_FUNC && regex(line, NEW_FUNC_CALL, matches)) {
            // try to track this function, if it's already tracked, don't worry
            string current_function = matches->at(3);
            if (find(currentFuncs.begin(), currentFuncs.end(), 
                    current_function) == currentFuncs.end()) {
                track_func_call(current_function);
                currentFuncs.push_back(current_function);
            }
        }
    }
}

/**
 * We iterate through all of the adjacency list and remove any elements with
 * a low support or confidence.
 */
void remove_low_support_confidence() {
    map<short, map<short, short> > temp;
    for (auto function : kaley_cuoco) {
        map<short, short> subtemp;
        for (auto count : function.second) {
            if (count.second >= SUPPORT && (count.second * 100 / 
                    mila_kunis.at(function.first) >= CONFIDENCE)) {
                subtemp.insert(count);
            }
        }
        if (!subtemp.empty()) {
            temp[function.first] = subtemp;
        }
    }
    kaley_cuoco = temp;
}

/**
 * Go through all the current functions and see if any of them are inside
 * kaley_cuoco.
 */
void analysis_code() {
    for (auto scope : selena_gomez) {
        for (auto function : scope.second) {
            // function is a short
            for (auto count : kaley_cuoco[function]) {
                if (scope.second.find(count.first) == scope.second.end()) {
                    string pair_1 = taylor_swift.at(function);
                    string pair_2 = taylor_swift.at(count.first);
                    string first = (pair_1 > pair_2 ? pair_2 : pair_1);
                    string second = (pair_1 > pair_2 ? pair_1 : pair_2);
                    cout << "bug: " << pair_1 << " in " << scope.first 
                        << ", pair: (" << first << ", " << second
                        << "), support: " << count.second << ", confidence: "
                        << setprecision(2) << fixed
                        << (count.second * 100.0 / mila_kunis.at(function)) << "%"
                        << endl;
                }
            }
        }
    }
}

int main(int argc, char** argv) {
    if (argc != 4 && argc != 2) {
        cerr << "Please input the correct number of arguments" << endl;
    }
    if (argc == 4) {
        STR2INT(argv[2], &SUPPORT);
        STR2INT(argv[3], &CONFIDENCE);
    } else {
        SUPPORT = 3;
        CONFIDENCE = 65;
    }

    scan_file(argv[1]);
    remove_low_support_confidence();
    /*
    for (auto function : kaley_cuoco) {
        for (auto count : function.second) {
                cout << taylor_swift.at(function.first) << "->" << taylor_swift.at(count.first) << ": " << count.second << endl;
        }
    }
    */
    analysis_code();

    return 0;
}
