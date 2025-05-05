/*----------------------------------------------------------------------
Usage:
   g++ -std=c++17 *.cpp
   ./test_p2.sh [num]
----------------------------------------------------------------------*/

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
void Task5(const vector<Rule>& rules) {
    // find nonterminals
    unordered_set<string> nonTerminalSet;
    for (const Rule& rule : rules) {
        nonTerminalSet.insert(rule.LHS);
    }

    // LHS
    unordered_map<string, vector<Rule>> rulesByLHS;
    for (const Rule& rule : rules) {
        rulesByLHS[rule.LHS].push_back(rule);
    }

    vector<Rule> newRules;
    unordered_map<string, int> newNonTermCounts;

    // process each nonterminal
    for (const auto& pair : rulesByLHS) {
        const string& nt = pair.first;
        vector<Rule> currRules = pair.second; // make a copy
        vector<Rule> factored;

        // keep factoring until no more common prefixes are found
        bool changed = true;
        while (changed && !currRules.empty()) {
            changed = false;

            // find the longest common prefix
            int maxPrefixLen = 0;
            vector<string> maxPrefix;
            vector<int> rulesToFactor;

            for (size_t i = 0; i < currRules.size(); i++) {
                for (size_t j = i + 1; j < currRules.size(); j++) {
                    // fimd common prefix length
                    int prefixLen = 0;
                    while (prefixLen < currRules[i].RHS.size() &&
                           prefixLen < currRules[j].RHS.size() &&
                           currRules[i].RHS[prefixLen] == currRules[j].RHS[prefixLen]) {
                        prefixLen++;
                    }

                    if (prefixLen > 0 && prefixLen > maxPrefixLen) {
                        maxPrefixLen = prefixLen;
                        maxPrefix = vector<string>(currRules[i].RHS.begin(),
                                                 currRules[i].RHS.begin() + prefixLen);
                        rulesToFactor.clear();
                        rulesToFactor.push_back(i);
                        rulesToFactor.push_back(j);
                    } else if (prefixLen == maxPrefixLen && prefixLen > 0) {
                        // check if same prefix
                        bool samePrefix = true;
                        for (int k = 0; k < prefixLen; k++) {
                            if (currRules[i].RHS[k] != maxPrefix[k]) {
                                samePrefix = false;
                                break;
                            }
                        }

                        if (samePrefix && find(rulesToFactor.begin(), rulesToFactor.end(), i) == rulesToFactor.end()) {
                            rulesToFactor.push_back(i);
                        }

                        samePrefix = true;
                        for (int k = 0; k < prefixLen; k++) {
                            if (currRules[j].RHS[k] != maxPrefix[k]) {
                                samePrefix = false;
                                break;
                            }
                        }

                        if (samePrefix && find(rulesToFactor.begin(), rulesToFactor.end(), j) == rulesToFactor.end()) {
                            rulesToFactor.push_back(j);
                        }
                    }
                }
            }

            // find all rules same prefix
            if (maxPrefixLen > 0) {
                for (size_t i = 0; i < currRules.size(); i++) {
                    if (find(rulesToFactor.begin(), rulesToFactor.end(), i) != rulesToFactor.end()) {
                        continue; // alreaddy in list
                    }

                    if (currRules[i].RHS.size() >= maxPrefixLen) {
                        bool matchPrefix = true;
                        for (int j = 0; j < maxPrefixLen; j++) {
                            if (currRules[i].RHS[j] != maxPrefix[j]) {
                                matchPrefix = false;
                                break;
                            }
                        }

                        if (matchPrefix) {
                            rulesToFactor.push_back(i);
                        }
                    }
                }

                // new nonterminal
                string newNT = nt + to_string(++newNonTermCounts[nt]);

                vector<string> newRHSPrefix = maxPrefix;
                newRHSPrefix.push_back(newNT);
                factored.push_back({nt, newRHSPrefix});

                for (int ruleIdx : rulesToFactor) {
                    vector<string> suffix;
                    if (currRules[ruleIdx].RHS.size() > maxPrefixLen) {
                        suffix.insert(suffix.end(),
                                     currRules[ruleIdx].RHS.begin() + maxPrefixLen,
                                     currRules[ruleIdx].RHS.end());
                    }
                    factored.push_back({newNT, suffix});
                }

                vector<Rule> remaining;
                for (size_t i = 0; i < currRules.size(); i++) {
                    if (find(rulesToFactor.begin(), rulesToFactor.end(), i) == rulesToFactor.end()) {
                        remaining.push_back(currRules[i]);
                    }
                }

                currRules = remaining;
                changed = true;
            }
        }

        // add remaining unfactored rules
        factored.insert(factored.end(), currRules.begin(), currRules.end());

        // add all factored rules for this nonterminal
        newRules.insert(newRules.end(), factored.begin(), factored.end());
    }

    // sort rules
    sort(newRules.begin(), newRules.end(), [](const Rule& a, const Rule& b) {
        // LHS
        if (a.LHS != b.LHS) {
            return a.LHS < b.LHS;
        }

        // RHS
        size_t minSize = min(a.RHS.size(), b.RHS.size());
        for (size_t i = 0; i < minSize; i++) {
            if (a.RHS[i] != b.RHS[i]) {
                return a.RHS[i] < b.RHS[i];
            }
        }

        return a.RHS.size() < b.RHS.size();
    });

    // print] the grammar
    for (const Rule& rule : newRules) {
        cout << rule.LHS << " -> ";
        for (const string& symbol : rule.RHS) {
            cout << symbol << " ";
        }
        cout << "#" << endl;
    }
}

// Task 6: eliminate left recursion
void Task6(const vector<Rule>& rules) {
    // nonterminals
    unordered_set<string> nonTerminalSet;
    vector<string> nonTerminals;

    for (const Rule& rule : rules) {
        if (nonTerminalSet.find(rule.LHS) == nonTerminalSet.end()) {
            nonTerminalSet.insert(rule.LHS);
            nonTerminals.push_back(rule.LHS);
        }
    }

    // sort nonterminals
    sort(nonTerminals.begin(), nonTerminals.end());

    unordered_map<string, vector<Rule>> rulesByLHS;
    for (const Rule& rule : rules) {
        rulesByLHS[rule.LHS].push_back(rule);
    }

    unordered_map<string, vector<Rule>> newRulesByLHS;

    for (size_t i = 0; i < nonTerminals.size(); i++) {
        string Ai = nonTerminals[i];
        vector<Rule> currentRules = rulesByLHS[Ai];

        // elim indirect left recursion
        for (size_t j = 0; j < i; j++) {
            string Aj = nonTerminals[j];

            vector<Rule> updatedRules;
            for (const Rule& rule : currentRules) {
                if (!rule.RHS.empty() && rule.RHS[0] == Aj) {
                    vector<string> gamma(rule.RHS.begin() + 1, rule.RHS.end());

                    for (const Rule& ajRule : newRulesByLHS[Aj]) {
                        vector<string> newRHS = ajRule.RHS;
                        newRHS.insert(newRHS.end(), gamma.begin(), gamma.end());
                        updatedRules.push_back({Ai, newRHS});
                    }
                } else {
                    updatedRules.push_back(rule);
                }
            }

            currentRules = updatedRules;
        }

        // elim direct left recursion
        vector<Rule> alphaRules; // rules starting with Ai (recursive)
        vector<Rule> betaRules;  // rules not starting with Ai (non-recursive)

        for (const Rule& rule : currentRules) {
            if (!rule.RHS.empty() && rule.RHS[0] == Ai) {
                // This rule has the form Ai -> Ai alpha
                alphaRules.push_back(rule);
            } else {
                // This rule has the form Ai -> beta
                betaRules.push_back(rule);
            }
        }

        if (alphaRules.empty()) {
            // no direct left recursion
            newRulesByLHS[Ai] = currentRules;
        } else {
            // new nonterminal Ai1
            string Ai1 = Ai + "1";

            // rewrite rules
            vector<Rule> aiRules;
            vector<Rule> ai1Rules;

            for (const Rule& rule : betaRules) {
                vector<string> newRHS = rule.RHS;
                newRHS.push_back(Ai1);
                aiRules.push_back({Ai, newRHS});
            }

            for (const Rule& rule : alphaRules) {
                vector<string> alpha(rule.RHS.begin() + 1, rule.RHS.end());
                alpha.push_back(Ai1);
                ai1Rules.push_back({Ai1, alpha});
            }

            // add epsilon rule for Ai1
            ai1Rules.push_back({Ai1, vector<string>()});

            newRulesByLHS[Ai] = aiRules;
            newRulesByLHS[Ai1] = ai1Rules;
        }
    }

    // combine all rules, sort
    vector<Rule> allRules;
    for (const auto& pair : newRulesByLHS) {
        allRules.insert(allRules.end(), pair.second.begin(), pair.second.end());
    }

    sort(allRules.begin(), allRules.end(), [](const Rule& a, const Rule& b) {
        // LHS
        if (a.LHS != b.LHS) {
            return a.LHS < b.LHS;
        }

        // RHS
        int minSize = min(a.RHS.size(), b.RHS.size());
        for (int i = 0; i < minSize; i++) {
            if (a.RHS[i] != b.RHS[i]) {
                return a.RHS[i] < b.RHS[i];
            }
        }

        return a.RHS.size() < b.RHS.size();
    });

    // pint the grammar
    for (const Rule& rule : allRules) {
        cout << rule.LHS << " -> ";
        for (const string& symbol : rule.RHS) {
            cout << symbol << " ";
        }
        cout << "#" << endl;
    }
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

        case 5: Task5(rules);
        break;

        case 6: Task6(rules);
        break;

        default:
            cout << "Error: unrecognized task number " << task << "\n";
        break;
    }
    return 0;
}