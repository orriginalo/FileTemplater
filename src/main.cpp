#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <map>

#include "../include/argparser.h"

using namespace std;
namespace fs = std::filesystem;

const string VERSION = "0.1.0";

fs::path templatesMainPath;
fs::path templatesPath;

map<fs::path, vector<fs::path>> templatesMap;

fs::path createTemplatesInLocalAppData(fs::path &pathToMainFolder) {
  if (!fs::exists(pathToMainFolder)) {
    fs::create_directory(pathToMainFolder);
    fs::create_directory(pathToMainFolder / "templates");
  }
  if (!fs::exists(pathToMainFolder / "templates")) {
    fs::create_directory(pathToMainFolder / "templates");
  }
  return pathToMainFolder / "templates";
}

fs::path getTemplatesPath() {
  string username = getenv("USERNAME");
  fs::path path   = "C:\\Users\\" + username + "\\AppData\\Local\\FileTemplater";
  return path;
}

void getExistsTemplates(fs::path &templatesPath) {
  for (const auto &entry : fs::directory_iterator(templatesPath)) {
    if (entry.is_directory()) {
      if (!fs::is_empty(entry.path())) {
        vector<fs::path> templateFiles;
        for (const auto &entry2 : fs::directory_iterator(entry.path())) {
          templateFiles.push_back(entry2.path());
        }
        templatesMap[entry.path()] = templateFiles;
      }
    }
  }
}

void listAllTemplates(map<fs::path, vector<fs::path>> &templatesMap) {
  for (const auto &[key, value] : templatesMap) {
    cout << "──────────────" << endl;
    cout << "\033[1;37m" << key.filename().string() << "\033[0m" << endl;
    for (int i = 0; i < value.size(); i++) {
      if (i != value.size() - 1) {
        cout << "├─ " << value[i].filename().string() << endl;
      } else {
        cout << "└─ " << value[i].filename().string() << endl;
      }
    }
  }
}

void printHelp() {
  cout << "\033[1;37m" << "Usage:" << "\033[0m" << endl;
  cout << "filetemplater" << "\033[3;37m" << " [template] [destination]" << "\033[0m" << " - copy files from template to destination" << endl;
  cout << "filetemplater" << "\033[3;37m" << " [template]" << "\033[0m" << " - list all files in the template" << endl;
  cout << "filetemplater" << "\033[3;37m" << " [options]" << "\033[0m" << " - see options" << endl;
  cout << "\033[1;37m" << "Options:" << "\033[0m" << endl;
  cout << "-h, --help - show this help" << endl;
  cout << "-v, --version - show version" << endl;
  cout << "-l, --list - show all templates" << endl;
  cout << "-m, --make `src` `alias` - create template from `src` with `alias`" << endl;
  exit(0);
}

void checkArgs(int argc, char *argv[]) {
  if (argc > 1) {
    if (checkArg(argc, argv, "-h", "--help")) {
      printHelp();
    }
    if (checkArg(argc, argv, "-v", "--version")) {
      cout << "Version: " << VERSION << endl;
      exit(0);
    }
    if (checkArg(argc, argv, "-l", "--list")) {
      listAllTemplates(templatesMap);
      exit(0);
    }
    if (argc == 2) {
      for (const auto &[key, value] : templatesMap) {
        if (key.filename().string() == argv[1]) {
          cout << "\033[1;37m" << key.filename().string() << "\033[0m" << endl;
          for (int i = 0; i < value.size(); i++) {
            if (i != value.size() - 1) {
              cout << "├─ " << value[i].filename().string() << endl;
            } else {
              cout << "└─ " << value[i].filename().string() << endl;
            }
          }
        }
      }
    }
    if (string(argv[1]) == "-m" || string(argv[1]) == "--make") {
      if (argc == 4) {
        fs::path src = fs::absolute(argv[2]);
        fs::path dst = templatesPath / argv[3];
        if (!fs::exists(dst)) {
          fs::create_directory(dst);
        }
        cout << "\033[1;37m" << "Creating template: " << "\"" << dst.filename().string() << "\"" << "\033[0m" << endl;
        for (const auto &entry : fs::directory_iterator(src)) {
          try {
            cout << "\033[1;37m" << "Copying file: " << "\033[1;33m" << entry.path().filename().string() << "\033[0m";
            copy(entry.path(), dst / entry.path().filename());
            cout << "\033[1;32m" << " | Done!" << "\033[0m" << endl;
          } catch (const fs::filesystem_error &e) {
            // cout << "\033[1;31m" << "Error copying file: " << "\033[0m" << e.what() << endl;
            cout << "\033[1;31m" << " | Error." << "\033[0m" << endl;
          }
        }
      } else {
        cout << "\033[1;37m" << "Usage:" << "\033[0m" << " filetemplater --make" << "\033[3;37m" << " [src] [alias]" << "\033[0m" << endl;
      }
    }
    if (string(argv[1]) == "-r" || string(argv[1]) == "--remove") {
      if (argc == 3) {
        fs::path dst = templatesPath / argv[2];
        if (fs::exists(dst)) {
          cout << "\033[1;37m" << "Removing template: " << "\033[1;33m" << dst.filename().string() << "\033[0m ";
          fs::remove_all(dst);
          cout << "\033[1;92m" << "| Done!" << "\033[0m" << endl;
        } else {
          cout << "\033[1;31m" << "Template doesn't exist: " << "\033[0m" << dst.string() << endl;
        }
      }
    }
  }
  if (argc > 2) {
    bool isTemplate = false;
    vector<fs::path> templateFiles;
    for (const auto &[templateFolder, containsFiles] : templatesMap) {
      if (templateFolder.filename().string() == argv[1]) {
        isTemplate = true;
        for (int i = 0; i < containsFiles.size(); i++) {
          templateFiles.push_back(containsFiles[i]);
        }
      }
    }
    fs::path dst;
    if (isTemplate) {
      if (fs::exists(argv[2])) {
        if (argv[2] == ".") {
          dst = fs::current_path();
        } else {
          dst = fs::absolute(argv[2]);
        }
      } else {
        cout << "Path doesn't exist: " << argv[2] << endl;
        exit(1);
      }
      cout << "\033[1;37m" << "Copying files into: " << "\033[1;33m" << dst.string() << "\033[0m" << endl;
      for (const auto &file : templateFiles) {
        try {
          fs::copy(file, dst / file.filename(), fs::copy_options::overwrite_existing);
        } catch (const fs::filesystem_error &e) {
          cout << "\033[1;31m" << "Error copying file: " << "\033[0m" << e.what() << endl;
          exit(1);
        }
      }
      cout << "\033[1;32m" << "Done!" << "\033[0m" << endl;
    }
  }
}

int main(int argc, char *argv[]) {
  setlocale(LC_ALL, "en_US.UTF-8");

  templatesMainPath = getTemplatesPath();
  templatesPath     = createTemplatesInLocalAppData(templatesMainPath);
  getExistsTemplates(templatesPath);

  checkArgs(argc, argv);

  if (argc == 1) {
    printHelp();
  }

  return 0;
}