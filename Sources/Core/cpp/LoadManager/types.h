#pragma once

#include <map>
#include <list>
#include <algorithm>

namespace SS
{
namespace Core
{
namespace ResourceManagers
{
namespace Types
{

			/// оболочечный файл дл€ GUID (реализует операции сравнени€)
			/** данный класс реализует операции сравнени€ дл€ GUID, а также преобразовани€ в строковое выражение */
			class CGuidShell
			{

				/// GUID, вокруг которого обернут данный класс
				GUID m_guid;

				/// ѕреобразует GUID в строковое представление
				/** 
					\return std::wstring - строковое представление GUID
				*/
				std::wstring inToString();
			public:

				/// »нициализаци€ данных класса 
				/** 
					\param guid - GUID
				*/
				CGuidShell(GUID guid);

				/// ќпераци€ сравнени€ дл€ константного объекта
				/** 
					\param value  - класс, с которым происходит сравнение
					\return bool - если меньше, то true
				*/
				bool operator<(const CGuidShell& value) const;

				/// оператор приведени€ типа к GUID
				operator GUID();

				/// оператор приведени€ типа к GUID дл€ константного объекта
				operator GUID() const;

				/// преобразует GUID в строковое выражение 
				/** 
					\return std::wstring строковое представление GUID
				*/
				std::wstring ToString();

				/// преобразует GUID в строковое выражение (дл€ константного объекта)
				/** 
					\return std::wstring строковое представление GUID
				*/
				std::wstring ToString() const;
			};



			/*! \typedef TLInterfaces
			*  \brief    ќписывает интерфейс
			*/
			typedef int TInterface;

			/*! \typedef TMWorkInterfaces
			*  \brief    ќписывает карту соответствий между GUID интерфейса и списком ссылок на указатели на него
			*/
			typedef std::map<CGuidShell,TInterface> TMWorkInterfaces;
			/*! \typedef TPWorkInterfaces
			*  \brief    ќписывает пару - указатель на базовый интерфейс и карту соответсвий между GUID интерфейса и списком ссылок на указатели на него
			*/
			typedef std::pair<SS::Interface::IBase*, TMWorkInterfaces> TPWorkInterfaces;

			/*! \typedef TMWorkEntities
			*  \brief    ќписывает карту соответствий между GUID класса и парой TMWorkInterfaces (библиотека классов)
			*/
			typedef std::map<CGuidShell,TPWorkInterfaces> TMWorkEntities;
			/*! \typedef TMWorkLibraries;			
			*  \brief    ќписывает карту соответствий между идентификатором модул€ и библиотекой классов
			*/
			typedef std::map<TCoreLibrary, TMWorkEntities> TMWorkLibraries;			
}
}
}
}