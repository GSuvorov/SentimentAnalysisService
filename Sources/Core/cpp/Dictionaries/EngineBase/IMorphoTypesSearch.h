#pragma once

#include "../../ASSInterface/TDictionaryManager.h"
#include "../../ASCInterface/IDBMS.h"
#include "../../ASCInterface/defines.h"
#include "../../ASSInterface/ICommon.h"
#include "../../ASSInterface/TDBMS_Names.h"

namespace SS   {
	namespace Dictionary   {
		namespace Core   {

			class IMorphoTypesSearch   {

				public:

					/// Инициализация путь к файлам данных и указатель на интерфейс базы IDataBase (для вызова Load())
					virtual BOOL Init(
						/// Путь к файлам данных
						IN  CONST _TCHAR * pszPathToDataFiles,
						/// Указатель на интерфейс базы IDataBase (для вызова Load())
						IN  SS::Interface::Core::DBMS::IDataBase * pIDataBase = NULL) = 0;

					/// Загружает/перезагружает словарь
					virtual BOOL Load(VOID) = 0;

					/// Освобождает память
					virtual BOOL Clear(VOID) = 0;

					/// добавляет часть информации для последующего поиска
					/** 
						\param oIDType - номер типа морфообразования
						\param oIDForm - номер окончания
						\param oIDEnding - индекс окончания
						\param oMorphoInfo - закодированная основная морфологическая информация
						\param oSpecialMorpho - закодированная дополнительная морфологическая информация
						\return BOOL - результат операции
					*/
					BOOL AddInfo(
						IN  SS::Dictionary::Types::TIDType        oIDType,
						IN  SS::Dictionary::Types::TIDForm        oIDForm, 	
						IN  SS::Dictionary::Types::TIDEnding      oIDEnding, 
						IN  SS::Dictionary::Types::TMorphoInfo    oMorphoInfo, 
						IN  SS::Dictionary::Types::TSpecialMorpho oSpecialMorpho);


					/// осуществляет поиск информации об окончании по индексу литерального представления
					/** 
						\param oIDType - номер типа морфообразования
						\param oIDEnding - индекс окончания
						\param p_l_MorphoType - указатель на список структур, содержащих полную информацию об окончании
						\return BOOL - результат операции
					*/
					virtual BOOL GetByForm(
						IN  SS::Dictionary::Types::TIDType oIDType, 
						IN  SS::Dictionary::Types::TIDEnding oIDEnding, 
						OUT std::list<SS::Dictionary::Types::SMorphoType> *p_l_MorphoType) = 0;

					/// осуществляет поиск информации об индексе литерального представления по индексу словоформы
					/** 
						\param oIDType	- тип морфообразования
						\param oIDForm	- индекс окончания
						\param pIDEnding - индекс окончания
						\return BOOL - результат операции
					*/
					virtual BOOL GetIDEndingByIDTypeAndIDForm(
						IN  SS::Dictionary::Types::TIDType oIDType, 
						IN  SS::Dictionary::Types::TIDForm oIDForm, 
						OUT SS::Dictionary::Types::TIDEnding * pIDEnding) = 0;

					/// осуществляет поиск морфологической информации по индексу окончания в типе морфообразования 
					/** 
						\param oIDType - тип морфообразования
						\param oIDForm - номер окончания
						\param pMorphoType - указатель на список структур, содержащих полную информацию об окончании
						\return BOOL - результат операции
					*/
					virtual BOOL GetMorphoTypeByIDTypeAndIDForm(
						IN   SS::Dictionary::Types::TIDType oIDType, 
						IN   SS::Dictionary::Types::TIDForm oIDForm, 
						OUT SS::Dictionary::Types::SMorphoType * pMorphoType) = 0;

					/// осуществляет поиск информации о всех окончаниях в типе морфообразования
					/** 
						\param oIDType - тип морфообразования
						\param p_l_MorphoType - указатель на список структур, содержащих полную информацию об окончании
						\return BOOL - результат операции
					*/
					virtual BOOL GetMorphoTypesListByIDType(
						IN   SS::Dictionary::Types::TIDType oIDType, 
						OUT std::list<SS::Dictionary::Types::SMorphoType> * p_l_MorphoType) = 0;

					/// осуществляет поиск морфологической информации по типу морфообразования
					/** 
						\param oIDType - тип морфообразования
						\param oIDForm - номер окончания
						\param oIDType - тип морфообразования
						\param pMorphoInfo - закодированная основная морфологическая информация
						\param pSpecialMorpho - закодированная дополнительная морфологическая информация
						\return BOOL - результат операции
					*/
					virtual BOOL GetMorphoInfoByIDType(
						IN   SS::Dictionary::Types::TIDType oIDType, 
						IN   SS::Dictionary::Types::TIDForm oIDForm, 
						OUT SS::Dictionary::Types::TMorphoInfo * pMorphoInfo, 
						OUT SS::Dictionary::Types::TSpecialMorpho * pSpecialMorpho) = 0;


               /// Находит по IDType соотвествующий MorphoInfo
               /// Шестеркин Дмитрий 12.04.05
               virtual BOOL GetMorphoInfoByIDType( 
                  IN   SS::Dictionary::Types::TIDType oIDType,
                  OUT SS::Dictionary::Types::TMorphoInfo * pMorphoInfo
               ) = 0;



				}; // class IMorphoTypesSearch

			} // namespace Core   {
		} // namespace Dictionary   {
} // namespace SS   {
