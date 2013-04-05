#include "DrawingUtil.h"
#include "script/scripting.h"

#import <UIKit/UIKit.h>
#import "FontLabel/FontManager.h"
#import "FontLabel/FontLabelStringDrawing.h"

//using namespace chaos;

void DrawingUtil::drawText(Texture* tex, char const* str, TypeLua const& settings){
	char const* font = 0;
	float pointSize = 10.f;
	void* data = 0;
	
	ScriptState ss(settings.getL());
	lua_State *L = ss.getState();
	int top = lua_gettop(L);
	
	do{
		ss.push_(settings);
		if( !lua_istable(L, -1) ){
			LOG( "expected a table for the setting (DrawText)" );
			break;
		}
		
		ss.push_("rect");
		lua_rawget( L, top+1);
		if( !lua_istable(L, -1) ){
			LOG( "expected a table for rect (DrawText)" );
			break;
		}
		lua_rawgeti(L, -1, 1);
		lua_rawgeti(L, -2, 2);
		lua_rawgeti(L, -3, 3);
		lua_rawgeti(L, -4, 4);
		
		float x = lua_tonumber(L, -4), y = lua_tonumber(L, -3);
		float width = lua_tonumber(L, -2), height = lua_tonumber(L, -1);
		lua_pop(L, 5);

		ss.push_("font");
		lua_rawget( L, top+1);
		font = lua_tostring(L, -1);
		
		ss.push_("size");
		lua_rawget( L, top+1);
		pointSize = lua_tonumber(L, -1);
		
		CGContextRef			context;
		CGColorSpaceRef			colorSpace;
				
		colorSpace = CGColorSpaceCreateDeviceRGB();
		data = calloc(height, width*4);
		context = CGBitmapContextCreate(data, width, height, 8, width * 4, colorSpace, kCGImageAlphaPremultipliedLast | kCGBitmapByteOrder32Big);
		CGContextSetFillColorSpace( context, colorSpace);
		CGContextSetStrokeColorSpace( context, colorSpace);
		CGColorSpaceRelease(colorSpace);
		
		CGFloat color[] = {0.f, 0.f, 0.f, 1.f};
		ss.push_("color");
		lua_rawget( L, top+1);
		if( lua_istable(L, -1) ){
			lua_rawgeti(L, -1, 1);
			lua_rawgeti(L, -2, 2);
			lua_rawgeti(L, -3, 3);
			lua_rawgeti(L, -4, 4);
			
			color[0] = lua_tonumber(L, -4);
			color[1] = lua_tonumber(L, -3);
			color[2] = lua_tonumber(L, -2);
			color[3] = lua_tonumber(L, -1);
		}
		
		CGContextSetFillColor(context, color);
		
		CGContextTranslateCTM(context, 0.0, height);
		CGContextScaleCTM(context, 1.0, -1.0); //NOTE: NSString draws in UIKit referential i.e. renders upside-down compared to CGBitmapContext referential
		
		UIGraphicsPushContext(context);
		UIFont *uiFont = [UIFont fontWithName: [NSString stringWithUTF8String: font] size: pointSize];
		if (uiFont != nil)
			[[NSString stringWithUTF8String: str] drawInRect:(CGRect){0,0,width,height} withFont:uiFont lineBreakMode:UILineBreakModeWordWrap alignment:UITextAlignmentLeft];
		else{
			ZFont *zFont = [[FontManager sharedManager] zFontWithName: [NSString stringWithUTF8String: font] pointSize:pointSize];
			[[NSString stringWithUTF8String: str] drawInRect:(CGRect){0,0,width,height} withZFont:zFont lineBreakMode:UILineBreakModeWordWrap alignment:UITextAlignmentLeft];
		}
		UIGraphicsPopContext();
		
		tex->loadSubData(data, x, y, width, height, Texture::Format2D_RGBA8888);
		
		CGContextRelease(context);
	}while (false);
	
	lua_settop(L, top);
	if( data != 0 )
		free(data);
}