#pragma once

#include <vector>
#include <string>
#include "../ASCInterface/IInterface.h"

namespace SS
{
	namespace Interface
	{
		namespace Core
		{

			///сервисы для баз-данных
			class IServiceDatabase : public virtual IBase
			{
			public:

				///осуществляем резервное копирование
				virtual HRESULT RunBackUp(	std::vector< std::pair<std::string, std::string> >* vPathes,
											const char* szBaseName	)=0;

				///осуществляем усечение базы
				virtual HRESULT RunShrink()=0;

				///осуществляем дефрагментацию базы
				virtual HRESULT RunDefrag(char* szTableName, char* szIndexNumber)=0;

			};

			///файловая база-данных
			class IOpenFiles : public virtual IBase
			{
			public:

				///Открываем хранилище данных
				virtual HRESULT OpenStorage(const char* szBaseName)=0;
				///3aкрываем хранилище данных
				virtual void CloseStorage()=0;

				///Очищаем хранилище данных
				virtual HRESULT ClearStorage(const char* szBaseName)=0;
				///Устанавливаем путь к базе(куда будем скидывать информацию)
				virtual void SetDatabasePath(char* szPath)=0;

				IOpenFiles(){};
				virtual ~IOpenFiles(){};
			};
		}

	}
}
