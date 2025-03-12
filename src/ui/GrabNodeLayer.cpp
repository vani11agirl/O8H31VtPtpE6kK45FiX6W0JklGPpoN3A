#include "GrabNodeLayer.hpp"

bool GrabNodeLayer::init()
{
    if (!CCLayer::init())
        return false;

    this->ignoreAnchorPointForPosition(false);
    this->setPosition(CCPointZero);
    this->setContentSize(CCPointZero);
    this->setAnchorPoint(ccp(0.5f, 0.5f));

    this->setTouchEnabled(true);

    return true;
}

CCPoint GrabNodeLayer::getAxisForLocked(LockedAxis axis)
{
    switch (axis)
    {
        case LockedAxis::Horizontal:
            return ccp(1, 0);

        case LockedAxis::Vertical:
            return ccp(0, 1);

        default:
            return ccp(1, 1);
    }
}

void GrabNodeLayer::setLockedAxis(LockedAxis axis)
{
    this->axis = axis;
}

LockedAxis GrabNodeLayer::getLockedAxis() const
{
    return axis;
}

void GrabNodeLayer::setNodeToGrab(CCNode* node)
{
    nodeToGrab = node;
}

CCNode* GrabNodeLayer::getNodeToGrab() const
{
    return nodeToGrab;
}

void GrabNodeLayer::setOnStartDrag(const std::function<void()>& callback)
{
    this->onStartDrag = callback;
}

void GrabNodeLayer::setOnEndDrag(const std::function<void()>& callback)
{
    this->onEndDrag = callback;
}

void GrabNodeLayer::registerWithTouchDispatcher(void)
{
    CCTouchDispatcher::get()->addTargetedDelegate(this, INT_MIN + 1, true);
}

bool GrabNodeLayer::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
    if (nodeToGrab && nodeIsVisible(this))
    {
        CCRect grabberBounds = CCRect(
            this->convertToWorldSpace(CCPointZero),
            this->getScaledContentSize()
        );

        if (grabberBounds.containsPoint(pTouch->getLocation()))
        {
            isDragging = true;
            startPos = nodeToGrab->getPosition();

            if (onStartDrag)
                onStartDrag();

            return true;
        }
    }
    return false;
}


void GrabNodeLayer::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent)
{
    if (isDragging)
    {
        nodeToGrab->setPosition(startPos + (pTouch->getLocation() - pTouch->getStartLocation()) * getAxisForLocked(axis));
    }
}

void GrabNodeLayer::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
{
    isDragging = false;
    if (onEndDrag) onEndDrag();
}

void GrabNodeLayer::ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent)
{
    ccTouchEnded(pTouch, pEvent);
}