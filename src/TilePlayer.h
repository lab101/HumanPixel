//
//  TilePlayer.h
//  HumanPixel
//
//  Created by Kris Meeusen on 31/08/13.
//
//

#ifndef __HumanPixel__TilePlayer__
#define __HumanPixel__TilePlayer__

#include <iostream>
#include "cinder/gl/Texture.h"
#include "cinder/Filesystem.h"
#include "cinder/TriMesh.h"

#include <map>


 enum playState {idle=0,black=1,white=2 };

class TilePlayer{

    
    ci::gl::Texture mTexture;
    ci::TriMesh mMesh;
    
    int frameNr;
    
    float lastUpdateTime;
    float lastTurnTime;
    
    static std::map<std::string,ci::gl::Texture> textures;
    


public:
    
    float width;
    playState state;
    playState previousState;

    void setup(ci::fs::path path);
    void update();
    void draw(ci::Vec2f pos);
    
    
    void setBlack();
    void setWhite();
    
    
    

    
};



#endif /* defined(__HumanPixel__TilePlayer__) */
