using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Xml.Linq;

using Linguistics.Coreference;
using TextMining.Core;
using TextMining.Service;

namespace OpinionMining.DirectIndirectSpeechExtensions
{
    /// <summary>
    /// 
    /// </summary>
    internal static class Common
    {
        private const int ATTRIBUTEVALUE_ISOBJECT_4AUTHORSUBJECT = -1;

        public static string GetRawCDataTextFromElementU( this XElement element )
        {
            if ( !element.IsElementU() )
                throw (new ArgumentException("This is not 'U' element"));
            
            return (((XText) element.FirstNode).Value);
        }
        public static bool IsSubsentEndWithU( this XElement subsent, Func< string, bool > uConditionFunction )
        {subsent.ThrowIfNotSubsent();

            var u = subsent.DescendantsExceptT()/*ElementsExceptT()*/.LastOrDefault();
            if ( u.IsNull() || !u.IsElementU() )
                return (false);
            var utext = u.GetRawCDataTextFromElementU().TrimWhiteSpaces();
            if ( !uConditionFunction( utext ) )
                return (false);

            return (true);
        }
        public static bool IsSubsentPrecedeU( this XElement subsent, string[] U_WORDS ) 
        {
            return (subsent.IsSubsentPrecedeU( utext => U_WORDS.Any( _ => utext.EndsWith( _ ) ) ));
        }
        public static bool IsSubsentPrecedeU( this XElement subsent, Func< string, bool > uConditionFunction )
        {subsent.ThrowIfNotSubsent();

            var u = subsent.ElementsBeforeSelf().LastOrDefault();
            if ( u.IsNull() || !u.IsElementU() )
                return (false);

            var utext = u.GetRawCDataTextFromElementU().TrimWhiteSpaces();
            return (uConditionFunction( utext ));
        }
        public static XElement GetElementU( this IEnumerable< XElement > elements, Func< string, bool > uConditionFunction )
        {
            var elementU = (from u in elements
                            where u.IsElementU()
                            let v = u.GetRawCDataTextFromElementU().TrimWhiteSpaces()
                            where uConditionFunction( v )
                            select u
                           )
                           .FirstOrDefault();

            return (elementU);
        }

        private static XElement TryGetAncestorHomogeneous( this XElement element )
        {
            foreach ( var a in element.Ancestors( TextMining.Core.XLinqExtensions.Consts.ENTITY_ELEMENT ) )
            {
                if ( a.GetAttributeTYPE() == TypeAttributeValue.Homogeneous )
                    return (a);
            }
            return (null);
        }
        /// <summary>
        /// SetAttributeISOBJECT with provision for Homogeneous
        /// </summary>
        private static XElement SetAttributeISOBJECT_wpfHomogeneous( this XElement @object, int attributeValueISOBJECT )
        {
            var homogeneous = @object.TryGetAncestorHomogeneous();
            if ( homogeneous != null )
            {
                homogeneous.SetAttributeISOBJECT( attributeValueISOBJECT );
                return (homogeneous);
            }

            @object.SetAttributeISOBJECT( attributeValueISOBJECT );
            return (@object);
        }

        private static XElement TryAllocateObject( this XElement element, ObjectAllocateMethod objectAllocateMethod, int attributeValueISOBJECT )
        {
            var @object = element.DescendantsAndSelfEntity().FirstOrDefault( e => Algorithms.IsObjectEntity( e ) );
            if ( @object.IsNotNull() &&
                 @object.GetAttributeISSUBJECT().IsNull() ) //and not has attribute 'ISSUBJECT'
            {
                return (@object.SetAttributeISOBJECT_wpfHomogeneous( attributeValueISOBJECT ));
            }

            if ( (objectAllocateMethod & ObjectAllocateMethod.FirstEntityWithRoleSubj) == ObjectAllocateMethod.FirstEntityWithRoleSubj )
            {
                #region [.FirstEntityWithRoleSubj.]
                @object = element.DescendantsAndSelfEntity().FirstOrDefault( e => e.HasRoleAttributeValue( RoleAttributeValue.Subj ) );
                if ( @object.IsNotNull() &&
                     @object.GetAttributeISSUBJECT().IsNull() ) //and not has attribute 'ISSUBJECT'
                {
                    return (@object.SetAttributeISOBJECT_wpfHomogeneous( attributeValueISOBJECT ));
                }

                if ( (objectAllocateMethod & ObjectAllocateMethod.FirstEntityWithTypePronoun) == ObjectAllocateMethod.FirstEntityWithTypePronoun )
                {
                    #region [.FirstEntityWithTypePronoun.]
                    @object = element.DescendantsAndSelfEntity().FirstOrDefault( e => e.HasTypeAttributeValue( TypeAttributeValue.Pronoun ) );
                    if ( @object.IsNotNull() &&
                         @object.GetAttributeISSUBJECT().IsNull() ) //and not has attribute 'ISSUBJECT'
                    {
                        return (@object.SetAttributeISOBJECT_wpfHomogeneous( attributeValueISOBJECT ));
                    }
                    #endregion
                }
                #endregion
            }

            return (null);
        }
        public static List< XElement > TryAllocateObjects4AuthorSubject( this XElement sent, ObjectAllocateMethod objectAllocateMethod, InquiriesSynonymsInfo isi )
        {sent.ThrowIfNotSent();

            var objects = new List< XElement >();

            var subsents = Algorithms.GetSubsents4AllocateInquiry( sent );
            foreach ( var subsent in subsents )
            {
                var inquiries = Algorithms.GetAllocateInquiries4Subsent( subsent, objectAllocateMethod, isi, false );
                foreach ( var inquiryEntity in inquiries )
                {
                    objects.Add( inquiryEntity.SetAttributeISOBJECT_wpfHomogeneous( ATTRIBUTEVALUE_ISOBJECT_4AUTHORSUBJECT ) );
                }
            }

            return (objects);
        }
        /*public static List< XElement > TryAllocateObjects4AuthorSubject4Automat( this XElement sent, InputParams4AutomatBase inputParams )
        {sent.ThrowIfNotSent();

            var objects = new List< XElement >();

            var subsents = Algorithms.GetSubsents4AllocateInquiry( sent );
            foreach ( var subsent in subsents )
            {
                var inquiries = Algorithms.GetAllocateInquiries4Automat4Subsent( subsent, inputParams, false );
                foreach ( var inquiryEntity in inquiries )
                {
                    inquiryEntity.SetAttributeISOBJECT( ATTRIBUTEVALUE_ISOBJECT_4AUTHORSUBJECT );
                    objects.Add( inquiryEntity );
                }
            }

            #region [.anaphoria | coreference4 inquiry.]
            var anaphoriaOrcoreference = Algorithms.GetAnaphoriaOrCoreference( subsents );

            foreach ( var CETorENTITY in anaphoriaOrcoreference )
            {
                if ( CETorENTITY.IsElementCET() )
                    CETorENTITY.RenameCET2ENTITY();

                CETorENTITY.SetAttributeISOBJECT( ATTRIBUTEVALUE_ISOBJECT_4AUTHORSUBJECT );
                objects.Add( CETorENTITY );
            }
            #endregion

            return (objects);
        }*/
        public static List< XElement > TryAllocateObjects( this IEnumerable< XElement > elements, 
                                                           ObjectAllocateMethod objectAllocateMethod, 
                                                           int attributeValueISOBJECT )
        {
            var objects = new List< XElement >();
            foreach ( var e in elements ) 
            {
                var @object = e.TryAllocateObject( objectAllocateMethod, attributeValueISOBJECT );
                if ( @object.IsNotNull() ) 
                {
                    objects.Add( @object );
                }
            }
            return (objects);
        }
        public static IEnumerable< XElement > TryAllocateObjectsInSubsents( this IEnumerable< XElement > subsents, 
                                                                            ObjectAllocateMethod objectAllocateMethod, 
                                                                            int id, 
                                                                            XElement breakSearchElement = null )
        {subsents.ForEachEx( _ => _.ThrowIfNotSubsent() );

            if ( breakSearchElement != null )
            {
                var list = new List< XElement >();
                foreach ( var subsent in subsents )
                {
                    #region commented. previous
                    /*
                    var e0 = subsent.ElementsExceptT().ToArray();
                    var e1 = e0.TakeWhile( _ => _ != breakSearchElement ).ToArray();

                    list.AddRange( e1.TryAllocateObjects( objectAllocateMethod, id ) );

                    if ( e0.Length != e1.Length )
                        break;
                    //*/
                    #endregion

                    var els = subsent.ElementsExceptT().ToArray();

                    var obj0 = els.TryAllocateObjects( objectAllocateMethod, id );
                    var obj1 = obj0.TakeWhile( o => o.Ancestors().All( a => a != breakSearchElement ) ).ToArray();

                    list.AddRange( obj1 );

                    if ( obj0.Count != obj1.Length )
                        break;
                }
                return (list.Distinct());
            }
            else
            {
                var objects = from subsent in subsents                          
                              let objs = subsent.ElementsExceptT().TryAllocateObjects( objectAllocateMethod, id )
                              from o in objs
                              select o;

                return (objects.Distinct());
            }
        }
        public static IList< XElement > TryAllocateObjectsInUnknownElements( this IEnumerable< XElement > unknownElements, 
                                                                             ObjectAllocateMethod objectAllocateMethod, 
                                                                             int id )
        {
            var list = new List< XElement >();

            foreach ( var unk in unknownElements )
            {
                switch ( unk.Name.LocalName )
                {
                    case TextMining.Core.XLinqExtensions.Consts.SENT_ELEMENT:
                        list.AddRangeEx( unk.ElementsExceptT().TryAllocateObjects( objectAllocateMethod, id ) );
                    break;

                    case TextMining.Core.XLinqExtensions.Consts.SUBSENT_ELEMENT:
                        var inquiries = Algorithms.GetAllocateInquiries4Subsent( unk, objectAllocateMethod, null, false );
                        foreach ( var inquiry in inquiries )
                        {
                            inquiry.SetAttributeISOBJECT_wpfHomogeneous( id );
                        }
                        list.AddRangeEx( inquiries );
                    break;

                    case TextMining.Core.XLinqExtensions.Consts.U_ELEMENT:
                    break;

                    default:
                        var @object = unk.TryAllocateObject( objectAllocateMethod, id );
                        if ( @object.IsNotNull() )
                        {
                            list.Add( @object );
                        }
                    break;
                }
            }
            return (list.Distinct().ToList());
        }

        public static bool IsHasSubject( this IEnumerable< XElement > elements )
        {
            return (elements.Any( e => e.DescendantsAndSelf().Any( _ => _.GetAttributeISSUBJECT().IsNotNull() ) ) );
        }
        public static bool IsHasObject( this IEnumerable< XElement > elements )
        {
            return (elements.Any( e => e.DescendantsAndSelf().Any( _ => _.GetAttributeISOBJECT().IsNotNull() ) ) );
        }
        public static bool IsHasObjectOrSubject( this IEnumerable< XElement > elements )
        {
            return (elements.Any( e => e.DescendantsAndSelf().Any( _ => _.GetAttributeISSUBJECT().IsNotNull() ||
                                                                        _.GetAttributeISOBJECT() .IsNotNull() ) ) );
        }
        public static bool IsNotHasObject( this IEnumerable< XElement > elements )
        {
            return (!elements.IsHasObject());
        }
        public static bool IsSubsentHasSubject( this XElement subsent )
        {subsent.ThrowIfNotSubsent();

            return (subsent.ElementsEntity().IsHasSubject());
        }
        public static bool IsSubsentHasObject( this XElement subsent )
        {subsent.ThrowIfNotSubsent();

            return (subsent.ElementsEntity().IsHasObject());
        }
        private static bool IsSubsentHasObjectOrSubject( this XElement subsent )
        {subsent.ThrowIfNotSubsent();

            return (subsent.ElementsEntity().IsHasObjectOrSubject());
        }
        public static bool IsSubsentNotHasSubject( this XElement subsent )
        {
            return (!subsent.IsSubsentHasSubject());
        }
        public static bool IsSubsentNotHasObjectAndSubject( this XElement subsent )
        {
            return (!subsent.IsSubsentHasObjectOrSubject());
        }

        public static bool IsInDirectSpeech( this IEnumerable< XElement > elements )
        {
            var first = elements.FirstOrDefault();
            if ( first.IsNotNull() )
                return (first.Ancestors().Any( _ => _.IsElementSUBSENT() && 
                                                   (_.GetAttributeSUBSENTTYPE() == SubsentenceType.DirectSpeech) ) );
            return (false);
        }
        public static bool IsNotInDirectSpeech( this IEnumerable< XElement > elements )
        {
            return (!elements.IsInDirectSpeech());
        }
        public static bool IsSubsentInDirectSpeech( this XElement subsent )
        {subsent.ThrowIfNotSubsent();

            return (subsent.Elements().IsInDirectSpeech());
        }

        /*public static bool IsContainsInSynonyms( this ObjectEssence @object, InputParams4AutomatBase inputParams )
        {
            return (@object.IsApproach2InquiryType     ( inputParams.InquiryType       ) && 
                    @object.Entity.IsContainsInSynonyms( inputParams.InquiriesSynonyms ));
        }*/
        /*public static bool IsContainsInSynonyms( this SubjectEssence subject, InputParams4AutomatBase inputParams )
        {
            return (subject.IsApproach2InquiryType     ( inputParams.InquiryType       ) && 
                    subject.Entity.IsContainsInSynonyms( inputParams.InquiriesSynonyms ));
        }*/
        private static bool IsContainsInInquiriesSynonyms( this EssenceItem ei, List< string > inquiriesSynonyms )
        {
            if ( inquiriesSynonyms.Contains( ei.Value.ToLowerEx() ) ||
                 inquiriesSynonyms.Contains( ei.Prof .ToLowerEx() ) 
               )
            {
                return (true);
            }
            return (false);
        }        
        public static bool IsContainsInSynonyms
            ( this Essence essence, List< string > inquiriesSynonyms, ICoreferenceInfo coreferenceInfo )
        {
            if ( inquiriesSynonyms == null )
                return (true);

            if ( Algorithms.IsHomogeneous( essence ) )
            {
                var elements = Algorithms.GetObjectsFromHomogeneous( essence.Entity, ObjectAllocateMethod.PhysicalOrJuridicalPersons );
                if ( elements.Contains( essence.Entity ) ) //(elements.Count == 1) && (elements.First() == essence.Entity) )
                {
                    elements = Algorithms.GetObjectsFromHomogeneous( essence.Entity, ObjectAllocateMethod.FirstEntityWithTypePronoun );
                }
                var essenceItems = Algorithms.ConstructEssenceItems4Homogeneous( elements, coreferenceInfo );

                essence.SetEssenceItems4Homogeneous( essenceItems );
            }

            foreach ( var ei in essence.EssenceItems )
            {
                if ( ei.IsContainsInInquiriesSynonyms( inquiriesSynonyms ) )
                {
                    return (true);
                }
            }
            return (false);
        }
        public static FilterBySynonyms? IsContainsInSynonyms
            ( this List< string > inquiriesSynonyms, SubjectEssence subject, ObjectEssence @object, ICoreferenceInfo coreferenceInfo )
        {
            if ( subject.IsContainsInSynonyms( inquiriesSynonyms, coreferenceInfo ) )
            {
                if ( @object.IsContainsInSynonyms( inquiriesSynonyms, coreferenceInfo ) )
                    return (FilterBySynonyms.SubjectAndObject);

                return (FilterBySynonyms.Subject);
            }
            else if ( @object.IsContainsInSynonyms( inquiriesSynonyms, coreferenceInfo ) )
            {
                return (FilterBySynonyms.Object);
            }
            return (null); //(FilterBySynonyms.NoFiltration);
        }

        private static ReadOnlyCollection< EssenceItem > ConstructEssenceItems4Homogeneous
            ( XElement essenceElement, ICoreferenceInfo coreferenceInfo /*, ObjectAllocateMethod objectAllocateMethod*/ )
        {
            //ГДЕ-ТО В "OM" КАК СУБЪЕКТЫ-ОБЪЕКТЫ ВЫДЕЛЯЮТСЯ ТОЛЬКО [OrgName's]  !!!!!!!!!
            var elements = essenceElement.GetObjectsFromHomogeneous( ObjectAllocateMethod.PhysicalOrJuridicalPersons /*objectAllocateMethod*/);
            if ( elements.Contains( essenceElement ) )//  (elements.Count == 1) && (elements.First() == essenceElement) )
            {
                elements = essenceElement.GetObjectsFromHomogeneous( ObjectAllocateMethod.FirstVerbEntityWithRoleObj );
            }
            if ( elements.Count == 0 ) throw (new ArgumentNullException("GetObjectsFromHomogeneous() == 0"));

            var lst = Algorithms.ConstructEssenceItems4Homogeneous( elements, coreferenceInfo );
            return (lst);
        }
        private static void ReprocessEssenceItems4Homogeneous
            ( Essence essence, ICoreferenceInfo coreferenceInfo /*, ObjectAllocateMethod objectAllocateMethod*/ )
        {
            if ( Algorithms.IsHomogeneous( essence ) )
            {
                var essenceItems = ConstructEssenceItems4Homogeneous( essence.Entity, coreferenceInfo /*, objectAllocateMethod*/ );
                essence.SetEssenceItems4Homogeneous( essenceItems );
            } 
        }        
        public static void ReprocessEssenceItems4Homogeneous
            ( OpinionMiningTuple opinionMiningTuple, ICoreferenceInfo coreferenceInfo /*, ObjectAllocateMethod objectAllocateMethod*/ )
        {
            if ( !opinionMiningTuple.Subject.IsAuthor )
            {
                ReprocessEssenceItems4Homogeneous( opinionMiningTuple.Subject, coreferenceInfo /*, objectAllocateMethod*/ );
            }
            if ( opinionMiningTuple.HasObject )
            {
                ReprocessEssenceItems4Homogeneous( opinionMiningTuple.Object, coreferenceInfo /*, objectAllocateMethod*/ );
            }
        }


        /*public static FilterBySynonyms? IsContainsInSynonyms( this InputParams4AutomatBase inputParams
                                                              , SubjectEssence     subject
                                                              , ObjectEssence      @object )
        {
            if ( subject.IsContainsInSynonyms( inputParams ) )
            {
                if ( @object.IsContainsInSynonyms( inputParams ) )
                    return (FilterBySynonyms.SubjectAndObject);

                return (FilterBySynonyms.Subject);
            }
            else if ( @object.IsContainsInSynonyms( inputParams ) )
            {
                return (FilterBySynonyms.Object);
            }
            return (null);
        }*/
        /*private static bool IsApproach2InquiryType( this SubjectEssence subject, LingvisticClient.TonInquiryType inquiryType )
        {
            if ( inquiryType == LingvisticClient.TonInquiryType.Other )
                return (true);
            return (subject.Entity.GetAutomatInquiryType() == inquiryType);
        }
        private static bool IsApproach2InquiryType( this ObjectEssence  @object, LingvisticClient.TonInquiryType inquiryType )
        {
            if ( inquiryType == LingvisticClient.TonInquiryType.Other )
                return (true);
            return (@object.Entity.GetAutomatInquiryType() == inquiryType);
        }        */

        public static XElement GetRightExtremeElement( this IEnumerable< SubjectData > collection )
        {
            if ( collection.AnyEx() )
            {
                var rightExtremeElement = collection.First().RightExtremeElement;
                foreach ( var _ in collection.Skip( 1 ) )
                {
                    if ( _.RightExtremeElement.IsAfter( rightExtremeElement ) )
                        rightExtremeElement = _.RightExtremeElement;
                }
                return (rightExtremeElement);
            }
            return (null);
        }
        public static XElement GetLeftExtremeElement( this IEnumerable< SubjectData > collection )
        {
            if ( collection.AnyEx() )
            {
                var leftExtremeElement = collection.First().LeftExtremeElement;
                foreach ( var _ in collection.Skip( 1 ) )
                {
                    if ( _.LeftExtremeElement.IsBefore( leftExtremeElement ) )
                        leftExtremeElement = _.LeftExtremeElement;
                }
                return (leftExtremeElement);
            }
            return (null);
        }
        public static void Add2List( this SubjectData subjectData, ref List< SubjectData > subjectDataList )
        {
            if ( subjectData.IsNotNull() )
            {
                if ( subjectDataList == null )
                {
                    subjectDataList = new List< SubjectData >();
                }
                subjectDataList.Add( subjectData );
            }
        }

        public static List< SubjectData > MakeDistinct( this List< SubjectData > subjectDataList )
        {
            if ( (subjectDataList != null) && (1 < subjectDataList.Count) )
            {
                //.1 - убираем ENTITY-дубли
                var hashSet = new HashSet< XElement >();
                var stack   = new Stack< SubjectData >(); //new List< SubjectData >();
                foreach ( var sd in subjectDataList.OrderByDescending( _sd1 => _sd1.SubjectEssences.MaxEx( _sd2 => _sd2.VerbValue.LengthEx() ) ) )
                {
                    var unique = sd.SubjectEssences.Where( s => hashSet.Add( s.Entity ) ).ToList();
                    if ( unique.Count == sd.SubjectEssences.Count )
                    {
                        stack.Push( sd );
                    }
                    else if ( unique.Any() )
                    {
                        var unique_sd = new SubjectData( unique, sd.LeftExtremeElement, sd.RightExtremeElement );
                        stack.Push( unique_sd );
                    }
                }

                //.2 - убираем вложенные ENTITY-дубли - [«Людмила Марковна навсегда останется в нашей памяти»,- говорится в телеграмме премьер-министра России Владимира Путина.]
                var result = new List< SubjectData >();
                for ( var sd = stack.Pop(); ; sd = stack.Pop() )
                {
                    #region
                    if ( sd.SubjectEssences.Count == 1 )
                    {
                        #region
                        var ae = sd.SubjectEssences.First().Entity.Ancestors().ToArray();
                        if ( stack.Any
                                ( 
                                _sd => _sd.SubjectEssences.Any
                                    ( 
                                    e => ae.Contains( e.Entity )
                                    ) 
                                ) 
                           )
                        {
                            continue;
                        }
                        #endregion
                    }
                    else
                    {
                        #region
                        var ae = sd.SubjectEssences.Select( _ => _.Entity.Ancestors().ToArray() ).ToArray();
                        if ( stack.Any
                                ( 
                                _sd => _sd.SubjectEssences.Any
                                    ( 
                                    e => ae.Any( a => a.Contains( e.Entity ) )
                                    ) 
                                ) 
                           )
                        {
                            continue;
                        }
                        #endregion
                    }

                    result.Add( sd );

                    if ( !stack.Any() )
                        break;
                    #endregion
                }
                return (result);
            }
            return (subjectDataList);            
        }

        public static void SetAttributeISSUBJECTforAllEntity( this IEnumerable< SubjectData > subjectData, int directAndIndirectSpeechGlobalNumber )
        {
            foreach ( var sd in subjectData )
            {
                sd.SetAttributeISSUBJECTforAllEntity( directAndIndirectSpeechGlobalNumber );
            }            
        }
        public static int GetAttributeIDForce( this IEnumerable< SubjectData > subjectData )
        {
            return (subjectData.First().FirstEntity().GetAttributeIDForce());
        }

        public static bool HasAnySubjectEntityAncestorInAnySubsents( this SubjectData subjectData, IEnumerable< XElement > subsents )
        {
            foreach ( var se in subjectData.SubjectEssences )
            {
                if ( se.Entity.Ancestors().Intersect( subsents ).Any() )
                {
                    return (true);
                }
            }
            return (false);
        }
        public static bool HasAnySubjectEntityAncestorInAnySubsents( this IEnumerable< SubjectData > subjectData, IEnumerable< XElement > subsents )
        {
            foreach ( var sd in subjectData )
            {
                if ( sd.HasAnySubjectEntityAncestorInAnySubsents( subsents ) )
                {
                    return (true);
                }
            }
            return (false);
        }
    }
    //===================================================//
}