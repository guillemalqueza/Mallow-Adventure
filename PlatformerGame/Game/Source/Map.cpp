
#include "App.h"
#include "Render.h"
#include "Textures.h"
#include "Map.h"
#include "Physics.h"
#include "Scene.h"

#include "Defs.h"
#include "Log.h"

#include <math.h>
#include "SDL_image/include/SDL_image.h"

Map::Map() : Module(), mapLoaded(false)
{
    name.Create("map");
}

// Destructor
Map::~Map()
{}

// Called before render is available
bool Map::Awake(pugi::xml_node& config)
{
    LOG("Loading Map Parser");
    bool ret = true;

    return ret;
}

bool Map::Start() {

    //Calls the functon to load the map, make sure that the filename is assigned
    SString mapPath = path;
    mapPath += name;
    Load(mapPath);

    //Initialize the pathfinding
    pathfinding = new PathFinding();

    //Initialize the navigation map
    uchar* navigationMap = NULL;
    CreateNavigationMap(mapData.width, mapData.height, &navigationMap);
    pathfinding->SetNavigationMap((uint)mapData.width, (uint)mapData.height, navigationMap);
    RELEASE_ARRAY(navigationMap);

    return true;
}

bool Map::Update(float dt)
{
    bool ret = true;

    if (mapLoaded == false)
        return false;

    ListItem<MapLayer*>* mapLayer;
    mapLayer = mapData.layers.start;

    // iterates the layers in the map
    while (mapLayer != NULL) {
        //Check if the property Draw exist get the value, if it's true draw the lawyer
        if (mapLayer->data->properties.GetProperty("Draw") != NULL && mapLayer->data->properties.GetProperty("Draw")->value) {
            //iterate all tiles in a layer
            for (int i = 0; i < mapData.width; i++) {
                for (int j = 0; j < mapData.height; j++) {
                    //Get the gid from tile
                    int gid = mapLayer->data->Get(i, j);


                    TileSet* tileSet = GetTilesetFromTileId(gid);
                    SDL_Rect tileRect = tileSet->GetRect(gid);

                    iPoint mapCoord = MapToWorld(i, j);

                    app->render->DrawTexture(tileSet->texture, mapCoord.x, mapCoord.y, &tileRect);

                }
            }
        }
        mapLayer = mapLayer->next;
    }

    return true;
}

TileSet* Map::GetTilesetFromTileId(int gid) const
{
    TileSet* set = NULL;

    ListItem<TileSet*>* tileSet;
    tileSet = mapData.tilesets.start;
    while (tileSet != NULL) {
        set = tileSet->data;
        if (gid >= tileSet->data->firstgid && gid < (tileSet->data->firstgid + tileSet->data->tilecount)) break;
        tileSet = tileSet->next;
    }

    return set;
}

// Called before quitting
bool Map::CleanUp()
{
    LOG("Unloading map");

    //Clean up pathfing class
    pathfinding->CleanUp();

    // Remove all tilesets
    ListItem<TileSet*>* item;
    item = mapData.tilesets.start;

    while (item != NULL)
    {
        RELEASE(item->data);
        item = item->next;
    }
    mapData.tilesets.Clear();

    // Remove all layers
    ListItem<MapLayer*>* layerItem;
    layerItem = mapData.layers.start;

    while (layerItem != NULL)
    {
        RELEASE(layerItem->data);
        layerItem = layerItem->next;
    }
    mapData.layers.Clear();

    DestroyAllColliders();

    return true;
}

// Load new map
bool Map::Load(SString mapFileName)
{
    bool ret = true;

    // retrieve the paremeters of the <map> node and save it into map data

    pugi::xml_document mapFileXML;
    pugi::xml_parse_result result = mapFileXML.load_file(mapFileName.GetString());

    if (result == NULL)
    {
        LOG("Could not load map xml file %s. pugi error: %s", mapFileName, result.description());
        ret = false;
    }
    else {

        //Fill mapData variable
        mapData.width = mapFileXML.child("map").attribute("width").as_int();
        mapData.height = mapFileXML.child("map").attribute("height").as_int();
        mapData.tileWidth = mapFileXML.child("map").attribute("tilewidth").as_int();
        mapData.tileHeight = mapFileXML.child("map").attribute("tileheight").as_int();

        SString orientationStr = mapFileXML.child("map").attribute("orientation").as_string();
        if (orientationStr == "orthogonal") {
            mapData.orientation = MapOrientation::ORTOGRAPHIC;
        }
        else if (orientationStr == "isometric") {
            mapData.orientation = MapOrientation::ISOMETRIC;
        }
        else {
            LOG("Map orientation not found");
            ret = false;
        }

       // Iterate the Tileset
        for (pugi::xml_node tilesetNode = mapFileXML.child("map").child("tileset"); tilesetNode != NULL; tilesetNode = tilesetNode.next_sibling("tileset")) {

            TileSet* tileset = new TileSet();

            //Load Tileset attributes
            tileset->name = tilesetNode.attribute("name").as_string();
            tileset->firstgid = tilesetNode.attribute("firstgid").as_int();
            tileset->margin = tilesetNode.attribute("margin").as_int();
            tileset->spacing = tilesetNode.attribute("spacing").as_int();
            tileset->tileWidth = tilesetNode.attribute("tilewidth").as_int();
            tileset->tileHeight = tilesetNode.attribute("tileheight").as_int();
            tileset->columns = tilesetNode.attribute("columns").as_int();
            tileset->tilecount = tilesetNode.attribute("tilecount").as_int();

            //Load Tileset image
            SString mapTex = path;
            mapTex += tilesetNode.child("image").attribute("source").as_string();
            tileset->texture = app->tex->Load(mapTex.GetString());

            mapData.tilesets.Add(tileset);

        }

        // Iterate all layers in the TMX and load each of them
        for (pugi::xml_node layerNode = mapFileXML.child("map").child("layer"); layerNode != NULL; layerNode = layerNode.next_sibling("layer")) {

            //Load the attributes and saved in a new MapLayer
            MapLayer* mapLayer = new MapLayer();
            mapLayer->id = layerNode.attribute("id").as_int();
            mapLayer->name = layerNode.attribute("name").as_string();
            mapLayer->width = layerNode.attribute("width").as_int();
            mapLayer->height = layerNode.attribute("height").as_int();

            // Load Layer Properties
            LoadProperties(layerNode, mapLayer->properties);

            //Reserve the memory for the data 
            mapLayer->tiles = new uint[mapLayer->width * mapLayer->height];
            memset(mapLayer->tiles, 0, mapLayer->width * mapLayer->height);

            //Iterate over all the tiles and assign the values in the data array
            int i = 0;
            for (pugi::xml_node tileNode = layerNode.child("data").child("tile"); tileNode != NULL; tileNode = tileNode.next_sibling("tile")) {
                mapLayer->tiles[i] = tileNode.attribute("gid").as_uint();
                i++;
            }

            //add the layer to the map
            mapData.layers.Add(mapLayer);
        }

        //Create colliders      
        CreateColliders();

        if (ret == true)
        {
            LOG("Successfully parsed map XML file :%s", mapFileName.GetString());
            LOG("width : %d height : %d", mapData.width, mapData.height);
            LOG("tile_width : %d tile_height : %d", mapData.tileWidth, mapData.tileHeight);

            LOG("Tilesets----");

            ListItem<TileSet*>* tileset;
            tileset = mapData.tilesets.start;
            while (tileset != NULL) {
                //iterate the tilesets
                LOG("name : %s firstgid : %d", tileset->data->name.GetString(), tileset->data->firstgid);
                LOG("tile width : %d tile height : %d", tileset->data->tileWidth, tileset->data->tileHeight);
                LOG("spacing : %d margin : %d", tileset->data->spacing, tileset->data->margin);
                tileset = tileset->next;
            }

            LOG("Layers----");

            ListItem<MapLayer*>* mapLayer;
            mapLayer = mapData.layers.start;

            while (mapLayer != NULL) {
                LOG("id : %d name : %s", mapLayer->data->id, mapLayer->data->name.GetString());
                LOG("Layer width : %d Layer height : %d", mapLayer->data->width, mapLayer->data->height);
                mapLayer = mapLayer->next;
            }
        }

        // Find the navigation layer
        ListItem<MapLayer*>* mapLayerItem;
        mapLayerItem = mapData.layers.start;
        navigationLayer = mapLayerItem->data;

        //Search the layer in the map that contains information for navigation
        while (mapLayerItem != NULL) {
            if (mapLayerItem->data->properties.GetProperty("Navigation") != NULL && mapLayerItem->data->properties.GetProperty("Navigation")->value) {
                navigationLayer = mapLayerItem->data;
                break;
            }
            mapLayerItem = mapLayerItem->next;
        }

        //Resets the map
        if (mapFileXML) mapFileXML.reset();
    }

    mapLoaded = ret;
    return ret;
}

iPoint Map::MapToWorld(int x, int y) const
{
    iPoint ret;

    // L09: DONE 3: Get the screen coordinates of tile positions for isometric maps 
    if (mapData.orientation == MapOrientation::ORTOGRAPHIC) {
        ret.x = x * mapData.tileWidth;
        ret.y = y * mapData.tileHeight;
    }

    if (mapData.orientation == MapOrientation::ISOMETRIC) {
        ret.x = x * mapData.tileWidth / 2 - y * mapData.tileWidth / 2;
        ret.y = x * mapData.tileHeight / 2 + y * mapData.tileHeight / 2;
    }

    return ret;
}

bool Map::LoadProperties(pugi::xml_node& node, Properties& properties)
{
    bool ret = false;

    for (pugi::xml_node propertieNode = node.child("properties").child("property"); propertieNode; propertieNode = propertieNode.next_sibling("property"))
    {
        Properties::Property* p = new Properties::Property();
        p->name = propertieNode.attribute("name").as_string();
        p->value = propertieNode.attribute("value").as_bool(); // (!!) I'm assuming that all values are bool !!

        properties.propertyList.Add(p);
    }

    return ret;
}

Properties::Property* Properties::GetProperty(const char* name)
{
    ListItem<Property*>* property = propertyList.start;
    Property* p = NULL;

    while (property)
    {
        if (property->data->name == name) {
            p = property->data;
            break;
        }
        property = property->next;
    }

    return p;
}

iPoint Map::WorldToMap(int x, int y) {

    iPoint ret(0, 0);

    if (mapData.orientation == MapOrientation::ORTOGRAPHIC) {
        ret.x = x / mapData.tileWidth;
        ret.y = y / mapData.tileHeight;
    }

    if (mapData.orientation == MapOrientation::ISOMETRIC) {
        float half_width = mapData.tileWidth / 2;
        float half_height = mapData.tileHeight / 2;
        ret.x = int((x / half_width + y / half_height) / 2);
        ret.y = int((y / half_height - (x / half_width)) / 2);
    }

    return ret;
}

int Map::GetTileWidth() {
    return mapData.tileWidth;
}

int Map::GetTileHeight() {
    return mapData.tileHeight;
}

void Map::CreateNavigationMap(int& width, int& height, uchar** buffer) const
{
    bool ret = false;

    //Sets the size of the map. The navigation map is a unidimensional array 
    uchar* navigationMap = new uchar[navigationLayer->width * navigationLayer->height];
    //reserves the memory for the navigation map
    memset(navigationMap, 1, navigationLayer->width * navigationLayer->height);

    for (int x = 0; x < mapData.width; x++)
    {
        for (int y = 0; y < mapData.height; y++)
        {
            //i is the index of x,y coordinate in a unidimensional array that represents the navigation map
            int i = (y * navigationLayer->width) + x;

            //Gets the gid of the map in the navigation layer
            int gid = navigationLayer->Get(x, y);

            //If the gid is a blockedGid is an area that I cannot navigate, so is set in the navigation map as 0, all the other areas can be navigated
            //!!!! make sure that you assign blockedGid according to your map
            if (gid == blockedGid) navigationMap[i] = 0;
            else navigationMap[i] = 1;
        }
    }

    *buffer = navigationMap;
    width = mapData.width;
    height = mapData.height;

}

bool Map::CreateColliders()
{
    ListItem<MapLayer*>* mapLayerItem;
    mapLayerItem = mapData.layers.start;

    bool ret = false;

    while (mapLayerItem != NULL) {

        if (mapLayerItem->data->name == "Collisions")
        {
            for (int x = 0; x < mapLayerItem->data->width; x++)
            {
                for (int y = 0; y < mapLayerItem->data->height; y++)
                {
                    if (mapLayerItem->data->Get(x, y) != 0)
                    {
                        iPoint pos = MapToWorld(x, y);

                        PhysBody* c1 = nullptr;

                        switch (mapLayerItem->data->Get(x, y))
                        {
                        case 1441:
                            c1 = app->physics->CreateRectangle(pos.x + (mapData.tileWidth / 2), pos.y + (mapData.tileHeight / 2), mapData.tileWidth, mapData.tileHeight, STATIC);
                            c1->ctype = ColliderType::PLATFORM;
                            ret = true;
                            break;
                        case 1442:
							c1 = app->physics->CreateRectangle(pos.x + (mapData.tileWidth / 4), pos.y + (mapData.tileHeight / 2), mapData.tileWidth / 2, mapData.tileHeight, STATIC);
							c1->ctype = ColliderType::L_WALL;
							ret = true;
							break;
                        case 1443:
                            c1 = app->physics->CreateRectangle(pos.x + 24, pos.y + (mapData.tileHeight / 2), mapData.tileWidth / 2, mapData.tileHeight, STATIC);
                            c1->ctype = ColliderType::R_WALL;
                            ret = true;
                            break;
                        case 1445:
                            c1 = app->physics->CreateRectangleSensor(pos.x + (mapData.tileWidth / 2), pos.y + (mapData.tileHeight / 2), mapData.tileWidth, mapData.tileHeight, STATIC);
                            c1->ctype = ColliderType::SPIKE;
                            ret = true;
                            break;
                        case 1447:
                            c1 = app->physics->CreateRectangleSensor(pos.x + (mapData.tileWidth / 2), pos.y + (mapData.tileHeight / 2), mapData.tileWidth, mapData.tileHeight, STATIC);
                            c1->ctype = ColliderType::CAMERA;
                            ret = true;
                            break;
                        case 1449:
                            c1 = app->physics->CreateRectangleSensor(pos.x + (mapData.tileWidth / 2), pos.y + (mapData.tileHeight / 2), mapData.tileWidth, mapData.tileHeight, STATIC);
                            c1->ctype = ColliderType::WALL_END;
                            ret = true;
                            break;
                        default:
                            break;
                        }
                    }
                }
            }
        }
        mapLayerItem = mapLayerItem->next;
    }

    return ret;
}

void Map::DestroyAllColliders()
{
    b2World* physicsWorld = app->physics->world;
    b2Body* body = physicsWorld->GetBodyList();

    while (body)
    {
        b2Body* nextBody = body->GetNext();
        PhysBody* physBody = static_cast<PhysBody*>(body->GetUserData());

        if (physBody)
        {
            ColliderType ctype = physBody->ctype;

            // Comprueba el tipo del collider y elimina solo los tipos específicos
            if (ctype == ColliderType::CAMERA || ctype == ColliderType::SPIKE || ctype == ColliderType::R_WALL || ctype == ColliderType::L_WALL || ctype == ColliderType::PLATFORM)
            {
                physicsWorld->DestroyBody(body);
            }
        }

        body = nextBody;
    }
}
