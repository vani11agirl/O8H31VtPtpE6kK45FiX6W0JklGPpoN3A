// For testing my grab node system
#include <Geode/Geode.hpp>

#include "../ui/GrabNodeLayer.hpp"
#include <Geode/ui/Popup.hpp>
#include <Geode/modify/MenuLayer.hpp>

using namespace geode::prelude;

class $modify(ml, MenuLayer) {
  bool init() {
    if (!MenuLayer::init()) return false;

    auto btn = CCMenuItemSpriteExtra::create(
      CCSprite::createWithSpriteFrameName("GJ_likeBtn_001.png"),
      this,
      menu_selector(ml::onBtn));

    auto menu = getChildByID("bottom-menu");
    if (!menu) return true;
    menu->addChild(btn);

    return true;
  }

  void onBtn(CCObject *sender) {
    auto layer = geode::Popup<>::create("Test", "Test popup", "Test");
    auto gnl = GrabNodeArea::create();
    gnl->setContentSize({300, 150});
    gnl->setID("gnl"_spr);
    gnl->ignoreAnchorPointForPosition(false);
    layer->addChild(gnl);
    layer->show();
    auto dragNode = GrabNode::create(gnl, CCSprite::create("draggable.png"_spr));
    dragNode->setContentSize({300, 50});
  }
};
