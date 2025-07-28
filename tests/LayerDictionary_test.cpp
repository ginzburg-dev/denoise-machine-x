#include <gtest/gtest.h>

#include <dmxdenoiser/LayerDictionary.hpp>

#include <iostream>
#include <string>
#include <vector>

using namespace dmxdenoiser;

TEST(LayerDictionary, CreateLayerDictionaryWithVector)
{
    std::vector<std::string> aovsRef{"beauty", "albedo", "normal", "depth", "position", "reflection", "specular"};
    LayerDictionary dict{std::vector<std::string>{ "normal", "albedo", "beauty", "position", "depth", "reflection", "specular"}};
    auto layerList = dict.data();
    for (int i = 0; i < layerList.size(); ++i)
    {
        EXPECT_EQ(aovsRef[i], layerList[i].name);
        EXPECT_EQ(i, layerList[i].offset);
    }
}

TEST(LayerDictionary, CreateLayerDictionaryWithInputFilesMap)
{
    std::vector<std::string> aovsRef{"beauty", "albedo", "normal", "depth"};
    InputFilesMap filesMap{ 
        {"file1.exr", {{"albedo", "albedo"}, {"beauty","default"}}},
        {"file2.exr", {{"depth","default"}, {"normal", "normal"}}}
    };
    LayerDictionary dict{filesMap};
    auto layerList = dict.data();
    for (int i = 0; i < layerList.size(); ++i)
    {
        EXPECT_EQ(aovsRef[i], layerList[i].name);
        EXPECT_EQ(i, layerList[i].offset);
    }
}

TEST(LayerDictionary, AddDublicatedLayerToDictionary)
{
    std::vector<std::string> aovsRef{"beauty", "albedo", "normal", "depth", "position", "reflection", "specular"};
    LayerDictionary dict{std::vector<std::string>{ "normal", "albedo", "beauty", "position", "depth", "reflection", "specular", "normal"}};
    auto layerList = dict.data();
    for (int i = 0; i < layerList.size(); ++i)
    {
        EXPECT_EQ(aovsRef[i], layerList[i].name);
        EXPECT_EQ(i, layerList[i].offset);
    }
}

TEST(LayerDictionary, AddLayerToDictionary)
{
    std::vector<std::string> aovsRef{"beauty", "albedo", "normal", "depth", "reflection", "specular"};
    LayerDictionary dict{std::vector<std::string>{ "normal", "beauty", "depth", "reflection", "specular"}};
    dict.addLayer("albedo");
    auto layerList = dict.data();
    for (int i = 0; i < layerList.size(); ++i)
    {
        EXPECT_EQ(aovsRef[i], layerList[i].name);
        EXPECT_EQ(i, layerList[i].offset);
    }
}

TEST(LayerDictionary, AddLayerOverrides)
{
    LayerDictionary dict{std::vector<std::string>{"beauty", "normal"}};
    dict.addLayer("normal");
    auto layers = dict.data();

    ASSERT_EQ(layers.size(), 2);
    EXPECT_EQ(layers[1].name, "normal");
}

TEST(LayerDictionary, RemoveLayerFromDictionary)
{
    std::vector<std::string> aovsRef{"beauty", "albedo", "normal", "depth", "position"};
    LayerDictionary dict{std::vector<std::string>{ "normal", "beauty", "position", "albedo", "depth", "reflection", "specular"}};
    dict.removeLayer("specular");
    dict.removeLayer("reflection");
    auto layerList = dict.data();
    for (int i = 0; i < layerList.size(); ++i)
    {
        EXPECT_EQ(aovsRef[i], layerList[i].name);
        EXPECT_EQ(i, layerList[i].offset);
    }
}

TEST(LayerDictionary, GetLayerOffset)
{
    LayerDictionary dict{std::vector<std::string>{ "normal", "beauty", "position", "albedo", "depth", "reflection", "specular"}};
    ASSERT_TRUE(dict.getLayerOffset("beauty"));
    EXPECT_EQ(0, dict.getLayerOffset("beauty"));

    ASSERT_TRUE(dict.getLayerOffset("albedo"));
    EXPECT_EQ(1, dict.getLayerOffset("albedo"));

    ASSERT_TRUE(dict.getLayerOffset("normal"));
    EXPECT_EQ(2, dict.getLayerOffset("normal"));

    ASSERT_TRUE(dict.getLayerOffset("depth"));
    EXPECT_EQ(3, dict.getLayerOffset("depth"));

    ASSERT_TRUE(dict.getLayerOffset("position"));
    EXPECT_EQ(4, dict.getLayerOffset("position"));

    ASSERT_TRUE(dict.getLayerOffset("reflection"));
    EXPECT_EQ(5, dict.getLayerOffset("reflection"));

    ASSERT_TRUE(dict.getLayerOffset("specular"));
    EXPECT_EQ(6, dict.getLayerOffset("specular"));
}

TEST(LayerDictionary, GetLayerOffsetFromEmptyDict)
{
    LayerDictionary dict{};
    ASSERT_FALSE(dict.getLayerOffset("beauty"));
}

TEST(LayerDictionary, GetLayerFromDictionary)
{
    LayerDictionary dict{std::vector<std::string>{ "normal", "beauty", "position", "albedo", "depth", "reflection", "specular"}};
    ASSERT_TRUE(dict.getLayer("beauty"));
    EXPECT_EQ(dict.getLayer("beauty")->name, "beauty");
    EXPECT_EQ(dict.getLayer("beauty")->offset, 0);
}

TEST(LayerDictionary, GetLayerFromEmptyDict)
{
    LayerDictionary dict{};
    ASSERT_FALSE(dict.getLayer("beauty"));
}
