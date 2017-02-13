#pragma once

namespace SS {
namespace CommonServices {
namespace UserTextStorage {


struct CCOMBase
{
   CCOMBase()
   {
      //Мне очень не нравится идея вызывать CoInitialize в конструкторе
      //и CoUninitialize в деструкторе, но это приходится делать т.к.
      //"это работает уже полгода без проблем"
      ::CoInitialize(NULL);
   }
   
   ~CCOMBase()
   {
      ::CoUninitialize();
   }

};


class TXMLOperation: public IXMLOperation, public CCOMBase
{
public:
   void TransformXML( const std::wstring &XMLIn, const std::wstring &XSLT, std::wstring &XMLOut );

public:
   RELEASE_IMPL()

   BEGIN_QI_MAP()
      QI_MAP_INHERIT_ENTRY( IID_XMLOperation, IXMLOperation )
      QI_MAP_INHERIT_ENTRY( IID_Base, IBase )
   END_QI_MAP()

};
     

} //namespace UserTextStorage
} //namespace CommonServices
} //namespace SS
