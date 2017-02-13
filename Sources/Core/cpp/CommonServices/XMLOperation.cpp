#include "stdafx.h"

namespace SS {
namespace CommonServices {
namespace UserTextStorage {


void TXMLOperation::TransformXML( const std::wstring &XMLIn, const std::wstring &XSLT, std::wstring &XMLOut )
{
   typedef CComPtr<IXMLDOMDocument> TDOMDocument;

   TDOMDocument DOMDocumentXML, DOMDocumentXSLT;
   VARIANT_BOOL isSuccessful;

   XMLOut.clear();

   if( DOMDocumentXML.CoCreateInstance(__uuidof(DOMDocument)) != S_OK  )
      APL_THROW( _T("Ошибка при создании DOMDocument") );

   APL_CHECK_S_OK( DOMDocumentXML->put_async( FALSE ) );
   APL_CHECK_S_OK( DOMDocumentXML->put_resolveExternals( FALSE ) );

   if( DOMDocumentXSLT.CoCreateInstance(__uuidof(DOMDocument)) != S_OK  )
      APL_THROW( _T("Ошибка при создании DOMDocument") );

   APL_CHECK_S_OK( DOMDocumentXSLT->put_async( FALSE ) );
   APL_CHECK_S_OK( DOMDocumentXSLT->put_resolveExternals( FALSE ) );

   CComBSTR BSTRIn(XMLIn.c_str());

   if( DOMDocumentXML->loadXML( BSTRIn, &isSuccessful ) != S_OK || !isSuccessful/*isSuccessful != TRUE */) // isSuccessful почему-то равен -1
      APL_THROW( _T("Ошибка при загрузке XML") );

   CComBSTR BSTRXSLT(XSLT.c_str());

   if( DOMDocumentXSLT->loadXML( BSTRXSLT, &isSuccessful ) != S_OK || !isSuccessful/*isSuccessful != TRUE */) // isSuccessful почему-то равен -1
      APL_THROW( _T("Ошибка при загрузке XSLT") );

   CComQIPtr<IXMLDOMNode> XSLTNode(DOMDocumentXSLT);
   CComBSTR BSTROut;

   if( DOMDocumentXML->transformNode(XSLTNode, &BSTROut) )
      APL_THROW( _T("Ошибка при трансформации XML") );

   XMLOut = BSTROut;
}


} //namespace UserTextStorage
} //namespace CommonServices
} //namespace SS
