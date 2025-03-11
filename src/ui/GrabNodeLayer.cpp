#include "GrabNodeLayer.hpp"

// GrabNodeLayer

int GrabNodeArea::addNode(GrabNode *node, int place) {
  if (!node || node->m_layer != nullptr) return -1;
  if (place == -1) place = m_nodes.size();
  m_nodes.insert(m_nodes.begin() + place, node);
  for (int i = place; i < m_nodes.size(); ++i) {
    m_nodes[i]->m_order = i;
  }
  addChild(node);
  node->m_order = place;
  return place;
}
int GrabNodeArea::moveNode(GrabNode *node, int place) {
  if (!node || node->m_layer != this || !hasNode(node)) return -1;
  if (place == -1) place = m_nodes.size();
  m_nodes.erase(m_nodes.begin() + node->m_order);
  if (place > node->m_order) --place;
  m_nodes.insert(m_nodes.begin() + place, node);
  for (int i = std::min(node->m_order, place); i < m_nodes.size(); ++i) {
    m_nodes[i]->m_order = i;
  }
  return place;
}
int GrabNodeArea::moveAfter(GrabNode *node, GrabNode *after) {
  if (!node || node->m_layer != this || !hasNode(node)) return -1;
  if (!after || after->m_layer != this || !hasNode(after)) return -1;
  return moveNode(node, after->m_order + 1);
}
int GrabNodeArea::moveBefore(GrabNode *node, GrabNode *before) {
  if (!node || node->m_layer != this || !hasNode(node)) return -1;
  if (!before || before->m_layer != this || !hasNode(before)) return -1;
  return moveNode(node, before->m_order);
}

void GrabNodeArea::removeNode(GrabNode *node) {
  if (!node || node->m_layer != this || !hasNode(node)) return;
  m_nodes.erase(m_nodes.begin() + node->m_order);
  for (int i = node->m_order; i < m_nodes.size(); ++i) {
    m_nodes[i]->m_order = i;
  }
  removeChild(node);
}
        
bool GrabNodeArea::hasNode(GrabNode *node) {
  return node && std::find(m_nodes.begin(), m_nodes.end(), node) != m_nodes.end();
}
GrabNode *GrabNodeArea::getNodeAt(int order) {
  if (order < 0 || order >= m_nodes.size()) return nullptr;
  return m_nodes[order];
}
int GrabNodeArea::getOrderOf(GrabNode *node) {
  if (!node || node->m_layer != this || !hasNode(node)) return -1;
  for (int i = 0; i < m_nodes.size(); ++i) {
    if (m_nodes[i] == node) {
      return i;
    }
  }
  return -1;
}
int GrabNodeArea::getNodeCount() {
  return m_nodes.size();
}
std::vector<GrabNode *> GrabNodeArea::getNodes() {
  return m_nodes;
}

GrabNodeArea *GrabNodeArea::create() {
  auto ret = new GrabNodeArea();
  if (ret && ret->init()) {
    ret->autorelease();
    return ret;
  } else {
    delete ret;
    return nullptr;
  }
}


// GrabNode

bool GrabNode::init(GrabNodeArea *layer, CCSprite *dragSprite, int order) {
  if (!CCMenu::init()) return false;
  if (!layer) return false;
  m_order = layer->addNode(this, order);
  if (m_order == -1) return false;
  m_layer = layer;
  m_dragSprite = dragSprite;
  if (m_dragSprite) addChild(m_dragSprite);
  return true;
}
  
int GrabNode::getOrder() {
  return m_order;
}

GrabNodeArea *GrabNode::getLayer() {
  return m_layer;
}

bool GrabNode::ccTouchBegan(cocos2d::CCTouch *touch, cocos2d::CCEvent *evt) {
  log::info("Test");
  if (!m_dragSprite) return CCMenu::ccTouchBegan(touch, evt);
  // Check if the touch is within the m_dragSprite
  // auto pos = convertToNodeSpace(touch->getLocation());
  log::info("Touch at: {}", m_dragSprite->convertTouchToNodeSpace(touch));
  return true;
}

void GrabNode::ccTouchEnded(cocos2d::CCTouch *touch, cocos2d::CCEvent *evt) {
  if (!m_isDragging) {
    CCMenu::ccTouchEnded(touch, evt);
    return;
  }
}

void GrabNode::ccTouchMoved(cocos2d::CCTouch *touch, cocos2d::CCEvent *evt) {
  if (!m_isDragging) {
    CCMenu::ccTouchMoved(touch, evt);
    return;
  }
}

void GrabNode::registerWithTouchDispatcher() {
  auto td = CCDirector::sharedDirector()->getTouchDispatcher();
  td->addTargetedDelegate(this, -2, true);
}

GrabNode *GrabNode::create(GrabNodeArea *layer, CCSprite *dragSprite, int order) {
  auto ret = new GrabNode();
  if (ret && ret->init(layer, dragSprite, order)) {
    ret->autorelease();
    return ret;
  } else {
    delete ret;
    return nullptr;
  }
}