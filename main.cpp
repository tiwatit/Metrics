#include <iostream>
#include <filesystem>
#include <fstream>
#include <regex>
#include <string>

namespace fs = std::filesystem;

struct Metrics {
    int totalLines = 0;
    int emptyLines = 0;
    int logicalLines = 0;
    int physicalLines = 0;
    int commentLines = 0;
    int cyclomaticComplexity = 1; // Start with 1 for the entry point of the program
};

void processFile(const fs::path& filePath, Metrics& metrics) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Could not open file: " << filePath << std::endl;
        return;
    }

    std::string line;
    std::regex logicalOpsRegex(R"((if|while|switch|case|&&|\|\||\?|\:|;))");
    std::regex forLoopRegex(R"(\bfor\s*\()");
    std::regex commentRegex(R"(^\s*//)");
    std::regex blockCommentStartRegex(R"(^\s*/\*)");
    std::regex blockCommentEndRegex(R"(\*/)");
    bool inBlockComment = false;

    while (std::getline(file, line)) {
        metrics.totalLines++;

        if (line.empty()) {
            metrics.emptyLines++;
        } else {
            metrics.physicalLines++;
        }

        if (inBlockComment) {
            metrics.commentLines++;
            if (std::regex_search(line, blockCommentEndRegex)) {
                inBlockComment = false;
            }
        } else if (std::regex_search(line, commentRegex)) {
            metrics.commentLines++;
        } else if (std::regex_search(line, blockCommentStartRegex)) {
            metrics.commentLines++;
            inBlockComment = true;
        } else {
            bool forLoopFound = std::regex_search(line, forLoopRegex);

            std::sregex_iterator logicalBegin(line.begin(), line.end(), logicalOpsRegex);
            std::sregex_iterator logicalEnd;
            int logicalOpsCount = std::distance(logicalBegin, logicalEnd);

            if (forLoopFound) {
                metrics.logicalLines += 1; // Count the 'for' loop as one logical line
                metrics.cyclomaticComplexity += 1; // Increase cyclomatic complexity for the loop
                logicalOpsCount--; // Subtract one to account for the 'for' loop itself
            }

            metrics.logicalLines += logicalOpsCount;
            metrics.cyclomaticComplexity += logicalOpsCount;
        }
    }

    file.close();
}

void traverseDirectory(const fs::path& dirPath, Metrics& metrics) {
    for (const auto& entry : fs::recursive_directory_iterator(dirPath)) {
        if (entry.is_regular_file()) {
            processFile(entry.path(), metrics);
        }
    }
}

int main() {
    std::string folderPath;
    std::cout << "Enter the folder path: ";
    std::getline(std::cin, folderPath);

    fs::path dirPath(folderPath);
    if (!fs::exists(dirPath) || !fs::is_directory(dirPath)) {
        std::cerr << "Invalid folder path." << std::endl;
        return 1;
    }

    Metrics metrics;
    traverseDirectory(dirPath, metrics);

    std::cout << "Total Lines: " << metrics.totalLines << std::endl;
    std::cout << "Empty Lines: " << metrics.emptyLines << std::endl;
    std::cout << "Logical Lines: " << metrics.logicalLines << std::endl;
    std::cout << "Physical Lines: " << metrics.physicalLines << std::endl;
    std::cout << "Comment Lines: " << metrics.commentLines << std::endl;
    std::cout << "Cyclomatic Complexity: " << metrics.cyclomaticComplexity << std::endl;

    double commentLevel = static_cast<double>(metrics.commentLines) / metrics.totalLines * 100;
    std::cout << "Level of Commenting: " << commentLevel << "%" << std::endl;

    return 0;
}
