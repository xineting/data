/*************************************************
 * HISTORY:                                      *
 *	06/22/2020 pyx : Created.                    *
 *	03/04/2021 pyx : Edited.                     *
=================================================*/

#include <assert.h>
#include <ctime>
#include <dirent.h>
#include <fstream>
#include <iostream>
#include <map>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <time.h>
#include <unistd.h>
#include <vector>

#include "clang/Tooling/Tooling.h"

#define PATH_SIZE 255

const char *const_basename(const char *path);
void file_to_string(std::vector<std::string> &record, const std::string &line,
                    char delimiter);
std::string getAbsolutePath(std::string);
std::string getCWD();
void trim(std::string &s);
