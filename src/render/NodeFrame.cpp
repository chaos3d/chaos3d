#include "2d/NodeFrame.h"
#include "2d/Transform.h"
#include "2d/Scene2DNode.h"
#include "2d/Sprite.h"
#include "script/lua.h"
#include "script/bind.h"

//using namespace chaos;

IMPLEMENT_CLASS(NodeFrame, LIB_DOMAIN)
IMPLEMENT_FUNC(intersect, &NodeFrame::intersect)
IMPLEMENT_FUNC(test, &NodeFrame::test)
IMPLEMENT_END

NodeFrame::NodeFrame(Scene2DNode* n, TypeLua const& lua) : mNode(n){
	ASSERT( n != 0 );
	lua_State *L(lua.getL());
	int top = lua_gettop(L);
	lua.get(L);

	ASSERT( lua_istable(L,-1) );
	LUA_GET4(L);
	mCenter[0] = (float)lua_tonumber(L,-4);
	mCenter[1] = (float)lua_tonumber(L,-3);
	mExtent[0] = (float)lua_tonumber(L,-2);
	mExtent[1] = (float)lua_tonumber(L,-1);

	lua_settop(L,top);
}

void NodeFrame::setFrame(float x, float y, float hW, float hH ){
	mCenter[0] = x;
	mCenter[1] = y;
	mExtent[0] = hW;
	mExtent[1] = hH;
}

void NodeFrame::setFrameFromeSprite(){
	if( mNode->getSprite() == 0 )
		return;

	Rectanglef const& rt(mNode->getSprite()->getFrame());
	setFrame( rt.Origin[0]+rt.Extent[0]/2.f, rt.Origin[1]+rt.Extent[1]/2.f,
	abs(rt.Extent[0]/2.f), abs(rt.Extent[1]/2.f));
}

bool NodeFrame::test(Vector3f const& p, Vector3f const& d) const{
	// trivial test
	if( mNode == 0 || mNode->getTransform() == 0 ||
		abs( mExtent[0] + mExtent[1] ) <= FLT_EPSILON)
		return false;

	Vector3f q;
	intersect(p,d,q);
	return (abs(q[0]-mCenter[0]) <= mExtent[0] && abs(q[1]-mCenter[1]) <= mExtent[1]);
}

void NodeFrame::intersect( Vector3f const& p, Vector3f const& d, Vector3f& q) const{
	Matrix4f const& global( mNode->getTransform()->getMatrix() );
    
	/*
     First, transform the ray into the client space, 
     where the boundary is in plain of axis x and y.
     Then, calculate the projection of ray in that plain (z=0).
     Last, test if it is within boundary.
     */
	Vector3f scale(
                   //mGlobal.GetColumn(0).Length(),
                   sqrt(global(0,0)*global(0,0) + global(1,0)*global(1,0) + global(2,0)*global(2,0)),
                   sqrt(global(0,1)*global(0,1) + global(1,1)*global(1,1) + global(2,1)*global(2,1)),
                   sqrt(global(0,2)*global(0,2) + global(1,2)*global(1,2) + global(2,2)*global(2,2))
                   );
    
	Matrix3f rotInv(
                    global(0,0)/scale[0], global(1,0), global(2,0),
                    global(0,1), global(1,1)/scale[1], global(2,1),
                    global(0,2), global(1,2), global(2,2)/scale[2]);
    
	Vector3f transform( global(0,3), global(1,3), global(2,3) );
	
	// only rotate d, because d is unit vector
	Vector3f sd = rotInv*d;
	sd[0] /= scale[0], sd[1] /= scale[1], sd[2] /= scale[2];	
	
	// inverse of affine transform * p
	Vector3f sp(rotInv * (p - transform));//(Vector3f(p[0]/scale[0],p[1]/scale[1],p[2]/scale[2])-transform));
	sp[0] /= scale[0], sp[1] /= scale[1], sp[2] /= scale[2];
	
	// R(t) = p + t*d = (x,y,0)
	float t = -sp[2]/sd[2];
	q[0] = sp[0] + t*sd[0], q[1] = sp[1] + t*sd[1];//,	q[2] = p[2] + t*d[2];
}

int NodeFrame::testIntersect( Vector3f const& p, Vector3f const& d, Vector3f& q, Vector2f const& extra) const{
	// trivial test
	if( mNode == 0 || mNode->getTransform() == 0 ||
		abs( mExtent[0] + mExtent[1] ) <= FLT_EPSILON)
		return 0;

	Matrix4f const& global( mNode->getTransform()->getMatrix() );

	/*
	First, transform the ray into the client space, 
	where the boundary is in plain of axis x and y.
	Then, calculate the projection of ray in that plain (z=0).
	Last, test if it is within boundary.
	*/
	Vector3f scale(
		//mGlobal.GetColumn(0).Length(),
		sqrt(global(0,0)*global(0,0) + global(1,0)*global(1,0) + global(2,0)*global(2,0)),
		sqrt(global(0,1)*global(0,1) + global(1,1)*global(1,1) + global(2,1)*global(2,1)),
		sqrt(global(0,2)*global(0,2) + global(1,2)*global(1,2) + global(2,2)*global(2,2))
		);

	Matrix3f rotInv(
		global(0,0)/scale[0], global(1,0), global(2,0),
		global(0,1), global(1,1)/scale[1], global(2,1),
		global(0,2), global(1,2), global(2,2)/scale[2]);

	Vector3f transform( global(0,3), global(1,3), global(2,3) );

	// only rotate d, because d is unit vector
	Vector3f sd = rotInv*d;
	sd[0] /= scale[0], sd[1] /= scale[1], sd[2] /= scale[2];	
	
	// inverse of affine transform * p
	Vector3f sp(rotInv * (p-transform));// (Vector3f(p[0]/scale[0],p[1]/scale[1],p[2]/scale[2])-transform));
	sp[0] /= scale[0], sp[1] /= scale[1], sp[2] /= scale[2];

	// R(t) = p + t*d = (x,y,0)
	float t = -sp[2]/sd[2];
	q[0] = sp[0] + t*sd[0], q[1] = sp[1] + t*sd[1];

	//LOG("\n%f,%f;",x,y);

	if( abs(q[0]-mCenter[0]) <= mExtent[0] + extra[0] && abs(q[1]-mCenter[1]) <= mExtent[1] + extra[1] ){
		int ret(1);
		/*if( mMask.data != 0 ){
			unsigned int x = (mMask.width/2 + (int)q[0])/mMask.unit, y = (mMask.height/2 + (int)q[2])/mMask.unit;
			unsigned int xw = (x*mMask.capacity) >> 5; // over 32
			unsigned int mask = *(mMask.data + xw + ((y*mMask.width*mMask.capacity)>>5));
			ret = (mask >> ((x-xw)*mMask.capacity));
		}*/

		q[2] = p[2] + t*d[2];
		return ret;
	}else
		return 0;

}

Scene2DNode* NodeFrame::pick( Vector3f const& p, Vector3f const& d, Vector3f& q, int& ret){
	// test if intersect against itself
	ret = testIntersect( p, d, q);
	if( ret == 0 )
		return 0;

	Scene2DNode* pick = mNode;
	// pick one of children
	
	Vector3f cq;
	int cret(0);
	for( Scene2DNode *child(mNode->lastChild()); child != 0; child = child->preSibling() ){
		if( child->getFrame() == 0 )
			continue;

		Scene2DNode* node = child->getFrame()->pick( p, d, cq, cret);
		if( node != 0 ){
			pick = node;
			break;
		}
	}

	if( pick != mNode )
		ret = cret, q = cq;

	return pick;
}

