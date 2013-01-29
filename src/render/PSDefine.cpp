#include "2d/PSDefine.h"
#include "script/bind.h"
#include "2d/PSSprite.h"
#include "2d/PSOperator.h"

//using namespace chaos;

/*
struct PointSpriteCollectionWrapper{
	typedef PointSpriteCollection::PSRenderer<Particle> Renderer;
	typedef PointSpriteCollection	Renderee;
};
*/

IMPLEMENT_CLASS(PS_plcs, LIB_DOMAIN)
IMPLEMENT_FACTORY( (LuaRefFactory<PS_plcs>) )
IMPLEMENT_FUNC(create, (&PS_plcs::createState<ParticleType, PSSprite>) )
IMPLEMENT_PROP(life, 0, (&PS_plcs::operator_<1>) )
IMPLEMENT_PROP(color, 0, (&PS_plcs::operator_<2>) )
IMPLEMENT_PROP(size, 0, (&PS_plcs::operator_<3>) )
IMPLEMENT_PROP(force, 0, (&PS_plcs::operator_<4>) )
IMPLEMENT_END;

IMPLEMENT_CLASS(PS_plcsv, LIB_DOMAIN)
//IMPLEMENT_SPEC( PS_plcsv, PS_plcsv, ReferencedCount, LIB_DOMAIN)
IMPLEMENT_FACTORY( (LuaRefFactory<PS_plcsv>) )
IMPLEMENT_FUNC( create, (&PS_plcsv::createState<ParticleType, PSSprite>) )
IMPLEMENT_PROP( life, 0, (&PS_plcsv::operator_<1>) )
IMPLEMENT_PROP( color, 0, (&PS_plcsv::operator_<2>) )
IMPLEMENT_PROP( size, 0, (&PS_plcsv::operator_<3>) )
IMPLEMENT_PROP( force, 0, (&PS_plcsv::operator_<4>) )
IMPLEMENT_PROP( velocity, 0, (&PS_plcsv::operator_<5>) )
IMPLEMENT_END;

IMPLEMENT_CLASS(PS_plcsav, LIB_DOMAIN)
//IMPLEMENT_SPEC( PS_plcsav, PS_plcsav, ReferencedCount, LIB_DOMAIN)
IMPLEMENT_FACTORY( (LuaRefFactory<PS_plcsav>) )
IMPLEMENT_FUNC( create, (&PS_plcsav::createState<ParticleType, PSSprite>) )
IMPLEMENT_PROP( life, 0, (&PS_plcsav::operator_<1>) )
IMPLEMENT_PROP( color, 0, (&PS_plcsav::operator_<2>) )
IMPLEMENT_PROP( size, 0, (&PS_plcsav::operator_<3>) )
IMPLEMENT_PROP( force, 0, (&PS_plcsav::operator_<4>) )
IMPLEMENT_PROP( accelerator, 0, (&PS_plcsav::operator_<5>) )
IMPLEMENT_PROP( velocity, 0, (&PS_plcsav::operator_<5>) )
IMPLEMENT_END;

////////////
////////////

IMPLEMENT_CLASS(PS_tfcsf, LIB_DOMAIN)
//IMPLEMENT_SPEC( PS_tfcsf, PS_tfcsf, ReferencedCount, LIB_DOMAIN)
IMPLEMENT_FACTORY( (LuaRefFactory<PS_tfcsf>) )
IMPLEMENT_FUNC( create, (&PS_tfcsf::createState<ParticleType, PSSprite>) )
IMPLEMENT_PROP( life, 0, (&PS_tfcsf::operator_<1>) )
IMPLEMENT_PROP( color, 0, (&PS_tfcsf::operator_<2>) )
IMPLEMENT_PROP( size, 0, (&PS_tfcsf::operator_<3>) )
IMPLEMENT_PROP( force, 0, (&PS_tfcsf::operator_<4>) )
IMPLEMENT_END;

IMPLEMENT_CLASS(PS_tfcsfv, LIB_DOMAIN)
//IMPLEMENT_SPEC( PS_tfcsfv, PS_tfcsfv, ReferencedCount, LIB_DOMAIN)
IMPLEMENT_FACTORY( (LuaRefFactory<PS_tfcsfv>) )
IMPLEMENT_FUNC( create, (&PS_tfcsfv::createState<ParticleType, PSSprite>) )
IMPLEMENT_PROP( life, 0, (&PS_tfcsfv::operator_<1>) )
IMPLEMENT_PROP( color, 0, (&PS_tfcsfv::operator_<2>) )
IMPLEMENT_PROP( size, 0, (&PS_tfcsfv::operator_<3>) )
IMPLEMENT_PROP( force, 0, (&PS_tfcsfv::operator_<4>) )
IMPLEMENT_PROP( velocity, 0, (&PS_tfcsfv::operator_<5>) )
IMPLEMENT_END;

IMPLEMENT_CLASS(PS_tfcsfav, LIB_DOMAIN)
//IMPLEMENT_SPEC( PS_tfcsfav, PS_tfcsfav, ReferencedCount, LIB_DOMAIN)
IMPLEMENT_FACTORY( (LuaRefFactory<PS_tfcsfav>) )
IMPLEMENT_FUNC( create, (&PS_tfcsfav::createState<ParticleType, PSSprite>) )
IMPLEMENT_PROP( life, 0, (&PS_tfcsfav::operator_<1>) )
IMPLEMENT_PROP( color, 0, (&PS_tfcsfav::operator_<2>) )
IMPLEMENT_PROP( size, 0, (&PS_tfcsfav::operator_<3>) )
IMPLEMENT_PROP( force, 0, (&PS_tfcsfav::operator_<4>) )
IMPLEMENT_PROP( accelerator, 0, (&PS_tfcsfav::operator_<5>) )
IMPLEMENT_PROP( velocity, 0, (&PS_tfcsfav::operator_<5>) )
IMPLEMENT_END;

