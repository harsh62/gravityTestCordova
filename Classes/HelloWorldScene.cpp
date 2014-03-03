//
//  HelloWorldScene.cpp
//  FreehandDemo
//
//  Created by Sónia Sousa on 3/24/13.
//  Copyright __MyCompanyName__ 2013. All rights reserved.
//
#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"

using namespace cocos2d;
using namespace CocosDenshion;

#define PTM_RATIO 32

enum {
    kTagParentNode = 1,
};

PhysicsSprite::PhysicsSprite()
: m_pBody(NULL)
{

}

void PhysicsSprite::setPhysicsBody(b2Body * body)
{
    m_pBody = body;
}



// this method will only get called if the sprite is batched.
// return YES if the physics values (angles, position ) changed
// If you return NO, then nodeToParentTransform won't be called.
bool PhysicsSprite::isDirty(void)
{
    return true;
}

// returns the transform matrix according the Chipmunk Body values
CCAffineTransform PhysicsSprite::nodeToParentTransform(void)
{
    b2Vec2 pos  = m_pBody->GetPosition();

    float x = pos.x * PTM_RATIO;
    float y = pos.y * PTM_RATIO;

    if ( isIgnoreAnchorPointForPosition() ) {
        x += m_obAnchorPointInPoints.x;
        y += m_obAnchorPointInPoints.y;
    }

    // Make matrix
    float radians = m_pBody->GetAngle();
    float c = cosf(radians);
    float s = sinf(radians);

    if( ! m_obAnchorPointInPoints.equals(CCPointZero) ){
        x += c*-m_obAnchorPointInPoints.x + -s*-m_obAnchorPointInPoints.y;
        y += s*-m_obAnchorPointInPoints.x + c*-m_obAnchorPointInPoints.y;
    }

    // Rot, Translate Matrix
    m_sTransform = CCAffineTransformMake( c,  s,
        -s,    c,
        x,    y );

    return m_sTransform;
}
//{second funciton to get called }
HelloWorld::HelloWorld()
{
    
    
    setTouchEnabled( true );
    setAccelerometerEnabled( true );

    CCSize s = CCDirector::sharedDirector()->getWinSize();
    // init physics
    CCSprite *backGroundSprite = CCSprite::create("bg.jpg");
    backGroundSprite->setPosition(ccp( s.width/2, s.height-300));
    
    addChild(backGroundSprite);
    
    this->initPhysics();

    CCSpriteBatchNode *parent = CCSpriteBatchNode::create("blocks.png", 100);
    m_pSpriteTexture = parent->getTexture();

    addChild(parent, 0, kTagParentNode);


    //addNewSpriteAtPosition(ccp(s.width/2, s.height/2));
    
  

    CCLabelTTF *label = CCLabelTTF::create("Tap screen", "Marker Felt", 32);
    addChild(label, 0);
    label->setColor(ccc3(0,0,255));
    label->setPosition(ccp( s.width/2, s.height-50));
    
    scheduleUpdate();
    
    target = CCRenderTexture::create(s.width, s.height, kCCTexture2DPixelFormat_RGBA8888);
    target->retain();
    target->setPosition(ccp(s.width / 2, s.height / 2));
    
    this->addChild(target);
    
    brush = CCSprite::create("smallBrush.png");
    brush->retain();
 

}


void HelloWorld::addRectangleBetweenPointsToBody(b2Body *body, CCPoint start, CCPoint end)
{
    
    float distance = sqrt( pow(end.x - start.x, 2) + pow(end.y - start.y, 2));
    
    float sx=start.x;
    float sy=start.y;
    float ex=end.x;
    float ey=end.y;
    float dist_x=sx-ex;
    float dist_y=sy-ey;
    float angle= atan2(dist_y,dist_x);
    
    float px= (sx+ex)/2/PTM_RATIO - body->GetPosition().x;
    float py = (sy+ey)/2/PTM_RATIO - body->GetPosition().y;
    
    float width = abs(distance)/PTM_RATIO;
    float height =  brush->boundingBox().size.height/PTM_RATIO;
    
    b2PolygonShape boxShape;
    boxShape.SetAsBox(width / 2, height / 2, b2Vec2(px,py),angle);
    
    b2FixtureDef boxFixtureDef;
    boxFixtureDef.shape = &boxShape;
    boxFixtureDef.density = 5;
    boxFixtureDef.friction = 0.1f;
    boxFixtureDef.restitution = 0.1f;
    
    boxFixtureDef.filter.categoryBits = 2;
    
    body->CreateFixture(&boxFixtureDef);
}

CCRect HelloWorld::getBodyRectangle(b2Body* body)
{
    
    CCSize s = CCDirector::sharedDirector()->getWinSize();
    
    float minX2 = s.width;
    float maxX2 = 0;
    float minY2 = s.height;
    float maxY2 = 0;
    
    const b2Transform& xf = body->GetTransform();
    for (b2Fixture* f = body->GetFixtureList(); f; f = f->GetNext())
    {
        
        b2PolygonShape* poly = (b2PolygonShape*)f->GetShape();
        int32 vertexCount = poly->m_vertexCount;
        b2Assert(vertexCount <= b2_maxPolygonVertices);
        
        for (int32 i = 0; i < vertexCount; ++i)
        {
            b2Vec2 vertex = b2Mul(xf, poly->m_vertices[i]);
            
            
            if(vertex.x < minX2)
            {
                minX2 = vertex.x;
            }
            
            if(vertex.x > maxX2)
            {
                maxX2 = vertex.x;
            }
            
            if(vertex.y < minY2)
            {
                minY2 = vertex.y;
            }
            
            if(vertex.y > maxY2)
            {
                maxY2 = vertex.y;
            }
        }
    }
    
    maxX2 *= PTM_RATIO;
    minX2 *= PTM_RATIO;
    maxY2 *= PTM_RATIO;
    minY2 *= PTM_RATIO;
    
    float width2 = maxX2 - minX2;
    float height2 = maxY2 - minY2;
    
    float remY2 = s.height - maxY2;
    
    return CCRectMake(minX2, remY2, width2, height2);
}


HelloWorld::~HelloWorld()
{
    delete world;
    world = NULL;
    
    //delete m_debugDraw;
}

//{third function called from HelloWorld class constructor} i.e initialising the physics
void HelloWorld::initPhysics()
{
    
    CCSize s = CCDirector::sharedDirector()->getWinSize();
    



    b2Vec2 gravity;
    gravity.Set(0.0f, -20.0f);
    world = new b2World(gravity);

    // Do we want to let bodies sleep?
    world->SetAllowSleeping(true);

    world->SetContinuousPhysics(true);

//     m_debugDraw = new GLESDebugDraw( PTM_RATIO );
//     world->SetDebugDraw(m_debugDraw);

    uint32 flags = 0;
    flags += b2Draw::e_shapeBit;
    //        flags += b2Draw::e_jointBit;
    //        flags += b2Draw::e_aabbBit;
    //        flags += b2Draw::e_pairBit;
    //        flags += b2Draw::e_centerOfMassBit;
    //m_debugDraw->SetFlags(flags);


    // Define the ground body.
    b2BodyDef groundBodyDef;
    groundBodyDef.position.Set(0, 0); // bottom-left corner

    // Call the body factory which allocates memory for the ground body
    // from a pool and creates the ground box shape (also from a pool).
    // The body is also added to the world.
    b2Body* groundBody = world->CreateBody(&groundBodyDef);

    // Define the ground box shape.
    b2EdgeShape groundBox;

    // bottom

    groundBox.Set(b2Vec2(0,0), b2Vec2(s.width/PTM_RATIO,0));
    groundBody->CreateFixture(&groundBox,0);

    // top
    groundBox.Set(b2Vec2(0,s.height/PTM_RATIO), b2Vec2(s.width/PTM_RATIO,s.height/PTM_RATIO));
    groundBody->CreateFixture(&groundBox,0);

    // left
    groundBox.Set(b2Vec2(0,s.height/PTM_RATIO), b2Vec2(0,0));
    groundBody->CreateFixture(&groundBox,0);

    // right
    groundBox.Set(b2Vec2(s.width/PTM_RATIO,s.height/PTM_RATIO), b2Vec2(s.width/PTM_RATIO,0));
    groundBody->CreateFixture(&groundBox,0);
    
    
    //Create the ball :)
    CCSize screenSize = CCDirector::sharedDirector()->getWinSize();

    CCSprite *_bgSprite = CCSprite::create("ball.png",CCRect(0, 0, 52, 52));
    //_bgSprite->setPosition(ccp(screenSize.width/2,0));
    addChild(_bgSprite);
    
    b2Body *_body;
    
    // Create ball body and shape
    b2BodyDef ballBodyDef;
    ballBodyDef.type = b2_dynamicBody;
    ballBodyDef.position.Set(screenSize.width/(PTM_RATIO*2), 300/PTM_RATIO);
    ballBodyDef.userData = _bgSprite;
    _body = world->CreateBody(&ballBodyDef);
    
    b2CircleShape circle;
    circle.m_radius = 26.0/PTM_RATIO;
    
    b2FixtureDef ballShapeDef;
    ballShapeDef.shape = &circle;
    ballShapeDef.density = 1.0f;
    ballShapeDef.friction = 0.5f;
    ballShapeDef.restitution = 0.7f;
    _body->CreateFixture(&ballShapeDef);
    
    
    //Initialize buttons
    CCMenuItem *pCloseItem = CCMenuItemImage::create(
                                                     "CloseNormal.png",
                                                     "CloseSelected.png",
                                                     this,
                                                     menu_selector(HelloWorld::menuCloseCallback) );
    
    CCMenu* pMenu = CCMenu::create(pCloseItem, NULL);
    pMenu->setPosition(20, screenSize.height-20);
    this->addChild(pMenu, 1);
    
    CCMenuItem *restartItem = CCMenuItemImage::create(
                                                     "restart.png",
                                                     "restart.png",
                                                     this,
                                                     menu_selector(HelloWorld::menuRestartCallback) );
    
    CCMenu* retartMenu = CCMenu::create(restartItem, NULL);
    retartMenu->setPosition(screenSize.width-20, screenSize.height-20);
    this->addChild(retartMenu, 1);

    
}

void HelloWorld::draw()
{
    //
    // IMPORTANT:
    // This is only for debug purposes
    // It is recommend to disable it
    //
    CCLayer::draw();

    ccGLEnableVertexAttribs( kCCVertexAttribFlag_Position );

    kmGLPushMatrix();

    world->DrawDebugData();

    kmGLPopMatrix();
}

void HelloWorld::addNewSpriteAtPosition(CCPoint p)
{
    CCLOG("Add sprite %0.2f x %02.f",p.x,p.y);
    CCNode* parent = getChildByTag(kTagParentNode);
    
    //We have a 64x64 sprite sheet with 4 different 32x32 images.  The following code is
    //just randomly picking one of the images
    int idx = (CCRANDOM_0_1() > .5 ? 0:1);
    int idy = (CCRANDOM_0_1() > .5 ? 0:1);
    PhysicsSprite *sprite = new PhysicsSprite();
    sprite->initWithTexture(m_pSpriteTexture, CCRectMake(32 * idx,32 * idy,32,32));
    sprite->autorelease();
    
    parent->addChild(sprite);
    
    sprite->setPosition( CCPointMake( p.x, p.y) );
    
    // Define the dynamic body.
    //Set up a 1m squared box in the physics world
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(p.x/PTM_RATIO, p.y/PTM_RATIO);
    
    b2Body *body = world->CreateBody(&bodyDef);
    
    // Define another box shape for our dynamic body.
    b2PolygonShape dynamicBox;
    dynamicBox.SetAsBox(.5f, .5f);//These are mid points for our 1m box
    
    // Define the dynamic body fixture.
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &dynamicBox;    
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.2f;
    fixtureDef.restitution = 0.8f;
    body->CreateFixture(&fixtureDef);
    
    sprite->setPhysicsBody(body);
}


void HelloWorld::update(float dt)
{
    //It is recommended that a fixed time step is used with Box2D for stability
    //of the simulation, however, we are using a variable time step here.
    //You need to make an informed choice, the following URL is useful
    //http://gafferongames.com/game-physics/fix-your-timestep/
    
    int velocityIterations = 8;
    int positionIterations = 1;
    
    // Instruct the world to perform a single step of simulation. It is
    // generally best to keep the time step and iterations fixed.
    world->Step(dt, velocityIterations, positionIterations);
    
    //Iterate over the bodies in the physics world
    for (b2Body* b = world->GetBodyList(); b; b = b->GetNext())
    {
        if (b->GetUserData() != NULL) {
            //Synchronize the AtlasSprites position and rotation with the corresponding body
            CCSprite* myActor = (CCSprite*)b->GetUserData();
            myActor->setPosition( CCPointMake( b->GetPosition().x * PTM_RATIO, b->GetPosition().y * PTM_RATIO) );
            myActor->setRotation( -1 * CC_RADIANS_TO_DEGREES(b->GetAngle()) );
        }    
    }
}


void HelloWorld::ccTouchesBegan(CCSet* touches, CCEvent* event)
{
    isSingleTouch =true;
    
    CCSize s = CCDirector::sharedDirector()->getWinSize();
    
    CCSetIterator it;
    CCTouch* touch;
    
    for( it = touches->begin(); it != touches->end(); it++)
    {
        touch = (CCTouch*)(*it);
        
        if(!touch)
            break;
        
		plataformPoints.clear();
        
        
        CCPoint location = touch->getLocationInView();
        location = CCDirector::sharedDirector()->convertToGL(location);
        
		plataformPoints.push_back(location);
        
        previousLocation = location;
        
        b2BodyDef myBodyDef;
        myBodyDef.type = b2_staticBody;
        myBodyDef.position.Set(location.x/PTM_RATIO,location.y/PTM_RATIO);
        currentPlatformBody = world->CreateBody(&myBodyDef);
        
    }
}

void HelloWorld::ccTouchesEnded(CCSet* touches, CCEvent* event)
{
    //Add a new body/atlas sprite at the touched location
    CCSetIterator it;
    CCTouch* touch;
    
    for( it = touches->begin(); it != touches->end(); it++)
    {
        touch = (CCTouch*)(*it);
        
        if(!touch)
            break;
        if(isSingleTouch){
            for(int i=0;i<spriteVector.size();i++){
  
                if(spriteVector.at(i)->boundingBox().containsPoint(previousLocation)){
                    spriteVector.at(i)->removeFromParent();
                    world->DestroyBody(b2BodyVector.at(i));
                    spriteVector.erase(spriteVector.begin()+i-1);
                    b2BodyVector.erase(b2BodyVector.begin()+i-1);
                    break;
                }
            }
            break;
        }
        
        b2BodyDef myBodyDef;
        myBodyDef.type = b2_dynamicBody; //this will be a dynamic body
        myBodyDef.position.Set(currentPlatformBody->GetPosition().x, currentPlatformBody->GetPosition().y); //set the starting position
        b2Body* newBody = world->CreateBody(&myBodyDef);
        
        
        for(int i=0; i < plataformPoints.size()-1; i++)
        {
            CCPoint start = plataformPoints[i];
            CCPoint end = plataformPoints[i+1];
            addRectangleBetweenPointsToBody(newBody,start,end);
            
        }
        
        world->DestroyBody(currentPlatformBody);
        
        
        CCSize s = CCDirector::sharedDirector()->getWinSize();
        
        CCRect bodyRectangle = getBodyRectangle(newBody);
        
        CCImage *pImage = target->newCCImage();
        CCTexture2D *tex = CCTextureCache::sharedTextureCache()->addUIImage(pImage,NULL);
        CC_SAFE_DELETE(pImage);
        
        //CCSprite *sprite = CCSprite::create(tex, bodyRectangle);
        
        CCSprite *sprite = new CCSprite();
        sprite->initWithTexture(tex, bodyRectangle);
        
        
        float anchorX = newBody->GetPosition().x * PTM_RATIO - bodyRectangle.origin.x;
        float anchorY = bodyRectangle.size.height - (s.height - bodyRectangle.origin.y - newBody->GetPosition().y * PTM_RATIO);
        
        sprite->setAnchorPoint(ccp(anchorX / bodyRectangle.size.width,  anchorY / bodyRectangle.size.height));
        newBody->SetUserData(sprite);
        addChild(sprite);
        
        spriteVector.push_back(sprite);
        b2BodyVector.push_back(newBody);
        
        removeChild(target,true);
        target->release();
        
        target = CCRenderTexture::create(s.width, s.height, kCCTexture2DPixelFormat_RGBA8888);
        target->retain();
        target->setPosition(ccp(s.width / 2, s.height / 2));
        
        this->addChild(target, 5);
        
        
    }
}

void HelloWorld::ccTouchesMoved(CCSet* touches, CCEvent* event)
{
    isSingleTouch=false;
    CCTouch *touch = (CCTouch *)touches->anyObject();
    CCPoint start = touch->getLocationInView();
    start = CCDirector::sharedDirector()->convertToGL(start);
    CCPoint end = touch->getPreviousLocationInView();
    end = CCDirector::sharedDirector()->convertToGL(end);
    
    target->begin();
    
    float distance = ccpDistance(start, end);
    
    for (int i = 0; i < distance; i++)
    {
        float difx = end.x - start.x;
        float dify = end.y - start.y;
        float delta = (float)i / distance;
        brush->setPosition(
                           ccp(start.x + (difx * delta), start.y + (dify * delta)));
        
		//brush->setOpacity(0.5);
        brush->visit();
    }
    target->end();
    
    
	CCSetIterator it;
	for( it = touches->begin(); it != touches->end(); it++)
    {
        CCTouch* touch = (CCTouch*)(*it);
        
        if(!touch)
            break;
        
        CCPoint location = touch->getLocationInView();
        
        location = CCDirector::sharedDirector()->convertToGL(location);
        
        float distance = sqrt( pow(location.x - previousLocation.x, 2) + pow(location.y - previousLocation.y, 2));
        
        if(distance > 15)
        {
            
            addRectangleBetweenPointsToBody(currentPlatformBody, previousLocation, location);
			plataformPoints.push_back(location);
            previousLocation = location;
        }
        
    }
    
}

//First function to get called {Constructor of the application}
CCScene* HelloWorld::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // add layer as a child to scene
    CCLayer* layer = new HelloWorld();
    scene->addChild(layer);
    layer->release();
    
    return scene;
}

void HelloWorld::menuCloseCallback(CCObject* pSender)
{
    CCDirector::sharedDirector()->end();
    exit(0);
}

void HelloWorld::menuRestartCallback(CCObject* pSender)
{

    CCDirector::sharedDirector()->replaceScene(scene());
}






