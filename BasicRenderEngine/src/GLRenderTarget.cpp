/*
 * GLRenderTarget.cpp
 *
 *  Created on: Mar 9, 2015
 *      Author: Collin
 */

#include "GLRenderTarget.h"

thread_local GLRenderTarget *GLRenderTarget::currentTarget = nullptr;
