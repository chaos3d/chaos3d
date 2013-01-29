#include "2d/gles/GLRender2DManager.h"

#include "2d/gles/glwrap.h"

//using namespace chaos;

/* Function pointers */
PFNGLCURRENTPALETTEMATRIXOES			glCurrentPaletteMatrixOES;
PFNGLLOADPALETTEFROMMODELVIEWMATRIXOES	glLoadPaletteFromModelViewMatrixOES;
PFNGLMATRIXINDEXPOINTEROES				glMatrixIndexPointerOES;
PFNGLWEIGHTPOINTEROES					glWeightPointerOES;

/* Function pointers */
PFNGLCLIPPLANEFIMG	glClipPlanefIMG;
PFNGLCLIPPLANEXIMG	glClipPlanexIMG;


/* Function pointers */
PFNGLVERTEXATTRIBPOINTERARB			glVertexAttribPointerARB;
PFNGLENABLEVERTEXATTRIBARRAYARB		glEnableVertexAttribArrayARB;
PFNGLDISABLEVERTEXATTRIBARRAYARB	glDisableVertexAttribArrayARB;
PFNGLPROGRAMSTRINGARB				glProgramStringARB;
PFNGLBINDPROGRAMARB					glBindProgramARB;
PFNGLDELETEPROGRAMSARB				glDeleteProgramsARB;
PFNGLGENPROGRAMSARB					glGenProgramsARB;
PFNGLISPROGRAMARB					glIsProgramARB;
PFNGLPROGRAMENVPARAMETER4FARB		glProgramEnvParameter4fARB;
PFNGLPROGRAMENVPARAMETER4FVARB		glProgramEnvParameter4fvARB;
PFNGLPROGRAMLOCALPARAMETER4FARB		glProgramLocalParameter4fARB;
PFNGLPROGRAMLOCALPARAMETER4FVARB	glProgramLocalParameter4fvARB;
PFNGLVERTEXATTRIB4FVARB				glVertexAttrib4fvARB;
PFNGLVERTEXATTRIB4XVIMG				glVertexAttrib4xvIMG;
PFNGLPROGRAMLOCALPARAMETER4XIMG		glProgramLocalParameter4xIMG;
PFNGLPROGRAMLOCALPARAMETER4XVIMG	glProgramLocalParameter4xvIMG;
PFNGLPROGRAMENVPARAMETER4XIMG		glProgramEnvParameter4xIMG;
PFNGLPROGRAMENVPARAMETER4XVIMG		glProgramEnvParameter4xvIMG;
PFNGLDRAWTEXIOES					glDrawTexiOES;
PFNGLDRAWTEXIVOES					glDrawTexivOES;
PFNGLDRAWTEXSOES					glDrawTexsOES;
PFNGLDRAWTEXSVOES					glDrawTexsvOES;
PFNGLDRAWTEXXOES					glDrawTexxOES;
PFNGLDRAWTEXXVOES					glDrawTexxvOES;
PFNGLDRAWTEXFOES					glDrawTexfOES;
PFNGLDRAWTEXFVOES					glDrawTexfvOES;
/* IMG_texture_stream */
PFNGLGETTEXSTREAMDEVICEATTRIBIVIMG	glGetTexStreamDeviceAttribivIMG;
PFNGLTEXBINDSTREAMIMG				glTexBindStreamIMG;
PFNGLGETTEXSTREAMDEVICENAMEIMG		glGetTexStreamDeviceNameIMG;
/* GL_EXT_multi_draw_arrays */
PFNGLMULTIDRAWELEMENTS				glMultiDrawElementsEXT;

/* GL_EXT_multi_draw_arrays */
PFNGLMAPBUFFEROES                   glMapBufferOES;
PFNGLUNMAPBUFFEROES                 glUnmapBufferOES;
PFNGLGETBUFFERPOINTERVOES           glGetBufferPointervOES;


/* GL_OES_Framebuffer_object*/

PFNGLISRENDERBUFFEROES glIsRenderbufferOES;
PFNGLBINDRENDERBUFFEROES glBindRenderbufferOES;
PFNGLDELETERENDERBUFFERSOES glDeleteRenderbuffersOES;
PFNGLGENRENDERBUFFERSOES glGenRenderbuffersOES;
PFNGLRENDERBUFFERSTORAGEOES glRenderbufferStorageOES;
PFNGLGETRENDERBUFFERPARAMETERIVOES glGetRenderbufferParameterivOES;
PFNGLISFRAMEBUFFEROES glIsFramebufferOES;
PFNGLBINDFRAMEBUFFEROES glBindFramebufferOES;
PFNGLDELETEFRAMEBUFFERSOES glDeleteFramebuffersOES;
PFNGLGENFRAMEBUFFERSOES glGenFramebuffersOES;
PFNGLCHECKFRAMEBUFFERSTATUSOES glCheckFramebufferStatusOES;
PFNGLFRAMEBUFFERTEXTURE2DOES glFramebufferTexture2DOES;
PFNGLFRAMEBUFFERRENDERBUFFEROES glFramebufferRenderbufferOES;
PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVOES glGetFramebufferAttachmentParameterivOES;
PFNGLGENERATEMIPMAPOES glGenerateMipmapOES;

    /* GL_OES_point_size_array */
//    PFNGLPOINTSIZEPOINTEROES glPointSizePointerOES;

/* GL_OES_blend_subtract */
PFNGLBLENDEQUATIONOESPROC glBlendEquationOES;

void GLRender2DManager::loadExtensions(){
#ifndef __APPLE__
	const GLubyte *pszGLExtensions;

	/* Retrieve GL extension string */
    pszGLExtensions = glGetString(GL_EXTENSIONS);

	/* GL_OES_matrix_palette */
	if (strstr((char *)pszGLExtensions, "GL_OES_matrix_palette"))
	{
		glCurrentPaletteMatrixOES			= (PFNGLCURRENTPALETTEMATRIXOES)eglGetProcAddress("glCurrentPaletteMatrixOES");
		glLoadPaletteFromModelViewMatrixOES	= (PFNGLLOADPALETTEFROMMODELVIEWMATRIXOES)eglGetProcAddress("glLoadPaletteFromModelViewMatrixOES");
		glMatrixIndexPointerOES				= (PFNGLMATRIXINDEXPOINTEROES)eglGetProcAddress("glMatrixIndexPointerOES");
		glWeightPointerOES					= (PFNGLWEIGHTPOINTEROES)eglGetProcAddress("glWeightPointerOES");
	}
	/* GL_IMG_user_clip_plane */
	if (strstr((char *)pszGLExtensions, "GL_IMG_user_clip_plane"))
	{
		/* glClipPlanexIMG and glClipPlanefIMG */
		glClipPlanexIMG = (PFNGLCLIPPLANEXIMG)eglGetProcAddress("glClipPlanexIMG");
		glClipPlanefIMG = (PFNGLCLIPPLANEFIMG)eglGetProcAddress("glClipPlanefIMG");
	}

	/* GL_IMG_vertex_program */
	if (strstr((char *)pszGLExtensions, "GL_IMG_vertex_program"))
	{
		/* glVertexAttribPointerARB */
		glVertexAttribPointerARB = (PFNGLVERTEXATTRIBPOINTERARB)eglGetProcAddress("glVertexAttribPointerARB");

		/* glEnableVertexAttribArrayARB */
		glEnableVertexAttribArrayARB = (PFNGLENABLEVERTEXATTRIBARRAYARB)eglGetProcAddress("glEnableVertexAttribArrayARB");

		/* glDisableVertexAttribArrayARB */
		glDisableVertexAttribArrayARB = (PFNGLDISABLEVERTEXATTRIBARRAYARB)eglGetProcAddress("glDisableVertexAttribArrayARB");

		/* glProgramStringARB */
		glProgramStringARB = (PFNGLPROGRAMSTRINGARB)eglGetProcAddress("glProgramStringARB");

		/* glBindProgramARB */
		glBindProgramARB = (PFNGLBINDPROGRAMARB)eglGetProcAddress("glBindProgramARB");

		/* glDeleteProgramsARB */
		glDeleteProgramsARB = (PFNGLDELETEPROGRAMSARB)eglGetProcAddress("glDeleteProgramsARB");

		/* glIsProgramARB */
		glIsProgramARB = (PFNGLISPROGRAMARB)eglGetProcAddress("glIsProgramARB");

		/* glGenProgramsARB */
		glGenProgramsARB = (PFNGLGENPROGRAMSARB)eglGetProcAddress("glGenProgramsARB");

		/* glVertexAttrib4fvARB */
		glVertexAttrib4fvARB = (PFNGLVERTEXATTRIB4FVARB)eglGetProcAddress("glVertexAttrib4fvARB");

		/* glVertexAttrib4xvIMG */
		glVertexAttrib4xvIMG = (PFNGLVERTEXATTRIB4XVIMG)eglGetProcAddress("glVertexAttrib4xvIMG");

		/* glProgramLocalParameter4xIMG */
		glProgramLocalParameter4xIMG = (PFNGLPROGRAMLOCALPARAMETER4XIMG)eglGetProcAddress("glProgramLocalParameter4xIMG");

		/* glProgramLocalParameter4xvIMG */
		glProgramLocalParameter4xvIMG = (PFNGLPROGRAMLOCALPARAMETER4XVIMG)eglGetProcAddress("glProgramLocalParameter4xvIMG");

		/* glProgramEnvParameter4xIMG */
		glProgramEnvParameter4xIMG = (PFNGLPROGRAMENVPARAMETER4XIMG)eglGetProcAddress("glProgramEnvParameter4xIMG");

		/* glProgramEnvParameter4xvIMG */
		glProgramEnvParameter4xvIMG = (PFNGLPROGRAMENVPARAMETER4XVIMG)eglGetProcAddress("glProgramEnvParameter4xvIMG");

		/* glProgramEnvParameter4fARB */
		glProgramEnvParameter4fARB = (PFNGLPROGRAMENVPARAMETER4FARB)eglGetProcAddress("glProgramEnvParameter4fARB");

		/* glProgramEnvParameter4fvARB */
		glProgramEnvParameter4fvARB = (PFNGLPROGRAMENVPARAMETER4FVARB)eglGetProcAddress("glProgramEnvParameter4fvARB");

		/* glProgramLocalParameter4fARB */
		glProgramLocalParameter4fARB = (PFNGLPROGRAMLOCALPARAMETER4FARB)eglGetProcAddress("glProgramLocalParameter4fARB");

		/* glProgGL_OES_draw_textureramLocalParameter4fvARB */
		glProgramLocalParameter4fvARB = (PFNGLPROGRAMLOCALPARAMETER4FVARB)eglGetProcAddress("glProgramLocalParameter4fvARB");
	}

	/* GL_OES_draw_texture */
	if (strstr((char *)pszGLExtensions, "GL_OES_draw_texture"))
	{
		/* glDrawTexiOES */
		glDrawTexiOES = (PFNGLDRAWTEXIOES)eglGetProcAddress("glDrawTexiOES");
		/* glDrawTexivOES */
		glDrawTexivOES = (PFNGLDRAWTEXIVOES)eglGetProcAddress("glDrawTexivOES");
		/* glDrawTexsOES */
		glDrawTexsOES = (PFNGLDRAWTEXSOES)eglGetProcAddress("glDrawTexsOES");
		/* glDrawTexsvOES */
		glDrawTexsvOES = (PFNGLDRAWTEXSVOES)eglGetProcAddress("glDrawTexsvOES");
		/* glDrawTexxOES */
		glDrawTexxOES = (PFNGLDRAWTEXXOES)eglGetProcAddress("glDrawTexxOES");
		/* glDrawTexxvOES */
		glDrawTexxvOES = (PFNGLDRAWTEXXVOES)eglGetProcAddress("glDrawTexxvOES");
		/* glDrawTexfOES */
		glDrawTexfOES = (PFNGLDRAWTEXFOES)eglGetProcAddress("glDrawTexfOES");
		/* glDrawTexfvOES */
		glDrawTexfvOES = (PFNGLDRAWTEXFVOES)eglGetProcAddress("glDrawTexfvOES");
	}

	/* IMG_texture_stream */
	if (strstr((char *)pszGLExtensions, "GL_IMG_texture_stream"))
	{
		glGetTexStreamDeviceAttribivIMG = (PFNGLGETTEXSTREAMDEVICEATTRIBIVIMG)eglGetProcAddress("glGetTexStreamDeviceAttribivIMG");
		glTexBindStreamIMG = (PFNGLTEXBINDSTREAMIMG)eglGetProcAddress("glTexBindStreamIMG");
		glGetTexStreamDeviceNameIMG = (PFNGLGETTEXSTREAMDEVICENAMEIMG)eglGetProcAddress("glGetTexStreamDeviceNameIMG");
	}

	/* GL_EXT_multi_draw_arrays */
	if (strstr((char *)pszGLExtensions, "GL_EXT_multi_draw_arrays"))
	{
		glMultiDrawElementsEXT = (PFNGLMULTIDRAWELEMENTS)eglGetProcAddress("glMultiDrawElementsEXT");
	}

    /* GL_EXT_multi_draw_arrays */
	if (strstr((char *)pszGLExtensions, "GL_OES_mapbuffer"))
	{
        glMapBufferOES = (PFNGLMAPBUFFEROES)eglGetProcAddress("glMapBufferOES");
        glUnmapBufferOES = (PFNGLUNMAPBUFFEROES)eglGetProcAddress("glUnmapBufferOES");
        glGetBufferPointervOES = (PFNGLGETBUFFERPOINTERVOES)eglGetProcAddress("glGetBufferPointervOES");
	}

    /* GL_OES_Framebuffer_object*/
	if (strstr((char *)pszGLExtensions, "GL_OES_framebuffer_object"))
	{
        glIsRenderbufferOES = (PFNGLISRENDERBUFFEROES)eglGetProcAddress("glIsRenderbufferOES") ;
        glBindRenderbufferOES = (PFNGLBINDRENDERBUFFEROES)eglGetProcAddress("glBindRenderbufferOES");
        glDeleteRenderbuffersOES = (PFNGLDELETERENDERBUFFERSOES)eglGetProcAddress("glDeleteRenderbuffersOES");
        glGenRenderbuffersOES = (PFNGLGENRENDERBUFFERSOES)eglGetProcAddress("glGenRenderbuffersOES");
        glRenderbufferStorageOES = (PFNGLRENDERBUFFERSTORAGEOES)eglGetProcAddress("glRenderbufferStorageOES");
        glGetRenderbufferParameterivOES = (PFNGLGETRENDERBUFFERPARAMETERIVOES)eglGetProcAddress("glGetRenderbufferParameterivOES");
        glIsFramebufferOES = (PFNGLISFRAMEBUFFEROES)eglGetProcAddress("glIsFramebufferOES");
        glBindFramebufferOES = (PFNGLBINDFRAMEBUFFEROES)eglGetProcAddress("glBindFramebufferOES");
        glDeleteFramebuffersOES = (PFNGLDELETEFRAMEBUFFERSOES)eglGetProcAddress("glDeleteFramebuffersOES");
        glGenFramebuffersOES = (PFNGLGENFRAMEBUFFERSOES)eglGetProcAddress("glGenFramebuffersOES");
        glCheckFramebufferStatusOES = (PFNGLCHECKFRAMEBUFFERSTATUSOES)eglGetProcAddress("glCheckFramebufferStatusOES");
        glFramebufferTexture2DOES = (PFNGLFRAMEBUFFERTEXTURE2DOES)eglGetProcAddress("glFramebufferTexture2DOES");
        glFramebufferRenderbufferOES = (PFNGLFRAMEBUFFERRENDERBUFFEROES)eglGetProcAddress("glFramebufferRenderbufferOES");
        glGetFramebufferAttachmentParameterivOES = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVOES)eglGetProcAddress("glGetFramebufferAttachmentParameterivOES");
        glGenerateMipmapOES = (PFNGLGENERATEMIPMAPOES)eglGetProcAddress("glGetFramebufferAttachmentParameterivOES");
	}

	/* GL_OES_point_size_array */
/*	if (strstr((char *)pszGLExtensions, "GL_OES_point_size_array"))
	{
		glPointSizePointerOES = (PFNGLPOINTSIZEPOINTEROES)eglGetProcAddress("glPointSizePointerOES");
	}
*/
	/* GL_EXT_multi_draw_arrays */
	if (strstr((char *)pszGLExtensions, "GL_OES_blend_subtract"))
	{
		glBlendEquationOES = (PFNGLBLENDEQUATIONOESPROC)eglGetProcAddress("glBlendEquationOES");
	}
#endif
}