#include <memory>
#include <gtest/gtest.h>
#include <filesystem>

#include "thmxParser.hpp"


TEST(TestLoadCMA, TestLoadCMAFromDisk)
{
    std::filesystem::path product_path(TEST_DATA_DIR);
    product_path /= "files";
    product_path /= "cma_example.thmx";
    auto contents = thmxParser::parseFile(product_path.string());
    EXPECT_EQ(contents.fileVersion, "1");
    EXPECT_EQ(contents.materials.size(), 5);
    EXPECT_EQ(contents.boundaryConditions.size(), 4);
    EXPECT_EQ(contents.polygons.size(), 5);
    EXPECT_EQ(contents.boundaryConditionPolygons.size(), 11);
}
