/*
 * GLRenderTargetView.cpp
 *
 *  Created on: May 24, 2015
 *      Author: Collin
 */

#include "GLRenderTargetView.h"

#include <iostream>
#include <GL/glew.h>

void GLRenderTargetView::drawTo()
{
	target->drawTo();
	glViewport(x, y, width, height);
	glScissor(x, y, width, height);
}
