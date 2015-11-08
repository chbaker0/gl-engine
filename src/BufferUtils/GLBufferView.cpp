/*
 * GLBufferView.cpp
 *
 *  Created on: Apr 5, 2015
 *      Author: Collin
 */

#include "BufferUtils/GLBufferView.h"

template class GLBufferViewImpl<GLBuffer*>;
template class GLBufferViewImpl<std::shared_ptr<GLBuffer>>;

template class GLMappableBufferViewImpl<GLMappableBuffer*>;
template class GLMappableBufferViewImpl<std::shared_ptr<GLMappableBuffer>>;
