#include "StdAfx.h"
#include ".\tbl_containers.h"

namespace SS
{
namespace Core
{
namespace NdxSE
{
namespace NdxProcessor
{
namespace Containers
{

//--------------------------------------------------------------------//

void CTrcID::ToString(wstring& wszValue)
{
	wchar_t buf[100];
	wsprintf(buf, L"TrcID: %u\n", GetTrcID());
	wszValue.assign(buf);

	wsprintf(buf, L"\tTable ID: %u\n", GetIDByType(etrcTable));
	wszValue.append(buf);
	wsprintf(buf, L"\tRow ID: %u\n", GetIDByType(etrcRow));
	wszValue.append(buf);
	wsprintf(buf, L"\tCell ID: %u\n", GetIDByType(etrcCell));
	wszValue.append(buf);
}

//--------------------------------------------------------------------//

}
}
}
}
}