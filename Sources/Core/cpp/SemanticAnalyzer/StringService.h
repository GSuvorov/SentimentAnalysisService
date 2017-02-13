#pragma once

namespace SS
{
	namespace Semantic
	{
		///сервис для строк, заканчивающихся 0-м символом
		class CStringService
		{
		public:
			CStringService(void);
			~CStringService(void);

			bool StrEqual(const TCHAR* sz1, const TCHAR* sz2);

			void LowerFirstChar(TCHAR* szWord);
			void UpperFirstChar(TCHAR* szWord);

			void LowerSz(TCHAR* szWord);
			void UpperSz(TCHAR* szWord);

		};
	}
}