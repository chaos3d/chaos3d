class RenderTexture : public RenderTarget {
public:
	Texture *getTexture();

private:
	Texture *_texture;
};
