#include "GameMap.h"
#include "Logger.h"
#include <cjson/cJSON.h>
#include <fstream>
#include <sstream>
#include <ctype.h>
#include <cstring>
#include <type_traits>

#define MAP_KEY_NAME                "name"
#define MAP_KEY_DESCR               "descr"
#define MAP_KEY_AUTHOR              "author"
#define MAP_KEY_MAP_FORM_VER        "mapFormatVer"
#define   MAP_KEY_MAP_FORM_VER_MAJ  "major"
#define   MAP_KEY_MAP_FORM_VER_MIN  "minor"
#define MAP_KEY_OBJS                "objects"
#define   MAP_KEY_OBJ_NAME          "name"
#define   MAP_KEY_OBJ_MODEL_NAME    "modelName"
#define   MAP_KEY_OBJ_TEX_NAME      "textureName"
#define   MAP_KEY_OBJ_FLAGS         "flags"
#define   MAP_KEY_OBJ_POS           "pos"
#define   MAP_KEY_OBJ_SCALE         "scale"
#define   MAP_KEY_OBJ_MODEL_ROT     "modelRot"
#define   MAP_KEY_OBJ_COLL_SHAPE    "collShape"
#define   MAP_KEY_OBJ_MASS          "mass"

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

template<typename T>
static T createVec3(const cJSON* item)
{
    static_assert(std::is_base_of<glm::vec3, T>::value || std::is_base_of<btVector3, T>::value,
            "T must be a vector3 type");

    float x{};
    {
        const cJSON* xJson = cJSON_GetObjectItem(item, "x");
        checkItemType<JsonType::Number>(xJson);
        x = cJSON_GetNumberValue(xJson);
    }

    float y{};
    {
        const cJSON* yJson = cJSON_GetObjectItem(item, "y");
        checkItemType<JsonType::Number>(yJson);
        y = cJSON_GetNumberValue(yJson);
    }

    float z{};
    {
        const cJSON* zJson = cJSON_GetObjectItem(item, "z");
        checkItemType<JsonType::Number>(zJson);
        z = cJSON_GetNumberValue(zJson);
    }

    return {x, y, z};
}

static btCollisionShape* createCollShape(const cJSON* item)
{
    const cJSON* typeJson = cJSON_GetObjectItem(item, "type");
    checkItemType<JsonType::String>(typeJson);
    char* typeStr = cJSON_GetStringValue(typeJson);
    assert(*typeStr != 0);
    {
        char* c = typeStr;
        while (*c != 0)
            *c++ = tolower(*c);
    }

    btCollisionShape* collShape{};

    if (std::strcmp(typeStr, "sphere") == 0)
    {
        const cJSON* radJson = cJSON_GetObjectItem(item, "radius");
        checkItemType<JsonType::Number>(radJson);

        collShape = new btSphereShape{(btScalar)cJSON_GetNumberValue(radJson)};
    }
    else if (std::strcmp(typeStr, "box") == 0)
    {
        const cJSON* sizeJson = cJSON_GetObjectItem(item, "size");
        checkItemType<JsonType::Object>(sizeJson);

        collShape = new btBoxShape{createVec3<btVector3>(sizeJson)};
    }
    else
    {
        throw std::runtime_error{std::string("Invalid collision shape: \"")+typeStr+'"'};
    }

    free(typeStr);
    return collShape;
}

static GameObject::flag_t createFlag(const cJSON* item)
{
    (void)item;
    return GameObject::defaultFlags; // TODO
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
        checkItemType<JsonType::Object>(obj);

        ObjectDescr* newObj = new ObjectDescr{}; // Do not free, it will be moved to a smart ptr

        if (const cJSON* nameJson = cJSON_GetObjectItem(obj, MAP_KEY_OBJ_NAME))
        {
            checkItemType<JsonType::String>(nameJson);
            newObj->objName = cJSON_GetStringValue(nameJson);
        }

        {
            const cJSON* modelNameJson = cJSON_GetObjectItem(obj, MAP_KEY_OBJ_MODEL_NAME);
            checkItemType<JsonType::String>(modelNameJson);
            newObj->modelName = cJSON_GetStringValue(modelNameJson);
        }

        {
            const cJSON* texNameJson = cJSON_GetObjectItem(obj, MAP_KEY_OBJ_TEX_NAME);
            checkItemType<JsonType::String>(texNameJson);
            newObj->textureName = cJSON_GetStringValue(texNameJson);
        }

        if (const cJSON* flagsJson = cJSON_GetObjectItem(obj, MAP_KEY_OBJ_FLAGS))
        {
            checkItemType<JsonType::String>(flagsJson);
            newObj->flags = createFlag(flagsJson);
        }

        {
            const cJSON* posJson = cJSON_GetObjectItem(obj, MAP_KEY_OBJ_POS);
            checkItemType<JsonType::Object>(posJson);
            newObj->pos = createVec3<glm::vec3>(posJson);
        }

        if (const cJSON* scaleJson = cJSON_GetObjectItem(obj, MAP_KEY_OBJ_SCALE))
        {
            checkItemType<JsonType::Object>(scaleJson);
            newObj->scale = createVec3<glm::vec3>(scaleJson); // TODO: Check if values are positive
        }

        if (const cJSON* modelRotJson = cJSON_GetObjectItem(obj, MAP_KEY_OBJ_MODEL_ROT))
        {
            checkItemType<JsonType::Object>(modelRotJson);
            newObj->modelRotDeg = createVec3<glm::vec3>(modelRotJson); // TODO: Check if values are positive
        }

        if (const cJSON* collShapeJson = cJSON_GetObjectItem(obj, MAP_KEY_OBJ_COLL_SHAPE))
        {
            checkItemType<JsonType::Object>(collShapeJson);
            newObj->collShape.reset(createCollShape(collShapeJson));
        }

        if (const cJSON* massJson = cJSON_GetObjectItem(obj, MAP_KEY_OBJ_MASS))
        {
            checkItemType<JsonType::Number>(massJson);
            // TODO: Support constant: STATIC
            newObj->mass = cJSON_GetNumberValue(massJson); // TODO: Check if value is positive
        }

        m_objects.emplace_back(newObj);
    }

    cJSON_free(json);

    Logger::log << "Loaded map" << Logger::End;
    Logger::verb << "----- Map info -----" << '\n';
    Logger::verb << '\n' << "Name: " << m_name;
    Logger::verb << '\n' << "Description: " << m_descr;
    Logger::verb << '\n' << "Author: " << m_author;
    Logger::verb << '\n' << "Map format version: " << mapFormatVerToStr(m_mapFormatVer);
    Logger::verb << '\n' << "Number of objects: " << m_objects.size();
    Logger::verb << Logger::End;
}
