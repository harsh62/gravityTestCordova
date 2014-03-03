//
//  HelloWorldScene.h
//  FreehandDemo
//
//  Created by Sónia Sousa on 3/24/13.
//  Copyright __MyCompanyName__ 2013. All rights reserved.
//
#ifndef __HELLO_WORLD_H__
#define __HELLO_WORLD_H__

// When you import this file, you import all the cocos2d classes
#include "cocos2d.h"
#include "Box2D.h"
#include <vector>


class PhysicsSprite : public cocos2d::CCSprite
{
public:
    PhysicsSprite();
    void setPhysicsBody(b2Body * body);
    virtual bool isDirty(void);
    virtual cocos2d::CCAffineTransform nodeToParentTransform(void);
private:
    b2Body* m_pBody;    // strong ref
};

class HelloWorld : public cocos2d::CCLayer {
public:
    ~HelloWorld();
    HelloWorld();
    bool isSingleTouch;
        
    vector<cocos2d::CCSprite*> spriteVector;
    vector<b2Body*> b2BodyVector;
    
    // returns a Scene that contains the HelloWorld as the only child
    static cocos2d::CCScene* scene();
    
    void menuCloseCallback(CCObject* pSender);
    void menuRestartCallback(CCObject* pSender);
    
    void initPhysics();
    // adds a new sprite at a given coordinate
    void addNewSpriteAtPosition(cocos2d::CCPoint p);

    virtual void draw();
    virtual void ccTouchesEnded(cocos2d::CCSet* touches, cocos2d::CCEvent* event);
	virtual void ccTouchesMoved(cocos2d::CCSet* touches, cocos2d::CCEvent* event);
	virtual void ccTouchesBegan(cocos2d::CCSet* touches, cocos2d::CCEvent* event);
    void update(float dt);
    
    void addRectangleBetweenPointsToBody(b2Body* body, cocos2d::CCPoint start, cocos2d::CCPoint end);
	cocos2d::CCRect getBodyRectangle(b2Body* body);
    
private:
    b2World* world;

    cocos2d::CCTexture2D* m_pSpriteTexture; // weak ref
    
	cocos2d::CCRenderTexture *target;
    cocos2d::CCSprite *brush;
    
	//GLESDebugDraw* debugDraw;
    
	cocos2d::CCPoint previousLocation;
	b2Body* currentPlatformBody;
    
	std::vector<cocos2d::CCPoint> plataformPoints;
};

#endif // __HELLO_WORLD_H__
