#include <memory>
#include <gtest/gtest.h>
#include <filesystem>

#include "thmxParser.hpp"


TEST(TestLoadCS03, TestLoadCS03FromDisk)
{
    std::filesystem::path product_path(TEST_DATA_DIR);
    product_path /= "files";
    product_path /= "CS03.thmx";
    auto contents = thmxParser::parseFile(product_path.string());
    EXPECT_EQ(contents.fileVersion, "1");
    EXPECT_EQ(contents.boundaryConditions.size(), 4u);
    EXPECT_EQ(contents.materials.size(), 13u);
    EXPECT_EQ(contents.polygons.size(), 99u);
}
