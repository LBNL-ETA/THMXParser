#include <memory>
#include <gtest/gtest.h>
#include <memory>
#include <sstream>
#include <filesystem>

#include "thmxParser.hpp"

#include "paths.h"

extern std::string test_dir;

class TestLoadCS03 : public testing::Test
{
protected:
    virtual void SetUp()
    {}
};


TEST_F(TestLoadCS03, TestLoadCS03FromDisk)
{
    std::filesystem::path product_path(test_dir);
    product_path /= "files";
    product_path /= "CS03.thmx";
    thmxParser::parseFile(product_path.string());
    
}
