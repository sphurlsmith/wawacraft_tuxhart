#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "stdlib.h"
#include "math.h"
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

#define NORTH 0
#define SOUTH 1
#define EAST 2
#define WEST 3
using namespace std;

sf::VideoMode rendVideo;

sf::RenderWindow gamewin(rendVideo.getDesktopMode(), "Wawacraft Lockhart Version", sf::Style::Fullscreen);
sf::Event eventhandler;

sf::Time UNI_TICK=sf::seconds(4);
sf::Clock UNI_CLOCK;

sf::Color orange(255, 192, 0);

sf::Color skyboxes[4]=
{
    sf::Color(47, 38, 61),
    sf::Color(192,192,192),
    sf::Color(100,200,255),
    sf::Color(155,55,1)
};

sf::Font wawafont;

sf::Texture wawa;
sf::Texture grass;
sf::Texture asphalt;
sf::Texture stone;
sf::Texture bricks;
sf::Texture wood;
sf::Texture cardboard;
sf::Texture mud;
sf::Texture tile;
sf::Texture leaves;

sf::Texture uni;

sf::Texture wood_t;
sf::Texture wood_s;

sf::Texture lant_t;
sf::Texture lant_s;

sf::Texture title;
sf::Texture pause;

sf::Text STATUS_BAR;

sf::Music track_player;

struct vect{
    double x;
    double y;
    double z;
};

struct pix{
    int x;
    int y;
};

struct p_vect{
    pix x;
    pix y;
    pix z;
};

const int MAXTRACKS=3;

const int WIN_X=rendVideo.getDesktopMode().width;
const int WIN_Y=rendVideo.getDesktopMode().height;

const int CENTER_X=WIN_X/2;
const int CENTER_Y=WIN_Y/2;

const int PIX_SIZE=16;
const int TEX_SIZE=16;
const int BLOCK_TYPES=14;
const int SPRITE_TYPES=2;

const int SHADE=10;

const int WORLD_SIZE=20;
const int TREE_NUM=20;

const int BASE_Z=0;
const int MAX_Z=5;
const int MIN_Z=-5;

const int STRENGTH=2;

bool gamemode=false;
bool procedural=false;

int VIEW_X=CENTER_X;
int VIEW_Y=CENTER_Y;

int sky=2;

int tracknum=1;

int UNIT_SIZE=30;
const int VIEWPOINT=200;

const int AIM_DIS=5;

pix proj_x={UNIT_SIZE, (UNIT_SIZE/2)};
pix proj_y={(-UNIT_SIZE), (UNIT_SIZE/2)};
pix proj_z={0, (-UNIT_SIZE)};

void updateProjection(){
    proj_x={UNIT_SIZE, (UNIT_SIZE/2)};
    proj_y={(-UNIT_SIZE), (UNIT_SIZE/2)};
    proj_z={0, (-UNIT_SIZE)};
}

class block{
    public:
        block(bool texp, int texcp, vect posp, sf::Color colp);

        bool getTex();

        int getTexcode();
        int getDepth();

        sf::Color getColor();
        vect getPosition();

        void setTex(bool v);
        void setTexcode(int v);
        void setColor(sf::Color v);

        void setDepth(int v);
        void setPosition(vect v);

        void translateCoords();

        virtual void renderTex();
        void renderUtex();

        virtual void render();

        vect pos;

    protected:
        bool tex;

        int texcode;
        int depth;

        sf::Color utex_c;

        p_vect s_pos;
};

bool block::getTex(){
    return tex;
}

int block::getTexcode(){
    return texcode;
}

int block::getDepth(){
    return depth;
}

sf::Color block::getColor(){
    return utex_c;
}

vect block::getPosition(){
    return pos;
}

block::block(bool texp, int texcp, vect posp, sf::Color colp):
    tex(texp),
    texcode(texcp),
    pos(posp),
    utex_c(colp)
    {

}

void block::setTex(bool v){
    tex=v;
}

void block::setTexcode(int v){
    texcode=v;
}

void block::setColor(sf::Color v){
    utex_c=v;
}

void block::setDepth(int v){
    depth=v;
}

void block::setPosition(vect v){
    pos=v;
}

void block::translateCoords(){
    s_pos.x={pos.x*proj_x.x, pos.x*proj_x.y};
    s_pos.y={pos.y*proj_y.x, pos.y*proj_y.y};
    s_pos.z={pos.z*proj_z.x, pos.z*proj_z.y};
}

void block::renderTex(){
    translateCoords();

    int render_x;
    int render_y;

    render_x=VIEW_X+s_pos.x.x+s_pos.y.x+s_pos.z.x;
    render_y=VIEW_Y+s_pos.x.y+s_pos.y.y+s_pos.z.y;

    sf::VertexArray upperFace(sf::Quads, 4);

    upperFace[0]=sf::Vertex(sf::Vector2f(render_x, render_y), sf::Vector2f(PIX_SIZE,PIX_SIZE));
    upperFace[1]=sf::Vertex(sf::Vector2f(render_x-UNIT_SIZE, render_y-UNIT_SIZE/2), sf::Vector2f(0,PIX_SIZE));
    upperFace[2]=sf::Vertex(sf::Vector2f(render_x, render_y-UNIT_SIZE), sf::Vector2f(0,0));
    upperFace[3]=sf::Vertex(sf::Vector2f(render_x+UNIT_SIZE, render_y-UNIT_SIZE/2), sf::Vector2f(PIX_SIZE, 0));

    sf::VertexArray leftFace(sf::Quads, 4);

    leftFace[0]=sf::Vertex(sf::Vector2f(render_x, render_y), sf::Vector2f(PIX_SIZE,0));
    leftFace[1]=sf::Vertex(sf::Vector2f(render_x-UNIT_SIZE, render_y-UNIT_SIZE/2), sf::Vector2f(0,0));
    leftFace[2]=sf::Vertex(sf::Vector2f(render_x-UNIT_SIZE, render_y+UNIT_SIZE/2), sf::Vector2f(0,PIX_SIZE));
    leftFace[3]=sf::Vertex(sf::Vector2f(render_x, render_y+UNIT_SIZE), sf::Vector2f(PIX_SIZE,PIX_SIZE));

    sf::VertexArray leftShade(sf::Quads, 4);

    leftShade[0]=sf::Vertex(sf::Vector2f(render_x, render_y), sf::Color(0,0,0,SHADE*5));
    leftShade[1]=sf::Vertex(sf::Vector2f(render_x-UNIT_SIZE, render_y-UNIT_SIZE/2), sf::Color(0,0,0,SHADE*5));
    leftShade[2]=sf::Vertex(sf::Vector2f(render_x-UNIT_SIZE, render_y+UNIT_SIZE/2), sf::Color(0,0,0,SHADE*5));
    leftShade[3]=sf::Vertex(sf::Vector2f(render_x, render_y+UNIT_SIZE), sf::Color(0,0,0,SHADE*5));

    sf::VertexArray rightFace(sf::Quads, 4);

    rightFace[0]=sf::Vertex(sf::Vector2f(render_x, render_y), sf::Vector2f(0,0));
    rightFace[1]=sf::Vertex(sf::Vector2f(render_x+UNIT_SIZE, render_y-UNIT_SIZE/2), sf::Vector2f(PIX_SIZE,0));
    rightFace[2]=sf::Vertex(sf::Vector2f(render_x+UNIT_SIZE, render_y+UNIT_SIZE/2), sf::Vector2f(PIX_SIZE,PIX_SIZE));
    rightFace[3]=sf::Vertex(sf::Vector2f(render_x, render_y+UNIT_SIZE), sf::Vector2f(0,PIX_SIZE));

    sf::VertexArray rightShade(sf::Quads, 4);

    rightShade[0]=sf::Vertex(sf::Vector2f(render_x, render_y), sf::Color(0,0,0, SHADE*10));
    rightShade[1]=sf::Vertex(sf::Vector2f(render_x+UNIT_SIZE, render_y-UNIT_SIZE/2), sf::Color(0,0,0, SHADE*10));
    rightShade[2]=sf::Vertex(sf::Vector2f(render_x+UNIT_SIZE, render_y+UNIT_SIZE/2), sf::Color(0,0,0, SHADE*10));
    rightShade[3]=sf::Vertex(sf::Vector2f(render_x, render_y+UNIT_SIZE), sf::Color(0,0,0, SHADE*10));

    sf::Vertex outlines[]=
    {
        sf::Vertex(sf::Vector2f(render_x-UNIT_SIZE, render_y-UNIT_SIZE/2), sf::Color(0,0,0)),
        sf::Vertex(sf::Vector2f(render_x, render_y), sf::Color(0,0,0)),
        sf::Vertex(sf::Vector2f(render_x-UNIT_SIZE, render_y-UNIT_SIZE/2), sf::Color(0,0,0)),
        sf::Vertex(sf::Vector2f(render_x, render_y-UNIT_SIZE), sf::Color(0,0,0)),
        sf::Vertex(sf::Vector2f(render_x+UNIT_SIZE, render_y-UNIT_SIZE/2), sf::Color(0,0,0)),
        sf::Vertex(sf::Vector2f(render_x, render_y), sf::Color(0,0,0)),
        sf::Vertex(sf::Vector2f(render_x+UNIT_SIZE, render_y-UNIT_SIZE/2), sf::Color(0,0,0)),
        sf::Vertex(sf::Vector2f(render_x+UNIT_SIZE, render_y+UNIT_SIZE/2), sf::Color(0,0,0)),
        sf::Vertex(sf::Vector2f(render_x, render_y+UNIT_SIZE), sf::Color(0,0,0)),
        sf::Vertex(sf::Vector2f(render_x, render_y), sf::Color(0,0,0)),
        sf::Vertex(sf::Vector2f(render_x, render_y+UNIT_SIZE), sf::Color(0,0,0)),
        sf::Vertex(sf::Vector2f(render_x-UNIT_SIZE, render_y+UNIT_SIZE/2), sf::Color(0,0,0)),
        sf::Vertex(sf::Vector2f(render_x-UNIT_SIZE, render_y-UNIT_SIZE/2), sf::Color(0,0,0))
    };

    switch(texcode){
        case 1:
            gamewin.draw(upperFace, &wawa);
            gamewin.draw(rightFace, &wawa);
            gamewin.draw(leftFace, &wawa);

            break;
        case 2:
            gamewin.draw(upperFace, &grass);
            gamewin.draw(rightFace, &grass);
            gamewin.draw(leftFace, &grass);
            break;
        case 3:
            gamewin.draw(upperFace, &asphalt);
            gamewin.draw(rightFace, &asphalt);
            gamewin.draw(leftFace, &asphalt);
            break;
        case 4:
            gamewin.draw(upperFace, &stone);
            gamewin.draw(rightFace, &stone);
            gamewin.draw(leftFace, &stone);
            break;
        case 5:
            gamewin.draw(upperFace, &bricks);
            gamewin.draw(rightFace, &bricks);
            gamewin.draw(leftFace, &bricks);
            break;
        case 6:
            gamewin.draw(upperFace, &wood);
            gamewin.draw(rightFace, &wood);
            gamewin.draw(leftFace, &wood);
            break;
        case 7:
            gamewin.draw(upperFace, &cardboard);
            gamewin.draw(rightFace, &cardboard);
            gamewin.draw(leftFace, &cardboard);
            break;
        case 8:
            gamewin.draw(upperFace, &mud);
            gamewin.draw(rightFace, &mud);
            gamewin.draw(leftFace, &mud);
            break;
        case 9:
            gamewin.draw(upperFace, &tile);
            gamewin.draw(rightFace, &tile);
            gamewin.draw(leftFace, &tile);
            break;
        case 10:
            tex=true;
            gamewin.draw(outlines, 13, sf::LineStrip);
            return;
            break;
        case 11:
            gamewin.draw(upperFace, &wood_t);
            gamewin.draw(rightFace, &wood_s);
            gamewin.draw(leftFace, &wood_s);
            break;
        case 12:
            gamewin.draw(upperFace, &lant_t);
            gamewin.draw(rightFace, &lant_s);
            gamewin.draw(leftFace, &lant_s);
            break;
        case 13:
            gamewin.draw(upperFace, &leaves);
            gamewin.draw(rightFace, &leaves);
            gamewin.draw(leftFace, &leaves);
            break;
        default:
            tex=false;
            break;
    }

    gamewin.draw(rightShade);
    gamewin.draw(leftShade);
    gamewin.draw(outlines, 13, sf::LineStrip);
}

void block::renderUtex(){
    translateCoords();

    int render_x;
    int render_y;

    render_x=VIEW_X+s_pos.x.x+s_pos.y.x+s_pos.z.x;
    render_y=VIEW_Y+s_pos.x.y+s_pos.y.y+s_pos.z.y;

    sf::Vertex cube[]=
    {
        sf::Vertex(sf::Vector2f(render_x, render_y), utex_c),
        sf::Vertex(sf::Vector2f(render_x-UNIT_SIZE, render_y-UNIT_SIZE/2), utex_c),
        sf::Vertex(sf::Vector2f(render_x, render_y-UNIT_SIZE), utex_c),
        sf::Vertex(sf::Vector2f(render_x+UNIT_SIZE, render_y-UNIT_SIZE/2), utex_c)
    };

    sf::Vertex outlines[]=
    {
        sf::Vertex(sf::Vector2f(render_x-UNIT_SIZE, render_y-UNIT_SIZE/2), sf::Color(0,0,0)),
        sf::Vertex(sf::Vector2f(render_x, render_y), sf::Color(0,0,0)),
        sf::Vertex(sf::Vector2f(render_x-UNIT_SIZE, render_y-UNIT_SIZE/2), sf::Color(0,0,0)),
        sf::Vertex(sf::Vector2f(render_x, render_y-UNIT_SIZE), sf::Color(0,0,0)),
        sf::Vertex(sf::Vector2f(render_x+UNIT_SIZE, render_y-UNIT_SIZE/2), sf::Color(0,0,0)),
        sf::Vertex(sf::Vector2f(render_x, render_y), sf::Color(0,0,0)),
        sf::Vertex(sf::Vector2f(render_x+UNIT_SIZE, render_y-UNIT_SIZE/2), sf::Color(0,0,0)),
        sf::Vertex(sf::Vector2f(render_x+UNIT_SIZE, render_y+UNIT_SIZE/2), sf::Color(0,0,0)),
        sf::Vertex(sf::Vector2f(render_x, render_y+UNIT_SIZE), sf::Color(0,0,0)),
        sf::Vertex(sf::Vector2f(render_x, render_y), sf::Color(0,0,0)),
        sf::Vertex(sf::Vector2f(render_x, render_y+UNIT_SIZE), sf::Color(0,0,0)),
        sf::Vertex(sf::Vector2f(render_x-UNIT_SIZE, render_y+UNIT_SIZE/2), sf::Color(0,0,0)),
        sf::Vertex(sf::Vector2f(render_x-UNIT_SIZE, render_y-UNIT_SIZE/2), sf::Color(0,0,0))
    };

    sf::Vertex shading[]=
    {
        sf::Vertex(sf::Vector2f(render_x, render_y), sf::Color(utex_c.r/SHADE, utex_c.g/SHADE, utex_c.b/SHADE)),
        sf::Vertex(sf::Vector2f(render_x+UNIT_SIZE, render_y-UNIT_SIZE/2), sf::Color(utex_c.r/SHADE, utex_c.g/SHADE, utex_c.b/SHADE)),
        sf::Vertex(sf::Vector2f(render_x+UNIT_SIZE, render_y+UNIT_SIZE/2), sf::Color(utex_c.r/SHADE, utex_c.g/SHADE, utex_c.b/SHADE)),
        sf::Vertex(sf::Vector2f(render_x, render_y+UNIT_SIZE), sf::Color(utex_c.r/SHADE, utex_c.g/SHADE, utex_c.b/SHADE))
    };

    sf::Vertex light[]=
    {
        sf::Vertex(sf::Vector2f(render_x, render_y), sf::Color(utex_c.r/(SHADE/2), utex_c.g/(SHADE/2), utex_c.b/(SHADE/2))),
        sf::Vertex(sf::Vector2f(render_x-UNIT_SIZE, render_y-UNIT_SIZE/2), sf::Color(utex_c.r/(SHADE/2), utex_c.g/(SHADE/2), utex_c.b/(SHADE/2))),
        sf::Vertex(sf::Vector2f(render_x-UNIT_SIZE, render_y+UNIT_SIZE/2), sf::Color(utex_c.r/(SHADE/2), utex_c.g/(SHADE/2), utex_c.b/(SHADE/2))),
        sf::Vertex(sf::Vector2f(render_x, render_y+UNIT_SIZE), sf::Color(utex_c.r/(SHADE/2), utex_c.g/(SHADE/2), utex_c.b/(SHADE/2)))
    };

    gamewin.draw(cube, 4, sf::Quads);
    gamewin.draw(shading, 4, sf::Quads);
    gamewin.draw(light, 4, sf::Quads);
    gamewin.draw(outlines, 13, sf::LineStrip);

    if(texcode>0){
        renderTex();
    }
}

void block::render(){
    translateCoords();

    int render_x;
    int render_y;

    render_x=VIEW_X+s_pos.x.x+s_pos.y.x+s_pos.z.x;
    render_y=VIEW_Y+s_pos.x.y+s_pos.y.y+s_pos.z.y;

    if(render_x+UNIT_SIZE>=0 && render_x-UNIT_SIZE<=WIN_X &&
       render_y+UNIT_SIZE>=0 && render_y-UNIT_SIZE<=WIN_Y){
        if(tex){
            renderTex();
        }else{
            renderUtex();
        }
    }
}

vector<block> CUBES;
vector<vect> CHUNKED;

block cursor(true, 2, {0,0,0}, orange);

void swapElements(int firstl, int secondl){
    block temp=CUBES.at(secondl);

    CUBES.at(secondl)=CUBES.at(firstl);
    CUBES.at(firstl)=temp;
}

int partitionSet(int length, int b, int e){
    int pivot_val=CUBES.at(e).getDepth();

    int x=b;

    for(int y=b; y<e; y++){
        if(CUBES.at(y).getDepth()>=pivot_val){
            swapElements(x, y);
            x++;
        }
    }

    swapElements(x, e);

    return x;
}

void recur(int length, int l, int h){
    if(l<h){
        int piv=partitionSet(length, l, h);

        recur(piv-l, l, piv-1);
        recur(h-piv, piv+1, h);
    }
}

void sortSet(){
    int setsize=CUBES.size();

    recur(setsize, 0, setsize-1);
}

void calculateDepthSet(){
    for(int x=0; x<CUBES.size(); x++){
        CUBES.at(x).setDepth(VIEWPOINT*3-(CUBES.at(x).pos.x+CUBES.at(x).pos.y+CUBES.at(x).pos.z));
    }
}

void calculateDepth(){
    cursor.setDepth(VIEWPOINT*3-(cursor.pos.x+cursor.pos.y+cursor.pos.z));
}

bool deleteBlock(block par){
    bool deleted=false;

    for(int x=0; x<CUBES.size(); x++){
        if(CUBES.at(x).pos.x==par.pos.x &&
           CUBES.at(x).pos.y==par.pos.y &&
           CUBES.at(x).pos.z==par.pos.z){
            deleted=true;
            CUBES.erase(CUBES.begin()+x);
           }
    }

    return deleted;
}

void placeBlock(block par){
    deleteBlock(par);
    CUBES.push_back(par);
}

class sprite:public block{
    public:
        sprite(bool texp, int texcp, vect posp, sf::Color colp, int dirp, double s);

        int getDirection();

        double getBaseSpeed();
        double getSpeed();

        bool vectIsOccupied();
        bool accountGravity();

        void calculateDepth();

        void renderTex();
        void renderXray();
        void render();

        void setDirection(int dir);
        void setSpeed(int sp);

        void moveSprite();

    protected:
        int direction;

        double base_speed;
        double speed;
};

sprite::sprite(bool texp, int texcp, vect posp, sf::Color colp, int dirp, double s):
    block(texp, texcp, posp, colp),
    direction(dirp),
    base_speed(s),
    speed(s){

}

int sprite::getDirection(){
    return direction;
}

double sprite::getBaseSpeed(){
    return base_speed;
}

double sprite::getSpeed(){
    return speed;
}

bool sprite::vectIsOccupied(){
    for(int x=0; x<CUBES.size(); x++){
        if(int(CUBES.at(x).pos.x)==int(pos.x) &&
           int(CUBES.at(x).pos.y)==int(pos.y) &&
           int(CUBES.at(x).pos.z)==int(pos.z)){
           return true;
           break;
           }
    }

    return false;
}

bool sprite::accountGravity(){
    bool ret=true;

    int oz=pos.z;

    for(int x=0; x<VIEWPOINT; x++){
        pos.z--;

        if(vectIsOccupied()){
            ret=false;
            pos.z++;
            break;
        }
    }

    return ret;
}

void sprite::calculateDepth(){
    depth=VIEWPOINT*3-(pos.x+pos.y+pos.z);
}

void sprite::renderTex(){
    translateCoords();

    int render_x;
    int render_y;

    if(gamemode && texcode==0){
        VIEW_X=CENTER_X-(s_pos.x.x+s_pos.y.x+s_pos.z.x);
        VIEW_Y=CENTER_Y-(s_pos.x.y+s_pos.y.y+s_pos.z.y);
    }

    render_x=VIEW_X+s_pos.x.x+s_pos.y.x+s_pos.z.x;
    render_y=VIEW_Y+s_pos.x.y+s_pos.y.y+s_pos.z.y;

    sf::Vertex sprite[]=
    {
        sf::Vertex(sf::Vector2f(render_x-UNIT_SIZE, render_y-UNIT_SIZE), sf::Vector2f(0, 0)),
        sf::Vertex(sf::Vector2f(render_x+UNIT_SIZE, render_y-UNIT_SIZE), sf::Vector2f(TEX_SIZE, 0)),
        sf::Vertex(sf::Vector2f(render_x+UNIT_SIZE, render_y+UNIT_SIZE), sf::Vector2f(TEX_SIZE, TEX_SIZE)),
        sf::Vertex(sf::Vector2f(render_x-UNIT_SIZE, render_y+UNIT_SIZE), sf::Vector2f(0, TEX_SIZE))
    };

    switch(texcode){
        case 0:
            gamewin.draw(sprite, 4, sf::Quads, &wawa);
            break;
        case 1:
            gamewin.draw(sprite, 4, sf::Quads, &uni);
            break;
        default:
            gamewin.draw(sprite, 4, sf::Quads, &wawa);
            break;
    }
}

void sprite::renderXray(){
    translateCoords();

    int render_x;
    int render_y;

    if(gamemode && texcode==0){
        VIEW_X=CENTER_X-(s_pos.x.x+s_pos.y.x+s_pos.z.x);
        VIEW_Y=CENTER_Y-(s_pos.x.y+s_pos.y.y+s_pos.z.y);
    }

    render_x=VIEW_X+s_pos.x.x+s_pos.y.x+s_pos.z.x;
    render_y=VIEW_Y+s_pos.x.y+s_pos.y.y+s_pos.z.y;


    sf::Vertex sprite[]=
    {
        sf::Vertex(sf::Vector2f(render_x-UNIT_SIZE, render_y-UNIT_SIZE), sf::Color(20,20,20)),
        sf::Vertex(sf::Vector2f(render_x+UNIT_SIZE, render_y-UNIT_SIZE), sf::Color(20,20,20)),
        sf::Vertex(sf::Vector2f(render_x+UNIT_SIZE, render_y+UNIT_SIZE), sf::Color(20,20,20)),
        sf::Vertex(sf::Vector2f(render_x-UNIT_SIZE, render_y+UNIT_SIZE), sf::Color(20,20,20))
    };

    gamewin.draw(sprite, 4, sf::Quads);
}

void sprite::render(){
    renderTex();
}

void sprite::setDirection(int dir){
    direction=dir;
}

void sprite::setSpeed(int sp){
    speed=sp;
}

void sprite::moveSprite(){
    switch(direction){
        case NORTH:
            pos.y-=speed;

            if(vectIsOccupied() || accountGravity()){
                if(vectIsOccupied()){
                    pos.z++;
                    if(vectIsOccupied()){
                       pos.z--;
                    }else{
                        break;
                    }
                }

                pos.y+=speed;
            }
            break;
        case SOUTH:
            pos.y+=speed;

            if(vectIsOccupied() || accountGravity()){
                if(vectIsOccupied()){
                    pos.z++;
                    if(vectIsOccupied()){
                       pos.z--;
                    }else{
                        break;
                    }
                }

                pos.y-=speed;
            }
            break;
        case EAST:
            pos.x+=speed;

            if(vectIsOccupied() || accountGravity()){
                if(vectIsOccupied()){
                    pos.z++;
                    if(vectIsOccupied()){
                       pos.z--;
                    }else{
                        break;
                    }
                }

                pos.x-=speed;
            }
            break;
        case WEST:
            pos.x-=speed;

            if(vectIsOccupied() || accountGravity()){
                if(vectIsOccupied()){
                    pos.z++;
                    if(vectIsOccupied()){
                       pos.z--;
                    }else{
                        break;
                    }
                }

                pos.x+=speed;
            }
            break;
    }

    if(pos.z<=-100){
        pos={0,0,1};
    }
}

sprite player(true, 0, {0,0,1}, orange, NORTH, 0.2);
sprite uni_npc(true, 1, {0,0,1}, orange, NORTH, 0.2);

void updateUni(){
    if(UNI_CLOCK.getElapsedTime().asSeconds()>=(rand()%int(UNI_TICK.asMilliseconds())+1)){
        int d=rand()%5;

        uni_npc.setDirection(d);
        UNI_CLOCK.restart();
    }

    uni_npc.moveSprite();
}

void renderBlocks(){
    for(int x=0; x<CUBES.size(); x++){
        if(CUBES.at(x).pos.z<player.pos.z){
            CUBES.at(x).render();
        }

        if(CUBES.at(x).pos.z<uni_npc.pos.z){
            CUBES.at(x).render();
        }
    }

    player.render();
    uni_npc.render();

    for(int x=0; x<CUBES.size(); x++){
        if(CUBES.at(x).pos.z>=player.pos.z){
            CUBES.at(x).render();

            if(CUBES.at(x).pos.x<=player.pos.x && CUBES.at(x).pos.y<=player.pos.y && CUBES.at(x).pos.z<=player.pos.z){
                player.render();
            }
        }
    }

    for(int x=0; x<CUBES.size(); x++){
        if(CUBES.at(x).pos.z>=uni_npc.pos.z){
            CUBES.at(x).render();

            if(CUBES.at(x).pos.x<=player.pos.x && CUBES.at(x).pos.y<=player.pos.y && CUBES.at(x).pos.z<=player.pos.z){
                player.render();
            }

            if(CUBES.at(x).pos.x<=uni_npc.pos.x && CUBES.at(x).pos.y<=uni_npc.pos.y && CUBES.at(x).pos.z<=uni_npc.pos.z){
                uni_npc.render();
            }
        }
    }
}

void saveWorld(string filename){
    ofstream savefile;

    savefile.open("worlds/"+filename+".wac");

    savefile << CUBES.size() << endl;

    for(int x=0; x<CUBES.size(); x++){
        savefile << CUBES.at(x).pos.x << " " <<
                    CUBES.at(x).pos.y << " " <<
                    CUBES.at(x).pos.z << " " <<
                    CUBES.at(x).getTexcode() << " " <<
                    CUBES.at(x).getDepth() << endl;
    }
}

void loadWorld(string filename){
    CUBES.clear();
    player.pos={0,0,1};

    ifstream savefile;
    savefile.open("worlds/"+filename+".wac");

    int worldSize;

    double xpos;
    double ypos;
    double zpos;

    int texcode;
    int depth;

    savefile >> worldSize;

    for(int x=0; x<worldSize; x++){
        savefile >> xpos >> ypos >> zpos >> texcode >> depth;
        block load(true, texcode, {xpos, ypos, zpos}, orange);

        if(texcode!=-2){
            CUBES.push_back(load);
        }
    }
}

void setTrack(int track){
    stringstream filename;
    string s;

    filename << "bgm/track_" << track << ".wav";
    s=filename.str();

    track_player.setLoop(true);
    track_player.openFromFile(s);
    track_player.play();
}

void muteUnmute(){
    if(track_player.getStatus()==sf::Music::Paused){
        track_player.play();
    }else{
        track_player.pause();
    }
}

void generateTree(vect base){
    block tree_cursor(true, 11, base, orange);

    int ox=base.x;
    int oy=base.y;
    int oz=base.z;

    vect tree[]=
    {
        {base.x, base.y, base.z+1},
        {base.x, base.y, base.z+2},
        {base.x, base.y, base.z+3},
        {base.x, base.y, base.z+4},
        {base.x, base.y-1, base.z+3},
        {base.x, base.y+1, base.z+3},
        {base.x-1, base.y, base.z+3},
        {base.x+1, base.y, base.z+3},
        {base.x-1, base.y-1, base.z+3},
        {base.x+1, base.y+1, base.z+3},
        {base.x+1, base.y-1, base.z+3},
        {base.x-1, base.y+1, base.z+3},
    };

    tree_cursor.setTexcode(11);

    for(int x=0; x<=2; x++){
        tree_cursor.pos=tree[x];
        placeBlock(tree_cursor);
    }

    tree_cursor.setTexcode(13);

    for(int x=3; x<12; x++){
        tree_cursor.pos=tree[x];
        placeBlock(tree_cursor);
    }
}

void generateWorld(int ox=0, int oy=0){
    srand(time(NULL));
    procedural=true;

    int depthmap[WORLD_SIZE][WORLD_SIZE];

    depthmap[0][0]=0;

    for(int y=0; y<WORLD_SIZE; y++){
        for(int x=0; x<WORLD_SIZE; x++){
            depthmap[x][y]=rand()%MAX_Z+MIN_Z;
        }
    }

    int overlay[WORLD_SIZE/STRENGTH][WORLD_SIZE/STRENGTH];

    for(int y=0; y<WORLD_SIZE/STRENGTH; y++){
        for(int x=0; x<WORLD_SIZE/STRENGTH; x++){
            overlay[x][y]=depthmap[x*STRENGTH][y*STRENGTH];
        }
    }

    for(int y=0; y<WORLD_SIZE; y++){
        for(int x=0; x<WORLD_SIZE; x++){
            int dot1=overlay[x/STRENGTH][y/STRENGTH];
            int dot2=overlay[x/STRENGTH][y/STRENGTH+1];
            int dot3=overlay[x/STRENGTH+1][y/STRENGTH];
            int dot4=overlay[x/STRENGTH+1][y/STRENGTH+1];

            depthmap[x][y]=(dot1+dot2+dot3+dot4+depthmap[x][y])/5;

            if(int(x)==0 && int(y)==0){
                depthmap[x][y]=0;
            }
        }
    }

    for(int y=1; y<WORLD_SIZE-1; y++){
        for(int x=1; x<WORLD_SIZE-1; x++){

            int dot1=depthmap[x][y];
            int dot2=depthmap[x][y+1];
            int dot3=depthmap[x+1][y];
            int dot4=depthmap[x+1][y+1];

            depthmap[x][y]+=(dot1+dot2+dot3+dot4);
            depthmap[x][y]/=5;
        }
    }

    block ground(true, 2, {0,0,0}, orange);

    for(int y=0; y<WORLD_SIZE; y++){
        for(int x=0; x<WORLD_SIZE; x++){
            ground.setTexcode(2);

            ground.pos={x-WORLD_SIZE/2+ox, y-WORLD_SIZE/2+oy, depthmap[x][y]};
            CUBES.push_back(ground);

            ground.setTexcode(8);
            ground.pos={x-WORLD_SIZE/2+ox, y-WORLD_SIZE/2+oy, depthmap[x][y]-1};
            CUBES.push_back(ground);
        }
    }

    for(int x=0; x<TREE_NUM; x++){
        int randx=(rand()%WORLD_SIZE)-WORLD_SIZE/2+ox;
        int randy=(rand()%WORLD_SIZE)-WORLD_SIZE/2+oy;

        generateTree({randx, randy, depthmap[randx][randy]-1});
    }

    CHUNKED.push_back({ox, oy, 0});
}

void setUpProgram(){
    srand(time(NULL));

    gamewin.setFramerateLimit(60);

    wawafont.loadFromFile("textures/bradhitc.ttf");

    wawa.loadFromFile("textures/eggcat.png");
    grass.loadFromFile("textures/grass.png");
    asphalt.loadFromFile("textures/asphalt.png");
    stone.loadFromFile("textures/stone.png");
    bricks.loadFromFile("textures/brick.png");
    wood.loadFromFile("textures/wood.png");
    cardboard.loadFromFile("textures/cardboard.png");
    mud.loadFromFile("textures/mud.png");
    tile.loadFromFile("textures/tiling.png");
    leaves.loadFromFile("textures/leaves.png");

    uni.loadFromFile("textures/uni.png");

    wood_s.loadFromFile("textures/woodside.png");
    wood_t.loadFromFile("textures/woodtop.png");

    lant_s.loadFromFile("textures/lantside.png");
    lant_t.loadFromFile("textures/lanttop.png");

    title.loadFromFile("textures/title.png");
    pause.loadFromFile("textures/pause.png");

    setTrack(tracknum);

    cursor.setColor(orange);
    CUBES.push_back(cursor);
}

void pauseLoop(){

    string inp;

    const int top=0;
    const int bottom=7;

    int option=0;
    bool left=false;

    sf::Text SAVEW;
    sf::Text LOADW;
    sf::Text QUIT;
    sf::Text RETURN;
    sf::Text SWITCHT;
    sf::Text MUTE;
    sf::Text SKIES;
    sf::Text WORLD;

    sf::Text PROMPT;

    sf::Text INPUT;

    SAVEW.setFont(wawafont);
    SAVEW.setCharacterSize(40);
    SAVEW.setFillColor(sf::Color::White);
    SAVEW.setPosition(sf::Vector2f(40, 240));
    SAVEW.setStyle(sf::Text::Bold);
    SAVEW.setString("SAVE WORLD");

    LOADW.setFont(wawafont);
    LOADW.setCharacterSize(40);
    LOADW.setFillColor(sf::Color::White);
    LOADW.setPosition(sf::Vector2f(40, 280));
    LOADW.setStyle(sf::Text::Bold);
    LOADW.setString("LOAD WORLD");

    SKIES.setFont(wawafont);
    SKIES.setCharacterSize(40);
    SKIES.setFillColor(sf::Color::White);
    SKIES.setPosition(sf::Vector2f(40, 400));
    SKIES.setStyle(sf::Text::Bold);
    SKIES.setString("CHANGE TIME OF DAY");

    WORLD.setFont(wawafont);
    WORLD.setCharacterSize(40);
    WORLD.setFillColor(sf::Color::White);
    WORLD.setPosition(sf::Vector2f(40, 440));
    WORLD.setStyle(sf::Text::Bold);
    WORLD.setString("GENERATE NEW WORLD");

    QUIT.setFont(wawafont);
    QUIT.setCharacterSize(40);
    QUIT.setFillColor(sf::Color::White);
    QUIT.setPosition(sf::Vector2f(40, 480));
    QUIT.setStyle(sf::Text::Bold);
    QUIT.setString("QUIT");

    RETURN.setFont(wawafont);
    RETURN.setCharacterSize(40);
    RETURN.setFillColor(sf::Color::White);
    RETURN.setPosition(sf::Vector2f(40, 200));
    RETURN.setStyle(sf::Text::Bold);
    RETURN.setString("CONTINUE");

    SWITCHT.setFont(wawafont);
    SWITCHT.setCharacterSize(40);
    SWITCHT.setFillColor(sf::Color::White);
    SWITCHT.setPosition(sf::Vector2f(40, 320));
    SWITCHT.setStyle(sf::Text::Bold);
    SWITCHT.setString("SWITCH TRACK");

    MUTE.setFont(wawafont);
    MUTE.setCharacterSize(40);
    MUTE.setFillColor(sf::Color::White);
    MUTE.setPosition(sf::Vector2f(40, 360));
    MUTE.setStyle(sf::Text::Bold);
    MUTE.setString("MUTE/UNMUTE MUSIC");

    PROMPT.setFont(wawafont);
    PROMPT.setCharacterSize(40);
    PROMPT.setFillColor(sf::Color::White);
    PROMPT.setString(">");

    INPUT.setFont(wawafont);
    INPUT.setCharacterSize(40);
    INPUT.setFillColor(orange);
    INPUT.setStyle(sf::Text::Bold);
    INPUT.setPosition(sf::Vector2f(0,0));

    sf::Vertex pause_v[]=
    {
        sf::Vertex(sf::Vector2f(0,0), sf::Vector2f(0,0)),
        sf::Vertex(sf::Vector2f(WIN_X, 0), sf::Vector2f(800, 0)),
        sf::Vertex(sf::Vector2f(WIN_X, WIN_Y), sf::Vector2f(800, 600)),
        sf::Vertex(sf::Vector2f(0, WIN_Y), sf::Vector2f(0, 600))
    };

    while(gamewin.isOpen()){
        while(gamewin.pollEvent(eventhandler)){
            switch(eventhandler.type){
                case sf::Event::Closed:
                    gamewin.close();
                    break;
                case sf::Event::TextEntered:
                    inp+=eventhandler.text.unicode;

                    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Backspace)){
                        inp="";
                    }

                    INPUT.setString(inp);
                    break;
                case sf::Event::KeyPressed:

                    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up)){
                        option--;
                    }

                    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down)){
                        option++;
                    }

                    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Backspace)){
                        inp="";
                    }

                    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)){
                        switch(option){
                            case 0:
                                left=true;
                                break;
                            case 1:
                                track_player.pause();

                                saveWorld(inp);

                                left=true;
                                track_player.play();
                                break;
                            case 2:
                                track_player.pause();

                                loadWorld(inp);

                                left=true;
                                track_player.play();
                                break;
                            case 3:
                                tracknum++;

                                if(tracknum>MAXTRACKS){
                                    tracknum=1;
                                }

                                setTrack(tracknum);
                                break;
                            case 4:
                                muteUnmute();
                                break;
                            case 5:
                                sky++;

                                if(sky>=4){
                                    sky=0;
                                }

                                left=true;

                                switch(sky){
                                case 0:
                                    STATUS_BAR.setString("Night.");
                                    break;
                                case 1:
                                    STATUS_BAR.setString("Dawn.");
                                    break;
                                case 2:
                                    STATUS_BAR.setString("Day.");
                                    break;
                                case 3:
                                    STATUS_BAR.setString("Dusk.");
                                    break;
                                }

                                break;
                            case 6:
                                CUBES.clear();
                                player.pos={0,0,1};

                                generateWorld();

                                left=true;

                                break;
                            case 7:
                                gamewin.close();
                                break;
                        }
                    }

                    if(option<top){
                        option=bottom;
                    }else if(option>bottom){
                        option=top;
                    }

                    break;
            }
        }

        if(left){
            break;
        }

        PROMPT.setPosition(sf::Vector2f(0, 40*option+200));

        gamewin.clear();
        gamewin.draw(pause_v, 4, sf::Quads, &pause);

        gamewin.draw(RETURN);
        gamewin.draw(SAVEW);
        gamewin.draw(LOADW);
        gamewin.draw(QUIT);
        gamewin.draw(SWITCHT);
        gamewin.draw(MUTE);
        gamewin.draw(SKIES);
        gamewin.draw(WORLD);

        gamewin.draw(PROMPT);
        gamewin.draw(INPUT);

        gamewin.display();
    }
}

void statusMessages(){
    STATUS_BAR.setFillColor(sf::Color(255,0,0));
    STATUS_BAR.setStyle(sf::Text::Bold);
    STATUS_BAR.setCharacterSize(40);
    STATUS_BAR.setFont(wawafont);

    string status;

    if(!gamemode){
        stringstream s;
        s << "X:" << cursor.pos.x << " Y:" << cursor.pos.y << " Z:" << cursor.pos.z;

        switch(cursor.getTexcode()){
            case 0:
                s << "\nOrange block";
                break;
            case 1:
                s << "\nWawa block";
                break;
            case 2:
                s << "\nGrass";
                break;
            case 3:
                s << "\nAsphalt";
                break;
            case 4:
                s << "\nStone";
                break;
            case 5:
                s << "\nBricks";
                break;
            case 6:
                s << "\nWood";
                break;
            case 7:
                s << "\nCardboard";
                break;
            case 8:
                s << "\nMud";
                break;
            case 9:
                s << "\nTiling";
                break;
            case 10:
                s << "\nGlass";
                break;
            case 11:
                s << "\nWood Log";
                break;
            case 12:
                s << "\nLantern";
                break;
            case 13:
                s << "\nLeaves";
                break;
        }

        status=s.str();
        STATUS_BAR.setString(status);
    }else{
        status="Playing as Wawa.";

        STATUS_BAR.setString(status);
    }
}

void gameLoopInput(){
    while(gamewin.pollEvent(eventhandler)){
        switch(eventhandler.type){
            case sf::Event::Closed:
                gamewin.close();
                break;
            case sf::Event::KeyPressed:
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::W)){
                    cursor.pos.z++;
                    statusMessages();
                }

                if(sf::Keyboard::isKeyPressed(sf::Keyboard::X)){
                    cursor.pos.z--;
                    statusMessages();
                }

                if(sf::Keyboard::isKeyPressed(sf::Keyboard::A)){
                    cursor.pos.x--;
                    statusMessages();
                }

                if(sf::Keyboard::isKeyPressed(sf::Keyboard::D)){
                    cursor.pos.x++;
                    statusMessages();

                }

                if(sf::Keyboard::isKeyPressed(sf::Keyboard::E)){
                    cursor.pos.y--;
                    statusMessages();
                }

                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Z)){
                    statusMessages();
                    cursor.pos.y++;
                }

                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Num1)){
                    if(cursor.getTexcode()>=0){
                        cursor.setTexcode(cursor.getTexcode()-1);
                    }

                    if(cursor.getTexcode()<0){
                        cursor.setTexcode(BLOCK_TYPES-1);
                    }

                    statusMessages();
                }

                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Num2)){
                    if(cursor.getTexcode()<=BLOCK_TYPES-1){
                        cursor.setTexcode(cursor.getTexcode()+1);
                    }

                    if(cursor.getTexcode()>=BLOCK_TYPES){
                        cursor.setTexcode(0);
                    }

                    statusMessages();
                }

                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Backspace)){
                    if(deleteBlock(cursor)){
                        STATUS_BAR.setString("Deleted block.");
                    }
                }

                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)){
                    STATUS_BAR.setString("Placed block.");
                    placeBlock(cursor);
                }

                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Tab)){
                    gamemode=!gamemode;
                    statusMessages();
                }

                if(sf::Keyboard::isKeyPressed(sf::Keyboard::P)){
                    pauseLoop();
                }

                if(sf::Keyboard::isKeyPressed(sf::Keyboard::O)){
                    pauseLoop();
                }

                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)){
                    pauseLoop();
                }

                if(sf::Keyboard::isKeyPressed(sf::Keyboard::F1)){
                    pauseLoop();
                }

                break;
            default:

                if(sf::Mouse::isButtonPressed(sf::Mouse::Left)){
                    UNIT_SIZE+=10;
                    updateProjection();
                }

                if(sf::Mouse::isButtonPressed(sf::Mouse::Right)){
                    UNIT_SIZE-=10;
                    updateProjection();
                }

                break;
        }
    }

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up)){
        VIEW_Y+=UNIT_SIZE;
    }

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down)){
        VIEW_Y-=UNIT_SIZE;
    }

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left)){
        VIEW_X+=UNIT_SIZE;
    }

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right)){
        VIEW_X-=UNIT_SIZE;
    }
}

void gameLoopInputPlayer(){
    statusMessages();

    while(gamewin.pollEvent(eventhandler)){
        switch(eventhandler.type){
            case sf::Event::Closed:
                gamewin.close();
                break;
            case sf::Event::KeyPressed:
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Tab)){
                    gamemode=!gamemode;
                    statusMessages();
                }

                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)){
                    pauseLoop();
                }

                break;
            default:
            if(sf::Mouse::isButtonPressed(sf::Mouse::Left)){
                    UNIT_SIZE+=10;
                    updateProjection();
                }

                if(sf::Mouse::isButtonPressed(sf::Mouse::Right)){
                    UNIT_SIZE-=10;
                    updateProjection();
                }

                break;
        }
    }

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up)){
        player.setDirection(NORTH);
        player.moveSprite();
    }

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down)){
        player.setDirection(SOUTH);
        player.moveSprite();
    }

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left)){
        player.setDirection(WEST);
        player.moveSprite();
    }

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right)){
        player.setDirection(EAST);
        player.moveSprite();
    }

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::M)){
        if(player.pos.x>uni_npc.pos.x){
            uni_npc.setDirection(EAST);
            uni_npc.moveSprite();
        }else if(player.pos.x<uni_npc.pos.x){
            uni_npc.setDirection(WEST);
            uni_npc.moveSprite();
        }

        if(player.pos.y>uni_npc.pos.y){
            uni_npc.setDirection(SOUTH);
            uni_npc.moveSprite();
        }else if(player.pos.y<uni_npc.pos.y){
            uni_npc.setDirection(NORTH);
            uni_npc.moveSprite();
        }
    }
}

void drawCursor(){
    sf::CircleShape mcursor(UNIT_SIZE/2);
    mcursor.setPointCount(3);

    mcursor.setOrigin(sf::Vector2f(UNIT_SIZE/2, UNIT_SIZE/2));

    if(!gamemode){
        int mouseX=sf::Mouse::getPosition(gamewin).x;
        int mouseY=sf::Mouse::getPosition(gamewin).y;

        mcursor.setPosition(sf::Vector2f(mouseX, mouseY));
    }else{
        vect crosshair=player.getPosition();

        switch(player.getDirection()){
            case NORTH:
                crosshair.y-=AIM_DIS;
                break;
            case SOUTH:
                crosshair.y+=AIM_DIS;
                break;
            case EAST:
                crosshair.x+=AIM_DIS;
                break;
            case WEST:
                crosshair.x-=AIM_DIS;
                break;
        }

        pix sx={crosshair.x*proj_x.x, crosshair.x*proj_x.y};
        pix sy={crosshair.y*proj_y.x, crosshair.y*proj_y.y};
        pix sz={crosshair.z*proj_z.x, crosshair.z*proj_z.y};

        int crosshair_sx=VIEW_X+sx.x+sy.x+sz.x;
        int crosshair_sy=VIEW_Y+sx.y+sy.y+sz.y;

        mcursor.setPosition(sf::Vector2f(crosshair_sx, crosshair_sy));
    }

    gamewin.draw(mcursor);
}

void titleLoop(){
    bool closed=false;

    sf::Vertex title_v[]=
    {
        sf::Vertex(sf::Vector2f(0,0), sf::Vector2f(0,0)),
        sf::Vertex(sf::Vector2f(WIN_X, 0), sf::Vector2f(800, 0)),
        sf::Vertex(sf::Vector2f(WIN_X, WIN_Y), sf::Vector2f(800, 600)),
        sf::Vertex(sf::Vector2f(0, WIN_Y), sf::Vector2f(0, 600))
    };

    while(gamewin.isOpen()){
        while(gamewin.pollEvent(eventhandler)){
            switch(eventhandler.type){
                case sf::Event::Closed:
                    gamewin.close();
                    break;
                case sf::Event::KeyPressed:
                    closed=true;
                    break;
            }
        }

        if(closed){
            break;
        }

        gamewin.draw(title_v, 4, sf::Quads, &title);
        gamewin.display();
    }
}

void gameLoop(){
    sf::Color bg(skyboxes[sky]);

    while(gamewin.isOpen()){
        bg=skyboxes[sky];

        if(gamemode){
            gameLoopInputPlayer();
        }else{
            gameLoopInput();
        }

        updateUni();

        calculateDepthSet();
        sortSet();

        gamewin.clear(bg);

        renderBlocks();
        cursor.render();

        drawCursor();
        gamewin.draw(STATUS_BAR);

        gamewin.display();
    }
}

int main(){
    setUpProgram();

    titleLoop();
    pauseLoop();

    gameLoop();

    return 0;
}
