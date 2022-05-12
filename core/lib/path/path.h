#ifndef IDE_PATH_H
#define IDE_PATH_H

#include <string>
#include <vector>

#include <iostream>

class path {
  public:
    static const std::string rootDirectory;

    /// Simplifies the given path
    /// Note: expects the path to be syntactically valid
    static std::string simplify(const std::string &path);

  private:
    static std::string getRootPath();
    static unsigned int reverseFindNth(const std::string &str, char ch, unsigned int n);
};

#endif  // IDE_PATH_H