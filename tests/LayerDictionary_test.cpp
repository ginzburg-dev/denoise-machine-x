#include <gtest/gtest.h>

#include <dmxdenoiser/LayerDictionary.hpp>

#include <algorithm>
#include <array>
#include <cstddef>
#include <iostream>
#include <string>
#include <vector>

using namespace dmxdenoiser;

void checkOffsets(const LayerDictionary& dict)
{
    std::size_t size = dict.size();
    for (int i = 0; i < size; ++i)
    {
        auto it = std::find_if(dict.data().begin(), dict.data().end(), 
            [&](const auto& pair){
                if (pair.second.offset == i)
                    return true;
                return false;
            });
        ASSERT_TRUE(it != dict.data().end());
            
    }
}

TEST(LayerDictionary, CreateLayerDictionaryWithVector)
{
    std::vector<std::string> aovsRef{"beauty", "albedo", "normal", "depth", "position", "reflection", "specular"};
    std::vector<std::string> layerList{ "normal", "albedo", "beauty", "position", "depth", "reflection", "specular"};
    LayerDictionary dict{layerList};
    for (int i = 0; i < aovsRef.size(); ++i)
    {
        ASSERT_TRUE(dict.getLayer(aovsRef[i]));
        EXPECT_EQ(aovsRef[i], dict.getLayer(aovsRef[i])->name);
        std::cout << dict.getLayer(aovsRef[i])->name << ' ' << dict.getLayer(aovsRef[i])->offset << '\n';
    }
    EXPECT_EQ(dict.size(), 7);
    checkOffsets(dict);
}

TEST(LayerDictionary, CreateLayerDictionaryWithInputFilesMap)
{
    std::vector<std::string> aovsRef{"beauty", "albedo", "normal", "depth"};
    InputFilesMap filesMap{ 
        {"file1.exr", {{"albedo", "albedo"}, {"beauty","default"}}},
        {"file2.exr", {{"depth","default"}, {"normal", "normal"}}}
    };
    LayerDictionary dict{filesMap};
    for (int i = 0; i < aovsRef.size(); ++i)
    {
        ASSERT_TRUE(dict.getLayer(aovsRef[i]));
        EXPECT_EQ(aovsRef[i], dict.getLayer(aovsRef[i])->name);
        std::cout << dict.getLayer(aovsRef[i])->name << ' ' << dict.getLayer(aovsRef[i])->offset << '\n';
    }
    checkOffsets(dict);
}

TEST(LayerDictionary, AddDublicatedLayerToDictionary)
{
    std::vector<std::string> aovsRef{"beauty", "albedo", "normal", "depth", "position", "reflection", "specular"};
    LayerDictionary dict{std::vector<std::string>{ "normal", "albedo", "beauty", "position", "depth", "reflection", "specular", "normal"}};
    for (int i = 0; i < aovsRef.size(); ++i)
    {
        ASSERT_TRUE(dict.getLayer(aovsRef[i]));
        EXPECT_EQ(aovsRef[i], dict.getLayer(aovsRef[i])->name);
        std::cout << dict.getLayer(aovsRef[i])->name << ' ' << dict.getLayer(aovsRef[i])->offset << '\n';
    }
    checkOffsets(dict);
}

TEST(LayerDictionary, AddLayerToDictionary)
{
    std::vector<std::string> aovsRef{"beauty", "albedo", "normal", "depth", "reflection", "specular"};
    LayerDictionary dict{std::vector<std::string>{ "normal", "beauty", "depth", "reflection", "specular"}};
    dict.addLayer("albedo");
    for (int i = 0; i < aovsRef.size(); ++i)
    {
        ASSERT_TRUE(dict.getLayer(aovsRef[i]));
        EXPECT_EQ(aovsRef[i], dict.getLayer(aovsRef[i])->name);
        std::cout << dict.getLayer(aovsRef[i])->name << ' ' << dict.getLayer(aovsRef[i])->offset << '\n';
    }
    checkOffsets(dict);
}

TEST(LayerDictionary, AddLayerOverrides)
{
    std::vector<std::string> aovsRef{"beauty", "normal"};
    LayerDictionary dict{std::vector<std::string>{"beauty", "normal"}};
    dict.addLayer("normal");
    for (int i = 0; i < aovsRef.size(); ++i)
    {
        ASSERT_TRUE(dict.getLayer(aovsRef[i]));
        EXPECT_EQ(aovsRef[i], dict.getLayer(aovsRef[i])->name);
        std::cout << dict.getLayer(aovsRef[i])->name << ' ' << dict.getLayer(aovsRef[i])->offset << '\n';
    }
    checkOffsets(dict);
}

TEST(LayerDictionary, RemoveLayerFromDictionary)
{
    std::vector<std::string> aovsRef{"beauty", "albedo", "normal", "depth", "position"};
    LayerDictionary dict{std::vector<std::string>{ "normal", "beauty", "position", "albedo", "depth", "reflection", "specular"}};
    dict.removeLayer("specular");
    dict.removeLayer("reflection");
    for (int i = 0; i < aovsRef.size(); ++i)
    {
        ASSERT_TRUE(dict.getLayer(aovsRef[i]));
        EXPECT_EQ(aovsRef[i], dict.getLayer(aovsRef[i])->name);
        std::cout << dict.getLayer(aovsRef[i])->name << ' ' << dict.getLayer(aovsRef[i])->offset << '\n';
    }
    checkOffsets(dict);
}

TEST(LayerDictionary, GetLayerOffset)
{
    LayerDictionary dict{std::vector<std::string>{ "normal", "beauty", "position", "albedo", "depth", "reflection", "specular"}};
    ASSERT_TRUE(dict.getLayer("beauty"));
    ASSERT_TRUE(dict.getLayer("albedo"));
    ASSERT_TRUE(dict.getLayer("normal"));
    ASSERT_TRUE(dict.getLayer("depth"));
    ASSERT_TRUE(dict.getLayer("position"));
    ASSERT_TRUE(dict.getLayer("reflection"));
    ASSERT_TRUE(dict.getLayer("specular"));
    checkOffsets(dict);
}

TEST(LayerDictionary, GetLayerFromEmptyDict)
{
    LayerDictionary dict{};
    ASSERT_FALSE(dict.getLayer("beauty"));
}

TEST(LayerDictionary, GetLayerFromDictionary)
{
    LayerDictionary dict{std::vector<std::string>{ "normal", "beauty", "position", "albedo", "depth", "reflection", "specular"}};
    ASSERT_TRUE(dict.getLayer("beauty"));
    EXPECT_EQ(dict.getLayer("beauty")->name, "beauty");
    checkOffsets(dict);
}
