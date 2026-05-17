#ifndef UI_H
#define UI_H

#include <iostream>
#include <vector>
#include <string>
#include <unordered_set>
#include <iomanip>
#include "data_structures.h"

using namespace std;

bool supportsUtf8();
bool supportsColor();
string repeatChar(char c, int count);
string repeatString(const string &s, int count);
string trim(const string &value);
string truncateText(const string &value, size_t width);
int getMenuChoice(int minChoice, int maxChoice);
bool confirmPrompt(const string &prompt);
void printSeparator(char ch = '=', int width = 64);
string getBoxTopBorder();
string childPrefix(bool isLast);
string branchPrefix(bool isLast);
string getBoxBottomBorder();
string getBoxMidSeparator();
string getBoxLine(const string &content);
void printBoxedContent(const vector<string> &lines);
void printHeader(StructureType active, size_t nodeCount, double loadTimeMs, const string &fileName);
void printMenuTitle(const string &title);
void printMetric(const string &name, double ms);
void printSuccess(const string &message);
void printError(const string &message);
void printWarning(const string &message);
void printMenuItems(const vector<string> &items);
void printMenuItemsBoxed(const vector<string> &items);
void printHashMapTreeUI(const CategoryMapManager &manager, const string &id, const string &prefix, bool isLast, int depthLimit, int currentDepth);
void printPointerTreeUI(const CategoryPointerTree &tree, CategoryNode *node, const string &prefix, bool isLast, int depthLimit, int currentDepth);
void printHierarchyUI(const AppState &state, const CategoryMapManager &hashManager, const CategoryPointerTree &pointerTree, int mode, const string &id, int depthLimit);
void printBenchmarkSummary(const vector<BenchmarkResult> &results, const string &title);
void printBenchmarkComparison(const BenchmarkResult &hashResult, const BenchmarkResult &pointerResult);
bool exportBenchmarkCsv(const string &outputFile, const vector<string> &datasetFiles, const vector<BenchmarkResult> &hashResults, const vector<BenchmarkResult> &pointerResults);
void benchmarkMenu();

#endif // UI_H
