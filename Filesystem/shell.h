#ifndef SHELL_H
#define SHELL_H
#include <iostream>
#include "filesystem.h"

std::vector<std::string> splitString(std::string, char);

class Shell
{
public:
    Shell(Filesystem& fs, std::istream& in, std::ostream& out);
    ~Shell();
    void start();

private:
    std::istream* input;
    std::ostream* output;
    Filesystem& fs;

    std::vector<std::string> currentPath_;
    std::vector<std::string> cmdLine_;

    void append();
    void copy();
    void create();
    void import() const;
    void pwd() const;
         
    void interpret();
    void printFile(std::string file) const;
    std::vector<std::string> combine(const std::string&) const;
};

#endif
