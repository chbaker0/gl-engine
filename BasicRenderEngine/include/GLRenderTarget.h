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
	virtual ~GLRenderTarget() = 0;

	virtual void drawTo() = 0;
};

inline GLRenderTarget::~GLRenderTarget() {}

#endif /* GL_RENDER_TARGET_H_INCLUDED */
