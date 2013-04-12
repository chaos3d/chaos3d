#ifndef _LPLATFORM_H
#define _LPLATFORM_H

#if defined(__cplusplus)
extern "C" {
#endif
    
struct lua_State;

/* open the lib */
int luaopen_lplatform(struct lua_State*);
    
#if defined(__cplusplus)
}
#endif

#endif