class EngineLoop{
public:
	EventHandler& getHandler();
	virtual bool startUp();
	virtual bool tearDown();
	virtual void collect();
	virtual bool loop();
};
