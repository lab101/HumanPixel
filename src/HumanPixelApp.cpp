#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"

#include "cinder/Utilities.h"

#include "cinder/Capture.h"
#include "cinder/gl/Texture.h"

#include "cinder/ip/Resize.h"
#include "cinder/ImageIo.h"

#include <boost/algorithm/string.hpp>


#include "cinder/audio/Output.h"
#include "cinder/audio/Io.h"

#include "cinder/Rand.h"

#include "cinder/gl/Fbo.h"


#include "cinder/Font.h"

#include "cinder/Text.h"
#include "cinder/Rand.h"
#include "cinder/gl/TextureFont.h"



#include "TilePlayer.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class HumanPixelApp : public AppNative {
  public:
    
    
	void setup();
	void mouseDown( MouseEvent event );
    void keyDown( KeyEvent event);
    
	void update();
	void draw();
    
    void resize();
    
    void loadImages();
    
private:
    
    CaptureRef			mCapture;
	gl::TextureRef		mTexture;
	gl::TextureRef		mTexturePreviewCam;
    
    gl::Fbo            mFbo;
    
    Surface             mSurface;
    Surface             mSmallSurface;
    Surface*            mSpriteSheet;
    
    audio::SourceRef mActionSound;
    audio::SourceRef mSaveSound;
    
    Font				mFont;
    cinder::gl::TextureFontRef	mTextureFont;


    
    TilePlayer player;
    
    double lastCapture;
    int captureNr;
    
    int const spriteWidth = 128;
    int const spriteHeight = 128;
    
    bool isCapturing;
    bool isRecordView;

    std::vector<fs::path> photos;
    std::vector<TilePlayer> pixels;
    
};


const int nrOfTiles = 300;

void HumanPixelApp::setup()
{
    setWindowSize(1024, 768);
    
    try {
        
		mCapture = Capture::create( 1024, 768 );
	//	mCapture->start();
        
        mSpriteSheet = new Surface(1024,1024,false);
        
        lastCapture = getElapsedSeconds();
        captureNr = 0;
        
        isCapturing = false;
        isRecordView = false;
        
        mActionSound = audio::load( loadAsset( "plop.mp3" ) );
        mSaveSound = audio::load( loadAsset( "save.mp3" ) );

        mFont = Font( "Helvetica", 60 );
        mTextureFont = gl::TextureFont::create( mFont );

        

//        gl::enableDepthRead();
//        gl::enableDepthWrite();
//        gl::enableAlphaBlending();


    
    
	}catch( ... ) {
		console() << "Failed to initialize capture" << std::endl;
	}
    
    
    
    loadImages();
  //  mSurface = new Surface(

}


void HumanPixelApp::loadImages(){
    pixels.clear();
    
    std::vector<fs::path> files;
    
    for( fs::directory_iterator it( getAssetPath("/")); it != fs::directory_iterator(); ++it )
		{
			string extension = it->path().extension().string();
            boost::algorithm::to_lower(extension);
            
			if( ! is_directory( *it ) &&  (extension == ".jpg" || extension == ".png")){
                    files.push_back(*it);

                }

			
		}

  
    Rand r;
    
    
    if(files.size() > 0){
        for(int i = 0; i < nrOfTiles;++i){
            
            TilePlayer tile;
            
            pixels.push_back(tile);
            pixels.back().setup(files.at(r.nextFloat(0, files.size())));
                
        }
    }


}

void HumanPixelApp::resize(){
    gl::Fbo::Format format;
    //  format.setSamples( 4 );
    format.enableDepthBuffer();
    format.enableColorBuffer();
    mFbo = gl::Fbo( getWindowWidth(), getWindowHeight() );

}

void HumanPixelApp::mouseDown( MouseEvent event )
{
}

void HumanPixelApp::update()
{
    
    
    // draw in fbo.
    mFbo.bindFramebuffer();
    gl::enableAlphaBlending();

    gl::clear();

   // gl::drawSolidCircle(Vec2f(10,10), 10);
    
    int sec= (time(NULL) % 200) / 2;

    gl::pushMatrices();
    
    Vec2f size =   mTextureFont->measureString(toString(sec));

    gl::translate(getWindowWidth()/2.0 , getWindowHeight()/2.0 - 230 ,0);
    gl::scale(21.4, 21.4);
    

    mTextureFont->drawString(toString(sec), Vec2f( - size.x/2,  size.y /2));
    gl::popMatrices();

    mFbo.unbindFramebuffer();
    
    
    
    
    
  //  return;
        
    
    for (vector<TilePlayer>::iterator it = pixels.begin(); it != pixels.end(); ++it) {
        it->update();
    }
    

    if( mCapture->checkNewFrame() ) {
        mSurface = mCapture->getSurface();
        

        
        float cropArea= 600;
        mSmallSurface = Surface(spriteWidth,spriteHeight,false);
        
        
        Area crop = Area( (mCapture->getWidth()-cropArea) / 2,  (mCapture->getHeight()-cropArea) / 2, cropArea, cropArea);
        //        Area crop = Area( 40,40, 200, 200);
        ci::ip::resize( mSurface, crop, &mSmallSurface, mSmallSurface.getBounds(), FilterTriangle() );

        mTexturePreviewCam = gl::Texture::create( mSmallSurface );

        
      //  mTexture = gl::Texture::create( mSmallSurface );

        
        
        if((getElapsedSeconds() -  lastCapture) > 0.10 && isCapturing){
            
            
            if(captureNr == 16 ||  captureNr == 40)
                audio::Output::play( mActionSound );

            lastCapture = getElapsedSeconds();
            
            int columnNr = captureNr % 8;
            int rowNr = captureNr / 8;
            
            mSpriteSheet->copyFrom(mSmallSurface, mSmallSurface.getBounds(),Vec2i(columnNr * spriteWidth,rowNr * spriteHeight));
        
           // std::cout << "capture " << rowNr  << std::endl;

        
            mTexture = gl::Texture::create( *mSpriteSheet );
            
 
            if(++captureNr >= 64){
                isCapturing = false;
                
                
                string filename = toString(time(NULL)) + ".jpg";
                writeImage(getAssetPath("/").string()  + filename, *mSpriteSheet);

                audio::Output::play( mSaveSound );

            }

        }
	}
}



void HumanPixelApp::keyDown(cinder::app::KeyEvent event){
 
    if(event.getCode() == event.KEY_SPACE && !isCapturing){
        lastCapture = getElapsedSeconds();
        captureNr = 0;
        
        isCapturing = true;
    }else if(event.getCode() == event.KEY_c){
        isRecordView = !isRecordView;
    }else if(event.getCode() == event.KEY_b){
        for (vector<TilePlayer>::iterator it = pixels.begin(); it != pixels.end(); ++it) {
            it->state = playState::black;
        }
        
    }else if(event.getCode() == event.KEY_w){
        for (vector<TilePlayer>::iterator it = pixels.begin(); it != pixels.end(); ++it) {
            it->state = playState::white;
        }
        
    }else if(event.getCode() == event.KEY_i){
        for (vector<TilePlayer>::iterator it = pixels.begin(); it != pixels.end(); ++it) {
            it->state = playState::idle;
        }
        
    } else if(event.getCode() == event.KEY_f){
        setFullScreen(!isFullScreen());
    }else if(event.getCode() == event.KEY_r){
        loadImages();
    }

}


void HumanPixelApp::draw()
{
	// clear out the window with black
    
    
    gl::clear((!isCapturing) ? Color( 0, 0, 0 ) : Color( 1, 0, 0 )  );
    
    
    
    //gl::draw( mFbo.getTexture()) ;
  // gl::draw( mFbo.getTexture(), Rectf(  0, getWindowHeight(),getWindowWidth(),0 ) );


   // mFbo.draw();
    
    //return;
    
    
    
    
    
    if(isRecordView)
    {
        gl::enableAlphaBlending();
        
        
//        if(isCapturing){
//            gl::color(1.0,.3,.0);
//            gl::drawSolidCircle(Vec2f(200,30), 20 + sin(getElapsedSeconds() * 4.0) * 1.2);
//        }
        
        gl::color(1.0,1.0,1.0);
        
        
        //    if(mTexture) gl::draw( mTexturePreviewCam,Rectf(0,0,1024,768) );
        if(mTexturePreviewCam)
            gl::draw( mTexturePreviewCam,Rectf(0,100,mTexturePreviewCam->getWidth() *4,mTexturePreviewCam->getHeight() *4) );
        
        
        int sec= (time(NULL) % 500) / 5;
        
        gl::drawString("time: " + toString(sec) , Vec2f(40,20));
        gl::drawString("camera width: " + toString(mCapture->getWidth()) + " camera height " + toString(mCapture->getHeight()) , Vec2f(40,40));

        
  
    }else{
        
        Surface s = Surface(mFbo.getTexture());
        
        Vec2f pos(0,0);
        Vec2f samplePos(0,0);

        for (vector<TilePlayer>::iterator it = pixels.begin(); it != pixels.end(); ++it) {
            
            it->draw(pos);
            
            samplePos = pos;
            samplePos.y = mFbo.getHeight() -  pos.y;
            
            if(s.getPixel(samplePos).r > 0.2){
                it->setWhite();
            }else{
                it->setBlack();
  
            }

            pos.x += it->width;
            
            
            if(pos.x > getWindowWidth()){
                pos.y += it->width;
                pos.x = 0;
            }
            

        }
    }

}

CINDER_APP_NATIVE( HumanPixelApp, RendererGl )
