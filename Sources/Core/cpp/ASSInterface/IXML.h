#pragma once

// {CE44A8A8-DCE0-417e-AD0E-FCF33B23BF60}
static const GUID IID_XMLOperation = 
{ 0xce44a8a8, 0xdce0, 0x417e, { 0xad, 0xe, 0xfc, 0xf3, 0x3b, 0x23, 0xbf, 0x60 } };


// {8FDD3CE3-A0F0-4420-AFD6-21D50443152A}
static const GUID CLSID_XMLOperation = 
{ 0x8fdd3ce3, 0xa0f0, 0x4420, { 0xaf, 0xd6, 0x21, 0xd5, 0x4, 0x43, 0x15, 0x2a } };

namespace SS
{
	namespace Interface
	{
		namespace Common
		{
		/*	class IXML
			{
			public:

				void Element(std::wstring wName, void* pClass ) = 0;
				void SubElement(std::wstring wName, void* pClass ) = 0;
			};


			class IXMLManager
			{
			public:
				void Serialize(IXMLStorage * pXMLStorage) = 0;
			};*/

			class IXMLStorage
			{
			public:
				virtual void GetXML(std::wstring * pwXML) = 0;
				virtual void SetXML(const std::wstring * pwXML) = 0;
			};


         /// Набор вспомогательных XML функций
         class IXMLOperation: public virtual SS::Interface::IBase
         {
         public:
            virtual ~IXMLOperation() {};

            /// XSLT преобразование строки при помощи  MSXML
            virtual void TransformXML( const std::wstring &XMLIn, const std::wstring &XSLT, std::wstring &XMLOut ) = 0;
         };

		};
	};
};
