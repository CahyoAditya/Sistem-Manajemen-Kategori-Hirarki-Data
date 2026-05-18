#include "ui.h"
#include <cstdlib>
#ifdef _WIN32
#include <direct.h>
#else
#include <unistd.h>
#include <sys/stat.h>
#endif

bool supportsUtf8()
{
    const char *lang = getenv("LANG");
    return lang != nullptr && string(lang).find("UTF-8") != string::npos;
}

bool supportsColor()
{
    return isatty(STDOUT_FILENO) != 0;
}

string repeatChar(char c, int count)
{
    return string(max(0, count), c);
}

string repeatString(const string &s, int count)
{
    string result;
    for (int i = 0; i < count; ++i)
        result += s;
    return result;
}

string trim(const string &value)
{
    size_t start = 0;
    while (start < value.size() && isspace((unsigned char)value[start]))
        start++;
    size_t end = value.size();
    while (end > start && isspace((unsigned char)value[end - 1]))
        end--;
    return value.substr(start, end - start);
}

string truncateText(const string &value, size_t width)
{
    if (value.size() <= width)
        return value;
    if (width <= 3)
        return string(width, '.');
    return value.substr(0, width - 3) + "...";
}

string branchPrefix(bool isLast)
{
    if (supportsUtf8())
        return isLast ? "└── " : "├── ";
    return isLast ? "+-- " : "|-- ";
}

string childPrefix(bool isLast)
{
    if (supportsUtf8())
        return isLast ? "    " : "│   ";
    return isLast ? "    " : "|   ";
}

int getMenuChoice(int minChoice, int maxChoice)
{
    int choice;
    while (true)
    {
        if (!(cin >> choice) || choice < minChoice || choice > maxChoice)
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "  ✗ Input tidak valid. Masukkan angka " << minChoice << "-" << maxChoice << ": ";
            continue;
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return choice;
    }
}

bool confirmPrompt(const string &prompt)
{
    while (true)
    {
        cout << prompt;
        string response;
        getline(cin, response);
        response = trim(response);
        if (response.empty())
            continue;
        char c = response[0];
        if (c == 'y' || c == 'Y')
            return true;
        if (c == 'n' || c == 'N')
            return false;
        cout << "  ✗ Input tidak valid. Masukkan y/n: ";
    }
}

void printSeparator(char ch, int width)
{
    cout << repeatChar(ch, width) << "\n";
}

string getBoxTopBorder()
{
    if (supportsUtf8())
        return string("╔") + repeatString("═", 62) + string("╗");
    return "+" + repeatChar('=', 62) + "+";
}

string getBoxBottomBorder()
{
    if (supportsUtf8())
        return string("╚") + repeatString("═", 62) + string("╝");
    return "+" + repeatChar('=', 62) + "+";
}

string getBoxMidSeparator()
{
    if (supportsUtf8())
        return string("╠") + repeatString("═", 62) + string("╣");
    return "+" + repeatChar('=', 62) + "+";
}

string getBoxLine(const string &content)
{
    string border = supportsUtf8() ? "║" : "|";
    int contentLen = content.length();
    int padding = max(0, 62 - contentLen);
    int padLeft = padding / 2;
    int padRight = padding - padLeft;
    return border + repeatChar(' ', padLeft) + content + repeatChar(' ', padRight) + border;
}

void printBoxedContent(const vector<string> &lines)
{
    cout << getBoxTopBorder() << "\n";
    for (const string &line : lines)
    {
        string border = supportsUtf8() ? "║" : "|";
        cout << border << " " << left << setw(61) << line << border << "\n";
    }
    cout << getBoxBottomBorder() << "\n";
}

void printHeader(StructureType active, size_t nodeCount, double loadTimeMs, const string &fileName)
{
    cout << "\n";
    vector<string> lines;
    lines.push_back("SISTEM MANAJEMEN KATEGORI & HIRARKI DATA");
    lines.push_back("Struktur Data P4 — Kelompok 10 — IPB University 2026");
    printBoxedContent(lines);

    cout << getBoxTopBorder() << "\n";
    string border = supportsUtf8() ? "║" : "|";
    cout << border << "  Struktur Aktif : " << left << setw(43) << kStructureNames[static_cast<int>(active)] << border << "\n";
    cout << border << "  Node Termuat   : " << left << setw(43) << nodeCount << border << "\n";
    cout << border << "  Waktu Load     : " << fixed << setprecision(2) << left << setw(43) << (to_string(loadTimeMs) + " ms") << border << "\n";
    if (!fileName.empty())
    {
        cout << border << "  Dataset file   : " << left << setw(43) << fileName << border << "\n";
    }
    cout << getBoxBottomBorder() << "\n";
}

void printMenuTitle(const string &title)
{
    cout << "\n";
    cout << getBoxTopBorder() << "\n";
    string border = supportsUtf8() ? "║" : "|";
    string titleLine = "  [" + title + "]";
    cout << border << " " << left << setw(61) << titleLine << border << "\n";
    cout << getBoxBottomBorder() << "\n";
}

void printMetric(const string &name, double ms)
{
    cout << "  [METRIK] Waktu " << left << setw(20) << name << " : " << fixed << setprecision(3) << ms << " ms\n";
}

void printSuccess(const string &message)
{
    cout << "  ✓ " << message << "\n";
}

void printError(const string &message)
{
    cout << "  ✗ " << message << "\n";
}

void printWarning(const string &message)
{
    cout << "  ! " << message << "\n";
}

void printMenuItems(const vector<string> &items)
{
    cout << "\n";
    for (const string &item : items)
    {
        cout << item << "\n";
    }
    cout << "\n";
}

void printMenuItemsBoxed(const vector<string> &items)
{
    cout << getBoxTopBorder() << "\n";
    string border = supportsUtf8() ? "║" : "|";
    for (const string &item : items)
    {
        cout << border << " " << left << setw(61) << item << border << "\n";
    }
    cout << getBoxBottomBorder() << "\n";
}

void printHashMapTreeUI(const CategoryMapManager &manager, const string &id, const string &prefix, bool isLast, int depthLimit, int currentDepth)
{
    const Category *cat = manager.getCategory(id);
    if (cat == nullptr)
        return;

    cout << prefix << branchPrefix(isLast) << "[" << cat->id << "] " << truncateText(cat->name, 40) << "\n";
    if (depthLimit >= 0 && currentDepth >= depthLimit)
        return;

    vector<string> children = manager.getChildIds(cat->id);
    for (size_t i = 0; i < children.size(); ++i)
    {
        bool nextIsLast = (i + 1 == children.size());
        printHashMapTreeUI(manager, children[i], prefix + childPrefix(isLast), nextIsLast, depthLimit, currentDepth + 1);
    }
}

void printPointerTreeUI(const CategoryPointerTree &tree, CategoryNode *node, const string &prefix, bool isLast, int depthLimit, int currentDepth)
{
    if (node == nullptr)
        return;

    cout << prefix << branchPrefix(isLast) << "[" << node->id << "] " << truncateText(node->name, 40) << "\n";
    if (depthLimit >= 0 && currentDepth >= depthLimit)
        return;

    vector<CategoryNode *> children;
    for (CategoryNode *child = node->firstChild; child != nullptr; child = child->nextSibling)
    {
        children.push_back(child);
    }

    for (size_t i = 0; i < children.size(); ++i)
    {
        bool nextIsLast = (i + 1 == children.size());
        printPointerTreeUI(tree, children[i], prefix + childPrefix(isLast), nextIsLast, depthLimit, currentDepth + 1);
    }
}

void printHierarchyUI(const AppState &state, const CategoryMapManager &hashManager, const CategoryPointerTree &pointerTree, int mode, const string &id, int depthLimit)
{
    if (state.activeStructure == StructureType::HashMapTree)
    {
        if (mode == 1)
        {
            vector<string> roots = hashManager.getRootIds();
            for (size_t i = 0; i < roots.size(); ++i)
            {
                bool isLast = (i + 1 == roots.size());
                printHashMapTreeUI(hashManager, roots[i], "", isLast, depthLimit, 1);
            }
        }
        else if (mode == 2)
        {
            printHashMapTreeUI(hashManager, id, "", true, depthLimit, 1);
        }
    }
    else
    {
        if (mode == 1)
        {
            vector<CategoryNode *> roots = pointerTree.getRootNodes();
            for (size_t i = 0; i < roots.size(); ++i)
            {
                bool isLast = (i + 1 == roots.size());
                printPointerTreeUI(pointerTree, roots[i], "", isLast, depthLimit, 1);
            }
        }
        else if (mode == 2)
        {
            CategoryNode *node = pointerTree.findNode(id);
            if (node != nullptr)
                printPointerTreeUI(pointerTree, node, "", true, depthLimit, 1);
        }
    }
}

void printBenchmarkSummary(const vector<BenchmarkResult> &results, const string &title)
{
    printMenuTitle(title);
    cout << "  ╔═══════════════╦══════════════╦══════════════╦══════════════╦══════════════╦══════════════╗\n";
    cout << "  ║ Dataset       ║ Build (ms)   ║ Insert (ms)  ║ Search (ms)  ║ Traversal(ms)║ Delete (ms)   ║\n";
    cout << "  ╠═══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╣\n";
    for (const auto &row : results)
    {
        cout << "  ║ " << left << setw(13) << row.label
             << " ║ " << right << setw(10) << fixed << setprecision(3) << row.build
             << " ║ " << setw(10) << row.insert
             << " ║ " << setw(10) << row.search
             << " ║ " << setw(11) << row.traversal
             << " ║ " << setw(11) << row.remove << " ║\n";
    }
    cout << "  ╚═══════════════╩══════════════╩══════════════╩══════════════╩══════════════╩══════════════╝\n";
}

void printBenchmarkComparison(const BenchmarkResult &hashResult, const BenchmarkResult &pointerResult)
{
    printMenuTitle("BENCHMARK PERBANDINGAN");
    cout << "  ╔══════════════╦════════════════════╦════════════════════╦══════════╗\n";
    cout << "  ║ Operasi      ║ HashMap Tree (ms)  ║ Pointer Tree (ms)  ║ Selisih   ║\n";
    cout << "  ╠══════════════╬════════════════════╬════════════════════╬══════════╣\n";
    auto printRow = [&](const string &label, double hashValue, double pointerValue) {
        double diff = pointerValue - hashValue;
        cout << "  ║ " << left << setw(12) << label
             << " ║ " << right << setw(18) << fixed << setprecision(3) << hashValue
             << " ║ " << setw(18) << pointerValue
             << " ║ " << setw(8) << showpos << fixed << setprecision(3) << diff << noshowpos << " ║\n";
    };
    printRow("Build", hashResult.build, pointerResult.build);
    printRow("Insert", hashResult.insert, pointerResult.insert);
    printRow("Search", hashResult.search, pointerResult.search);
    printRow("Traversal", hashResult.traversal, pointerResult.traversal);
    printRow("Delete", hashResult.remove, pointerResult.remove);
    cout << "  ╚══════════════╩════════════════════╩════════════════════╩══════════╝\n";
    cout << "\n  Keterangan Selisih: (+) Pointer Tree lebih lambat/besar, (-) lebih cepat/kecil\n";
}

bool exportBenchmarkCsv(const string &outputFile, const vector<string> &datasetFiles, const vector<BenchmarkResult> &hashResults, const vector<BenchmarkResult> &pointerResults)
{
    if (datasetFiles.size() != hashResults.size() || datasetFiles.size() != pointerResults.size())
        return false;
    string folder = "output";
#ifdef _WIN32
    _mkdir(folder.c_str());
#else
    mkdir(folder.c_str(), 0755);
#endif
    ofstream csv(outputFile);
    if (!csv.is_open())
        return false;

    csv << "operasi,ukuran_data,hashmap_ms,pointer_ms\n";
    for (size_t i = 0; i < datasetFiles.size(); ++i)
    {
        string label = hashResults[i].label;
        csv << "Build," << label << "," << hashResults[i].build << "," << pointerResults[i].build << "\n";
        csv << "Insert," << label << "," << hashResults[i].insert << "," << pointerResults[i].insert << "\n";
        csv << "Search," << label << "," << hashResults[i].search << "," << pointerResults[i].search << "\n";
        csv << "Traversal," << label << "," << hashResults[i].traversal << "," << pointerResults[i].traversal << "\n";
        csv << "Delete," << label << "," << hashResults[i].remove << "," << pointerResults[i].remove << "\n";
    }
    csv.close();
    return true;
}

void benchmarkMenu()
{
    vector<string> datasetFiles = {"dataset_1000.json", "dataset_5000.json", "dataset_10000.json"};
    printMenuTitle("BENCHMARK & PERBANDINGAN");
    vector<string> benchmarkItems = {
        "[1] Benchmark Struktur Aktif",
        "[2] Benchmark Kedua Struktur (side-by-side)",
        "[3] Ekspor Hasil ke CSV",
        "[0] Kembali"
    };
    printMenuItemsBoxed(benchmarkItems);
    cout << "  Pilihan: ";
    int choice = getMenuChoice(0, 3);

    if (choice == 1)
    {
        printMenuTitle("BENCHMARK STRUKTUR AKTIF");
        cout << "  Pilih ukuran dataset:\n";
        cout << "  [1] 1.000 node    [2] 5.000 node    [3] 10.000 node    [4] Semua ukuran\n";
        cout << "  Pilihan: ";
        int sizeChoice = getMenuChoice(1, 4);

        vector<BenchmarkResult> results;
        if (sizeChoice == 4)
        {
            for (const auto &file : datasetFiles)
            {
                vector<Category> categories = loadCategoryListFromJson(file);
                if (!categories.empty())
                {
                    results.push_back(benchmarkStructure(categories, StructureType::HashMapTree));
                }
            }
            printBenchmarkSummary(results, "Benchmark Struktur Aktif");
        }
        else
        {
            string file = datasetFiles[sizeChoice - 1];
            vector<Category> categories = loadCategoryListFromJson(file);
            if (categories.empty())
            {
                printError("Tidak dapat memuat dataset " + file + ".");
                return;
            }
            auto result = benchmarkStructure(categories, StructureType::HashMapTree);
            printBenchmarkSummary({result}, "Benchmark Struktur Aktif");
        }
    }
    else if (choice == 2)
    {
        printMenuTitle("BENCHMARK PERBANDINGAN");
        string compareFile = "dataset_10000.json";
        vector<Category> categories = loadCategoryListFromJson(compareFile);
        if (categories.empty())
        {
            printError("Tidak dapat memuat dataset 10.000 node.");
            return;
        }
        auto hashResult = benchmarkStructure(categories, StructureType::HashMapTree);
        auto pointerResult = benchmarkStructure(categories, StructureType::PointerTree);
        printBenchmarkComparison(hashResult, pointerResult);
    }
    else if (choice == 3)
    {
        vector<BenchmarkResult> hashResults;
        vector<BenchmarkResult> pointerResults;
        for (const auto &file : datasetFiles)
        {
            vector<Category> categories = loadCategoryListFromJson(file);
            if (!categories.empty())
            {
                hashResults.push_back(benchmarkStructure(categories, StructureType::HashMapTree));
                pointerResults.push_back(benchmarkStructure(categories, StructureType::PointerTree));
            }
        }
        string outputFile = "output/benchmark_result.csv";
        if (exportBenchmarkCsv(outputFile, datasetFiles, hashResults, pointerResults))
        {
            printSuccess("File berhasil diekspor ke " + outputFile);
        }
        else
        {
            printError("Gagal menulis file CSV.");
        }
    }
}
