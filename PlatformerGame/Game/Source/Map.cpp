
#include "App.h"
#include "Render.h"
#include "Textures.h"
#include "Map.h"
#include "Physics.h"

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
    bool ret = Load(mapPath);

    return ret;
}

bool Map::Update(float dt)
{
    if(mapLoaded == false)
        return false;

    ListItem<MapLayer*>* mapLayerItem;
    mapLayerItem = mapData.maplayers.start;

    while (mapLayerItem != NULL) {

        if (mapLayerItem->data->properties.GetProperty("Draw") != NULL && mapLayerItem->data->properties.GetProperty("Draw")->value) {

            for (int x = 0; x < mapLayerItem->data->width; x++)
            {
                for (int y = 0; y < mapLayerItem->data->height; y++)
                {
                    int gid = mapLayerItem->data->Get(x, y);
                    TileSet* tileset = GetTilesetFromTileId(gid);

                    SDL_Rect r = tileset->GetTileRect(gid);
                    iPoint pos = MapToWorld(x, y);

                    app->render->DrawTexture(tileset->texture,
                        pos.x,
                        pos.y,
                        &r);
                }
            }
        }
        mapLayerItem = mapLayerItem->next;

    }

    return true;
}

iPoint Map::MapToWorld(int x, int y) const
{
    iPoint ret;

    ret.x = x * mapData.tileWidth;
    ret.y = y * mapData.tileHeight;

    return ret;
}

iPoint Map::WorldToMap(int x, int y) 
{
    iPoint ret(0, 0);

    //

    return ret;
}

// Get relative Tile rectangle
SDL_Rect TileSet::GetTileRect(int gid) const
{
    SDL_Rect rect = { 0 };
    int relativeIndex = gid - firstgid;

    rect.w = tileWidth;
    rect.h = tileHeight;
    rect.x = margin + (tileWidth + spacing) * (relativeIndex % columns);
    rect.y = margin + (tileWidth + spacing) * (relativeIndex / columns);

    return rect;
}

TileSet* Map::GetTilesetFromTileId(int gid) const
{
    ListItem<TileSet*>* item = mapData.tilesets.start;
    TileSet* set = NULL;

    while (item)
    {
        set = item->data;
        if (gid < (item->data->firstgid + item->data->tilecount))
        {
            break;
        }
        item = item->next;
    }

    return set;
}

// Called before quitting
bool Map::CleanUp()
{
    LOG("Unloading map");

    // Remove all layers
    ListItem<MapLayer*>* layerItem;
    layerItem = mapData.maplayers.start;

    while (layerItem != NULL)
    {
        RELEASE_ARRAY(layerItem->data->data);
        RELEASE(layerItem->data);
        layerItem = layerItem->next;
    }
    mapData.maplayers.Clear();

    // Remove all tilesets
    ListItem<TileSet*>* tilesetItem;
    tilesetItem = mapData.tilesets.start;

    while (tilesetItem != NULL)
    {
        app->tex->UnLoad(tilesetItem->data->texture);
        RELEASE(tilesetItem->data);
        tilesetItem = tilesetItem->next;
    }
    mapData.tilesets.Clear();

    DestroyAllColliders();
    return true;
}

// Load new map
bool Map::Load(SString mapFileName)
{
    bool ret = true;

    pugi::xml_document mapFileXML;
    pugi::xml_parse_result result = mapFileXML.load_file(mapFileName.GetString());

    if(result == NULL)
    {
        LOG("Could not load map xml file %s. pugi error: %s", mapFileName.GetString(), result.description());
        ret = false;
    }

    if(ret == true)
    {
        ret = LoadMap(mapFileXML);
    }

    if (ret == true)
    {
        ret = LoadTileSet(mapFileXML);
    }

    if (ret == true)
    {
        ret = LoadAllLayers(mapFileXML.child("map"));
    }

    CreateColliders();
    
    // NOTE: Later you have to create a function here to load and create the colliders from the map

    //PhysBody* c1 = app->physics->CreateRectangle(224 + 128, 543 + 32, 256, 64, STATIC);
    //c1->ctype = ColliderType::PLATFORM;

    //PhysBody* c2 = app->physics->CreateRectangle(352 + 64, 384 + 32, 128, 64, STATIC);
    //c2->ctype = ColliderType::PLATFORM;

    //PhysBody* c3 = app->physics->CreateRectangle(256, 704 + 32, 576, 64, STATIC);
    //c3->ctype = ColliderType::PLATFORM;

    //PhysBody* c4 = app->physics->CreateRectangleSensor(0,500, 140, 420, STATIC);
    //c4->ctype = ColliderType::WALL;

    //PhysBody* c5 = app->physics->CreateRectangle(0, 500, 100, 420, STATIC);
    //c5->ctype = ColliderType::WALL;

    //PhysBody* c6 = app->physics->CreateRectangle(180, 710, 30, 30, STATIC);
    //c6->ctype = ColliderType::JUMP;
    
    if(ret == true)
    {
        LOG("Successfully parsed map XML file :%s", mapFileName.GetString());
        LOG("width : %d height : %d",mapData.width,mapData.height);
        LOG("tile_width : %d tile_height : %d",mapData.tileWidth, mapData.tileHeight);
        
        LOG("Tilesets----");

        ListItem<TileSet*>* tileset;
        tileset = mapData.tilesets.start;

        while (tileset != NULL) {
            LOG("name : %s firstgid : %d",tileset->data->name.GetString(), tileset->data->firstgid);
            LOG("tile width : %d tile height : %d", tileset->data->tileWidth, tileset->data->tileHeight);
            LOG("spacing : %d margin : %d", tileset->data->spacing, tileset->data->margin);
            tileset = tileset->next;
        }

        LOG("Layers----");

        ListItem<MapLayer*>* mapLayer;
        mapLayer = mapData.maplayers.start;

        while (mapLayer != NULL) {
            LOG("id : %d name : %s", mapLayer->data->id, mapLayer->data->name.GetString());
            LOG("Layer width : %d Layer height : %d", mapLayer->data->width, mapLayer->data->height);
            mapLayer = mapLayer->next;
        }
    }

    if(mapFileXML) mapFileXML.reset();

    mapLoaded = ret;

    return ret;
}

bool Map::LoadMap(pugi::xml_node mapFile)
{
    bool ret = true;
    pugi::xml_node map = mapFile.child("map");

    if (map == NULL)
    {
        LOG("Error parsing map xml file: Cannot find 'map' tag.");
        ret = false;
    }
    else
    {
        //Load map general properties
        mapData.height = map.attribute("height").as_int();
        mapData.width = map.attribute("width").as_int();
        mapData.tileHeight = map.attribute("tileheight").as_int();
        mapData.tileWidth = map.attribute("tilewidth").as_int();
        mapData.type = MAPTYPE_UNKNOWN;
    }

    return ret;
}

bool Map::LoadTileSet(pugi::xml_node mapFile){

    bool ret = true; 

    pugi::xml_node tileset;
    for (tileset = mapFile.child("map").child("tileset"); tileset && ret; tileset = tileset.next_sibling("tileset"))
    {
        TileSet* set = new TileSet();

        set->name = tileset.attribute("name").as_string();
        set->firstgid = tileset.attribute("firstgid").as_int();
        set->margin = tileset.attribute("margin").as_int();
        set->spacing = tileset.attribute("spacing").as_int();
        set->tileWidth = tileset.attribute("tilewidth").as_int();
        set->tileHeight = tileset.attribute("tileheight").as_int();
        set->columns = tileset.attribute("columns").as_int();
        set->tilecount = tileset.attribute("tilecount").as_int();

        SString texPath = path;
        texPath += tileset.child("image").attribute("source").as_string();
        set->texture = app->tex->Load(texPath.GetString());

        mapData.tilesets.Add(set);
    }

    return ret;
}

bool Map::LoadLayer(pugi::xml_node& node, MapLayer* layer)
{
    bool ret = true;

    //Load the attributes
    layer->id = node.attribute("id").as_int();
    layer->name = node.attribute("name").as_string();
    layer->width = node.attribute("width").as_int();
    layer->height = node.attribute("height").as_int();

    LoadProperties(node, layer->properties);

    //Reserve the memory for the data 
    layer->data = new uint[layer->width * layer->height];
    memset(layer->data, 0, layer->width * layer->height);

    //Iterate over all the tiles and assign the values
    pugi::xml_node tile;
    int i = 0;
    for (tile = node.child("data").child("tile"); tile && ret; tile = tile.next_sibling("tile"))
    {
        layer->data[i] = tile.attribute("gid").as_int();
        i++;
    }

    return ret;
}

bool Map::LoadAllLayers(pugi::xml_node mapNode) {
    bool ret = true;

    for (pugi::xml_node layerNode = mapNode.child("layer"); layerNode && ret; layerNode = layerNode.next_sibling("layer"))
    {
        //Load the layer
        MapLayer* mapLayer = new MapLayer();
        ret = LoadLayer(layerNode, mapLayer);

        //add the layer to the map
        mapData.maplayers.Add(mapLayer);
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

        properties.list.Add(p);
    }

    return ret;
}

bool Map::CreateColliders()
{
    ListItem<MapLayer*>* mapLayerItem;
    mapLayerItem = mapData.maplayers.start;

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

Properties::Property* Properties::GetProperty(const char* name)
{
    ListItem<Property*>* item = list.start;
    Property* p = NULL;

    while (item)
    {
        if (item->data->name == name) {
            p = item->data;
            break;
        }
        item = item->next;
    }

    return p;
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
