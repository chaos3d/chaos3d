class RenderLoop {
public:
	float recentFPS();
	void setSampleRate(float);
	void renderLoop();

protected:
	virtual void doRender();

private:
	std::vector<RenderPass> _pass;
};
