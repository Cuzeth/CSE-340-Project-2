#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include "lexer.h"

using namespace std;

// grammar rule struct
struct Rule {
    string LHS;
    vector<string> RHS;
};

// read the grammar from input
vector<Rule> ReadGrammar() {
    vector<Rule> rules;
    LexicalAnalyzer lexer;
    Token token = lexer.GetToken();

    while (token.token_type != HASH) {
        // parse left hand side
        if (token.token_type != ID) {
            cout << "SYNTAX ERROR !!!!!!!!!!!!!!!" << endl;
            exit(1);
        }

        string lhs = token.lexeme;
        token = lexer.GetToken();

        if (token.token_type != ARROW) {
            cout << "SYNTAX ERROR !!!!!!!!!!!!!!!" << endl;
            exit(1);
        }

        // parse right hand side
        do {
            vector<string> rhs;
            token = lexer.GetToken();

            // handle epsilon (empty right side)
            if (token.token_type == STAR || token.token_type == OR) {
                // empty
            } else {
                // parse ID list
                while (token.token_type == ID) {
                    rhs.push_back(token.lexeme);
                    token = lexer.GetToken();
                }
            }

            // add rule to grammar
            rules.push_back({lhs, rhs});

            if (token.token_type == STAR) {
                break; // end of rule
            } else if (token.token_type != OR) {
                cout << "SYNTAX ERROR !!!!!!!!!!!!!!!" << endl;
                exit(1);
            }
        } while (true);

        token = lexer.GetToken();
    }

    return rules;
}

/*
 * Task 1:
 * Printing the terminals, then nonterminals of grammar in appearing order
 * output is one line, and all names are space delineated
*/
void Task1(const vector<Rule>& rules) {
    // set for efficient lookups
    unordered_set<string> nonTerminalSet;
    // store all terminals and nonterminals in order of first appearance
    vector<string> allSymbols;
    vector<string> terminals;
    vector<string> nonTerminals;

    // collect all nonterminals (from LHS)
    for (const Rule& rule : rules) {
        nonTerminalSet.insert(rule.LHS);
    }

    // collect all symbols in their order of appearance
    for (const Rule& rule : rules) {
        // first check LHS (nonterminal)
        if (find(allSymbols.begin(), allSymbols.end(), rule.LHS) == allSymbols.end()) {
            allSymbols.push_back(rule.LHS);
        }

        // check RHS
        for (const string& symbol : rule.RHS) {
            if (find(allSymbols.begin(), allSymbols.end(), symbol) == allSymbols.end()) {
                allSymbols.push_back(symbol);
            }
        }
    }

    // separate terminals and nonterminals while preserving order
    for (const string& symbol : allSymbols) {
        if (nonTerminalSet.find(symbol) != nonTerminalSet.end()) {
            nonTerminals.push_back(symbol);
        } else {
            terminals.push_back(symbol);
        }
    }

    // print terminals followed by nonterminals
    for (const string& t : terminals) {
        cout << t << " ";
    }

    for (const string& nt : nonTerminals) {
        cout << nt << " ";
    }

}

/*
 * Task 2:
 * Print out nullable set of the grammar in specified format.
*/
void Task2(const vector<Rule>& rules) {
    // identify nonterminals in grammar appearance order
    unordered_set<string> nonTerminalSet;
    vector<string> orderedNonTerminals;

    // identify all nonterminals from LHS
    for (const Rule& rule : rules) {
        nonTerminalSet.insert(rule.LHS);
    }

    // collect all symbols in their order of appearance
    vector<string> allSymbols;
    for (const Rule& rule : rules) {
        // LHS
        if (find(allSymbols.begin(), allSymbols.end(), rule.LHS) == allSymbols.end()) {
            allSymbols.push_back(rule.LHS);
        }

        // RHS
        for (const string& symbol : rule.RHS) {
            if (find(allSymbols.begin(), allSymbols.end(), symbol) == allSymbols.end()) {
                allSymbols.push_back(symbol);
            }
        }
    }

    // extract ordered nonterminals
    for (const string& symbol : allSymbols) {
        if (nonTerminalSet.find(symbol) != nonTerminalSet.end()) {
            orderedNonTerminals.push_back(symbol);
        }
    }

    // calculate nullable nonterminals
    bool changed = true;
    unordered_set<string> nullable;

    // add all nonterminals with epsilon rules
    for (const Rule& rule : rules) {
        if (rule.RHS.empty()) {
            nullable.insert(rule.LHS);
        }
    }

    // apply rule 2 until no change
    while (changed) {
        changed = false;
        for (const Rule& rule : rules) {
            if (nullable.find(rule.LHS) != nullable.end()) {
                continue; // nullable
            }

            bool allNullable = true;
            for (const string& symbol : rule.RHS) {
                if (nullable.find(symbol) == nullable.end()) {
                    allNullable = false;
                    break;
                }
            }

            if (allNullable && !rule.RHS.empty()) {
                nullable.insert(rule.LHS);
                changed = true;
            }
        }
    }

    // print nullable nonterminals in order of appearance
    cout << "Nullable = {";
    bool first = true;
    for (const string& nt : orderedNonTerminals) {
        if (nullable.find(nt) != nullable.end()) {
            if (!first) cout << ",";
            cout << " " << nt;
            first = false;
        }
    }
    cout << " }";
}

// Task 3: FIRST sets
void Task3(const vector<Rule>& rules) {
    // identify nonterminals in grammar appearance order
    unordered_set<string> nonTerminalSet;
    vector<string> orderedNonTerminals;
    vector<string> orderedTerminals;
    vector<string> allSymbols;

    // collect all nonterminals from LHS
    for (const Rule& rule : rules) {
        nonTerminalSet.insert(rule.LHS);
    }

    // collect all symbols in their order of appearance
    for (const Rule& rule : rules) {
        // LHS (nonterminal)
        if (find(allSymbols.begin(), allSymbols.end(), rule.LHS) == allSymbols.end()) {
            allSymbols.push_back(rule.LHS);
        }

        // RHS
        for (const string& symbol : rule.RHS) {
            if (find(allSymbols.begin(), allSymbols.end(), symbol) == allSymbols.end()) {
                allSymbols.push_back(symbol);
            }
        }
    }

    // separate terminals and nonterminals while preserving order
    for (const string& symbol : allSymbols) {
        if (nonTerminalSet.find(symbol) != nonTerminalSet.end()) {
            orderedNonTerminals.push_back(symbol);
        } else {
            orderedTerminals.push_back(symbol);
        }
    }

    // calculate nullable nonterminals
    unordered_set<string> nullable;
    bool changed = true;

    // add all nonterminals with epsilon rules
    for (const Rule& rule : rules) {
        if (rule.RHS.empty()) {
            nullable.insert(rule.LHS);
        }
    }

    // apply nullable calculation until no change
    while (changed) {
        changed = false;
        for (const Rule& rule : rules) {
            if (nullable.find(rule.LHS) != nullable.end()) {
                continue; // already nullable
            }

            bool allNullable = true;
            for (const string& symbol : rule.RHS) {
                if (nonTerminalSet.find(symbol) == nonTerminalSet.end() ||
                    nullable.find(symbol) == nullable.end()) {
                    allNullable = false;
                    break;
                }
            }

            if (allNullable && !rule.RHS.empty()) {
                nullable.insert(rule.LHS);
                changed = true;
            }
        }
    }

    // FIRST sets
    unordered_map<string, set<string>> FIRST;

    // for terminals, FIRST(a) = {a}
    for (const string& t : orderedTerminals) {
        FIRST[t].insert(t);
    }

    // FIRST sets for nonterminals
    for (const string& nt : orderedNonTerminals) {
        FIRST[nt] = set<string>();
    }

    // compute FIRST sets
    changed = true;
    while (changed) {
        changed = false;

        for (const Rule& rule : rules) {
            // skip epsilon rules
            if (rule.RHS.empty()) continue;

            // for each position in RHS
            size_t i = 0;
            bool shouldContinue = true;

            while (i < rule.RHS.size() && shouldContinue) {
                shouldContinue = false;
                const string& symbol = rule.RHS[i];

                // if it's a terminal add it to FIRST(LHS)
                if (nonTerminalSet.find(symbol) == nonTerminalSet.end()) {
                    if (FIRST[rule.LHS].insert(symbol).second) {
                        changed = true;
                    }
                } else {
                    // if it's a nonterminal add FIRST(symbol) to FIRST(LHS)
                    for (const string& firstSymbol : FIRST[symbol]) {
                        if (FIRST[rule.LHS].insert(firstSymbol).second) {
                            changed = true;
                        }
                    }

                    // check if this nonterminal is nullable
                    if (nullable.find(symbol) != nullable.end()) {
                        shouldContinue = true;
                    }
                }

                i++;
            }
        }
    }

    // print FIRST sets for nonterminals in order
    for (const string& nt : orderedNonTerminals) {
        cout << "FIRST(" << nt << ") = { ";
        bool first = true;

        // print terminals in order of appearance
        for (const string& t : orderedTerminals) {
            if (FIRST[nt].find(t) != FIRST[nt].end()) {
                if (!first) cout << ", ";
                cout << t;
                first = false;
            }
        }

        cout << " }" << endl;
    }
}

// Task 4: FOLLOW sets
void Task4(const vector<Rule>& rules) {

    // track nonterminals in order in the grammar
    vector<string> orderedNonTerminals;
    unordered_set<string> nonTerminalSet;

    // track terminals in order
    vector<string> orderedTerminals;
    unordered_set<string> terminalSet;

    // find start symbol
    string startSymbol = rules[0].LHS;

    // add start symbol first
    orderedNonTerminals.push_back(startSymbol);
    nonTerminalSet.insert(startSymbol);

    // find all nonterminals and terminals in order
    for (const Rule& rule : rules) {
        // add LHS if not already added
        if (nonTerminalSet.find(rule.LHS) == nonTerminalSet.end()) {
            orderedNonTerminals.push_back(rule.LHS);
            nonTerminalSet.insert(rule.LHS);
        }

        // process RHS
        for (const string& symbol : rule.RHS) {
            // chcek if it's a nonterminal by looking if it's in LHS
            bool isNonTerminal = false;
            for (const Rule& r : rules) {
                if (r.LHS == symbol) {
                    isNonTerminal = true;
                    break;
                }
            }

            if (isNonTerminal) {
                // nonterminal
                if (nonTerminalSet.find(symbol) == nonTerminalSet.end()) {
                    orderedNonTerminals.push_back(symbol);
                    nonTerminalSet.insert(symbol);
                }
            } else {
                // terminal
                if (terminalSet.find(symbol) == terminalSet.end()) {
                    orderedTerminals.push_back(symbol);
                    terminalSet.insert(symbol);
                }
            }
        }
    }

    // nullable nonterminals
    unordered_set<string> nullable;
    bool changed = true;

    // add nonterminals with epsilon rules
    for (const Rule& rule : rules) {
        if (rule.RHS.empty()) {
            nullable.insert(rule.LHS);
        }
    }

    // apply nullable calculation until no change
    while (changed) {
        changed = false;
        for (const Rule& rule : rules) {
            if (nullable.find(rule.LHS) != nullable.end()) {
                continue; // already nullable
            }

            bool allNullable = true;
            for (const string& symbol : rule.RHS) {
                if (nonTerminalSet.find(symbol) == nonTerminalSet.end() ||
                    nullable.find(symbol) == nullable.end()) {
                    allNullable = false;
                    break;
                }
            }

            if (allNullable && !rule.RHS.empty()) {
                nullable.insert(rule.LHS);
                changed = true;
            }
        }
    }

    // FIRST sets
    unordered_map<string, set<string>> FIRST;

    // terminals, FIRST(a) = {a}
    for (const string& symbol : orderedTerminals) {
        FIRST[symbol].insert(symbol);
    }

    // FIRST sets for nonterminals
    changed = true;
    while (changed) {
        changed = false;

        for (const Rule& rule : rules) {
            if (rule.RHS.empty()) continue;

            size_t i = 0;
            bool shouldContinue = true;

            while (i < rule.RHS.size() && shouldContinue) {
                shouldContinue = false;
                const string& symbol = rule.RHS[i];

                if (nonTerminalSet.find(symbol) == nonTerminalSet.end()) {
                    // terminal symbol
                    if (FIRST[rule.LHS].insert(symbol).second) {
                        changed = true;
                    }
                } else {
                    // nonterminal symbol add its FIRST set
                    for (const string& firstSym : FIRST[symbol]) {
                        if (FIRST[rule.LHS].insert(firstSym).second) {
                            changed = true;
                        }
                    }

                    if (nullable.find(symbol) != nullable.end()) {
                        shouldContinue = true;
                    }
                }

                i++;
            }
        }
    }

    // FOLLOW sets
    unordered_map<string, set<string>> FOLLOW;

    // add $ to FOLLOW(S) (the start symbol)
    FOLLOW[startSymbol].insert("$");

    // apply rules for FOLLOW sets
    changed = true;
    while (changed) {
        changed = false;

        for (const Rule& rule : rules) {
            for (size_t i = 0; i < rule.RHS.size(); i++) {
                const string& B = rule.RHS[i]; // cur symbol

                // only  nonterminals
                if (nonTerminalSet.find(B) == nonTerminalSet.end()) {
                    continue;
                }

                if (i + 1 < rule.RHS.size()) {
                    size_t j = i + 1;
                    bool allNullable = true;

                    while (j < rule.RHS.size()) {
                        const string& nextSymbol = rule.RHS[j];

                        // add FIRST(nextSymbol) to FOLLOW(B)
                        for (const string& symbol : FIRST[nextSymbol]) {
                            if (FOLLOW[B].insert(symbol).second) {
                                changed = true;
                            }
                        }

                        if (nullable.find(nextSymbol) == nullable.end()) {
                            allNullable = false;
                            break;
                        }

                        j++;
                    }

                    // if all symbols after B are nullable, add FOLLOW(A) to FOLLOW(B)
                    if (allNullable) {
                        for (const string& symbol : FOLLOW[rule.LHS]) {
                            if (FOLLOW[B].insert(symbol).second) {
                                changed = true;
                            }
                        }
                    }
                } else {
                    for (const string& symbol : FOLLOW[rule.LHS]) {
                        if (FOLLOW[B].insert(symbol).second) {
                            changed = true;
                        }
                    }
                }
            }
        }
    }

    // print the FOLLOW sets in the expected order
    for (const string& nt : orderedNonTerminals) {
        cout << "FOLLOW(" << nt << ") = { ";

        // print symbols in order
        bool first = true;

        // print $ first
        if (FOLLOW[nt].find("$") != FOLLOW[nt].end()) {
            cout << "$";
            first = false;
        }

        // print the rest of the terminals in order in the grammar
        for (const string& t : orderedTerminals) {
            if (FOLLOW[nt].find(t) != FOLLOW[nt].end()) {
                if (!first) cout << ", ";
                cout << t;
                first = false;
            }
        }

        cout << " }" << endl;
    }
}

// Task 5: left factoring
void Task5()
{
}

// Task 6: eliminate left recursion
void Task6()
{
}

int main (int argc, char* argv[])
{
    int task;

    if (argc < 2)
    {
        cout << "Error: missing argument\n";
        return 1;
    }

    /*
       Note that by convention argv[0] is the name of your executable,
       and the first argument to your program is stored in argv[1]
     */

    task = atoi(argv[1]);

    vector<Rule> rules = ReadGrammar();  // Reads the input grammar from standard input
    // and represent it internally in data structures
    // ad described in project 2 presentation file

    switch (task) {
        case 1: Task1(rules);
        break;

        case 2: Task2(rules);
        break;

        case 3: Task3(rules);
        break;

        case 4: Task4(rules);
        break;

        case 5: Task5();
        break;

        case 6: Task6();
        break;

        default:
            cout << "Error: unrecognized task number " << task << "\n";
        break;
    }
    return 0;
}