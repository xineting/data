/*************************************************
 * HISTORY:                                      *
 *	06/22/2020 pyx : Created.                    *
 *	03/04/2021 pyx : Edited.                     *
=================================================*/

#include "Util.h"

// FIXME:WIN32 UNSUPPORT!
const char *const_basename(const char *path) {
  const char *ret = path;
  while (true) {
    char ch = *path;
    if (ch == '\0') {
      break;
    } else if (ch == '/') {
      ret = path + 1;
    }
    path++;
  }
  return ret;
}

bool isgidit(char c) {
  if (c >= '0' && c <= '9')
    return true;
  else
    return false;
}

void file_to_string(std::vector<std::string> &record, const std::string &line,
                    char delimiter) // FIXME
{
  int linepos;
  char c;
  int linemax = line.length();
  std::string curstring = "";
  record.clear();
  linepos = 0;
  bool ok = false;
  if (line[0] == '<')
    ok = true;
  while (linepos < linemax) {
    c = line[linepos];
    if (c == '.')
      ok = true;

    if (c == delimiter && isgidit(line[linepos + 1]) && ok &&
        curstring.size()) {
      record.push_back(curstring);
      curstring = "";
    } else {
      curstring += c;
    }
    ++linepos;
  }
  if (curstring.size())
    record.push_back(curstring);
  return;
}

std::string getAbsolutePath(std::string filepath) {
  assert(!filepath.empty());

  if (filepath[0] == '/')
    return filepath;

  char path[PATH_SIZE];
  if (!getcwd(path, PATH_SIZE)) {
    llvm::errs() << "Get path fail!"
                 << "\n";
    exit(1);
  }
  std::string directory = path;
  filepath = directory + "/" + filepath;
  return filepath;
}

std::string getCWD() {

  char path[PATH_SIZE];
  if (!getcwd(path, PATH_SIZE)) {
    llvm::errs() << "Get path fail!"
                 << "\n";
    exit(1);
  }
  std::string directory = path;

  return directory;
}

void trim(std::string &s) {
  if (s.empty()) {
    return;
  }
  s.erase(0, s.find_first_not_of(" "));
  s.erase(s.find_last_not_of(" ") + 1);
}