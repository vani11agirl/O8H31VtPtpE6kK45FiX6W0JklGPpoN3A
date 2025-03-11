#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class GrabNode;

class GrabNodeArea : public CCNode {
    protected:
        std::vector<GrabNode *> m_nodes = {};
    public:
        // These all return the actual place it's put into, or -1 if no node or something like that
        int addNode(GrabNode *node, int place = -1);
        int moveNode(GrabNode *node, int place);
        int moveAfter(GrabNode *node, GrabNode *after);
        int moveBefore(GrabNode *node, GrabNode *before);

        void removeNode(GrabNode *node);
        
        bool hasNode(GrabNode *node);
        GrabNode *getNodeAt(int order);
        int getOrderOf(GrabNode *node);
        int getNodeCount();
        std::vector<GrabNode *> getNodes();
        
        static GrabNodeArea* create();
};

class GrabNode : public CCMenu {
    // so it can change the order
    friend class GrabNodeArea;
    protected:
        int m_order = -1;
        GrabNodeArea *m_layer = nullptr;
        CCSprite *m_dragSprite = nullptr;

        bool m_isDragging = false;
        CCPoint m_startPos = CCPointZero;
        CCPoint m_offset = CCPointZero;

        bool ccTouchBegan(cocos2d::CCTouch *touch, cocos2d::CCEvent *event) override;
        void ccTouchEnded(cocos2d::CCTouch *touch, cocos2d::CCEvent *event) override;
        void ccTouchMoved(cocos2d::CCTouch *touch, cocos2d::CCEvent *event) override;
        void ccTouchCancelled(cocos2d::CCTouch *touch,
                                cocos2d::CCEvent *event) override {
            ccTouchEnded(touch, event);
        }
        void registerWithTouchDispatcher() override;

        bool init(GrabNodeArea *layer, CCSprite *dragSprite, int order);
    public:
        int getOrder();
        GrabNodeArea *getLayer();

        static GrabNode *create(GrabNodeArea *layer, CCSprite *dragSprite, int order = -1);
};