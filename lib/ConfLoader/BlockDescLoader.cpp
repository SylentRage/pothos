// Copyright (c) 2016-2016 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include <Pothos/Plugin.hpp>
#include <Poco/JSON/Object.h>
#include <Poco/JSON/Array.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Path.h>
#include <Poco/File.h>
#include <fstream>
#include <sstream>
#include <cassert>
#include <map>

/***********************************************************************
 * Load a JSON block description from file and register the descriptions.
 * return a list of registration paths and a list of paths for blocks.
 **********************************************************************/
static std::vector<Pothos::PluginPath> blockDescParser(std::istream &is, std::vector<Pothos::PluginPath> &blockPaths)
{
    std::vector<Pothos::PluginPath> entries;

    //parse the stream into a JSON array
    const auto result = Poco::JSON::Parser().parse(is);
    Poco::JSON::Array::Ptr arrayOut;
    if (result.type() == typeid(Poco::JSON::Object::Ptr))
    {
        arrayOut = new Poco::JSON::Array();
        arrayOut->add(result.extract<Poco::JSON::Object::Ptr>());
    }
    else arrayOut = result.extract<Poco::JSON::Array::Ptr>();
    for (size_t i = 0; i < arrayOut->size(); i++)
    {
        auto obj = arrayOut->getObject(i);
        assert(obj);
        std::stringstream ossJsonObj;
        obj->stringify(ossJsonObj);
        const std::string JsonObjStr(ossJsonObj.str());

        std::vector<std::string> paths;
        paths.push_back(obj->getValue<std::string>("path"));
        if (obj->has("aliases")) for (const auto &alias : *obj->getArray("aliases"))
        {
            paths.push_back(alias.toString());
        }

        //register the block description for every path
        for (const auto &path : paths)
        {
            const auto pluginPath = Pothos::PluginPath("/blocks/docs", path);
            Pothos::PluginRegistry::add(pluginPath, JsonObjStr);
            entries.push_back(pluginPath);
            blockPaths.push_back(Pothos::PluginPath("/blocks", path));
        }
    }
    return entries;
}

/***********************************************************************
 * Load a JSON block description described by a config file section
 **********************************************************************/
static std::vector<Pothos::PluginPath> blockDescLoader(const std::map<std::string, std::string> &config)
{
    //config file path set by caller
    const auto confFilePathIt = config.find("confFilePath");
    if (confFilePathIt == config.end() or confFilePathIt->second.empty())
        throw Pothos::Exception("missing confFilePath");

    //determine JSON description file path
    const auto jsonIt = config.find("json");
    if (jsonIt == config.end() or jsonIt->second.empty())
        throw Pothos::Exception("JSON file not specified");
    Poco::Path jsonPath(jsonIt->second);
    jsonPath.makeAbsolute(Poco::Path(confFilePathIt->second).makeParent());
    if (not Poco::File(jsonPath).exists())
        throw Pothos::Exception(jsonPath.toString() + " does not exist");

    //open an input file stream
    std::ifstream ifs(Poco::Path::expand(jsonPath.toString()));

    std::vector<Pothos::PluginPath> blockPaths;
    return blockDescParser(ifs, blockPaths);
}

/***********************************************************************
 * loader registration
 **********************************************************************/
pothos_static_block(pothosFrameworkRegisterBlockDescLoader)
{
    Pothos::PluginRegistry::addCall("/framework/conf_loader/block_desc", &blockDescLoader);
}
