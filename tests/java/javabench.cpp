#include <iostream>
#include <filesystem>
#include <cassert>
#include <string>
#include <chrono>
#include <vector>
#include <fstream>

#include <cellar/output.hpp>
#include <cellar/java.hpp>

class JavaFileList
{
public:
    JavaFileList(int argc, char**argv)
    {
        for(int i=0; i<argc; ++i)
        {
            auto path = argv[i];
            if(std::filesystem::is_directory(path))
            {
                for(auto p : std::filesystem::recursive_directory_iterator(path))
                {
                    TryAdd(p);
                }
            }
            else if(std::filesystem::is_regular_file(path))
            {
                TryAdd(std::filesystem::directory_entry(path));
            }
            else
            {
                // Error
            }
        }
    }

    std::vector<std::string> files;
    std::size_t totalSize = 0;
private:

    void TryAdd(const std::filesystem::directory_entry & entry)
    {
        auto path = entry.path();

        if(path.extension() == ".java")
        {
            files.push_back(path);
            totalSize += entry.file_size();
        }
    }
};

bool ReadFile(const char * path, std::vector<char> & output)
{
    std::ifstream file(path, std::ios::ate);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    output.resize(size);
    return !!file.read(output.data(), size);
}

int main(int argc, char**argv)
{
    std::cout << "Running Cellar Java benchmark\n";

    if(argc<=2)
    {
        std::cout
            << "Usage: javabench <mode> path...\n"
            << "<mode> is one of:\n"
            << "   find - find files, and report their size\n"
            << "   read - read the files but does not scan them\n"
            << "   tokenize -  lexical analysis of files\n"
            << "   parse - generates parse trees of all files\n";
        return 0; 
    }

    std::string mode = argv[1];

    auto t1 = std::chrono::high_resolution_clock::now();

    JavaFileList list { argc-2, argv+2 };

    std::cout << "Number of Java files found = " << list.files.size() << std::endl;
    std::cout << "Total bytes in Java files = " << list.totalSize << std::endl;

    if( mode=="find" )
    {
    }
    else if(mode == "read")
    {
        std::vector<char> contents;

        for(auto & p : list.files)
        {
            if(!ReadFile(p.c_str(), contents))
            {
                std::cout << "Error reading file " << p << std::endl;
            }
        }
    }
    else if(mode == "tokenize")
    {
        std::vector<char> contents;

        std::size_t number_of_tokens = 0;
        std::size_t number_of_lexer_errors = 0;
        auto lexer = cellar::java::lexer();

        for(auto & p : list.files)
        {
            if(!ReadFile(p.c_str(), contents))
            {
                std::cout << "Error reading file " << p << std::endl;
            }
            else
            {
                auto stream = lexer.tokenize(contents.data(), contents.data() + contents.size());
                while(stream.lex_including_whitespace())
                {
                    const bool show_errors = true;
                    if(show_errors && stream.token() <0 && stream.token() != cellar::Whitespace)
                    {
                        ++number_of_lexer_errors;
                        std::cout << "Lexer error in " << p << ":" << stream.begin().location.row << ": " << stream << std::endl;
                    }
                    if(stream.token()>=0)
                        ++number_of_tokens;
                }
            }
        }
        if(number_of_lexer_errors>0)
        {
            std::cout << "Found " << number_of_lexer_errors << " lexer errors\n";
        }
        std::cout << "Total number of tokens = " << number_of_tokens << std::endl;
    }
    else if(mode == "parse")
    {
        std::vector<char> contents;
        cellar::tree tree;

        auto parser = cellar::java::parser();

        for(auto & p : list.files)
        {
            if(!ReadFile(p.c_str(), contents))
            {
                std::cout << "Error reading file " << p << std::endl;
            }
            else
            {
                parser.parse(contents.data(), contents.data() + contents.size(), tree);

                if(tree)
                {
                    std::cout << "Parse success!\n";
                    std::cout << tree;
                }
                else
                {
                    std::cout << "Node I have = " << tree.root().id() << std::endl;
                    std::cout << "Syntax error at " << p << ":" << tree.errorLocation.row << ":" << tree.errorLocation.col << std::endl;
                }
            }
        }
    }
    else
    {
        std::cerr << "Unrecognised mode - must be one of: find, read, tokenize or parse\n";
        return 1;
    }

    auto t2 = std::chrono::high_resolution_clock::now();
    auto time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
    std::cout << "Total time = " << time_span.count() << "s\n";

    return 0;
}
