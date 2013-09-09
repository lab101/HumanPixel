

#include "TilePlayer.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"

using namespace ci;
using namespace ci::app;
using namespace std;

const float tileUV = 0.125;
//const float width = 200;






std::map<std::string,ci::gl::Texture> TilePlayer::textures;

//int Something::s_nValue = 1;

void TilePlayer::setup(ci::fs::path path){


    state = idle;

    frameNr = 0;
    
    width = 80;
    
    mMesh.appendVertex(Vec3f(0,0,0));
    mMesh.appendVertex(Vec3f(0,width,0));
    mMesh.appendVertex(Vec3f(width,0,0));
    mMesh.appendVertex(Vec3f(width,width,0));
    
    mMesh.appendTexCoord(Vec2f(0,0));
    mMesh.appendTexCoord(Vec2f(0,tileUV));
    mMesh.appendTexCoord(Vec2f(tileUV,0));
    mMesh.appendTexCoord(Vec2f(tileUV,tileUV));
    
    
    mMesh.appendTriangle(0, 1, 2);
    mMesh.appendTriangle(1, 2, 3);
    
    lastUpdateTime = getElapsedSeconds();
    
    
    std::map<std::string,gl::Texture>::iterator it;

    it=textures.find(path.string());
    
    if(it == textures.end())
    {
        mTexture = loadImage(path);
        textures.insert ( std::pair<std::string,gl::Texture>(path.string(),mTexture) );
        std::cout << "loading " << path << std::endl;

    }else{
        mTexture = textures[path.string()];
        std::cout << "yeah cached " << std::endl;
    }
//        
//        )
//        
//        
//    
//    if (info_.find(names_[i]) != info_.end())


    // first insert function version (single parameter):
    // mymap.insert ( std::pair<char,int>('a',100) );

}



void TilePlayer::update(){
    if (getElapsedSeconds() -  lastUpdateTime > 0.004) {
        lastUpdateTime = getElapsedSeconds();
        
        int columnNr = frameNr % 8;
        int rowNr = frameNr / 8;


        mMesh.getTexCoords()[0] = Vec2f(columnNr * tileUV,rowNr * tileUV);
        mMesh.getTexCoords()[1] = Vec2f(columnNr * tileUV,rowNr * tileUV + tileUV);
        mMesh.getTexCoords()[2] = Vec2f(columnNr * tileUV + tileUV,rowNr * tileUV);
        mMesh.getTexCoords()[3] = Vec2f(columnNr * tileUV + tileUV,rowNr * tileUV + tileUV);
        
        
        ci::Rand r;
      //  bool forceIdle = (getElapsedSeconds() -  lastTurnTime > 24) && state == black && (r.nextFloat(0, 10) < 1) ;
//        
//        
//        playState tmpState = state;
     //  if(forceIdle){
       //     state = idle;
       // }
//        
        switch (state) {
            case idle:
                ++frameNr;
                if(frameNr > 16) frameNr = 0;

                break;
            case black:
                if(frameNr == 40){
                    //frameNr = 11;
                }else{
                    if(frameNr < 40) frameNr++;
                    if(frameNr > 40) frameNr--;
                }


                break;
            case white:
                if(frameNr == 63){
                   // frameNr = 14;
                }else{
                    if(frameNr < 63) frameNr++;
                    if(frameNr > 63) frameNr--;
                }

                break;
                
            default:
                break;
        }
        
       // std::cout << frameNr <<  "coords " << columnNr << " : " << rowNr << std::endl;

    }
    

}


void TilePlayer::setWhite(){
    
   // if(state == white) return;

    previousState = state;
    state = white;
    lastTurnTime = getElapsedSeconds();
}

void TilePlayer::setBlack(){
   // if(state == black) return;

    previousState = state;
    state = black;
    lastTurnTime = getElapsedSeconds();


}



void TilePlayer::draw(ci::Vec2f pos){

    //gl::draw(mTexture);

	mTexture.enableAndBind();
    
    gl::pushMatrices();
    gl::translate(pos);
    gl::draw(mMesh);
    gl::popMatrices();

   // mTexture.unbind();
    

    
}
