#pragma once

#include <list>

using namespace std;

namespace SS
{
	namespace CoSeDi
	{
		namespace DataTypes
		{
			enum EWorkRegion
			{
				wrSemantic		= 1,
				wrCombinatory	= 2,
				wrSyntax			= 3
			} ;


			struct  S_bf_ui_WorkRegions{
				unsigned short
					m_wrSemantic		:1,
					m_wrCombinatory	:1,
					m_wrSyntax			:1,
					EmptyBits			:13;	//пустые биты

					S_bf_ui_WorkRegions() : m_wrSemantic(0), m_wrCombinatory(0), m_wrSyntax(0), EmptyBits(0)
					{
					}
					bool IsSyntax()
					{
						return (m_wrSyntax == 1 ) ? true : false;
					}
					bool IsSemantic()
					{
						return (m_wrSemantic == 1 ) ? true : false;
					}
					bool IsCombinatory()
					{
						return (m_wrCombinatory == 1 ) ? true : false;
					}
					void SetSyntax()
					{	
						m_wrSyntax = 1;
					}
					void SetSemantic()
					{	
						m_wrSemantic = 1;
					}
					void SetCombinatory()
					{					
						m_wrCombinatory = 1;
					}
					void UnSetSyntax()
					{	
						m_wrSyntax = 0;
					}
					void UnSetSemantic()
					{	
						m_wrSemantic = 0;
					}
					void UnSetCombinatory()
					{					
						m_wrCombinatory = 0;
					}
					
					S_bf_ui_WorkRegions& operator= (const unsigned short value)
					{
						memcpy(this, &value, 2);
						return *this;
					}

					/*unsigned short& operator= (const S_bf_ui_WorkRegions value)
					{
						memcpy(this, &value, 2);
						return *this;
					}*/
				};

			enum ESemantic
			{
			};

			struct SExistence
			{
				INT32	IDSource;
				INT32	IDExistence;
			};

			struct SCombinatoryAccess
			{
				INT32	ID;
				INT32	IDType;
				CHAR	IDForm;
				INT32	CombiType;
				INT32	IDSource;
				INT32	Level;
//				INT32	IDArea;
			};

			struct SSemanticsAccess
			{
				INT32	IDSource;
				INT32	IDSuffics;
				INT32	IDClass;
				CHAR  IDForm;
			};		
			struct SSemanticsClassesTreeAccess
			{
				INT32	IDClass;
				INT32	IDParent;
				string	Name;
			};
			/*struct SSyntaxAccess
			{
				INT32	IDSource;
				INT32	IDSuffics;
				INT32	SyntaxInfo;
			};*/

			struct SSyntaxAccess
			{
				INT32	IDSource;
				INT32	SyntaxInfo;
			};
			
			

			struct  SuiIDSource{
				int
					ushIDRoot		:24,	// индекс основы
					uchIDSuffics	:8;		// индекс суффикса

					SuiIDSource() : ushIDRoot(0), uchIDSuffics(0)//, EmptyBits(0)
					{
					}
					bool operator == (const SuiIDSource& value) const
					{
						return ((ushIDRoot== value.ushIDRoot)&&
							(uchIDSuffics	== value.uchIDSuffics))
							?true:
						false;
					}
					SuiIDSource& operator= (const SuiIDSource& value)
					{
						ushIDRoot		= value.ushIDRoot;
						uchIDSuffics	= value.uchIDSuffics;
						return *this;
					}
					
					SuiIDSource& operator= (const int& value)
					{
						memcpy(this, &value, 4);
						return *this;
					}
				};

			typedef SuiIDSource TIDSource;

			struct SFormInfo  {
				SuiIDSource m_stIDSource;
				unsigned char m_uchDictionaryNumber;
				short       m_shIDForm;
				SFormInfo() : m_shIDForm(0), m_uchDictionaryNumber(0)
				{}
				bool operator == (const SFormInfo & value) const	{
					return ((m_stIDSource	==	value.m_stIDSource)&&
						     (m_shIDForm		==	value.m_shIDForm))? true : false;
				}

				SFormInfo & operator= (const SFormInfo & value)	{
					m_stIDSource	= m_stIDSource;
					m_shIDForm		= value.m_shIDForm;
					return * this;
				}
			};

			typedef SFormInfo TFormInfo;
			
			typedef list<TFormInfo> T_list_FormInfo;

			typedef unsigned int	TSyntaxInfo;	

			/* Типы комбинаций */
			enum ECombiType   {
				ctNewLexicalUnit      =  0, //
				ctVerbWithParticle    = -1, //
				ctVerbWithPrep        = -2, //
				ctVerbWithPrepPart    = -3, //
				ctPhrasalVerbWithPrep = -4  //
			};

			struct SCombinationInfo
			{
				short				shIDType;
				ECombiType			eCombiType;
				// EAreas			eIDArea;
				T_list_FormInfo	m_T_list_FormInfo;

				SCombinationInfo() : shIDType(0), eCombiType(ctNewLexicalUnit)
				{}
				
				bool operator == (const SCombinationInfo& value) const
				{
					return (
						(shIDType				== value.shIDType)&&
						(eCombiType				== value.eCombiType)&&
						// (eIDArea				== value.eIDArea )&&
						(m_T_list_FormInfo	== value.m_T_list_FormInfo)
						)
						?true:
					false;
				}
			};

			typedef SCombinationInfo TCombinationInfo;

			typedef list<int> TlistOfCombination;

			struct SSourceInCombination
			{
				TIDSource				m_IDSource;
				TlistOfCombination	m_listOfCombination;
			};

		};
	};
};