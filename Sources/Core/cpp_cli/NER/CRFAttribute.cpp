#include "CRFAttribute.h"

NER::CRFAttribute::CRFAttribute(System::String^ attributeName, int position)
{
    _attributeName = attributeName;
    _position = position;
}

// Получить название аттрибута
System::String^ NER::CRFAttribute::GetAttributeName()
{
    return _attributeName;
}

// Полчуить индекс позиции аттрибута
int NER::CRFAttribute::GetPosition()
{
    return _position;
}

