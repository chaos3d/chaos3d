/* 
 * Abstract factory for creating render resources
 */
struct DriverCap{
	uint8_t max_texture;
};

class RenderDriver{
public:
	virtual RenderTarget* createTarget() = 0;
	virtual void getCap(DriverCap&) = 0;
};
