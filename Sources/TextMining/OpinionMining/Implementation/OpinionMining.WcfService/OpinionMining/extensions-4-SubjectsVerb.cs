using System;
using System.Linq;
using System.Xml.Linq;
using System.Collections.Generic;

using TextMining.Core;
using TextMining.Service;

namespace OpinionMining.SubjectsVerbExtensions
{
    internal static class Extensions4SubjectsVerb
    {
        private static bool IsEntityConditionMI( this XElement entity, string entityMI )
        {
            const string MI_U1 = "U";
            const string MI_U2 = ",U";

            var mi = entity.GetAttributeMI();
            if ( !mi.IsEmptyOrNull() )
            {
                if ( (mi.StartsWith( entityMI ) || mi.Contains( ',' + entityMI ) ) )
                    return (true);

                if ( (mi.StartsWith( MI_U1 ) || mi.Contains( MI_U2 ) ) )
                    return (true);
            }
            return (false);
        }
        private static bool IsVerbConditionMI( this XElement verb, string verbMI )
        {
            var mi = verb.GetAttributeMI();
            if ( !mi.IsEmptyOrNull() )
            {
                if ( (mi.EndsWith( verbMI ) || mi.Contains( verbMI + ',' ) ) )
                    return (true);
            }
            return (false);
        }
        private static bool IsVerbsConditionMI( this IList< VerbCollocationProcessingItem > verbCollocationItems, string verbMI )
        {
            var hasVerbs = false;
            foreach ( var item in verbCollocationItems )
            {
                if ( item.TypeOfXmlElement == TypeOfXmlElement.VERB )
                {
                    hasVerbs = true;
                    if ( !item.XElement.IsVerbConditionMI( verbMI ) )
                        return (false);
                }
            }
            return (hasVerbs);

            /*
            if ( verbCollocationItems.Any( _ => (_.TypeOfXmlElement == TypeOfXmlElement.VERB) && 
                                                !_.XElement.IsVerbConditionMI( verbMI ) ) )
            {
                return (false);
            }
            return (true);
            */
        }
        private static XElement FindVERBtypeVERB( this IEnumerable< XElement > elements, Func< XElement, bool > verbConditionFunction, string  verbMI )
        {
            foreach ( var e in elements )
            {
                if ( e.IsElementVERBtypeVERB() )
                {
                    foreach ( var v in e.DescendantsAndSelfVerb() )
                    {
                        if ( v.IsElementVERBtypeVERB() && 
                             v.IsVerbConditionMI( verbMI ) &&
                             verbConditionFunction( v ) )
                        {
                            return (v);
                        }
                    }
                }
            }
            return (null);
        }
        public  static XElement FindVERBtypeDEEPR( this IEnumerable< XElement > elements, Func< XElement, bool > verbConditionFunction, string  verbMI )
        {
            foreach ( var e in elements )
            {
                if ( e.IsElementVERBtypeDEEPR() )
                {
                    foreach ( var v in e.DescendantsAndSelfVerb() )
                    {
                        if ( v.IsElementVERBtypeDEEPR() && 
                             v.IsVerbConditionMI( verbMI ) &&
                             verbConditionFunction( v ) )
                        {
                            return (v);
                        }
                    }
                }
            }
            return (null);
        }

        private static bool IsHomogeneousObjectType( this XElement entity, TypeAttributeValue? type )
        {
            if ( type == TypeAttributeValue.Homogeneous )
            {
                foreach ( var d in entity.DescendantsEntity() )
                {
                    if ( d.GetAttributeTYPE().IsObjectType() )
                    {
                        return (true);
                    }
                }
            }
            return (false); 
        }
        private static bool IsHomogeneousObjectTypeOrgName( this XElement entity, TypeAttributeValue? type )
        {
            if ( type == TypeAttributeValue.Homogeneous )
            {
                foreach ( var d in entity.DescendantsEntity() )
                {
                    if ( d.GetAttributeTYPE().IsObjectTypeOrgName() )
                    {
                        return (true);
                    }
                }
            }
            return (false); 
        }
        private static bool IsObjectTypeOrgName( this TypeAttributeValue? type )
        {
            if ( type.HasValue )
            {
                switch ( type )
                {
                    case TypeAttributeValue.OrgName:
                    case TypeAttributeValue.OrgNoInfo:
                    case TypeAttributeValue.UnknownProper:
                    /*-not-need-//case TypeAttributeValue.OrgAbbr:*/
                        return (true);
                }
            }
            return (false); 
        }
        private static bool IsObjectTypeOrHomogeneousWithObjectType( this XElement entity )
        {
            var type = entity.GetAttributeTYPE();
            return (type.IsObjectType() || entity.IsHomogeneousObjectType( type ));
        }
        private static bool IsObjectTypeOrgNameOrHomogeneousWithObjectTypeOrgName( this XElement entity )
        {
            var type = entity.GetAttributeTYPE();
            return (type.IsObjectTypeOrgName() || entity.IsHomogeneousObjectTypeOrgName( type ));
        }

        public  static bool IsDirectSubjectEntity( this XElement entity, string entityMI )
        {
            if ( (entity.IsElementENTITY()) &&
                 (entity.IsObjectTypeOrHomogeneousWithObjectType()) && 
                 (entity.IsEntityConditionMI( entityMI ))
               )
            {
                return (true);
            }
            return (false);
        }        
        private static bool IsDirectSubjectEntityNested( this XElement entity, string entityMI, out XElement resultEntity )
        {
            resultEntity = null;
            if ( entity.IsElementENTITY() )
            {
                foreach ( var e in entity.DescendantsAndSelfEntity() )
                {
                    if ( e.IsDirectSubjectEntity( entityMI ) )
                    {
                        resultEntity = e;
                        return (true);
                    }
                }
            }
            return (false);
        }        
        private static bool IsDirectSubjectEntityOrgName( this XElement entity, string entityMI )
        {
            if ( (entity.IsElementENTITY()) &&
                 (entity.IsObjectTypeOrgNameOrHomogeneousWithObjectTypeOrgName()) &&
                 (entity.IsEntityConditionMI( entityMI ))
               )
            {
                return (true);
            }
            return (false);
        }        
        public  static SubjectAndAnaphoriaEntityTuple ResolveAnaphoria4SubjectEntity( this XElement entityOrCet, string entityMI )
        {
            if ( entityOrCet.IsElementENTITY() || entityOrCet.IsElementCET() )
            {
                var link = entityOrCet.GetAttributeLINK();
                if ( link.HasValue )
                {
                    var linkedEntity = entityOrCet.Document.FindEntityById( link.Value );
                    if ( (linkedEntity != null) && linkedEntity.IsDirectSubjectEntity( entityMI ) )
                    {
                        if ( entityOrCet.IsElementCET() )
                            entityOrCet.RenameCET2ENTITY();

                        return (new SubjectAndAnaphoriaEntityTuple( entityOrCet, linkedEntity ));
                    }
                }
            }
            return (null);

            #region [.commented. anaphoria | coreference4 inquiry.]
            /*var anaphoriaOrcoreference = 
                    from subsent in subsents
                    where subsent.ElementsEntity().All( _ => !_.IsTypeInquiry() )
                    let CETorENTITY = subsent.Elements()
                        .FirstOrDefault( _ => (_.IsElementENTITY() || _.IsElementCET()) 
                                                &&  _.GetAttributeLINK().HasValue )
                    where ( CETorENTITY != null )
                    let linkedENTITY = subsent.Document.FindEntityById( CETorENTITY.GetAttributeLINK().Value )
                    where (linkedENTITY != null && linkedENTITY.IsTypeInquiry() )
                    select CETorENTITY;

            foreach ( var CETorENTITY in anaphoriaOrcoreference )
            {
                if ( CETorENTITY.IsElementCET() )
                    CETorENTITY.RenameCET2ENTITY();
                CETorENTITY.MarkAsInquiry();

                inquiryAllocateStatus  = InquiryAllocateStatus.Allocated;
                inquiryFoundInSentence = true;
            }*/
            #endregion
        }
        private static SubjectAndAnaphoriaEntityTuple ResolveAnaphoria4SubjectEntityNested( this XElement entityOrCet, string entityMI )
        {
            var resultEntity = default(XElement);
            if ( entityOrCet.IsElementENTITY() || entityOrCet.IsElementCET() )
            {
                var link = entityOrCet.GetAttributeLINK();
                if ( link.HasValue )
                {
                    var linkedEntity = entityOrCet.Document.FindEntityById( link.Value );
                    if ( (linkedEntity != null) && linkedEntity.IsDirectSubjectEntityNested( entityMI, out resultEntity ) )
                    {
                        if ( entityOrCet.IsElementCET() )
                            entityOrCet.RenameCET2ENTITY();

                        return (new SubjectAndAnaphoriaEntityTuple( resultEntity /*entityOrCet*/, linkedEntity ));
                    }
                }
            }
            return (null);
        }
        private static SubjectAndAnaphoriaEntityTuple ResolveAnaphoria4SubjectEntityOrgName( this XElement entityOrCet, string entityMI )
        {
            if ( entityOrCet.IsElementENTITY() || entityOrCet.IsElementCET() )
            {
                var link = entityOrCet.GetAttributeLINK();
                if ( link.HasValue )
                {
                    var linkedEntity = entityOrCet.Document.FindEntityById( link.Value );
                    if ( (linkedEntity != null) && linkedEntity.IsDirectSubjectEntityOrgName( entityMI ) )
                    {
                        if ( entityOrCet.IsElementCET() )
                            entityOrCet.RenameCET2ENTITY();

                        return (new SubjectAndAnaphoriaEntityTuple( entityOrCet, linkedEntity ));
                    }
                }
            }
            return (null);
        }
        public static SubjectAndAnaphoriaEntityTuple GetSubjectAndAnaphoriaEntityTuple( this XElement entity, string entityMI )
        {
            if ( entity.IsDirectSubjectEntity( entityMI ) )
                return (new SubjectAndAnaphoriaEntityTuple( entity ));

            return (entity.ResolveAnaphoria4SubjectEntity( entityMI ));
        }

        //getting SubjectData by verb-subsent-@SUBJ-attribute
        private static XElement GetEntityBySubsentSUBJ( this XElement verb )
        {
            var subsent = verb.GetAncestorSubsent();
            if ( subsent.IsNotNull() )
            {
                var subj = subsent.AttributeSubjValue();
                if ( subj.HasValue )
                {
                    var sent = subsent.GetAncestorSent();
                    if ( sent.IsNotNull() )
                    {
                        var entity = sent.FindEntityByIdNotInDirectSpeech( subj.Value );
                        if ( entity.IsNotNull() )
                        {
                            return (entity);
                        }
                    }
                }
            }
            return (null);
        }
        private static SubjectData ToSubjectData( this SubjectAndAnaphoriaEntityTuple sewra, XElement verb )
        {
            if ( sewra.IsNotNull() )
            {
                var se = new SubjectEssence( sewra, verb );
                var leftExtremeElement  = verb.IsAfter( se.Entity )    ? se.Entity : verb;
                var rightExtremeElement = (leftExtremeElement == verb) ? se.Entity : verb;
                return (new SubjectData( se.RepeatOnce().ToList(), leftExtremeElement, rightExtremeElement ));
            }
            return (null);
        }
        private static SubjectData GetSubjectDataBySUBJ( this XElement verb, string entityMI )
        {
            var e = verb.GetEntityBySubsentSUBJ();
            if ( e.IsNotNull() )
            {
                var sewra = default(SubjectAndAnaphoriaEntityTuple);
                if ( e.IsDirectSubjectEntity( entityMI ) )
                    sewra = new SubjectAndAnaphoriaEntityTuple( e );
                else 
                    sewra = e.ResolveAnaphoria4SubjectEntity( entityMI );

                return (sewra.ToSubjectData( verb ));
            }
            return (null);
        }        
        private static SubjectData GetSubjectDataNestedWithBBySUBJ( this XElement verb, string entityMI, bool breakByQuote )
        {
            var e = verb.GetEntityBySubsentSUBJ();
            if ( e.IsNotNull() )
            {
                var sewra = default(SubjectAndAnaphoriaEntityTuple);
                var resultEntity = default(XElement);

                if ( e.IsDirectSubjectEntityNested( entityMI, out resultEntity ) )
                    sewra = new SubjectAndAnaphoriaEntityTuple( resultEntity );
                else
                    sewra = e.ResolveAnaphoria4SubjectEntityNested( entityMI );

                if ( sewra.IsNotNull() )
                {
                    var hasB = (sewra.Anaphoria ?? sewra.Entity)
                                .ElementsBeforeSelf()
                                .Reverse()
                                .TakeWhile( _ => !_.IsElementVERBtypeVERB() && 
                                                 !_.IsBreakingPunctuationSign( breakByQuote ) )
                                .Any( _ => _.IsElementCET() &&
                                           _.GetAttributeVALUEorContent().TrimWhiteSpaces().ToLower() == CET_B );
                    if ( hasB )
                    {
                        return (sewra.ToSubjectData( verb ));
                    }
                }
            }
            return (null);
        }
        private static SubjectData GetSubjectDataOrgNameWithBBySUBJ( this XElement verb, string entityMI, bool breakByQuote )
        {
            var e = verb.GetEntityBySubsentSUBJ();
            if ( e.IsNotNull() )
            {
                var sewra = default(SubjectAndAnaphoriaEntityTuple);

                if ( e.IsDirectSubjectEntityOrgName( entityMI ) )
                    sewra = new SubjectAndAnaphoriaEntityTuple( e );
                else
                    sewra = e.ResolveAnaphoria4SubjectEntityOrgName( entityMI );

                if ( sewra.IsNotNull() )
                {
                    var hasB = (sewra.Anaphoria ?? sewra.Entity)
                                .ElementsBeforeSelf()
                                .Reverse()
                                .TakeWhile( _ => !_.IsElementVERBtypeVERB() && 
                                                 !_.IsBreakingPunctuationSign( breakByQuote ) )
                                .Any( _ => _.IsElementCET() &&
                                           _.GetAttributeVALUEorContent().TrimWhiteSpaces().ToLower() == CET_B );
                    if ( hasB )
                    {
                        return (sewra.ToSubjectData( verb ));
                    }
                }
            }
            return (null);
        }

        //getting Subjects
        private const string CET_B = "в";
        private static readonly char[] BREAKING_SIGN_PUNCTUATION = new[] { /*',',*/ ';', ':', };
        public static bool IsBreakingPunctuationSign( this XElement element, bool breakByQuote )
        {
            //кавычки => Cet type-of-quote
            if ( breakByQuote && element.IsQuote() )
            {
                return (true);
            }

            //запятая, точка с запятой, двоеточие
            if ( element.IsElementU() )
            {
                return (element.Value.IndexOfAny( BREAKING_SIGN_PUNCTUATION ) != -1);
            }

            return (false);
        }
        private static List< SubjectAndAnaphoriaEntityTuple > GetSubjects( this IEnumerable< XElement > sequence
                                                                           , string   entityMI
                                                                           , XElement breakSearchElement
                                                                           , bool     breakByQuote
                                                                           , bool     breakAfterFirstFound = false )
        {
            var sewra = default(SubjectAndAnaphoriaEntityTuple);
            var list  = new List< SubjectAndAnaphoriaEntityTuple >();
            foreach ( var e in sequence
                                .TakeWhile( _ => !_.IsElementVERBtypeVERB() && 
                                                 !_.IsBreakingPunctuationSign( breakByQuote ) && 
                                                 (_ != breakSearchElement) ) )
            {
                if ( e.IsDirectSubjectEntity( entityMI ) )
                    list.Add( new SubjectAndAnaphoriaEntityTuple( e ) );
                else if ( (sewra = e.ResolveAnaphoria4SubjectEntity( entityMI )) != null )
                    list.Add( sewra );

                if ( breakAfterFirstFound && list.Any() )
                    break;
            }
            return (list);
        }
        private static List< SubjectAndAnaphoriaEntityTuple > GetFirstSubject( this IEnumerable< XElement > sequence, string entityMI, XElement breakSearchElement, bool breakByQuote )
        {
            return (sequence.GetSubjects( entityMI, breakSearchElement, breakByQuote, true ));
        }
        private static List< SubjectAndAnaphoriaEntityTuple > GetSubjectsNested( this IEnumerable< XElement > sequence
                                                                                 , string   entityMI
                                                                                 , XElement breakSearchElement
                                                                                 , bool     breakByQuote
                                                                                 /*, bool     breakAfterFirstFound = false*/ )
        {
            var sewra = default(SubjectAndAnaphoriaEntityTuple);
            var list  = new List< SubjectAndAnaphoriaEntityTuple >();
            var resultEntity = default(XElement);
            foreach ( var e in sequence
                                .TakeWhile( _ => !_.IsElementVERBtypeVERB() && 
                                                 !_.IsBreakingPunctuationSign( breakByQuote ) && 
                                                 (_ != breakSearchElement) ) 
                                )
            {
                if ( e.IsDirectSubjectEntityNested( entityMI, out resultEntity ) )
                {
                    list.Add( new SubjectAndAnaphoriaEntityTuple( resultEntity ) );
                }
                else if ( (sewra = e.ResolveAnaphoria4SubjectEntityNested( entityMI )) != null )
                {
                    list.Add( sewra );
                }

                /*if ( breakAfterFirstFound && list.Any() )
                    break;*/
            }
            return (list);
        }
        private static List< SubjectAndAnaphoriaEntityTuple > GetSubjectsNestedWithBOnRight( this IEnumerable< XElement > sequence
                                                                                             , string   entityMI
                                                                                             , XElement breakSearchElement
                                                                                             , bool     breakByQuote
                                                                                             /*, bool     breakAfterFirstFound = false*/ )
        {
            var sewra = default(SubjectAndAnaphoriaEntityTuple);
            var list  = new List< SubjectAndAnaphoriaEntityTuple >();
            var resultEntity = default(XElement);
            foreach ( var e in sequence
                                .TakeWhile( _ => !_.IsElementVERBtypeVERB() && 
                                                 !_.IsBreakingPunctuationSign( breakByQuote ) && 
                                                 (_ != breakSearchElement) )
                                .SkipWhile( _ => !_.IsElementCET() || 
                                                 (_.GetAttributeVALUEorContent().TrimWhiteSpaces().ToLower() != CET_B) ) 
                                )
            {
                if ( e.IsDirectSubjectEntityNested( entityMI, out resultEntity ) )
                {
                    list.Add( new SubjectAndAnaphoriaEntityTuple( resultEntity ) );
                }
                else if ( (sewra = e.ResolveAnaphoria4SubjectEntityNested( entityMI )) != null )
                {
                    list.Add( sewra );
                }

                /*if ( breakAfterFirstFound && list.Any() )
                    break;*/
            }
            return (list);
        }
        private static List< SubjectAndAnaphoriaEntityTuple > GetSubjectsNestedWithBOnLeft( this IEnumerable< XElement > sequence
                                                                                            , string   entityMI
                                                                                            , XElement breakSearchElement
                                                                                            , bool     breakByQuote
                                                                                            /*, bool     breakAfterFirstFound = false*/ )
        {
            var existsCetB = false;

            var sewra = default(SubjectAndAnaphoriaEntityTuple);
            var list  = new List< SubjectAndAnaphoriaEntityTuple >();
            var resultEntity = default(XElement);
            foreach ( var e in sequence
                                .TakeWhile( _ => !_.IsElementVERBtypeVERB() && 
                                                 !_.IsBreakingPunctuationSign( breakByQuote ) && 
                                                 (_ != breakSearchElement) ) )
            {
                if ( e.IsElementCET() )
                {
                    if ( e.GetAttributeVALUEorContent().TrimWhiteSpaces().ToLower() == CET_B )
                    {
                        existsCetB = true;
                    }
                }
                else if ( e.IsDirectSubjectEntityNested( entityMI, out resultEntity ) )
                {
                    list.Add( new SubjectAndAnaphoriaEntityTuple( resultEntity ) );
                }
                else if ( (sewra = e.ResolveAnaphoria4SubjectEntityNested( entityMI )) != null )
                {
                    list.Add( sewra );
                }

                /*if ( breakAfterFirstFound && list.Any() )
                    break;*/
            }
            //CET_B is not found
            if ( !existsCetB )
                return (new List< SubjectAndAnaphoriaEntityTuple >());
            return (list);
        }
        private static List< SubjectAndAnaphoriaEntityTuple > GetSubjectsOrgNameWithBOnRight( this IEnumerable< XElement > sequence
                                                                                              , string   entityMI
                                                                                              , XElement breakSearchElement
                                                                                              , bool     breakByQuote
                                                                                              /*, bool     breakAfterFirstFound = false*/ )
        {
            var sewra = default(SubjectAndAnaphoriaEntityTuple);
            var list  = new List< SubjectAndAnaphoriaEntityTuple >();
            foreach ( var e in sequence
                                .TakeWhile( _ => !_.IsElementVERBtypeVERB() && 
                                                 !_.IsBreakingPunctuationSign( breakByQuote ) && 
                                                 (_ != breakSearchElement) )
                                .SkipWhile( _ => !_.IsElementCET() || 
                                                 (_.GetAttributeVALUEorContent().TrimWhiteSpaces().ToLower() != CET_B) ) 
                                )
            {
                if ( e.IsDirectSubjectEntityOrgName( entityMI ) )
                {
                    list.Add( new SubjectAndAnaphoriaEntityTuple( e ) );
                }
                else if ( (sewra = e.ResolveAnaphoria4SubjectEntityOrgName( entityMI )) != null )
                {
                    list.Add( sewra );
                }

                /*if ( breakAfterFirstFound && list.Any() )
                    break;*/
            }
            return (list);
        }
        private static List< SubjectAndAnaphoriaEntityTuple > GetSubjectsOrgNameWithBOnLeft( this IEnumerable< XElement > sequence
                                                                                             , string   entityMI
                                                                                             , XElement breakSearchElement
                                                                                             , bool     breakByQuote
                                                                                             /*, bool     breakAfterFirstFound = false*/ )
        {
            var existsCetB = false;

            var sewra = default(SubjectAndAnaphoriaEntityTuple);
            var list  = new List< SubjectAndAnaphoriaEntityTuple >();
            foreach ( var e in sequence
                                .TakeWhile( _ => !_.IsElementVERBtypeVERB() && 
                                                 !_.IsBreakingPunctuationSign( breakByQuote ) && 
                                                 (_ != breakSearchElement) ) )
            {
                if ( e.IsElementCET() )
                {
                    if ( e.GetAttributeVALUEorContent().TrimWhiteSpaces().ToLower() == CET_B )
                    {
                        existsCetB = true;
                    }
                }
                else if ( e.IsDirectSubjectEntityOrgName( entityMI ) )
                {
                    list.Add( new SubjectAndAnaphoriaEntityTuple( e ) );
                }
                else if ( (sewra = e.ResolveAnaphoria4SubjectEntityOrgName( entityMI )) != null )
                {
                    list.Add( sewra );
                }

                /*if ( breakAfterFirstFound && list.Any() )
                    break;*/
            }
            //CET_B is not found
            if ( !existsCetB )
                return (new List< SubjectAndAnaphoriaEntityTuple >());
            return (list);
        }


        //Verbs
        private static bool IsContainsSubjectData( this IEnumerable< XElement > elements, out SubjectData subjectData
                                                   , Func< XElement, bool > verbConditionFunction
                                                   , string   entityMI
                                                   , string   verbMI
                                                   , XElement breakSearchElement
                                                   , bool     breakByQuote )
        {
            subjectData = null;

            //search [verb] by condition
            var verb = elements.FindVERBtypeVERB( verbConditionFunction, verbMI );
            if ( verb.IsNull() )
                return (false);

            //search [entity] on the right from [verb]
            var rigthEntity = verb.ElementsAfterSelfExceptT().GetSubjects( entityMI, breakSearchElement, breakByQuote );
            //search [entity] on the left from [verb]
            var leftEntity  = verb.ElementsBeforeSelf().Reverse().GetSubjects( entityMI,  breakSearchElement, breakByQuote );

            if ( !rigthEntity.Any() && !leftEntity.AnyEx() )
            {
                //try-get SubjectData by verb-subsent-@SUBJ-attribute
                if ( (subjectData = verb.GetSubjectDataBySUBJ( entityMI )) != null )
                    return (true);
                return (false);
            }
            
            var tuples = new List< SubjectEssence >();
            tuples.Add( leftEntity, rigthEntity, verb );

            var leftExtremeElement  = leftEntity .AnyEx() ? leftEntity.First().Entity : verb;
            var rightExtremeElement = rigthEntity.Any()   ? rigthEntity.Last().Entity : verb;

            //result-OK
            subjectData = new SubjectData( tuples, leftExtremeElement, rightExtremeElement );
            return (true);
        }
        public static bool IsContainsSubjectData( this IEnumerable< XElement > elements, Language language, out SubjectData subjectData
                                                  , DictionaryTypeVerbs dictionaryType
                                                  , string   entityMI
                                                  , string   verbMI
                                                  , XElement breakSearchElement = null
                                                  , bool     breakByQuote       = false )
        {
            Func< XElement, bool > verbConditionFunction = 
                verb => language.DictionaryManager.ExistsInVerbs( verb.GetAttributeVALUE().ToLowerEx(), dictionaryType );

            return (elements.IsContainsSubjectData( out subjectData, verbConditionFunction, entityMI, verbMI, breakSearchElement, breakByQuote ) );
        }
        //Verbs-NestedWithB
        private static bool IsContainsSubjectDataNestedWithB( this IEnumerable< XElement > elements, out SubjectData subjectData
                                                              , Func< XElement, bool > verbConditionFunction
                                                              , string   entityMI
                                                              , string   verbMI
                                                              , XElement breakSearchElement
                                                              , bool     breakByQuote )
        {
            subjectData = null;

            //search [verb] by condition
            var verb = elements.FindVERBtypeVERB( verbConditionFunction, verbMI );
            if ( verb.IsNull() )
                return (false);

            //search [entity] on the right from [verb]
            var rigthEntity = verb.ElementsAfterSelfExceptT().GetSubjectsNestedWithBOnRight( entityMI, breakSearchElement, breakByQuote );
            //search [entity] on the left from [verb]
            var leftEntity  = verb.ElementsBeforeSelf().Reverse().GetSubjectsNestedWithBOnLeft( entityMI,  breakSearchElement, breakByQuote );

            if ( !rigthEntity.Any() && !leftEntity.AnyEx() )
            {
                //try-get SubjectData by verb-subsent-@SUBJ-attribute
                if ( (subjectData = verb.GetSubjectDataNestedWithBBySUBJ( entityMI, breakByQuote )) != null )
                    return (true);
                return (false);
            }
            
            var tuples = new List< SubjectEssence >();
            tuples.Add( leftEntity, rigthEntity, verb );

            var leftExtremeElement  = leftEntity .AnyEx() ? leftEntity.First().Entity : verb;
            var rightExtremeElement = rigthEntity.Any()   ? rigthEntity.Last().Entity : verb;

            //result-OK
            subjectData = new SubjectData( tuples, leftExtremeElement, rightExtremeElement );
            return (true);
        }
        public static bool IsContainsSubjectDataNestedWithB( this IEnumerable< XElement > elements, Language language, out SubjectData subjectData
                                                             , DictionaryTypeVerbs dictionaryType
                                                             , string   entityMI
                                                             , string   verbMI                                                             
                                                             , XElement breakSearchElement = null
                                                             , bool     breakByQuote       = false )
        {
            Func< XElement, bool > verbConditionFunction = 
                verb => language.DictionaryManager.ExistsInVerbs( verb.GetAttributeVALUE().ToLowerEx(), dictionaryType );

            return (elements.IsContainsSubjectDataNestedWithB( out subjectData, verbConditionFunction, entityMI, verbMI, breakSearchElement, breakByQuote ) );
        }
        //Verbs-OrgNameWithB
        private static bool IsContainsSubjectDataOrgNameWithB( this IEnumerable< XElement > elements, out SubjectData subjectData
                                                               , Func< XElement, bool > verbConditionFunction
                                                               , string   entityMI
                                                               , string   verbMI
                                                               , XElement breakSearchElement
                                                               , bool     breakByQuote )
        {
            subjectData = null;

            //search [verb] by condition
            var verb = elements.FindVERBtypeVERB( verbConditionFunction, verbMI );
            if ( verb.IsNull() )
                return (false);

            //search [entity] on the right from [verb]
            var rigthEntity = verb.ElementsAfterSelfExceptT().GetSubjectsOrgNameWithBOnRight( entityMI, breakSearchElement, breakByQuote );
            //search [entity] on the left from [verb]
            var leftEntity  = verb.ElementsBeforeSelf().Reverse().GetSubjectsOrgNameWithBOnLeft( entityMI,  breakSearchElement, breakByQuote );

            if ( !rigthEntity.Any() && !leftEntity.AnyEx() )
            {
                //try-get SubjectData by verb-subsent-@SUBJ-attribute
                if ( (subjectData = verb.GetSubjectDataOrgNameWithBBySUBJ( entityMI, breakByQuote )) != null )
                    return (true);
                return (false);
            }
            
            var tuples = new List< SubjectEssence >();
            tuples.Add( leftEntity, rigthEntity, verb );

            var leftExtremeElement  = leftEntity .AnyEx() ? leftEntity.First().Entity : verb;
            var rightExtremeElement = rigthEntity.Any()   ? rigthEntity.Last().Entity : verb;

            //result-OK
            subjectData = new SubjectData( tuples, leftExtremeElement, rightExtremeElement );
            return (true);
        }
        public static bool IsContainsSubjectDataOrgNameWithB( this IEnumerable< XElement > elements, Language language, out SubjectData subjectData
                                                              , DictionaryTypeVerbs dictionaryType
                                                              , string   entityMI
                                                              , string   verbMI
                                                              , XElement breakSearchElement = null
                                                              , bool     breakByQuote       = false )
        {
            Func< XElement, bool > verbConditionFunction = 
                verb => language.DictionaryManager.ExistsInVerbs( verb.GetAttributeVALUE().ToLowerEx(), dictionaryType );

            return (elements.IsContainsSubjectDataOrgNameWithB( out subjectData, verbConditionFunction, entityMI, verbMI, breakSearchElement, breakByQuote ) );
        }
        //Verbs-EntityOpinion        
        private static IList< VerbCollocationProcessingItem > GetEntityOpinion( this IEnumerable< XElement > elements
                                                                                , Language language
                                                                                , string   entityMI
                                                                                , XElement breakSearchElement
                                                                                , bool     breakByQuote )
        {
            foreach ( var element in elements
                                        .TakeWhile( _ => !_.IsElementVERBtypeVERB() && 
                                                         !_.IsBreakingPunctuationSign( breakByQuote ) && 
                                                         (_ != breakSearchElement) ) )
            {
                var processingItem = new VerbCollocationProcessingItem( element );

                if ( processingItem.FirstWordInChainOfWords.IsEmptyOrNull() )
                    continue;

                var result = language.DictionaryManager.GetMultiWordsLists
                             (
                             processingItem.FirstWordInChainOfWords, DictionaryTypeMultiWords.EntityOpinion
                             );
                if ( result == null )
                    continue;

                foreach ( var stringArrayWeight in result.StringArrayWeightList.OrderByDescending( _stringArrayWeight => _stringArrayWeight.StringArray.Length ) )
                {
                    var chainOfWords4Compare = processingItem.GetChainOfWords4Compare( stringArrayWeight.StringArray.Length );

                    if ( !chainOfWords4Compare.IsNull() )
                    {
                        var chainOfWords           = chainOfWords4Compare.Item1;
                        var chainOfProcessingItems = chainOfWords4Compare.Item2;

                        var bingo = stringArrayWeight.StringArray.SequenceEqual( chainOfWords );
                        if ( bingo )
                        {
                            return (chainOfProcessingItems);
                        }  
                    }
                }
            }
            return (null);
        }
        private static bool IsContainsSubjectEntityOpinion( this IEnumerable< XElement > elements, out SubjectData subjectData
                                                            , Language language
                                                            , Func< XElement, bool > verbConditionFunction
                                                            , string   entityMI
                                                            , string   verbMI
                                                            , XElement breakSearchElement
                                                            , bool     breakByQuote )
        {
            subjectData = null;

            //search [verb] by condition
            var verb = elements.FindVERBtypeVERB( verbConditionFunction, verbMI );
            if ( verb.IsNull() )
                return (false);

            var entityOpinion = verb.ElementsAfterSelfExceptT().GetEntityOpinion( language, entityMI, breakSearchElement, breakByQuote );
            if ( !entityOpinion.AnyEx() )
                return (false);

            //var first = verb;
            var last = entityOpinion.GetMostRightXElement();
            var rigthEntity1 = entityOpinion.Last().XElement.DescendantsExceptT().GetFirstSubject( entityMI, breakSearchElement, breakByQuote );
            //search [entity] on the right from [verb]
            var rigthEntity2 = last.ElementsAfterSelfExceptT().GetSubjectsNested( entityMI, breakSearchElement, breakByQuote );
            //search [entity] on the left from [verb]
            var leftEntity  = verb.ElementsBeforeSelf().Reverse().GetSubjectsNested( entityMI,  breakSearchElement, breakByQuote );

            if ( !rigthEntity1.Any() && !rigthEntity2.Any() && !leftEntity.AnyEx() )
                return (false);            
            
            var tuples = new List< SubjectEssence >();
            tuples.Add( leftEntity, rigthEntity1, rigthEntity2, verb );

            var leftExtremeElement  = leftEntity.AnyEx() ? leftEntity.First().Entity : verb;
            var rightExtremeElement = default(XElement);
            if ( rigthEntity1.AnyEx() && !rigthEntity2.AnyEx() ) {
                rightExtremeElement = entityOpinion.Last().XElement;
            } else if ( rigthEntity2.AnyEx() ) {
                rightExtremeElement = rigthEntity2.Last().Entity;
            } else {
                rightExtremeElement = verb;
            }

            //result-OK
            subjectData = new SubjectData( tuples, leftExtremeElement, rightExtremeElement );
            return (true);
        }
        public static bool IsContainsSubjectEntityOpinion( this IEnumerable< XElement > elements, Language language, out SubjectData subjectData
                                                             , DictionaryTypeVerbs dictionaryType
                                                             , string   entityMI
                                                             , string   verbMI
                                                             , XElement breakSearchElement = null
                                                             , bool     breakByQuote       = false )
        {
            Func< XElement, bool > verbConditionFunction = 
                verb => language.DictionaryManager.ExistsInVerbs( verb.GetAttributeVALUE().ToLowerEx(), dictionaryType );

            return (elements.IsContainsSubjectEntityOpinion( out subjectData, language, verbConditionFunction, entityMI, verbMI, breakSearchElement, breakByQuote ) );
        }
        private static bool IsContainsSubjectEntityOpinion_2( this IEnumerable< XElement > elements, out SubjectData subjectData
                                                              , Language language
                                                              , Func< XElement, bool > verbConditionFunction
                                                              , string   entityMI
                                                              , string   verbMI
                                                              , XElement breakSearchElement
                                                              , bool     breakByQuote )
        {
            subjectData = null;

            //search [verb] by condition
            var verb = elements.FindVERBtypeVERB( verbConditionFunction, verbMI );
            if ( verb.IsNull() )
                return (false);

            var entityOpinion = verb.ElementsAfterSelfExceptT().GetEntityOpinion( language, entityMI, breakSearchElement, breakByQuote );
            if ( !entityOpinion.AnyEx() )
                return (false);

            //search [entity] on the right from [verb]
            var rigthEntity  = verb.ElementsAfterSelf().GetSubjectsNested( entityMI, breakSearchElement, breakByQuote );

            if ( !rigthEntity.AnyEx() )
                return (false);            
            
            var tuples = new List< SubjectEssence >();
            tuples.Add( rigthEntity, verb );

            var leftExtremeElement  = verb;
            var rightExtremeElement = rigthEntity.First().Entity;

            //result-OK
            subjectData = new SubjectData( tuples, leftExtremeElement, rightExtremeElement );
            return (true);
        }
        public static bool IsContainsSubjectEntityOpinion_2( this IEnumerable< XElement > elements, Language language, out SubjectData subjectData
                                                             , DictionaryTypeVerbs dictionaryType
                                                             , string   entityMI
                                                             , string   verbMI
                                                             , XElement breakSearchElement = null
                                                             , bool     breakByQuote       = false )
        {
            Func< XElement, bool > verbConditionFunction = 
                verb => language.DictionaryManager.ExistsInVerbs( verb.GetAttributeVALUE().ToLowerEx(), dictionaryType );

            return (elements.IsContainsSubjectEntityOpinion_2( out subjectData, language, verbConditionFunction, entityMI, verbMI, breakSearchElement, breakByQuote ) );
        }
        //Verbs-EntityOpinionWithB
        private static bool IsContainsSubjectEntityOpinionWithB( this IEnumerable< XElement > elements, out SubjectData subjectData
                                                                 , Language language
                                                                 , Func< XElement, bool > verbConditionFunction
                                                                 , string   entityMI
                                                                 , string   verbMI
                                                                 , XElement breakSearchElement
                                                                 , bool     breakByQuote )
        {
            subjectData = null;

            //search [verb] by condition
            var verb = elements.FindVERBtypeVERB( verbConditionFunction, verbMI );
            if ( verb.IsNull() )
                return (false);

            var entityOpinion = verb.ElementsAfterSelfExceptT().GetEntityOpinion( language, entityMI, breakSearchElement, breakByQuote );
            if ( !entityOpinion.AnyEx() )
                return (false);

            //var first = verb;
            var last = entityOpinion.GetMostRightXElement();
            //search [entity] on the right from [verb]
            var rigthEntity2 = last.ElementsAfterSelfExceptT().GetSubjectsNestedWithBOnRight( entityMI, breakSearchElement, breakByQuote );
            //search [entity] on the left from [verb]
            var leftEntity  = verb.ElementsBeforeSelf().Reverse().GetSubjectsNestedWithBOnLeft( entityMI,  breakSearchElement, breakByQuote );

            if ( !rigthEntity2.Any() && !leftEntity.AnyEx() )
                return (false);

            var rigthEntity1 = entityOpinion.Last().XElement.DescendantsExceptT().GetFirstSubject( entityMI, breakSearchElement, breakByQuote );
            
            var tuples = new List< SubjectEssence >();
            tuples.Add( leftEntity, rigthEntity1, rigthEntity2, verb );

            var leftExtremeElement  = leftEntity.AnyEx() ? leftEntity.First().Entity : verb;
            var rightExtremeElement = default(XElement);
            if ( rigthEntity1.AnyEx() && !rigthEntity2.AnyEx() ) {
                rightExtremeElement = entityOpinion.Last().XElement;
            } else if ( rigthEntity2.AnyEx() ) {
                rightExtremeElement = rigthEntity2.Last().Entity;
            } else {
                rightExtremeElement = verb;
            }

            //result-OK
            subjectData = new SubjectData( tuples, leftExtremeElement, rightExtremeElement );
            return (true);
        }
        public static bool IsContainsSubjectEntityOpinionWithB( this IEnumerable< XElement > elements, Language language, out SubjectData subjectData
                                                                , DictionaryTypeVerbs dictionaryType
                                                                , string   entityMI
                                                                , string   verbMI
                                                                , XElement breakSearchElement = null
                                                                , bool     breakByQuote       = false )
        {
            Func< XElement, bool > verbConditionFunction = 
                verb => language.DictionaryManager.ExistsInVerbs( verb.GetAttributeVALUE().ToLowerEx(), dictionaryType );

            return (elements.IsContainsSubjectEntityOpinionWithB( out subjectData, language, verbConditionFunction, entityMI, verbMI, breakSearchElement, breakByQuote ) );
        }

        //Phrasis1 (same as 'MultiWords - IntrWords1')
        private static IList< VerbCollocationProcessingItem > GetVerbCollocationByPhrasis1( this IEnumerable< XElement > elements, Language language )
        {
            foreach ( var element in elements )
            {
                var processingItem = new VerbCollocationProcessingItem( element );

                if ( processingItem.FirstWordInChainOfWords.IsEmptyOrNull() )
                    continue;

                var result = language.DictionaryManager.GetPhrasis1Lists
                        (
                        processingItem.FirstWordInChainOfWords, DictionaryTypePhrasis1.Phrasis1
                        );

                if ( result == null )
                    continue;


                foreach ( var stringArrayWeight in result.StringArrayWeightList.OrderByDescending( _stringArrayWeight => _stringArrayWeight.StringArray.Length ) )
                {
                    var chainOfWords4Compare = processingItem.GetChainOfWords4Compare( stringArrayWeight.StringArray.Length );

                    if ( !chainOfWords4Compare.IsNull() )
                    {
                        var chainOfWords           = chainOfWords4Compare.Item1;
                        var chainOfProcessingItems = chainOfWords4Compare.Item2;

                        var bingo = stringArrayWeight.StringArray.SequenceEqual( chainOfWords );
                        if ( bingo /*&& chainOfProcessingItems.Any( _ => _.TypeOfXmlElement == TypeOfXmlElement.VERB )*/ )
                        {
                            return (chainOfProcessingItems);
                        }  
                    }
                }
            }
            return (null);
        }
        private static XElement GetMostLeftXElement( this IList< VerbCollocationProcessingItem > verbCollocationItems )
        {
            var rightElement     = verbCollocationItems.First().XElement;
            var minAnsestorCount = rightElement.Ancestors().Count();
            foreach ( var element in verbCollocationItems.Skip( 1 ).Select( _ => _.XElement ) )
            {
                if ( element.Ancestors().Count() < /*!*/ minAnsestorCount )
                {
                    rightElement     = element;
                    minAnsestorCount = rightElement.Ancestors().Count();
                }
            }
            return (rightElement);
        }
        private static XElement GetMostRightXElement( this IList< VerbCollocationProcessingItem > verbCollocationItems )
        {
            var rightElement     = verbCollocationItems.First().XElement;
            var minAnsestorCount = rightElement.Ancestors().Count();
            foreach ( var element in verbCollocationItems.Skip( 1 ).Select( _ => _.XElement ) )
            {
                if ( element.Ancestors().Count() <=/*!*/ minAnsestorCount )
                {
                    rightElement     = element;
                    minAnsestorCount = rightElement.Ancestors().Count();
                }
            }
            return (rightElement);
        }
        private static string GetTextValue( VerbCollocationProcessingItem item )
        {
            switch ( item.TypeOfXmlElement )
            {
                case TypeOfXmlElement.U:
                    return (item.CDataText4ElementU.Trim());
                default:
                    return (item.PROF.IsEmptyOrNull() ? item.VALUE : item.PROF);
            }            
        }
        private static string GetTextValue( this IList< VerbCollocationProcessingItem > verbCollocationItems )
        {
            return (verbCollocationItems.Select( GetTextValue ).JoinBySpace());
        }
        public static bool IsContainsSubjectData( this IEnumerable< XElement > elements, Language language, out SubjectData subjectData
                                                  , DictionaryTypePhrasis1 dictionaryType
                                                  , string   entityMI
                                                  , string   verbMI
                                                  , XElement breakSearchElement = null
                                                  , bool     breakByQuote       = false )
        {
            //by default - rigth-entity-search-only for type [DictionaryTypePhrasis]
            var onlyRigthEntitySearch = true;

            subjectData = null;

            //search [verb-Collocation] by condition
            var verbCollocationItems = elements.GetVerbCollocationByPhrasis1( language );
            if ( !verbCollocationItems.AnyEx() )
                return (false);

            if ( !verbCollocationItems.IsVerbsConditionMI( verbMI ) )
                return (false);

            //search [entity] on the right from [verb]
            var first = verbCollocationItems.GetMostLeftXElement();
            var last  = verbCollocationItems.GetMostRightXElement();

            var rigthEntity = verbCollocationItems.Last().XElement.DescendantsExceptT().GetFirstSubject( entityMI, breakSearchElement, breakByQuote )
                              .Concat
                              (
                              last.ElementsAfterSelfExceptT().GetSubjects( entityMI, breakSearchElement, breakByQuote )
                              )
                              .ToArray();
            //search [entity] on the left from [verb]
            var leftEntity  = onlyRigthEntitySearch ? null : first.ElementsBeforeSelf().Reverse().GetSubjects( entityMI, breakSearchElement, breakByQuote );

            if ( !rigthEntity.Any() && !leftEntity.AnyEx() )
                return (false);

            var verbValue = verbCollocationItems.GetTextValue();

            var tuples = new List< SubjectEssence >();
            tuples.Add( leftEntity, rigthEntity, verbValue );

            var leftExtremeElement  = leftEntity.AnyEx() ? leftEntity.First().Entity : last;
            var rightExtremeElement = rigthEntity.Any()  ? rigthEntity.Last().Entity : last;

            //result-OK
            subjectData = new SubjectData( tuples, leftExtremeElement, rightExtremeElement );
            return (true);
        }

        //Phrasis2
        private static IList< VerbCollocationProcessingItem > GetVerbCollocationByPhrasis2( this IEnumerable< XElement > elements, Language language )
        {
            foreach ( var element in elements )
            {
                var processingItem = new VerbCollocationProcessingItem( element );

                if ( processingItem.FirstWordInChainOfWords.IsEmptyOrNull() )
                    continue;

                var result = language.DictionaryManager.GetPhrasis2Lists
                        (
                        processingItem.FirstWordInChainOfWords, DictionaryTypePhrasis2.Phrasis2
                        );

                if ( result == null )
                    continue;


                foreach ( var stringArrayWeight in result.StringArrayWeightList.OrderByDescending( _stringArrayWeight => _stringArrayWeight.StringArray.Length ) )
                {
                    bool bingo = false;

                    var chainOfProcessingItems = new List< VerbCollocationProcessingItem >();
                        chainOfProcessingItems.Add( processingItem );
                    foreach ( var w in stringArrayWeight.StringArray )
                    {
                        bingo = false;

                        //разрывная двухсловная глагольная коллокация: между словами от нуля до любого кол-во элементов Cet/U/Entity 
                        if ( w == "*" )
                        {
                        #region
                            //var count = chainOfProcessingItems.Count;
                            for (; ; )
                            {
                                var prev = processingItem;
                                processingItem = processingItem.NextCollocationSibling;
                                if ( processingItem == null )
                                {
                                    break;
                                }
                                if ( processingItem.IsTypeOfXmlElement_Cet_U_Entity )
                                {
                                   chainOfProcessingItems.Add( processingItem );
                                }
                                else if ( processingItem.TypeOfXmlElement == TypeOfXmlElement.VERB )
                                {
                                    //if ( chainOfProcessingItems.Count > count )
                                    //{
                                    processingItem = prev;
                                    bingo = true;
                                    //}
                                    break;
                                }
                                else
                                {
                                    break;
                                }
                            }

                            if ( !bingo )
                                break;                           
                        #endregion
                        }
                        else
                        {
                        #region
                            processingItem = processingItem.NextCollocationSibling;
                            if ( processingItem == null )
                            {
                                break;
                            }
                            switch ( processingItem.TypeOfXmlElement )
                            {
                                case TypeOfXmlElement.VERB:
                                    if ( processingItem.VALUE == w )
                                    {
                                        chainOfProcessingItems.Add( processingItem );
                                        bingo = true;
                                    }
                                break;

                                default:
                                    break;
                            }

                            if ( !bingo )
                                break;
                        #endregion
                        }
                    }

                    if ( bingo /*&& chainOfProcessingItems.Any( _ => _.TypeOfXmlElement == TypeOfXmlElement.VERB )*/ )
                    {
                        return (chainOfProcessingItems);
                    }
                }
            }
            return (null);
        }
        public static bool IsContainsSubjectData( this IEnumerable< XElement > elements, Language language, out SubjectData subjectData
                                                  , DictionaryTypePhrasis2 dictionaryType
                                                  , string   entityMI
                                                  , string   verbMI
                                                  , XElement breakSearchElement = null
                                                  , bool     breakByQuote       = false )
        {
            subjectData = null;

            //search [verb-Collocation] by condition
            var verbCollocationItems = elements.GetVerbCollocationByPhrasis2( language );
            if ( !verbCollocationItems.AnyEx() )
                return (false);

            if ( !verbCollocationItems.IsVerbsConditionMI( verbMI ) )
                return (false);

            //search [entity] on the right from [verb]
            var first = verbCollocationItems.GetMostLeftXElement();
            var last  = verbCollocationItems.GetMostRightXElement();

            var rigthEntity = verbCollocationItems.Last().XElement.DescendantsExceptT().GetFirstSubject( entityMI, breakSearchElement, breakByQuote )
                              .Concat
                              (
                              last.ElementsAfterSelfExceptT().GetSubjects( entityMI, breakSearchElement, breakByQuote )
                              )
                              .ToArray();
            //search [entity] on the left from [verb]
            var leftEntity  = first.ElementsBeforeSelf().Reverse().GetSubjects( entityMI, breakSearchElement, breakByQuote );

            if ( !rigthEntity.Any() && !leftEntity.AnyEx() )
                return (false);

            var verbValue = verbCollocationItems.GetTextValue();

            var tuples = new List< SubjectEssence >();
            tuples.Add( leftEntity, rigthEntity, verbValue );

            var leftExtremeElement  = leftEntity.AnyEx() ? leftEntity.First().Entity : last;
            var rightExtremeElement = rigthEntity.Any()  ? rigthEntity.Last().Entity : last;

            //result-OK
            subjectData = new SubjectData( tuples, leftExtremeElement, rightExtremeElement );
            return (true);
        }

        //MultiWords - IntrWords1 (same as 'Phrasis1')
        private static IList< VerbCollocationProcessingItem > GetVerbCollocationByIntrWords1( this IEnumerable< XElement > elements, Language language )
        {
            foreach ( var element in elements )
            {
                var processingItem = new VerbCollocationProcessingItem( element );

                if ( processingItem.FirstWordInChainOfWords.IsEmptyOrNull() )
                    continue;

                var result = language.DictionaryManager.GetMultiWordsLists
                        (
                        processingItem.FirstWordInChainOfWords, DictionaryTypeMultiWords.IntrWords1
                        );

                if ( result == null )
                    continue;


                foreach ( var stringArrayWeight in result.StringArrayWeightList.OrderByDescending( _stringArrayWeight => _stringArrayWeight.StringArray.Length ) )
                {
                    var chainOfWords4Compare = processingItem.GetChainOfWords4Compare( stringArrayWeight.StringArray.Length );

                    if ( !chainOfWords4Compare.IsNull() )
                    {
                        var chainOfWords           = chainOfWords4Compare.Item1;
                        var chainOfProcessingItems = chainOfWords4Compare.Item2;

                        var bingo = stringArrayWeight.StringArray.SequenceEqual( chainOfWords );
                        if ( bingo /*&& chainOfProcessingItems.Any( _ => _.TypeOfXmlElement == TypeOfXmlElement.VERB )*/ )
                        {
                            return (chainOfProcessingItems);
                        }  
                    }
                }
            }
            return (null);
        }
        public static bool IsContainsSubjectData( this IEnumerable< XElement > elements, Language language, out SubjectData subjectData
                                                  , DictionaryTypeMultiWords dictionaryType
                                                  , string   entityMI
                                                  , XElement breakSearchElement = null
                                                  , bool     breakByQuote       = false )
        {
            //by default - rigth-entity-search-only for type [DictionaryTypeMultiWords]
            var onlyRigthEntitySearch = true;

            subjectData = null;

            //search [verb-Collocation] by condition
            var verbCollocationItems = elements.GetVerbCollocationByIntrWords1( language );
            if ( !verbCollocationItems.AnyEx() )
                return (false);

            /*if ( !verbCollocationItems.IsVerbsConditionMI( verbMI ) )
                return (false);*/

            //search [entity] on the right from [verb]
            var first = verbCollocationItems.GetMostLeftXElement();
            var last  = verbCollocationItems.GetMostRightXElement();

            var rigthEntity = verbCollocationItems.Last().XElement.DescendantsExceptT().GetFirstSubject( entityMI, breakSearchElement, breakByQuote )
                              .Concat
                              ( 
                              last.ElementsAfterSelfExceptT().GetSubjects( entityMI, breakSearchElement, breakByQuote )
                              )
                              .ToArray();
            //search [entity] on the left from [verb]
            var leftEntity  = onlyRigthEntitySearch ? null : first.ElementsBeforeSelf().Reverse().GetSubjects( entityMI, breakSearchElement, breakByQuote );

            if ( !rigthEntity.Any() && !leftEntity.AnyEx() )
                return (false);

            var verbValue = verbCollocationItems.GetTextValue();

            var tuples = new List< SubjectEssence >();
            tuples.Add( leftEntity, rigthEntity, verbValue );

            var leftExtremeElement  = leftEntity.AnyEx() ? leftEntity.First().Entity : last;
            var rightExtremeElement = rigthEntity.Any()  ? rigthEntity.Last().Entity : last;

            //result-OK
            subjectData = new SubjectData( tuples, leftExtremeElement, rightExtremeElement );
            return (true);
        }       
    }
}