/*
 * GLRenderTargetView.h
 *
 *  Created on: May 24, 2015
 *      Author: Collin
 */

#ifndef GL_RENDER_TARGET_VIEW_H_INCLUDED
#define GL_RENDER_TARGET_VIEW_H_INCLUDED

#include "GLRenderTarget.h"

class GLRenderTargetView : public GLRenderTarget
{
private:
	GLRenderTarget *target;
	unsigned int x, y, width, height;

public:
	GLRenderTargetView(GLRenderTarget *target_in,
	                   unsigned int x_in, unsigned int y_in,
	                   unsigned int width_in, unsigned int height_in):
	                	   target(target_in), x(x_in), y(y_in), width(width_in), height(height_in) {}

	void drawTo() override;
};

#endif /* GL_RENDER_TARGET_VIEW_H_INCLUDED */
