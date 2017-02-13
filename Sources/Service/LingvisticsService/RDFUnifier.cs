using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;

#if WITH_OM_TM
using NodeName = Lingvistics.NodeName;
#else
using Linguistics.Core;

using NodeName = Linguistics.Core.UnitTextType;
#endif

namespace Lingvistics
{
    /// <summary>
    /// Объединитель RDF-ов
    /// </summary>
    internal static class RDFUnifier
    {
        /// <summary>
        /// Объединение коллекции RDF-ов
        /// </summary>
        /// <param name="collection">коллекция RDF-ов и длин исходных текстов</param>
        /// <returns>объединенный RDF</returns>
        public static string Union( IEnumerable< Tuple< string, int > > collection, out bool isEmpty )
        {
            var result = default(string);
            if ( collection.Any() )
            {
                int partShift = 0;
                int entityIDShift = 0;
                int subSentenceIdShift = 0;
                XElement xe = null;
                foreach ( var part in collection )
                {
                    if ( part.Item1 != null )
                    {
                        var another_xe = CorrectRDF( part.Item1, partShift, ref entityIDShift, ref subSentenceIdShift );
                        if ( xe == null )
                        {
                            xe = another_xe;
                        }
                        else
                        {
                            xe.Add( another_xe.Elements() );
                        }
                    }
                    partShift += part.Item2;
                }
                if ( xe != null )
                {
                    result = xe.ToString();
                }
                isEmpty = !xe.HasElements;
            }
            else
            {
                isEmpty = true;
            }
            return (result);
        }

        /// <summary>
        /// Добавление части к целому RDF-у
        /// </summary>
        /// <param name="part">часть</param>
        /// <param name="partShift">сдвиг части</param>
        /// <param name="entityIdShift">сдвиг ID сущностей</param>
        /// <param name="subSentenceIdShift">сдвиг ID подпредложений</param>
        private static XElement CorrectRDF( string part, int partShift, ref int entityIdShift, ref int subSentenceIdShift )
        {
            var xml = XElement.Parse( part );
            int entityCount = 0;
            int subSentenceCount = 0;
            foreach ( XElement sentence in xml.Elements() )
            {
                UpdateAttributeValue( sentence, BlockAttribute.POS, partShift );
                foreach ( XElement element in sentence.Descendants() )
                {
                    if ( element.Name == NodeName.SUB_SENT.ToString() )
                    {
                        if ( UpdateAttributeValue( element, BlockAttribute.ID, subSentenceIdShift ) )
                            ++subSentenceCount;
                        UpdateAttributeValue( element, BlockAttribute.SUBJ, entityIdShift );
                    }
                    else if ( (element.Name == NodeName.ENTITY.ToString()) ||
                              (element.Name == NodeName.CET   .ToString()) ||
                              (element.Name == NodeName.VERB  .ToString()) 
                            )
                    {
                        if ( UpdateAttributeValue( element, BlockAttribute.ID, entityIdShift ) )
                            ++entityCount;
                        UpdateAttributeValue( element, BlockAttribute.LINK, entityIdShift );
                    }
                }
            }
            entityIdShift += entityCount;
            subSentenceIdShift += subSentenceCount;
            return xml;
        }

        /// <summary>
        /// Обновление значения атрибута
        /// </summary>
        /// <param name="element">элемент</param>
        /// <param name="attributeName">имя атрибута</param>
        /// <param name="shift">сдвиг</param>
        /// <returns>флаг было ли обновлено значение атрибута</returns>
        public static bool UpdateAttributeValue( XElement element, BlockAttribute attributeName, int shift )
        {
            var attribute = element.Attribute( attributeName.ToString() );
            if ( attribute != null )
            {
                element.SetAttributeValue( attributeName.ToString(), uint.Parse( attribute.Value ) + shift );
                return true;
            }
            return false;
        }
    }
}
