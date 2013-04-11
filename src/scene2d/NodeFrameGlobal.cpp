#include "NodeFrameGlobal.h"
#include "Scene2DNode.h"

using namespace Wm4;
//using namespace chaos;

Scene2DNode* NodeFrameGlobal::pick( Vector3f const& p, Vector3f const& d, Vector3f& q, int& ret){
	Scene2DNode *pick(0);
	for( Scene2DNode *child(mNode->lastChild()); child != 0; child = child->preSibling() ){
		if( child->getFrame() == 0 )
			continue;

		Scene2DNode* node = child->getFrame()->pick( p, d, q, ret);
		if( node != 0 ){
			pick = node;
			break;
		}
	}
	return pick;
}

