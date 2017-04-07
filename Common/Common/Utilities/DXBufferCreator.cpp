#include "stdafx.h"
using namespace Utilities;

DXBufferCreator* const DXBufferCreator::pInstance = new DXBufferCreator;

DXBufferCreator* DXBufferCreator::Create()
{
	return pInstance;
}

DXBufferCreator::~DXBufferCreator()
{

}

