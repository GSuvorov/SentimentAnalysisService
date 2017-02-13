#pragma once

#include <windows.h>

namespace SS
{
namespace Core
{
namespace CommonServices
{

	///конвертирует строки
	class CConvertMultiByteAndWideChar
	{
		wchar_t* m_pWideCharStr;

	public:
		CConvertMultiByteAndWideChar() : m_pWideCharStr(NULL)
		{

		};

		~CConvertMultiByteAndWideChar()
		{
			Release();
		};

		void Release()
		{		
			if(m_pWideCharStr)
			{
				delete m_pWideCharStr;
				m_pWideCharStr = NULL;
			};
		};
		/// конвертирует ANSI в WideChar
		wchar_t* MultiByteToWideChar(PCSTR pMultiByteStr)
		{
			if(!m_pWideCharStr)
			{
				int iSizeWideCharString = ::MultiByteToWideChar(CP_THREAD_ACP, 0, pMultiByteStr, -1, NULL, 0);

				if (!iSizeWideCharString)
					return NULL;

				m_pWideCharStr = new wchar_t[iSizeWideCharString+1];
				::MultiByteToWideChar(CP_THREAD_ACP, 0, pMultiByteStr, -1, m_pWideCharStr, iSizeWideCharString);
			};

			return m_pWideCharStr;
		}
	};
}
}
}