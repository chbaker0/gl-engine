/*
 * GLRenderTarget.h
 *
 *  Created on: Mar 4, 2015
 *      Author: Collin
 */

#ifndef GL_RENDER_TARGET_H_INCLUDED
#define GL_RENDER_TARGET_H_INCLUDED

class GLRenderTarget
{
public:
	virtual ~GLRenderTarget() {}

	virtual void drawTo() = 0;
};

#endif /* GL_RENDER_TARGET_H_INCLUDED */
