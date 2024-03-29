#include <memory>
#include <gtest/gtest.h>
#include <memory>
#include <sstream>
#include <filesystem>

#include "thmxParser.hpp"

#include "paths.h"

extern std::string test_dir;

class TestLoadCMA : public testing::Test
{
protected:
    virtual void SetUp()
    {}
};


TEST_F(TestLoadCMA, TestLoadCMAFromDisk)
{
    std::filesystem::path product_path(test_dir);
    product_path /= "files";
    product_path /= "cma_example.thmx";
    auto contents = thmxParser::parseFile(product_path.string());
	EXPECT_EQ(contents.fileVersion, "1");
	EXPECT_EQ(contents.materials.size(), 5);
	EXPECT_EQ(contents.boundaryConditions.size(), 4);
	EXPECT_EQ(contents.polygons.size(), 5);
	EXPECT_EQ(contents.boundaryConditionPolygons.size(), 11);
}
