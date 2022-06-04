#include "GameMap.h"
#include "Logger.h"
#include <cjson/cJSON.h>
#include <fstream>
#include <sstream>

#define MAP_KEY_NAME                "name"
#define MAP_KEY_DESCR               "descr"
#define MAP_KEY_AUTHOR              "author"
#define MAP_KEY_MAP_FORM_VER        "mapFormatVer"
#define MAP_KEY_MAP_FORM_VER_MAJ    "major"
#define MAP_KEY_MAP_FORM_VER_MIN    "minor"
#define MAP_KEY_OBJS                "objects"

enum class JsonType
{
    Bool,
    Null,
    Number,
    String,
    Array,
    Object,
    Raw,
};

template <JsonType TypeVal>
static void checkItemType(const cJSON* item)
{
    static constexpr const char* typeNames[] = {
        "Bool",
        "Null",
        "Number",
        "String",
        "Array",
        "Object",
        "Raw",
    };

    if constexpr (TypeVal == JsonType::Bool)
    {
        if (cJSON_IsBool(item)) return;
    }
    else if constexpr (TypeVal == JsonType::Null)
    {
        if (cJSON_IsNull(item)) return;
    }
    else if constexpr (TypeVal == JsonType::Number)
    {
        if (cJSON_IsNumber(item)) return;
    }
    else if constexpr (TypeVal == JsonType::String)
    {
        if (cJSON_IsString(item)) return;
    }
    else if constexpr (TypeVal == JsonType::Array)
    {
        if (cJSON_IsArray(item)) return;
    }
    else if constexpr (TypeVal == JsonType::Object)
    {
        if (cJSON_IsObject(item)) return;
    }
    else if constexpr (TypeVal == JsonType::Raw)
    {
        if (cJSON_IsRaw(item)) return;
    }
    else
    {
        assert(0 && "Tried to check for invalid JSON type");
        abort();
    }

    const std::string itemStr = (item ? cJSON_Print(item) : "<nullptr>");
    throw std::runtime_error(std::string("Expected JSON item of type ")
            +typeNames[(int)TypeVal]+", but got something else: \'"
            +itemStr+'\'');
}

static GameMap::MapFormatVer createMapFormatVer(const cJSON* item)
{
    checkItemType<JsonType::Object>(item);

    auto isWhole{[](double x){ // -> bool
        return round(x) == x;
    }};

    uint major{};
    {
        const cJSON* majorJson = cJSON_GetObjectItem(item, MAP_KEY_MAP_FORM_VER_MAJ);
        checkItemType<JsonType::Number>(majorJson);
        const double num = cJSON_GetNumberValue(majorJson);
        if (!isWhole(num))
        {
            throw std::runtime_error{MAP_KEY_MAP_FORM_VER ":" MAP_KEY_MAP_FORM_VER_MAJ
                ": Expected value of type int, got float"};
        }
        major = num;
    }

    uint minor{};
    {
        const cJSON* minorJson = cJSON_GetObjectItem(item, MAP_KEY_MAP_FORM_VER_MIN);
        checkItemType<JsonType::Number>(minorJson);
        const double num = cJSON_GetNumberValue(minorJson);
        if (!isWhole(num))
        {
            throw std::runtime_error{MAP_KEY_MAP_FORM_VER ":" MAP_KEY_MAP_FORM_VER_MIN
                ": Expected value of type int, got float"};
        }
        minor = num;
    }

    return {.major = major, .minor = minor};

}

static btCollisionShape* createCollShape(const cJSON* item)
{
    return nullptr; // TODO
}

static glm::vec3 createVec3(const cJSON* item)
{
    return {}; // TODO
}

static std::string readFile(const std::string& path)
{
    std::fstream file;
    file.open(path, std::ios_base::in);
    if (!file.is_open())
        assert(false); // TODO

    std::stringstream ss;
    ss << file.rdbuf();
    file.close();
    return ss.str();
}

static void printParseErr(const std::string& content, size_t errPos)
{
    assert(errPos < content.length());

    std::stringstream ss;
    ss << content;
    size_t pos{};
    size_t lineI{};
    std::string line;
    while (std::getline(ss, line))
    {
        if (errPos >= pos && errPos < pos+line.length()+1)
        {
            Logger::err << "Error in map JSON: in line " << lineI+1 << ", col " << errPos-pos+1 << "\n"
                << "\t: " << line << "\n"
                << "\t: " << std::string(errPos-pos, ' ') << "^" << Logger::End;
            break;
        }
        pos += line.length()+1;
        ++lineI;
    }
}

static std::string mapFormatVerToStr(const GameMap::MapFormatVer& val)
{
    return std::to_string(val.major) + '.' + std::to_string(val.minor);
}

GameMap::GameMap(const std::string& path)
{
#if 0 // ----- TEST -----
    static constexpr float floorScale = 20.0f;
    //int x = 0;
    //int y = 0;
    //for (int x{-100}; x <= 100; ++x)
    //{
    //    for (int y{-100}; y <= 100; ++y)
    //    {
            m_objects.push_back(std::unique_ptr<ObjectDescr>{new ObjectDescr{
                    .objName = "Plane",
                    .modelName = "plane.obj",
                    .textureName = "floor_tiled_stone/square_floor_diff_4k.jpg",
                    .pos = {
                        //x*floorScale,
                        0.0f,
                        0.0f,
                        //y*floorScale,
                        0.0f,
                    },
                    .scale = {
                        floorScale,
                        1.0f,
                        floorScale,
                    },
                    .collShape{new btBoxShape{btVector3{floorScale/2, .0005f, floorScale/2}}},
                    .mass = 0.f,
            }});
    //    }
    //}

    //m_objects.push_back(std::unique_ptr<ObjectDescr>{new ObjectDescr{
    //        .objName = "Teapot 1",
    //        .modelName = "teapot.obj",
    //        .textureName = "concrete/4K-concrete_50_basecolor.png",
    //        .pos = {
    //            2.5f,
    //            100.0f,
    //            2.3f,
    //        },
    //        .scale = {
    //            //0.05f,
    //            //0.05f,
    //            //0.05f,
    //            1.0f,
    //            1.0f,
    //            1.0f,
    //        },
    //        .collShape = new btBoxShape{btVector3{1.f, 1.f, 1.f}},
    //        //.collShape = new btSphereShape{0.5f},
    //        .mass = 1.f,
    //}});

    //m_objects.push_back(std::unique_ptr<ObjectDescr>{new ObjectDescr{
    //        .objName = "Chair",
    //        .modelName = "chair.obj",
    //        .textureName = "afromosia/2K_afromosia_basecolor.png",
    //        .pos = {
    //            2.0f,
    //            0.0f,
    //            4.0f,
    //        },
    //        .scale = {
    //            1.0f,
    //            1.0f,
    //            1.0f,
    //        },
    //}});

    for (int x{}; x < 6; ++x)
    {
        for (int z{}; z < 6; ++z)
        {
            for (int y{}; y < 10; ++y)
            {
                m_objects.push_back(std::unique_ptr<ObjectDescr>{new ObjectDescr{
                        .objName = "Cube",
                        .modelName = "cube.obj",
                        .textureName = "private/gaben.jpeg",
                        .pos = {
                            0.0f+x*1.01f,
                            0.0f+y*1.01f,
                            0.0f+z*1.01f,
                        },
                        .scale = {
                            1.0f,
                            1.0f,
                            1.0f,
                        },
                        .modelRotDeg = {
                            90.0f,
                            90.0f,
                            0.0f,
                        },
                        .collShape{new btBoxShape{btVector3{.5f, .5f, .5f}}},
                        .mass = 1.f,
                }});
            }
#if 0
        }
    }
#endif

    m_objects.push_back(std::unique_ptr<ObjectDescr>{new ObjectDescr{
            .objName = "Sphere",
            .modelName = "ico_sphere.obj",
            .textureName = "private/hl3.jpg",
            .pos = {
                3.0f,
                100.0f,
                3.0f,
            },
            .scale = {
                4.0f,
                4.0f,
                4.0f,
            },
            .collShape{new btSphereShape{1.85f}},
            .mass = 10.0f,
    }});

    m_objects.push_back(std::unique_ptr<ObjectDescr>{new ObjectDescr{
            .objName = "Quake Guy",
            .modelName = "private/player.obj",
            .textureName = "private/player_0.png",
            .pos = {
                2.0f,
                11.0f,
                0.0f,
            },
            .scale = {
                0.05f,
                0.05f,
                0.05f,
            },
            .collShape{new btBoxShape{{0.6f, 1.2f, 0.4f}}},
            .mass=1.0f,
    }});
#endif

    Logger::log << "Loading map: \"" << path << '"' << Logger::End;

    const std::string fileContent = readFile(path);
    const char* fileContentPtr = fileContent.c_str();

    cJSON* json = cJSON_Parse(fileContentPtr);
    if (!json)
    {
        printParseErr(fileContent, cJSON_GetErrorPtr()-fileContentPtr);
        throw std::runtime_error{"Error in map file"};
    }

    if (!cJSON_IsObject(json))
    {
        Logger::err << "Root is not an Object" << Logger::End;
        throw std::runtime_error{"Error in map file"};
    }

    {
        const cJSON* nameJson = cJSON_GetObjectItem(json, MAP_KEY_NAME);
        checkItemType<JsonType::String>(nameJson);
        m_name = cJSON_GetStringValue(nameJson);
    }

    if (const cJSON* descrJson = cJSON_GetObjectItem(json, MAP_KEY_DESCR)) // Don't check if not given
    {
        checkItemType<JsonType::String>(descrJson);
        m_descr = cJSON_GetStringValue(descrJson);
    }

    if (const cJSON* authorJson = cJSON_GetObjectItem(json, MAP_KEY_AUTHOR)) // Don't check if not given
    {
        checkItemType<JsonType::String>(authorJson);
        m_author = cJSON_GetStringValue(authorJson);
    }

    {
        const cJSON* formatVerJson = cJSON_GetObjectItem(json, MAP_KEY_MAP_FORM_VER);
        checkItemType<JsonType::Object>(formatVerJson);
        m_mapFormatVer = createMapFormatVer(formatVerJson);
    }

    const cJSON* objs = cJSON_GetObjectItem(json, MAP_KEY_OBJS);
    checkItemType<JsonType::Array>(objs);
    Logger::verb << "\"" MAP_KEY_OBJS "\" array length: "+std::to_string(cJSON_GetArraySize(objs)) << Logger::End;
    const cJSON* obj{};
    cJSON_ArrayForEach(obj, objs)
    {
    }

    Logger::log << "Loaded map" << Logger::End;
    Logger::verb << "----- Map info -----" << '\n';
    Logger::verb << '\n' << "Name: " << m_name;
    Logger::verb << '\n' << "Description: " << m_descr;
    Logger::verb << '\n' << "Author: " << m_author;
    Logger::verb << '\n' << "Map format version: " << mapFormatVerToStr(m_mapFormatVer);
    Logger::verb << '\n' << "Number of objects: " << m_objects.size();
    Logger::verb << Logger::End;
}
