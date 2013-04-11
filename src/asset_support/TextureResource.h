#include "asset/Resource.h"
#include "re/Texture.h"

class TextureResource : public Resource {
public:
	virtual bool load();
	virtual bool unload();
	virtual bool offline();
	virtual bool reload();
	virtual bool is_loaded();

	virtual Texture* getTexture();
private:
	Texture* _texture;
};
