#pragma once

#include ".\settings_manager.h"
#include "..\ASCInterface\ICommonServices.h"

namespace SS
{
namespace CommonServices
{

//----------------------------------------------------------------//

///редактор настроек, позволяет добавлять, удалять, изменять, получать настройки
class  CSettingsEditor :
			public virtual CSettingsManager,
			public SS::Interface::Core::CommonServices::ISettingsServerEdit
{
public:

	//------------ISettingsServerEdit 
	//ISettingsServerEdit implementation
	///возвращает список параметров. последний - null
	void GetParameterList (std::vector<std::wstring>* pOutSettingsNames, bool bShowPrivate);
	///добавить параметр
	bool			AddParameter		(const wchar_t*		wszName, 
											SS::Interface::Core::CommonServices::EParameterType	eType, 
											bool			bIsArray		= false, 
											bool			bIsPublic		= true,
											bool			bIsReadOnly		= false);
	///получить тип параметра по его имени
	SS::Interface::Core::CommonServices::EParameterType GetParameterType		
										(const wchar_t*		wszName, 
										bool*			pbIsArray = NULL);
	///является ли настрока публичной
	unsigned int GetPublicity				(const wchar_t* wszName);
	///является ли настройка нередактируемой
	unsigned int GetAccessibility			(const wchar_t* wszName);
	///удалить параметр
	void			DeleteParameter		(const wchar_t* wszName);
	///устанавливает имя существующего параметра
	void SetParameterName			(const wchar_t* wszName, const wchar_t* wszNewName);
	///устанавливает публичность существующего параметра
	void SetParameterPublicity		(const wchar_t* wszName, bool bIsPublic);
	///устанавливает тип доступа к существующему параметру
	void SetParameterAccessibility	(const wchar_t* wszName, bool bIsReadOnly);
	///устанавливает тип существующего параметра (если новый тип не совпадает со старым, все значения обнуляются)
	void SetParameterType			(const wchar_t* wszName, SS::Interface::Core::CommonServices::EParameterType eType);
	///добавить значение в массив значения настройки
	void AddParameterValue(const wchar_t* wszName, SS::Interface::Core::CommonServices::EParameterType eType, void* pValue);
	//-----------Eof-ISettingsServerEdit 

};

//----------------------------------------------------------------//

}
}