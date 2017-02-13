//FillSQL.h
#pragma once
#include "./Connect.h"
#include "../ASSInterface/TBaseNames.h"

namespace SS
{
namespace UnitTests
{
namespace SyntaxConstructions
{
	///Заполнение SQL баз из DBMS
	class CFillSQL : public SS::CConnectSQL
	{
	public:
		///Конструктор
		CFillSQL(): CConnectSQL((wchar_t*)SS::MANAGER::Constants::c_szSyntaxConstructionsRus){};
		///Деструктор
		~CFillSQL(){};
		///Заполнение SQL базы из DBMS
		void Fill();
	};

}
}
}