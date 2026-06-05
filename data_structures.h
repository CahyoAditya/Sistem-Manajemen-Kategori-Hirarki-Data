#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H

#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>
#include <fstream>
#include <chrono>
#include <algorithm>
#include <limits>
#include <iomanip>
#include <cctype>
#include <cmath>
#include <functional>

using namespace std;

enum class StructureType
{
    HashMapTree,
    PointerTree
};

extern const vector<string> kStructureNames;

struct Category
{
    string id;
    string name;
    string parent_id;
    int level;
    bool is_active;
    vector<string> child_ids;
};

struct AppState
{
    StructureType activeStructure = StructureType::HashMapTree;
    string activeFile = "";
    vector<Category> dataset;
    bool datasetLoaded = false;
    double loadTimeMs = 0.0;
    size_t nodeCount = 0;
    size_t rootCount = 0;
    int maxDepth = 0;
    string deepestNodeId;
    string deepestNodeName;
    size_t widestNodeSize = 0;
    string widestNodeId;
};

struct DatasetLoadResult
{
    vector<Category> categories;
    vector<string> orphanIds;
    double loadTimeMs = 0.0;
    string errorMessage;
    bool success = false;
};

struct BenchmarkResult
{
    string label;
    double build = 0.0;
    double insert = 0.0;
    double search = 0.0;
    double traversal = 0.0;
    double remove = 0.0;
    double memKb = 0.0;
};

bool isValidCategoryId(const string &id);
bool isValidCategoryName(const string &name);
size_t estimateMemoryUsageKB(size_t nodeCount);
size_t getProcessMemoryUsageKB();
vector<Category> loadCategoryListFromJson(const string &filename);
DatasetLoadResult loadDatasetFromJsonFile(const string &filename);
void calculateDatasetStats(const vector<Category> &categories, AppState &state);
void buildActiveStructures(AppState &state, class CategoryMapManager &hashManager, class CategoryPointerTree &pointerTree);
void collectSubtreeIdsHashMap(const class CategoryMapManager &manager, const string &id, vector<string> &result);
void collectSubtreeIdsPointerTree(class CategoryNode *node, vector<string> &result);
void removeCategoriesFromDataset(vector<Category> &dataset, const unordered_set<string> &idsToRemove);
BenchmarkResult benchmarkStructure(const vector<Category> &categories, StructureType type, int repeat = 3);
bool exportBenchmarkCsv(const string &outputFile, const vector<string> &datasetFiles, const vector<BenchmarkResult> &hashResults, const vector<BenchmarkResult> &pointerResults);

class CategoryMapManager
{
private:
    unordered_map<string, Category> categoryMap;
    unordered_map<string, string> nameToIdMap;
    string dbFile = "dataset_kategori.json";
    void deleteRecursive(const string &id);
    size_t countSubtreeNodesRecursive(const string &id);

public:
    void printTree(const string &root_id, const string &indent);
    void loadData();
    void saveData();
    void insertCategory(const string &id, const string &name, const string &parent_id);
    bool insertCategoryRaw(const string &id, const string &name, const string &parent_id);
    bool deleteCategoryRaw(const string &id);
    void searchCategory(const string &query);
    bool existsCategory(const string &query) const;
    void deleteCategory(const string &id);
    void loadDataFromJsonFile(const string &filename);
    void loadDataFromVector(const vector<Category> &categories);
    string resolveId(const string &query) const;
    bool exists(const string &query) const;
    const Category *getCategory(const string &query) const;
    vector<string> getChildIds(const string &id) const;
    vector<string> getRootIds() const;
    size_t countSubtreeNodes(const string &root_id);
};

struct CategoryNode
{
    string id;
    string name;
    string parent_id;
    int level;
    bool is_active;
    CategoryNode *firstChild;
    CategoryNode *nextSibling;
    CategoryNode *parent;

    CategoryNode(const string &id, const string &name, const string &parent_id, int level);
};

class CategoryPointerTree
{
private:
    CategoryNode *root;
    unordered_map<string, CategoryNode *> idIndex;
    unordered_map<string, CategoryNode *> nameIndex;
    void clearSubtree(CategoryNode *node);
    void attachChild(CategoryNode *parent, CategoryNode *child);
    size_t countNodesRecursive(const CategoryNode *node) const;

public:
    CategoryPointerTree();
    ~CategoryPointerTree();
    void clearTree();
    void loadDataFromVector(const vector<Category> &categories);
    CategoryNode *searchById(const string &id) const;
    CategoryNode *searchByName(const string &name) const;
    bool exists(const string &query) const;
    CategoryNode *findNode(const string &query) const;
    vector<CategoryNode *> getRootNodes() const;
    void printTree(CategoryNode *node, const string &indent) const;
    void printHierarchy() const;
    size_t countSubtreeNodes(const string &id) const;
    bool insertCategory(const string &id, const string &name, const string &parent_id);
    bool removeFromParent(CategoryNode *node);
    void deleteSubtree(CategoryNode *node);
    bool deleteCategory(const string &id);
};

#endif
