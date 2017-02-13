#pragma once

namespace SS
{
	namespace Core
	{
		namespace Features
		{
			namespace Base
			{
				/// базовый класс для всех классов, хранящих информацию о типе характеристики (имя, кол-во и имена значений, строковые описания)
				class CBaseType
				{
				public:
					CBaseType(void);
					virtual ~CBaseType(void);

					//virtual unsigned int GetValueCount() = 0;
					///получение имени типа
					virtual const char* GetTypeName()  const = 0;
					///получение строкового описания типа
					virtual const char* GetTypeDescription()  const = 0;
					//virtual const char* GetValueName(unsigned int n) = 0;
					//virtual const char* GetValueDescription(unsigned int n) = 0;

					///получение кол-ва значений (для сетов)
					virtual unsigned int GetValueCount() const {return 0;};
					//virtual const char* GetTypeName(){return "";};
					//virtual const char* GetTypeDescription(){return "";};
					///получение имени значения по номеру значения
					virtual const char* GetValueName(unsigned int n) const {return "";};
					///получение строкового описания значения по номеру
					virtual const char* GetValueDescription(unsigned int n) const {return "";};
				};
			}
		}
	}
}