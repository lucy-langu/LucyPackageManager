#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>
#include <regex>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <iomanip>
#include <map>
#include <cstdint>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#else
#include <sys/utsname.h>
#endif

namespace fs = std::filesystem;
namespace UI {
    const std::string RESET   = "\033[0m";
    const std::string BOLD    = "\033[1m";
    const std::string DIM     = "\033[2m";
    
    const std::string RED     = "\033[38;5;196m";
    const std::string GREEN   = "\033[38;5;46m";
    const std::string YELLOW  = "\033[38;5;226m";
    const std::string BLUE    = "\033[38;5;33m";
    const std::string MAGENTA = "\033[38;5;135m";
    const std::string CYAN    = "\033[38;5;51m";
    const std::string GRAY    = "\033[38;5;240m";

    void clearScreen() { std::cout << "\033[2J\033[1;1H"; }

    void drawLogo() {
        std::cout << MAGENTA << BOLD << R"(
 ██╗     ██████╗ ███╗   ███╗
 ██║     ██╔══██╗████╗ ████║
 ██║     ██████╔╝██╔████╔██║
 ██║     ██╔═══╝ ██║╚██╔╝██║
 ███████╗██║     ██║ ╚═╝ ██║
 ╚══════╝╚═╝     ╚═╝     ╚═╝
)" << RESET;
        std::cout << CYAN << BOLD << " Lucy Package Manager " << GRAY << "v3.0 (Secured Core)" << RESET << "\n";
        std::cout << GRAY << " ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << RESET << "\n\n";
    }

    void signature() {
        std::cout << "\n" << GRAY << " ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << RESET << "\n";
        std::cout << DIM << " Engineered by " << RESET << YELLOW << BOLD << "DR.F0G3J3" << RESET << DIM << " for the Lucy Ecosystem." << RESET << "\n\n";
    }

    void logInfo(const std::string& msg) { std::cout << CYAN << " ℹ " << RESET << DIM << msg << RESET << "\n"; }
    void logSuccess(const std::string& msg) { std::cout << GREEN << BOLD << " ✔ " << RESET << msg << "\n"; }
    void logError(const std::string& msg) { std::cout << RED << BOLD << " ✖ " << RESET << msg << "\n"; }
    void logWarn(const std::string& msg) { std::cout << YELLOW << BOLD << " ⚠ " << RESET << msg << "\n"; }

    // Stunning Qt-styled animated CLI progress bar loader
    void minimalLoader(const std::string& task, int ms_duration) {
        const int width = 40;
        const std::string frames[] = {"⠋", "⠙", "⠹", "⠸", "⠼", "⠴", "⠦", "⠧", "⠇", "⠏"};
        int total_steps = width;
        int frame_count = 10;
        
        for (int i = 0; i <= total_steps; ++i) {
            std::string spinner = frames[i % frame_count];
            std::cout << "\r" << MAGENTA << BOLD << spinner << " " << RESET << CYAN << std::left << std::setw(32) << task << RESET;
            std::cout << GRAY << "⟦" << GREEN;
            for (int j = 0; j < i; ++j) std::cout << "█";
            for (int j = i; j < total_steps; ++j) std::cout << "░";
            std::cout << GRAY << "⟧ " << YELLOW << std::setw(3) << (i * 100 / total_steps) << "%" << RESET;
            std::cout.flush();
            std::this_thread::sleep_for(std::chrono::milliseconds(ms_duration / total_steps));
        }
        std::cout << "\n\n";
    }

    void drawTable(const std::vector<std::vector<std::string>>& rows) {
        if (rows.empty()) return;
        std::cout << CYAN << " ┌──────────────┬──────────────┬──────────────┐\n";
        std::cout << " │ " << BOLD << std::left << std::setw(12) << "PACKAGE" << RESET << CYAN 
                  << "│ " << BOLD << std::left << std::setw(12) << "VERSION" << RESET << CYAN 
                  << "│ " << BOLD << std::left << std::setw(12) << "LUCY REQ" << RESET << CYAN << " │\n";
        std::cout << " ├──────────────┼──────────────┼──────────────┤\n" << RESET;
        for (const auto& row : rows) {
            std::cout << CYAN << " │ " << RESET << std::left << std::setw(12) << row[0] 
                      << CYAN << " │ " << RESET << std::left << std::setw(12) << row[1] 
                      << CYAN << " │ " << RESET << std::left << std::setw(12) << row[2] << CYAN << " │\n" << RESET;
        }
        std::cout << CYAN << " └──────────────┴──────────────┴──────────────┘\n" << RESET;
    }
}

// ==========================================
// core .. system va ina ...
// ==========================================
struct PackageMeta {
    std::string name;
    std::string version;
    std::string lucy_version;
    std::map<std::string, std::string> dependencies;
};

class LucyEnv {
public:
    std::string home_dir;
    std::string stdlib_dir;
    std::string meta_dir;
    std::string tmp_dir;

    LucyEnv() {
#if defined(_WIN32) || defined(_WIN64)
        const char* program_files = getenv("ProgramFiles");
        std::string base_root = program_files ? std::string(program_files) + "\\Lucy" : "C:\\Program Files\\Lucy";
        stdlib_dir = base_root + "\\stdlib";
        meta_dir = base_root + "\\packages";
        tmp_dir = base_root + "\\tmp";
#else
        const char* home = getenv("HOME");
        if (!home) { UI::logError("Unable to resolve HOME."); exit(1); }
        home_dir = std::string(home);
        std::string lucy_root = home_dir + "/.local/lib/lucy";
        stdlib_dir = lucy_root + "/stdlib";
        meta_dir = lucy_root + "/packages";
        tmp_dir = lucy_root + "/tmp";
#endif

        fs::create_directories(stdlib_dir);
        fs::create_directories(meta_dir);
        fs::create_directories(tmp_dir);
    }
};

class TomlParser {
public:
    static std::string findTomlFile(const fs::path& dir = fs::current_path()) {
        if (fs::exists(dir / "package.toml")) return (dir / "package.toml").string();
        if (fs::exists(dir / "project.toml")) return (dir / "project.toml").string();
        return "";
    }

    // Helper to clean comments and whitespace from lines
    static std::string sanitizeLine(const std::string& raw_line) {
        std::string line = raw_line;
        // Remove comments starting with '#'
        size_t comment_pos = line.find('#');
        if (comment_pos != std::string::npos) {
            line = line.substr(0, comment_pos);
        }
        // Trim leading/trailing whitespaces
        const auto strBegin = line.find_first_not_of(" \t\r\n");
        if (strBegin == std::string::npos) return "";
        const auto strEnd = line.find_last_not_of(" \t\r\n");
        return line.substr(strBegin, strEnd - strBegin + 1);
    }

    static PackageMeta parse(const std::string& filepath) {
        PackageMeta meta;
        std::ifstream file(filepath);
        if (!file.is_open()) return meta;

        std::string raw_line, line;
        std::regex name_re(R"rgx(name\s*=\s*"([^"]+)")rgx");
        std::regex ver_re(R"rgx(version\s*=\s*"([^"]+)")rgx");
        std::regex lucy_ver_re(R"rgx(version-lucy\s*=\s*"(?:>=)?([^"]+)")rgx");
        std::regex dep_re(R"rgx("([^>=]+)(?:>=([^"]+))?")rgx");
        
        bool in_deps = false;
        std::smatch match;

        while (std::getline(file, raw_line)) {
            line = sanitizeLine(raw_line);
            if (line.empty()) continue;

            if (std::regex_search(line, match, name_re) && meta.name.empty()) meta.name = match[1];
            if (std::regex_search(line, match, ver_re) && meta.version.empty()) meta.version = match[1];
            if (std::regex_search(line, match, lucy_ver_re)) meta.lucy_version = match[1];
            
            if (line.find("[dependencies]") != std::string::npos) { in_deps = true; continue; }
            if (in_deps && line.find("[") != std::string::npos) { in_deps = false; }
            if (in_deps && std::regex_search(line, match, dep_re)) { meta.dependencies[match[1]] = match[2]; }
        }
        return meta;
    }
};

// ==========================================
// .plm / .lpm
// ==========================================
class LpmPacker {
private:
    static const uint8_t MAGIC_KEY = 0x5C; // XOR Obfuscation Key
    static constexpr const char* MAGIC_HEADER = "LPM_NAME_KEY";

    static void cryptBuffer(std::string& data) {
        for (char& c : data) c ^= MAGIC_KEY;
    }

public:
    static bool packDirectory(const fs::path& sourceDir, const std::string& outFilename) {
        std::ofstream out(outFilename, std::ios::binary);
        if (!out.is_open()) return false;

        out.write(MAGIC_HEADER, 15);

        for (const auto& entry : fs::recursive_directory_iterator(sourceDir)) {
            if (entry.is_regular_file()) {
                std::string ext = entry.path().extension().string();
                if (ext == ".lpm" || ext == ".plm") continue; // Skip 
                std::string relPath = fs::relative(entry.path(), sourceDir).string();
                
                std::ifstream in(entry.path(), std::ios::binary);
                std::string content((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
                cryptBuffer(content);

                size_t pathLen = relPath.size();
                out.write(reinterpret_cast<const char*>(&pathLen), sizeof(pathLen));
                out.write(relPath.c_str(), pathLen);

                size_t contentLen = content.size();
                out.write(reinterpret_cast<const char*>(&contentLen), sizeof(contentLen));
                out.write(content.data(), contentLen);
            }
        }
        return true;
    }

    static bool unpackToDir(const std::string& lpmFile, const fs::path& destDir) {
        std::ifstream in(lpmFile, std::ios::binary);
        if (!in.is_open()) return false;

        char magic[15];
        in.read(magic, 15);
        if (std::string(magic, 15) != MAGIC_HEADER) {
            UI::logError("Corrupted or Invalid .plm / .lpm package format!");
            return false;
        }

        while (in.peek() != EOF) {
            size_t pathLen;
            if (!in.read(reinterpret_cast<char*>(&pathLen), sizeof(pathLen))) break;
            std::string relPath(pathLen, '\0');
            in.read(&relPath[0], pathLen);

            size_t contentLen;
            in.read(reinterpret_cast<char*>(&contentLen), sizeof(contentLen));
            std::string content(contentLen, '\0');
            in.read(&content[0], contentLen);

            cryptBuffer(content);

            fs::path fullPath = destDir / relPath;
            fs::create_directories(fullPath.parent_path());
            std::ofstream out(fullPath, std::ios::binary);
            out.write(content.data(), contentLen);
        }
        return true;
    }
};

// ==========================================
// PACKAGE MANAGER
// ==========================================
class PackageManager {
private:
    LucyEnv env;

    bool checkDependencies(const PackageMeta& meta) {
        for (const auto& [dep_name, dep_ver] : meta.dependencies) {
            std::string dep_path = env.meta_dir + "/" + dep_name + ".toml";
            if (!fs::exists(dep_path)) {
                UI::logError("Missing dependency: " + dep_name + " (Requires >= " + dep_ver + ")");
                return false;
            }
            PackageMeta installed_dep = TomlParser::parse(dep_path);
            if (installed_dep.version < dep_ver) {
                UI::logError("Version mismatch: " + dep_name + " requires >= " + dep_ver + ", found " + installed_dep.version);
                return false;
            }
            UI::logInfo("Dependency satisfied: " + dep_name + " v" + installed_dep.version);
        }
        return true;
    }

    void recursiveInstall(const fs::path& basePath) {
        std::string configPath = TomlParser::findTomlFile(basePath);
        if (configPath.empty()) return;

        PackageMeta meta = TomlParser::parse(configPath);
        if (meta.name.empty()) return;

        for (const auto& entry : fs::directory_iterator(basePath)) {
            if (entry.is_regular_file() && entry.path().extension() == ".lucy") {
                fs::copy_file(entry.path(), env.stdlib_dir + "/" + meta.name + ".lucy", fs::copy_options::overwrite_existing);
                break;
            }
        }

        fs::copy_file(configPath, env.meta_dir + "/" + meta.name + ".toml", fs::copy_options::overwrite_existing);
        UI::logSuccess("Installed module: " + meta.name + " (v" + meta.version + ")");

        fs::path libPath = basePath / "lib";
        if (fs::exists(libPath) && fs::is_directory(libPath)) {
            for (const auto& entry : fs::directory_iterator(libPath)) {
                if (entry.is_directory()) {
                    recursiveInstall(entry.path());
                }
            }
        }
    }

    bool checkLibFolderInteractive(const fs::path& projPath) {
        fs::path libPath = projPath / "lib";
        bool isEmpty = true;

        if (fs::exists(libPath) && fs::is_directory(libPath)) {
            for (const auto& entry : fs::directory_iterator(libPath)) {
                (void)entry;
                isEmpty = false;
                break;
            }
        }

        if (!fs::exists(libPath) || isEmpty) {
            UI::logWarn("No dependencies found in /lib folder.");
            std::cout << UI::CYAN << " Do you want to proceed with installation anyway? (y/n): " << UI::RESET;
            char ans;
            std::cin >> ans;
            if (ans != 'y' && ans != 'Y') {
                UI::logInfo("Installation aborted by user.");
                return false;
            }
        }
        return true;
    }

public:
    void initProject() {
        fs::path pkgDir = "my-package";
        if (fs::exists(pkgDir)) {
            UI::logWarn("Folder 'my-package' already exists.");
            return;
        }

        fs::create_directories(pkgDir / "lib");

        std::ofstream toml(pkgDir / "package.toml");
        toml << "[package]\nname = \"my_package\"\nversion = \"1.0.0\"\nversion-lucy = \">=1.0\"\n\n";
        toml << "[dependencies]\n# \"some_lib\" = \">=1.2.0\"\n";
        toml.close();

        std::ofstream main_lucy(pkgDir / "main.lucy");
        main_lucy << "# Entry point for Lucy Project\n";
        main_lucy << "print \"Hello from Lucy!\"\n";
        main_lucy.close();

        std::ofstream readme(pkgDir / "README.MD");
        readme << "# My Package\n\nCreated with Lucy Package Manager (LPM).\n";
        readme.close();

        UI::minimalLoader("Scaffolding package environment", 400);
        UI::logSuccess("Created my-package/ with main.lucy, README.MD, package.toml, and /lib");
    }

    void createPackage() {
        std::string configPath = TomlParser::findTomlFile();
        if (configPath.empty()) {
            UI::logError("Missing package.toml / project.toml. Cannot package.");
            return;
        }
        PackageMeta meta = TomlParser::parse(configPath);
        if (meta.name.empty() || meta.version.empty()) {
            UI::logError("Invalid package manifest parameters.");
            return;
        }

        std::string packName = meta.name + "-" + meta.version + ".plm";
        UI::logInfo("Compressing and encrypting package structure into .plm archive...");
        
        UI::minimalLoader("Building " + packName, 800);
        
        if (LpmPacker::packDirectory(fs::current_path(), packName)) {
            UI::logSuccess("Successfully created proprietary package: " + UI::YELLOW + packName + UI::RESET);
        } else {
            UI::logError("Failed to build package.");
        }
    }

    void installLocal() {
        std::string configPath = TomlParser::findTomlFile();
        if (configPath.empty()) {
            UI::logError("No package.toml or project.toml found in current directory.");
            return;
        }

        UI::logInfo("Parsing package metadata...");
        PackageMeta meta = TomlParser::parse(configPath);
        if (meta.name.empty() || meta.version.empty()) {
            UI::logError("Invalid manifest file. Missing name or version.");
            return;
        }

        if (!checkLibFolderInteractive(fs::current_path())) return;

        UI::logInfo("Verifying dependency tree...");
        if (!checkDependencies(meta)) return;

        std::string source_file = "";
        for (const auto& entry : fs::directory_iterator(".")) {
            if (entry.is_regular_file() && entry.path().extension() == ".lucy") {
                source_file = entry.path().string();
                break;
            }
        }

        if (source_file.empty()) {
            UI::logError("No .lucy source file found to install.");
            return;
        }

        UI::minimalLoader("Installing " + meta.name + " v" + meta.version + " globally", 600);

        try {
            fs::copy_file(source_file, env.stdlib_dir + "/" + meta.name + ".lucy", fs::copy_options::overwrite_existing);
            fs::copy_file(configPath, env.meta_dir + "/" + meta.name + ".toml", fs::copy_options::overwrite_existing);
        } catch (const std::exception& e) {
            UI::logError("Failed to write files to Lucy stdlib.");
            return;
        }

        if (fs::exists("lib") && fs::is_directory("lib")) {
            for (const auto& entry : fs::directory_iterator("lib")) {
                if (entry.is_directory()) recursiveInstall(entry.path());
            }
        }

        UI::logSuccess("Package '" + meta.name + "' and its dependencies are installed globally!");
    }

    void installFromLpm(const std::string& lpmFile) {
        if (!fs::exists(lpmFile)) {
            UI::logError("File not found: " + lpmFile);
            return;
        }

        std::string extractSession = env.tmp_dir + "/session_" + std::to_string(std::time(nullptr));
        fs::create_directories(extractSession);

        UI::minimalLoader("Decrypting & Unpacking proprietary package archive", 700);

        if (!LpmPacker::unpackToDir(lpmFile, extractSession)) {
            fs::remove_all(extractSession);
            return;
        }

        if (!checkLibFolderInteractive(extractSession)) {
            fs::remove_all(extractSession);
            return;
        }

        UI::logInfo("Resolving recursive dependency tree...");
        recursiveInstall(extractSession);

        fs::remove_all(extractSession);
        UI::logSuccess("All packages & dependencies installed globally.");
    }

    void uninstall(const std::string& pkg_name) {
        std::string pkg_file = env.stdlib_dir + "/" + pkg_name + ".lucy";
        std::string meta_file = env.meta_dir + "/" + pkg_name + ".toml";

        if (!fs::exists(pkg_file) && !fs::exists(meta_file)) {
            UI::logError("Package '" + pkg_name + "' is not installed.");
            return;
        }

        UI::minimalLoader("Removing " + pkg_name + " from system", 500);
        if (fs::exists(pkg_file)) fs::remove(pkg_file);
        if (fs::exists(meta_file)) fs::remove(meta_file);

        UI::logSuccess("Successfully uninstalled '" + pkg_name + "'.");
    }

    void listPackages() {
        std::vector<std::vector<std::string>> rows;
        for (const auto& entry : fs::directory_iterator(env.meta_dir)) {
            if (entry.path().extension() == ".toml") {
                PackageMeta meta = TomlParser::parse(entry.path().string());
                rows.push_back({meta.name, meta.version, meta.lucy_version});
            }
        }

        if (rows.empty()) {
            UI::logWarn("No packages installed in Lucy stdlib.");
            return;
        }
        
        UI::logInfo("Found " + std::to_string(rows.size()) + " installed packages:");
        UI::drawTable(rows);
    }

    void showInfo(const std::string& pkg_name) {
        std::string meta_file = env.meta_dir + "/" + pkg_name + ".toml";
        if (!fs::exists(meta_file)) {
            UI::logError("Cannot find metadata for '" + pkg_name + "'.");
            return;
        }

        PackageMeta meta = TomlParser::parse(meta_file);
        std::cout << "\n" << UI::MAGENTA << " ╔════════════════════════════════════════════╗\n";
        std::cout << " ║ " << UI::BOLD << UI::CYAN << "PACKAGE INFO" << UI::MAGENTA << std::setw(32) << "║\n";
        std::cout << " ╠════════════════════════════════════════════╣\n";
        std::cout << " ║ " << UI::RESET << "Name:       " << UI::YELLOW << meta.name << UI::MAGENTA << std::setw(34 - meta.name.length()) << "║\n";
        std::cout << " ║ " << UI::RESET << "Version:    " << UI::GREEN << meta.version << UI::MAGENTA << std::setw(34 - meta.version.length()) << "║\n";
        std::cout << " ║ " << UI::RESET << "Lucy Reqs:  " << UI::BLUE << meta.lucy_version << UI::MAGENTA << std::setw(34 - meta.lucy_version.length()) << "║\n";
        std::cout << " ╚════════════════════════════════════════════╝\n" << UI::RESET;
    }
};

// ==========================================
// MAIN ENTRY
// ==========================================
int main(int argc, char* argv[]) {
    UI::clearScreen();
    UI::drawLogo();

#if !defined(_WIN32) && !defined(_WIN64)
    struct utsname buffer;
    if (uname(&buffer) != 0 || (std::string(buffer.sysname) != "Linux" && std::string(buffer.sysname) != "Darwin")) {
        UI::logWarn("Running on a non-standard Unix-like system.");
    }
#endif

    if (argc < 2) {
        std::cout << UI::CYAN << " Usage: " << UI::RESET << "lpm <command> [args]\n\n";
        std::cout << UI::BOLD << " Commands:\n" << UI::RESET;
        std::cout << "   " << UI::GREEN << "init" << UI::RESET << "                      Initialize new package (my-package/)\n";
        std::cout << "   " << UI::GREEN << "package" << UI::RESET << "                   Build secured .plm package\n";
        std::cout << "   " << UI::GREEN << "install" << UI::RESET << "                   Install current project globally\n";
        std::cout << "   " << UI::GREEN << "install --lpm <file>" << UI::RESET << "      Install an .lpm/.plm archive recursively\n";
        std::cout << "   " << UI::GREEN << "uninstall <pkg>" << UI::RESET << "           Remove a package completely\n";
        std::cout << "   " << UI::GREEN << "list" << UI::RESET << "                      Show installed packages as a table\n";
        std::cout << "   " << UI::GREEN << "info <pkg>" << UI::RESET << "                Show detailed info of a package\n";
        UI::signature();
        return 1;
    }

    std::string cmd = argv[1];
    PackageManager manager;

    if (cmd == "init") {
        manager.initProject();
    } else if (cmd == "package") {
        manager.createPackage();
    } else if (cmd == "install") {
        if (argc == 4 && std::string(argv[2]) == "--lpm") {
            manager.installFromLpm(argv[3]);
        } else if (argc == 2) {
            manager.installLocal();
        } else {
            UI::logError("Invalid install syntax. Use 'lpm install' or 'lpm install --lpm <file>'");
        }
    } else if (cmd == "list") {
        manager.listPackages();
    } else if (cmd == "uninstall" && argc == 3) {
        manager.uninstall(argv[2]);
    } else if (cmd == "info" && argc == 3) {
        manager.showInfo(argv[2]);
    } else {
        UI::logError("Invalid command syntax. Run 'lpm' without args for help.");
    }

    UI::signature();
    return 0;
}

