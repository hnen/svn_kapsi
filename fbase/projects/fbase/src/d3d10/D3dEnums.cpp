
#include "../const.h"
#include "D3dEnums.h"

const D3dFormat D3dFormat::RGBA_BYTE(DXGI_FORMAT_R8G8B8A8_UINT, sizeof(byte), 3);
const D3dFormat D3dFormat::FLOAT(DXGI_FORMAT_R32_FLOAT, sizeof(float), 1);
const D3dFormat D3dFormat::RGBA_FLOAT(DXGI_FORMAT_R32G32B32A32_FLOAT, sizeof(float), 4);
const D3dFormat D3dFormat::DEPTH_FLOAT(DXGI_FORMAT_D32_FLOAT, sizeof(float), 1);

