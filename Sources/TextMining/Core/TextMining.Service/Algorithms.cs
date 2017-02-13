using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Xml.Linq;

using Linguistics.Coreference;
using Lingvistics;
using OpinionMining;
using TextMining.Core;

#if WITH_INPROC_LINGVISTIC
using BlockAttribute  = Lingvistics.BlockAttribute;
using EntityRole      = Lingvistics.EntityRole;
using EntityType      = Lingvistics.EntityType;
using NodeName        = Lingvistics.NodeName;
#else
using BlockAttribute  = Linguistics.Core.BlockAttribute;
using EntityRole      = Linguistics.Core.EntityRole;
using EntityType      = Linguistics.Core.EntityType;
using NodeName        = Linguistics.Core.UnitTextType;
using SubSentenceType = Linguistics.Core.SubSentenceType;
#endif

namespace TextMining.Service
{
    /// <summary>
    /// 
    /// </summary>
    public class InquiriesSynonymsInfo
    {
        public InquiriesSynonymsInfo( List< string > inquiriesSynonyms )
        {
            if ( !inquiriesSynonyms.IsEmptyOrNull() ) 
            {
                InquiriesSynonyms = new HashSet< string >( inquiriesSynonyms );

                var lst = new List< string[] >();
                foreach ( var inquiriesSynonym in InquiriesSynonyms )
                {
                    lst.Add( inquiriesSynonym.SplitBySpace() );
                }
                InquiriesSynonymsByWords = lst;
            }
        }

        public HashSet< string > InquiriesSynonyms
        {
            get;
            private set;
        }
        public IEnumerable< string[] > InquiriesSynonymsByWords
        {
            get;
            private set;
        }

        public bool HasInquiriesSynonyms
        {
            get { return (InquiriesSynonyms != null); }
        }
    }

    /// <summary>
    /// 
    /// </summary>
    public static class Algorithms
    {
        public static bool IsObjectEntity( this XElement entity )
        {
            if ( entity.IsElementENTITY() &&
                 entity.GetAttributeTYPE().IsObjectType() && 
                !entity.HasAttributeNOTTHEME()
               )
            {
                return (true);
            }
            return (false); 
        }
        /// <summary>
        /// В лингвистике появились новые типы: 
        /// TYPE="Event"
        /// TYPE="Prod"
        /// 1. Они уже определены в тональности, но пока они не отбираются в качестве объекта тональности. Их нужно подключить к спуску отбираемых сущностей.
        /// Пример: 
        ///   Около 83 % ДТП по-прежнему происходят по вине водителей, которые грубо нарушают Правила дорожного движения.
        /// ДТП TYPE="Event"
        /// 2. И еще, нужно исключить из отбираемых типов сущностей TYPE ="OrgAbbr"
        /// </summary>
        public static bool IsObjectType( this TypeAttributeValue? type )
        {
            if ( type.HasValue )
            {
                switch ( type )
                {
                    case TypeAttributeValue.PersonInfo:
                    case TypeAttributeValue.PersonName:
                    case TypeAttributeValue.PersonNoInfo:
                    case TypeAttributeValue.OrgName: 
                    case TypeAttributeValue.OrgNoInfo:
                    case TypeAttributeValue.UnknownProper:
                    case TypeAttributeValue.Event:
                    case TypeAttributeValue.Product:
                        return (true);
                }
            }
            return (false); 
        }

        private static readonly HashSet< string > SubjAndPronounStopWords = new HashSet< string >(
            new[] { "сколько", "столько", "всякий", "любой", "каждый", "несколько", "нисколько", "никакой", "ничей" }
        );
        private static bool IsObjectPronoun( this XElement entity )
        {
            if ( entity.HasTypeAttributeValue( TypeAttributeValue.Pronoun ) /*&& !entity.HasAttributeNOTTHEME()*/ )
            {
                return (!SubjAndPronounStopWords.Contains( entity.GetAttributeVALUE().ToLowerEx() ));
            }
            return (false);
        }
        private static bool IsObjectSubj( this XElement entity )
        {
            if ( entity.HasRoleAttributeValue( RoleAttributeValue.Subj ) /*&& !entity.HasAttributeNOTTHEME()*/ )
            {
                return (!SubjAndPronounStopWords.Contains( entity.GetAttributeVALUE().ToLowerEx() ));
            }
            return (false);
        }
        private static bool IsObjectObj( this XElement entity )
        {
            if ( entity.HasRoleAttributeValue( RoleAttributeValue.Obj ) /*&& !entity.HasAttributeNOTTHEME()*/ )
            {
                return (!SubjAndPronounStopWords.Contains( entity.GetAttributeVALUE().ToLowerEx() ));
            }
            return (false);
        }
        /*private static bool IsObjectObjWithoutAttributeNOTTHEME(this XElement entity)
        {
            if ( entity.HasRoleAttributeValue( RoleAttributeValue.Obj ) && !entity.HasAttributeNOTTHEME() )
            {
                return (!SubjAndPronounStopWords.Contains( entity.GetAttributeVALUE().ToLowerEx() ));
            }
            return (false);
        }*/

        private static bool IsObjectEntity4Homogeneous( this XElement entity, ObjectAllocateMethod objectAllocateMethod )
        {
            if ( entity.IsElementENTITY() &&
                 entity.GetAttributeTYPESAVEDorTYPE().IsObjectType() &&
                !entity.HasAttributeNOTTHEME()
               )
            {
                return (true);
            }

            #region [.FirstEntityWithRoleSubj & FirstEntityWithTypePronoun.]
            if ( (objectAllocateMethod & ObjectAllocateMethod.FirstEntityWithRoleSubj) == ObjectAllocateMethod.FirstEntityWithRoleSubj )
            {
                #region [.FirstEntityWithRoleSubj.]
                if ( entity.IsElementENTITY() &&
                     entity.IsObjectSubj()
                   )
                {
                    return (true);
                }
                #endregion

                if ( (objectAllocateMethod & ObjectAllocateMethod.FirstEntityWithTypePronoun) == ObjectAllocateMethod.FirstEntityWithTypePronoun )
                {
                    #region [.FirstEntityWithTypePronoun.]
                    //if ( subsent.GetAttributeSUBSENTTYPE() != SubsentenceType.Introductory )
                    {                        
                        if ( entity.IsObjectPronoun() )
                        {
                            return (true);
                        }
                    }
                    #endregion

                    if ( (objectAllocateMethod & ObjectAllocateMethod.FirstVerbEntityWithRoleObj) == ObjectAllocateMethod.FirstVerbEntityWithRoleObj )
                    {
                        #region [.FirstVerbEntityWithRoleObj.]
                        var subsent = entity.GetAncestorSubsent();
                        if ( (subsent != null) && 
                                subsent.ElementsVerb().Any( verb => 
                                        verb.HasTypeAttributeValue( TypeAttributeValue.Verb ) &&
                                        verb.MIhasCharAtPosition( 'P', 6 )
                                    ) 
                            )
                        {
                            if ( entity.IsObjectObj() )
                            {
                                return (true);
                            } 
                        }
                        #endregion
                    }
                }
            }
            #endregion

            return (false); 
        }
        public static HashSet< XElement > GetObjectsFromHomogeneous( this XElement element
                                                                     , ObjectAllocateMethod objectAllocateMethod
                                                                     , HashSet< XElement >  result = null )
        {
            if ( result == null )
                 result = new HashSet< XElement >();
            else 
                 result.Clear();

            foreach ( var d in element.Descendants() )
            {
                if ( !d.HasAttributeVALUE() && 
                     !d.HasAttributeVALUETHEME() )
                    continue;

                if ( result.ContainsAny( d.Ancestors() ) )
                    continue;

                if ( d.IsObjectEntity4Homogeneous( objectAllocateMethod ) )
                    result.Add( d );
            }

            if ( result.Count == 0 )
            {
                result.Add( element );
            }

            return (result);
        }
        public static XElement GetObjectBySynonyms( this XElement element, InquiriesSynonymsInfo isi )
        {
            if ( (isi == null) || !isi.HasInquiriesSynonyms )
                throw (new ArgumentException("isi"));

            var entity = element.DescendantsAndSelfEntity()
                                .GetPotentialInquiryEntity2( isi );

            return (entity);
        }
        public static ReadOnlyCollection< EssenceItem > ConstructEssenceItems4Homogeneous
            ( IEnumerable< XElement > elements, ICoreferenceInfo coreferenceInfo )
        {
            var lst = new List< EssenceItem >();

            foreach ( var e in elements )
            {
                var type = e.GetAttributeTYPESAVEDorTYPE();
                if ( !type.HasValue ) throw (new ArgumentNullException("type"));

                var name = coreferenceInfo.TryGetMasterName( e ) ?? e.GetVALUETHEMEorVALUE();
                lst.Add( new EssenceItem( name, e.GetAttributePROF(), type.Value ) );
            }  

            return (lst.AsReadOnly());
        }
        public static bool IsHomogeneous( Essence essence )
        {
            if ( (essence.EssenceItems.Count == 1) && 
                 (essence.EssenceItems[ 0 ].Type == EntityType.Homogeneous) )
            {
                return (true);
            }
            return (false);
        }

        private static XElement GetPotentialInquiryEntity( this IEnumerable< XElement > descendantsAndSelfEntity, InquiriesSynonymsInfo isi )
        {
            foreach ( var entity in descendantsAndSelfEntity )
            {
                var type = entity.GetAttributeTYPE();
                if ( type.HasValue )
                {
                    switch ( type.Value )
                    {
                        case TypeAttributeValue.Adverb:
                        case TypeAttributeValue.Adjective:
                        case TypeAttributeValue.Pronoun:
                        case TypeAttributeValue.NounVerb:
                        case TypeAttributeValue.OrgAbbr:
                        #region
                        {
                            /*
                            1. VALUE
                            */
                            var value = entity.GetAttributeVALUE().ToLowerEx();
                            if ( isi.InquiriesSynonyms.Contains( value ) )
                            {
                                return (entity);
                            }

                            if ( SearchForSubsequenceInValue( value, isi ) )
                            {
                                return (entity);
                            }
                        }
                        #endregion
                        break;

                        case TypeAttributeValue.Unknown:
                        case TypeAttributeValue.UnknownProper:
                        #region
                        {
                            /*
                            1. VALUE 
                            2. PRE+PROF
                            */

                            var value = entity.GetAttributeVALUE().ToLowerEx();
                            if ( isi.InquiriesSynonyms.Contains( value ) )
                            {
                                return (entity);
                            }

                            if ( SearchForSubsequenceInValue( value, isi ) )
                            {
                                return (entity);
                            }

                            if ( SearchForSubsequenceInPreProf( entity, isi ) )
                            {
                                return (entity);
                            }
                        }
                        #endregion
                        break;

                        case TypeAttributeValue.PersonName:
                        case TypeAttributeValue.PersonInfo:
                        #region
                        {
                            /*
                            1.	VALUE_THEME
                            2.	Комбинации: 
                                LN + FN + MN или 
                                FN + MN + LN или 
                                FN + MN или 
                                LN + FN или 
                                FN + LN или 
                                LN или
                                FN
                            3.	VALUE
                            */

                            //value-theme
                            var valueTheme = entity.GetAttributeVALUETHEME().ToLowerEx();
                            if ( isi.InquiriesSynonyms.Contains( valueTheme ) )
                            {
                                return (entity);
                            }

                            //LN-FN-MN
                            var ln = entity.GetAttributeLN().ToLowerEx();
                            var fn = entity.GetAttributeFN().ToLowerEx();
                            var mn = entity.GetAttributeMN().ToLowerEx();
                            var ln_fn = (ln + ' ' + fn).Trim();
                            var fn_mn = (fn + ' ' + mn).Trim();
                            if ( isi.InquiriesSynonyms.Contains( fn_mn )                     ||
                                 isi.InquiriesSynonyms.Contains( ln_fn )                     ||
                                 isi.InquiriesSynonyms.Contains( ln    )                     ||
                                 isi.InquiriesSynonyms.Contains( fn    )                     ||
                                 isi.InquiriesSynonyms.Contains( (ln_fn + ' ' + mn).Trim() ) ||
                                 isi.InquiriesSynonyms.Contains( (fn_mn + ' ' + ln).Trim() ) ||
                                 isi.InquiriesSynonyms.Contains( (fn    + ' ' + ln).Trim() )
                               )
                            {
                                return (entity);
                            }
                            var fn_ends_dot = (fn != null) ? fn.EndsWith(".") : false;
                            var mn_ends_dot = (mn != null) ? mn.EndsWith(".") : false;
                            if ( fn_ends_dot )
                            {
                                fn_mn = (fn + mn).Trim();
                                if ( isi.InquiriesSynonyms.Contains( fn_mn )               ||
                                     isi.InquiriesSynonyms.Contains( (ln_fn       + mn).Trim() ) ||
                                     isi.InquiriesSynonyms.Contains( (fn_mn + ' ' + ln).Trim() ) ||
                                     isi.InquiriesSynonyms.Contains( (fn          + ln).Trim() )
                                   )
                                {
                                    return (entity);
                                }

                                if ( mn_ends_dot )
                                {
                                    if ( isi.InquiriesSynonyms.Contains( (fn_mn + ln).Trim() ) )
                                    {
                                        return (entity);
                                    }
                                }
                            }
                            else if ( mn_ends_dot )
                            {
                                if ( isi.InquiriesSynonyms.Contains( (fn_mn + ln).Trim() ) )
                                {
                                    return (entity);
                                }
                            }

                            //value
                            var value = entity.GetAttributeVALUE().ToLowerEx();
                            if ( isi.InquiriesSynonyms.Contains( value ) )
                            {
                                return (entity);
                            }

                            if ( SearchForSubsequenceInValue( value, isi ) )
                            {
                                return (entity);
                            }
                        }
                        #endregion
                        break;

                        case TypeAttributeValue.PersonNoInfo:
                        case TypeAttributeValue.OrgName:
                        case TypeAttributeValue.OrgNoInfo:
                        case TypeAttributeValue.PlaceName:
                        case TypeAttributeValue.PlaceNoInfo:
                        case TypeAttributeValue.PseudoGeo:
                        case TypeAttributeValue.Event:
                        case TypeAttributeValue.Product:
                        #region
                        {
                            /*
                            1.	VALUE_THEME
                            2.	VALUE
                            3.	PRE+PROF
                            */

                            var valueTheme = entity.GetAttributeVALUETHEME().ToLowerEx();
                            if ( isi.InquiriesSynonyms.Contains( valueTheme ) )
                            {
                                return (entity);
                            }

                            var value = entity.GetAttributeVALUE().ToLowerEx();
                            if ( isi.InquiriesSynonyms.Contains( value ) )
                            {
                                return (entity);
                            }

                            if ( SearchForSubsequenceInValue( value, isi ) )
                            {
                                return (entity);
                            }

                            if ( SearchForSubsequenceInPreProf( entity, isi ) )
                            {
                                return (entity);
                            }
                        }
                        #endregion
                        break;
                    }
                }
            }

            return (null);
        }
        private static XElement GetPotentialInquiryEntity2( this IEnumerable< XElement > descendantsAndSelfEntity, InquiriesSynonymsInfo isi )
        {
            foreach ( var entity in descendantsAndSelfEntity )
            {
                var type = entity.GetAttributeTYPESAVEDorTYPE();
                if ( type.HasValue )
                {
                    switch ( type.Value )
                    {
                        case TypeAttributeValue.Adverb:
                        case TypeAttributeValue.Adjective:
                        case TypeAttributeValue.Pronoun:
                        case TypeAttributeValue.NounVerb:
                        case TypeAttributeValue.OrgAbbr:
                        #region
                        {
                            /*
                            1. VALUE
                            */
                            var value = entity.GetAttributeVALUE().ToLowerEx();
                            if ( isi.InquiriesSynonyms.Contains( value ) )
                            {
                                return (entity);
                            }

                            if ( SearchForSubsequenceInValue( value, isi ) )
                            {
                                return (entity);
                            }
                        }
                        #endregion
                        break;

                        case TypeAttributeValue.Unknown:
                        case TypeAttributeValue.UnknownProper:
                        #region
                        {
                            /*
                            1. VALUE 
                            2. PRE+PROF
                            */

                            var value = entity.GetAttributeVALUE().ToLowerEx();
                            if ( isi.InquiriesSynonyms.Contains( value ) )
                            {
                                return (entity);
                            }

                            if ( SearchForSubsequenceInValue( value, isi ) )
                            {
                                return (entity);
                            }

                            if ( SearchForSubsequenceInPreProf( entity, isi ) )
                            {
                                return (entity);
                            }
                        }
                        #endregion
                        break;

                        case TypeAttributeValue.PersonName:
                        case TypeAttributeValue.PersonInfo:
                        #region
                        {
                            /*
                            1.	VALUE_THEME
                            2.	Комбинации: 
                                LN + FN + MN или 
                                FN + MN + LN или 
                                FN + MN или 
                                LN + FN или 
                                FN + LN или 
                                LN или
                                FN
                            3.	VALUE
                            */

                            //value-theme
                            var valueTheme = entity.GetAttributeVALUETHEME().ToLowerEx();
                            if ( isi.InquiriesSynonyms.Contains( valueTheme ) )
                            {
                                return (entity);
                            }

                            //LN-FN-MN
                            var ln = entity.GetAttributeLN().ToLowerEx();
                            var fn = entity.GetAttributeFN().ToLowerEx();
                            var mn = entity.GetAttributeMN().ToLowerEx();
                            var ln_fn = (ln + ' ' + fn).Trim();
                            var fn_mn = (fn + ' ' + mn).Trim();
                            if ( isi.InquiriesSynonyms.Contains( fn_mn )                     ||
                                 isi.InquiriesSynonyms.Contains( ln_fn )                     ||
                                 isi.InquiriesSynonyms.Contains( ln    )                     ||
                                 isi.InquiriesSynonyms.Contains( fn    )                     ||
                                 isi.InquiriesSynonyms.Contains( (ln_fn + ' ' + mn).Trim() ) ||
                                 isi.InquiriesSynonyms.Contains( (fn_mn + ' ' + ln).Trim() ) ||
                                 isi.InquiriesSynonyms.Contains( (fn    + ' ' + ln).Trim() )
                               )
                            {
                                return (entity);
                            }
                            var fn_ends_dot = (fn != null) ? fn.EndsWith(".") : false;
                            var mn_ends_dot = (mn != null) ? mn.EndsWith(".") : false;
                            if ( fn_ends_dot )
                            {
                                fn_mn = (fn + mn).Trim();
                                if ( isi.InquiriesSynonyms.Contains( fn_mn )               ||
                                     isi.InquiriesSynonyms.Contains( (ln_fn       + mn).Trim() ) ||
                                     isi.InquiriesSynonyms.Contains( (fn_mn + ' ' + ln).Trim() ) ||
                                     isi.InquiriesSynonyms.Contains( (fn          + ln).Trim() )
                                   )
                                {
                                    return (entity);
                                }

                                if ( mn_ends_dot )
                                {
                                    if ( isi.InquiriesSynonyms.Contains( (fn_mn + ln).Trim() ) )
                                    {
                                        return (entity);
                                    }
                                }
                            }
                            else if ( mn_ends_dot )
                            {
                                if ( isi.InquiriesSynonyms.Contains( (fn_mn + ln).Trim() ) )
                                {
                                    return (entity);
                                }
                            }

                            //value
                            var value = entity.GetAttributeVALUE().ToLowerEx();
                            if ( isi.InquiriesSynonyms.Contains( value ) )
                            {
                                return (entity);
                            }

                            if ( SearchForSubsequenceInValue( value, isi ) )
                            {
                                return (entity);
                            }
                        }
                        #endregion
                        break;

                        case TypeAttributeValue.PersonNoInfo:
                        case TypeAttributeValue.OrgName:
                        case TypeAttributeValue.OrgNoInfo:
                        case TypeAttributeValue.PlaceName:
                        case TypeAttributeValue.PlaceNoInfo:
                        case TypeAttributeValue.PseudoGeo:
                        case TypeAttributeValue.Event:
                        case TypeAttributeValue.Product:
                        #region
                        {
                            /*
                            1.	VALUE_THEME
                            2.	VALUE
                            3.	PRE+PROF
                            */

                            var valueTheme = entity.GetAttributeVALUETHEME().ToLowerEx();
                            if ( isi.InquiriesSynonyms.Contains( valueTheme ) )
                            {
                                return (entity);
                            }

                            var value = entity.GetAttributeVALUE().ToLowerEx();
                            if ( isi.InquiriesSynonyms.Contains( value ) )
                            {
                                return (entity);
                            }

                            if ( SearchForSubsequenceInValue( value, isi ) )
                            {
                                return (entity);
                            }

                            if ( SearchForSubsequenceInPreProf( entity, isi ) )
                            {
                                return (entity);
                            }
                        }
                        #endregion
                        break;
                    }
                }
            }

            return (null);
        }
        private static bool SearchForSubsequenceInPreProf( XElement entity, InquiriesSynonymsInfo isi )
        {
            var pre  = entity.GetAttributePRE() .ToLowerEx();
            var prof = entity.GetAttributePROF().ToLowerEx();

            if ( pre != null && prof != null )
            {
                var array = (pre + ' ' + prof).SplitBySpace()
                                              .Select( v => v.Trim( '\'', ' ' ) )
                                              .ToArray();
                foreach ( var inquiriesSynonymByWords in isi.InquiriesSynonymsByWords )
                {
                    if ( array.SearchForSubsequence( inquiriesSynonymByWords ) )
                    {
                        return (true);
                    }
                }
            }
            return (false);
        }
        private static bool SearchForSubsequenceInValue( string value, InquiriesSynonymsInfo isi )
        {
            if ( value != null )
            {
                var array = value.SplitBySpace()
                                 .Select( v => v.Trim( '\'', ' ' ) )
                                 .ToArray();
                foreach ( var inquiriesSynonymByWords in isi.InquiriesSynonymsByWords )
                {
                    if ( array.SearchForSubsequence( inquiriesSynonymByWords ) )
                    {
                        return (true);
                    }
                }
            }
            return (false);
        }
        /*private static bool SearchForSubsequenceInValue( string adj, string value, HashSet< string > inquiriesSynonyms )
        {
            //если атрибут PRE содержит более одного слова, то выполнять поиск по подстроке VALUE 
            if ( (adj != null) && adj.Contains( ',' ) && (value != null) )
            {
                var valueArray = value.SplitBySpace()
                                      .Select( v => v.Trim( '\'' ) )
                                      .ToArray();

                foreach ( var inquiriesSynonym in inquiriesSynonyms )
                {
                    if ( valueArray.SearchForSubsequence( inquiriesSynonym.SplitBySpace() ) )
                    {
                        return (true);
                    }
                }
            }
            return (false);
        }*/

        public static IEnumerable< XElement > GetAllocateInquiries4Subsent( XElement              subsent, 
                                                                            ObjectAllocateMethod  objectAllocateMethod,
                                                                            InquiriesSynonymsInfo isi, 
                                                                            bool                  markAsInquiry )
        {subsent.ThrowIfNotSubsent();

            var inquiries = new List< XElement >();

            //object of monitoring is defined
            if ( (isi != null) && isi.HasInquiriesSynonyms )
            {
                #region
                var inquiryEntitiesSyn = from top_level_entity in subsent.ElementsEntity()
                                         let entity = top_level_entity.DescendantsAndSelfEntity()
                                                                      .GetPotentialInquiryEntity( isi )
                                         where entity.IsNotNull()
                                         select entity;

                foreach ( var inquiryEntity in inquiryEntitiesSyn )
                {
                    if ( markAsInquiry ) 
                    {
                        inquiryEntity.MarkAsInquiry();
                    }
                    inquiries.Add( inquiryEntity );
                } 
                #endregion

                //if has-inquiries-synonyms - return what founded!
                return (inquiries);
            }

            #region [.object of monitoring NOT defined.]
            var inquiryEntities = from top_level_entity in subsent.ElementsEntity()
                                  let entity = top_level_entity.DescendantsAndSelfEntity()
                                                               .FirstOrDefault( e => e.IsObjectEntity() )
                                  where entity.IsNotNull()
                                  select entity;
            foreach ( var inquiryEntity in inquiryEntities )
            {
                if ( markAsInquiry ) 
                {
                    inquiryEntity.MarkAsInquiry();
                }
                inquiries.Add( inquiryEntity );
            }
            #endregion

            #region [.Дополнение !!!!.]
            //Если ОМ не нашелся в подпредложении, то ОМ считать первый <ENTITY ROLE="Subj">
            if ( !inquiries.Any() && ((objectAllocateMethod & ObjectAllocateMethod.FirstEntityWithRoleSubj) == ObjectAllocateMethod.FirstEntityWithRoleSubj) )
            {
                #region [.FirstEntityWithRoleSubj.]
                var inquiryEntity = (from top_level_entity in subsent.ElementsEntity()
                                     from entity in top_level_entity.DescendantsAndSelfEntity()
                                     where entity.IsObjectSubj()
                                     select entity
                                    ).FirstOrDefault();
                if ( inquiryEntity != null )
                {
                    if ( markAsInquiry )
                    {
                        inquiryEntity.SetAttributeITSFIRSTENTITYWITHROLESUBJ();
                        inquiryEntity.MarkAsInquiry();
                    }
                    inquiries.Add( inquiryEntity );
                } 
                #endregion

                //случае отсутствия сущностей и подлежащего (Subj) объектом тональности считать местоимение ENTITY[@TYPE=pronoun]. 
                //(не нужно искать в подпредложениях типа SUB_SENT TYPE="Introductory")
                if ( !inquiries.Any() && (objectAllocateMethod & ObjectAllocateMethod.FirstEntityWithTypePronoun) == ObjectAllocateMethod.FirstEntityWithTypePronoun )
                {
                    #region [.FirstEntityWithTypePronoun.]
                    if ( subsent.GetAttributeSUBSENTTYPE() != SubsentenceType.Introductory )
                    {                        
                        inquiryEntity = (from top_level_entity in subsent.ElementsEntity()
                                         from entity in top_level_entity.DescendantsAndSelfEntity()
                                         where entity.IsObjectPronoun()
                                         select entity
                                        ).FirstOrDefault();
                        if ( inquiryEntity != null )
                        {
                            if ( markAsInquiry )
                            {
                                inquiryEntity.SetAttributeITSFIRSTENTITYWITHTYPEPRONOUN();
                                inquiryEntity.MarkAsInquiry();
                            }
                            inquiries.Add( inquiryEntity );
                        }
                    }
                    #endregion

                    //В случае, если ничего не сработало, т.е. ОТ по правилам не найден, но в подпредложении есть VERB@TYPE=VERB, то смотрим у него шестое значение MI=*****А/P. 
                    //Если MI==P, то в качестве ОТ брать первое ENTITY@ROLE=OBJ
                    if ( !inquiries.Any() && (objectAllocateMethod & ObjectAllocateMethod.FirstVerbEntityWithRoleObj) == ObjectAllocateMethod.FirstVerbEntityWithRoleObj )
                    {
                        #region [.FirstVerbEntityWithRoleObj.]
                        if ( subsent.ElementsVerb().Any( verb => 
                                        verb.HasTypeAttributeValue( TypeAttributeValue.Verb ) &&
                                        verb.MIhasCharAtPosition( 'P', 6 )
                                    ) 
                           )
                        {
                            inquiryEntity = (from top_level_entity in subsent.ElementsEntity()
                                             from entity in top_level_entity.DescendantsAndSelfEntity()
                                             where entity.IsObjectObj()
                                             select entity
                                            ).FirstOrDefault();
                            if ( inquiryEntity != null )
                            {
                                if ( markAsInquiry )
                                {
                                    inquiryEntity.SetAttributeITSFIRSTVERBENTITYWITHROLEOBJ();
                                    inquiryEntity.MarkAsInquiry();
                                }
                                inquiries.Add( inquiryEntity );
                            } 
                        }
                        #endregion
                    }
                }
            }
            #endregion

            return (inquiries);
        }
        private static XElement GetAllocateInquiry4Subsent( XElement              subsent, 
                                                            ObjectAllocateMethod  objectAllocateMethod,
                                                            InquiriesSynonymsInfo isi, 
                                                            bool                  markAsInquiry )
        {subsent.ThrowIfNotSubsent();

            //object of monitoring is defined
            if ( (isi != null) && isi.HasInquiriesSynonyms )
            {
                #region
                var inquiryEntitiesSyn = from top_level_entity in subsent.ElementsEntity()
                                         let entity = top_level_entity.DescendantsAndSelfEntity()
                                                                      .GetPotentialInquiryEntity( isi )
                                         where (entity != null)
                                         select entity;

                var inquiryEntity = inquiryEntitiesSyn.FirstOrDefault();
                if ( inquiryEntity != null )
                {
                    if ( markAsInquiry )
                    {
                        inquiryEntity.MarkAsInquiry();
                    }
                    return (inquiryEntity);
                }
                #endregion

                //if has-inquiries-synonyms and no found from they - no defined inquiry - return NULL!
                return (null);
            }

            #region [.object of monitoring NOT defined.]
            var inquiryEntities = (from top_level_entity in subsent.ElementsEntity()
                                   let entity = top_level_entity.DescendantsAndSelfEntity()
                                                                .FirstOrDefault( e => e.IsObjectEntity() )
                                   where (entity != null)
                                   select entity
                                  ).ToArray();

            if ( inquiryEntities.Length != 0 )
            {
                var inquiryEntity = inquiryEntities.FirstOrDefault( e => e.IsObjectSubj() && !e.HasAttributeNOTTHEME() )
                                    ??
                                    inquiryEntities.FirstOrDefault( e => e.IsObjectSubj() )
                                    ?? 
                                    inquiryEntities.FirstOrDefault();

                if ( markAsInquiry ) 
                {
                    inquiryEntity.MarkAsInquiry();
                }
                return (inquiryEntity);
            }
            #endregion

            #region [.Дополнение !!!!.]
            //Если ОМ не нашелся в подпредложении, то ОМ считать первый <ENTITY ROLE="Subj">
            if ( (objectAllocateMethod & ObjectAllocateMethod.FirstEntityWithRoleSubj) == ObjectAllocateMethod.FirstEntityWithRoleSubj )
            {
                #region [.FirstEntityWithRoleSubj.]
                var inquiryEntity = (from top_level_entity in subsent.ElementsEntity()
                                     from entity in top_level_entity.DescendantsAndSelfEntity()
                                     where entity.IsObjectSubj()
                                     select entity
                                    ).FirstOrDefault();
                if ( inquiryEntity != null )
                {
                    if ( markAsInquiry )
                    {
                        inquiryEntity.SetAttributeITSFIRSTENTITYWITHROLESUBJ();
                        inquiryEntity.MarkAsInquiry();
                    }
                    return (inquiryEntity);
                } 
                #endregion

                //случае отсутствия сущностей и подлежащего (Subj) объектом тональности считать местоимение ENTITY[@TYPE=pronoun]. 
                //(не нужно искать в подпредложениях типа SUB_SENT TYPE="Introductory")
                if ( (objectAllocateMethod & ObjectAllocateMethod.FirstEntityWithTypePronoun) == ObjectAllocateMethod.FirstEntityWithTypePronoun )
                {
                    #region [.FirstEntityWithTypePronoun.]
                    if ( subsent.GetAttributeSUBSENTTYPE() != SubsentenceType.Introductory )
                    {
                        inquiryEntity = (from top_level_entity in subsent.ElementsEntity()
                                         from entity in top_level_entity.DescendantsAndSelfEntity()
                                         where IsObjectPronoun( entity )
                                         select entity
                                        ).FirstOrDefault();
                        if ( inquiryEntity != null )
                        {
                            if ( markAsInquiry )
                            {
                                inquiryEntity.SetAttributeITSFIRSTENTITYWITHTYPEPRONOUN();
                                inquiryEntity.MarkAsInquiry();
                            }
                            return (inquiryEntity);
                        }
                    }
                    #endregion

                    //В случае, если ничего не сработало, т.е. ОТ по правилам не найден, но в подпредложении есть VERB@TYPE=VERB, то смотрим у него шестое значение MI=*****А/P. 
                    //Если MI==P, то в качестве ОТ брать первое ENTITY@ROLE=OBJ
                    if ( (objectAllocateMethod & ObjectAllocateMethod.FirstVerbEntityWithRoleObj) == ObjectAllocateMethod.FirstVerbEntityWithRoleObj )
                    {
                        #region [.FirstVerbEntityWithRoleObj.]
                        if ( subsent.ElementsVerb().Any( verb => 
                                        verb.HasTypeAttributeValue( TypeAttributeValue.Verb ) &&
                                        verb.MIhasCharAtPosition( 'P', 6 )
                                    ) 
                           )
                        {
                            inquiryEntity = (from top_level_entity in subsent.ElementsEntity()
                                             from entity in top_level_entity.DescendantsAndSelfEntity()
                                             where entity.IsObjectObj()
                                             select entity
                                            ).FirstOrDefault();
                            if ( inquiryEntity != null )
                            {
                                if ( markAsInquiry )
                                {
                                    inquiryEntity.SetAttributeITSFIRSTVERBENTITYWITHROLEOBJ();
                                    inquiryEntity.MarkAsInquiry();
                                }
                                return (inquiryEntity);
                            } 
                        }
                        #endregion
                    }
                }                
            }
            #endregion

            return (null);
        }
        private static bool AllocateInquiry4Subsent( XElement              subsent,                                                      
                                                     ObjectAllocateMethod  objectAllocateMethod,
                                                     InquiriesSynonymsInfo isi )
        {
            var inquiryEntity = GetAllocateInquiry4Subsent( subsent, objectAllocateMethod, isi, true );

            return (inquiryEntity != null); 
        }        
        public static bool AllocateInquiry4Sent( XElement sent, ObjectAllocateMethod objectAllocateMethod, InquiriesSynonymsInfo isi )
        {sent.ThrowIfNotSent();
            
            var inquiryFoundInSent = false; 

            var subsents = sent.GetSubsents4AllocateInquiry();
            foreach ( var subsent in subsents )
            {
                var inquiryFoundInSubsent = AllocateInquiry4Subsent( subsent, objectAllocateMethod, isi );
                if ( inquiryFoundInSubsent )
                {
                    inquiryFoundInSent = true;
                }                    
            }

            return (inquiryFoundInSent);
        }

        public static IEnumerable< XElement > GetAnaphoriaOrCoreference( IEnumerable< XElement > subsents )
        {
            var anaphoriaOrcoreference = 
                    from subsent in subsents
                    where subsent.ElementsEntity().All( e => !e.IsTypeInquiry() )
                    let CETorENTITY = subsent.ElementsExceptT()
                        .FirstOrDefault( e => (e.IsElementENTITY() || e.IsElementCET()) 
                                                &&  e.GetAttributeLINK().HasValue )
                    where ( CETorENTITY != null )
                    let link = CETorENTITY.GetAttributeLINK()
                    where ( link.HasValue )
                    let linkedENTITY = subsent.FindEntityById_4Subsent( link.Value ) //subsent.Document.FindEntityById( link.Value )
                    where (linkedENTITY != null && linkedENTITY.IsTypeInquiry() )
                    select CETorENTITY;

            return (anaphoriaOrcoreference);
        }

        /// <summary>
        /// Use only over iterator, not .ToArray() or .ToList()!!!
        /// </summary>
        /// <param name="sent"></param>
        /// <returns></returns>
        public static IEnumerable< XElement > GetSubsents4AllocateInquiry( this XElement sent )
        {sent.ThrowIfNotSent();

            #region commented. previous
            //var subsents = from subsent in sent.DescendantsSubsentence()
            //               where ( !subsent.AttributeTypeAreEqual( SubsentenceType.Participial, 
            //                                                       SubsentenceType.DanglingParticiple ) )
            //               select subsent; 
            #endregion

            /*Нужно сделать одну проверку при выделении объекта мониторинга. 
                1. Если подпредложение имеет тип  Default  и имеет атрибут SUBJ="N", 
                    который ссылается на ID внешнего ENTITY (из другого SUB_SENT),  то 
                2. Посмотреть на наличие ОМ в подпредложении, на которое у него ссылка SUBJ="N" и 
                3. Если ОМ в том подпредложении есть, то в рассматриваемом (Default @SUBJ="N") 
                    - не искать объект мониторинга (пропускать на этом этапе).*/
            var subsents = (
                                from subsent in sent.DescendantsSubsentence()
                                where ( !subsent.AttributeTypeAreEqual( SubsentenceType.Participial, 
                                                                        SubsentenceType.DanglingParticiple ) 
                                            &&
                                        (!subsent.AttributeTypeAreEqual( SubsentenceType.Default ) 
                                            ||
                                        subsent.NotIsParticipialOrDanglingParticipleOrDefault())
                                    )
                                select subsent
                           )
                           .Concat
                           (
                                from subsent in sent.DescendantsSubsentence()
                                where ( subsent.AttributeTypeAreEqual( SubsentenceType.Default ) )
                                let a = subsent.GetIfIsParticipialOrDanglingParticipleOrDefault() 
                                where ( (a != null) && !a.Item2.GetAncestorSubsent().HasElementTypeInquiry() )
                                select subsent
                           );

            return (subsents);
        }
        public static Tuple< SubsentenceType, XElement > GetIfIsParticipialOrDanglingParticipleOrDefault( this XElement subsent )
        {subsent.ThrowIfNotSubsent();

            if ( subsent.AttributeTypeAreEqual( SubsentenceType.Participial,
                                                SubsentenceType.DanglingParticiple,
                                                SubsentenceType.Default ) )
            {
                var subj = subsent.AttributeSubjValue();
                if ( subj.HasValue )
                {
                    var subjEntity = subsent.FindEntityById_4Subsent( subj.Value );
                    if ( subjEntity.IsNotNull() && (subjEntity.GetAncestorSubsent() != subsent) )
                    {
                        return (new Tuple< SubsentenceType, XElement >( subsent.GetAttributeSUBSENTTYPE(), subjEntity ));
                    }
                }
            }
            return (null);
        }
        public static Tuple< SubsentenceType, XElement > GetIfIsSubordinateOrDefault( this XElement subsent )
        {subsent.ThrowIfNotSubsent();

            if ( subsent.GetAttributeSUBSENTTYPE() == SubsentenceType.Subordinate )
            {
                var linkEntityOrCet = subsent.GetLinkEntityOrCet();
                return (new Tuple< SubsentenceType, XElement >( SubsentenceType.Subordinate, linkEntityOrCet ));
            }
            if ( subsent.GetAttributeSUBSENTTYPE() == SubsentenceType.Default )
            {
                var linkEntityOrCet = subsent.GetLinkEntityOrCet();
                if ( linkEntityOrCet != null )
                {
                    return (new Tuple< SubsentenceType, XElement >( SubsentenceType.Default, linkEntityOrCet ));
                }
            }
            return (null);

            #region commneted
            /*
            if ( subsent.AttributeTypeAreEqual( SubsentenceType.Subordinate,
                                                SubsentenceType.Default ) )
            {
                var linkEntityOrCet = subsent.GetLinkEntityOrCet();
                if ( linkEntityOrCet != null )
                {
                    return (new Tuple< SubsentenceType, XElement >( subsent.GetAttributeTYPE2(), linkEntityOrCet ));
                }
            }
            return (null);*/
            #endregion
        }
        private static bool NotIsParticipialOrDanglingParticipleOrDefault( this XElement subsent )
        {
            return (subsent.GetIfIsParticipialOrDanglingParticipleOrDefault() == null);    
        }

        #region [.SUB_SENT[ @TYPE=('Participial' || 'DanglingParticiple') ] => SUB_SENT[ @TYPE='Default' ].]
        //Если границы SUB_SENT[ @TYPE='Default' ] == SUB_SENT[ @TYPE=('Participial' || 'DanglingParticiple') ],
        //то предложение делать SUB_SENT[ @TYPE='Default' ]

        private static void ChangeTypeOfSomeSubsents( XDocument xdoc )
        {
            var pdp_subsents
                      = (from subsent in xdoc.Descendants(TextMining.Core.XLinqExtensions.Consts.SUBSENT_ELEMENT)
                            where ( subsent.GetAttributeSUBSENTTYPE() == SubsentenceType.Default )

                            let elements = subsent.Elements().Take( 2 ).ToArray()
                            where ( elements.Length == 1 )

                            let pdp_subsent = elements.First()
                            where ( pdp_subsent.IsElementSUBSENT() )

                            let type = pdp_subsent.GetAttributeSUBSENTTYPE()
                            where ( type == SubsentenceType.Participial || type == SubsentenceType.DanglingParticiple )
                         select pdp_subsent
                        ).ToArray();

            foreach ( var pdp_subsent in pdp_subsents )
            {
                pdp_subsent.SetAttribute( 
                    TextMining.Core.XLinqExtensions.Consts.TYPE_ATTRIBUTE, 
                    SubsentenceType.Subordinate.ToString() 
                );
            }
        }
        #endregion

        #region [.Код Олега из Лингвистики.]
        private static IEnumerable<XElement> GetElementsAfterCurrent(XElement parentSubSent, XElement startSubSent)
        {
            bool isCollected = false;
			return GetElementsAfterCurrent(parentSubSent, startSubSent, ref isCollected);
        }
		private static IEnumerable<XElement> GetElementsAfterCurrent(XElement parentSubSent, XElement startSubSent, ref bool isCollected)
        {
            List<XElement> collection = new List<XElement>();
			foreach (XElement element in parentSubSent.Elements())
            {
                if (isCollected)
                    collection.Add(element);
                else if (element == startSubSent)
                    isCollected = true;
                else if (element.Name.IsEqual(NodeName.SUB_SENT))
					collection.AddRange(GetElementsAfterCurrent(element, startSubSent, ref isCollected));
            }
            return collection;
        }
        /// <summary>
        /// Переименование элементов разметки внутри сущностей
        /// </summary>
        /// <param name="sentence"></param>
        private static void RenameUnmarkingText( XElement sentence )
        {
            foreach ( XElement element in sentence.Descendants() )
            {
                if ( element.Name.IsEqual( NodeName.U ) && 
                    !element.Parent.Name.IsEqual( NodeName.SENT ) &&
                    !element.Parent.Name.IsEqual( NodeName.SUB_SENT ) &&
					!element.Parent.Attribute( BlockAttribute.TYPE ).IsEqual( EntityType.Homogeneous ))
                {
                    element.Name = NodeName.T.ToString();
                }
            }
        }

        private static bool IsEqual(this string str, object value)
        {
            return (value != null) && (str == value.ToString());
        }
        private static bool IsEmpty(this string str)
        {
            return string.IsNullOrEmpty(str);
        }
        private static bool IsEqual(this XName str, object value)
        {
            return str.ToString().IsEqual(value);
        }
        private static string Attribute(this XElement element, object attribute)
        {
            string value = "";
            if (element != null)
            {
                string attributeName = attribute.ToString();
                if (element.Attribute(attributeName) != null)
                    value = element.Attribute(attributeName).Value;
                else if (attributeName.IsEqual(BlockAttribute.PROF) || attributeName.IsEqual(BlockAttribute.FULLNAME))
                    value = element.Attribute(BlockAttribute.VALUE.ToString()).Value;
            }
            return value.Trim();
        }
        private static IEnumerable<XElement> Elements(this XElement element, NodeName name)
        {
            return element.Elements(name.ToString());
        }
        private static IEnumerable<XElement> Elements(this XDocument xdoc, NodeName name)
        {
            return xdoc.Elements(name.ToString());
        }
        private static void SetType(this XElement element, object type)
        {
            element.SetAttributeValue(BlockAttribute.TYPE.ToString(), type.ToString());
        }
        private static bool IsContainAttribute(this XElement element, BlockAttribute attribute)
        {
            return element.IsContainAttribute(attribute.ToString());
        }
        private static bool IsContainAttribute(this XElement element, string attribute)
        {
            bool contain = false;
            if (element != null)
                contain = (element.Attribute(attribute) != null);
            return contain;
        }
        private static void SetAttributeValue(this XElement element, object attributeName, object value)
        {
            element.SetAttributeValue(attributeName.ToString(), value);
        }
		public static int GetPosition(this XElement element)
		{
			return element.IsContainAttribute(BlockAttribute.POS) ? int.Parse(element.Attribute(BlockAttribute.POS)) : -1;
		}
		public static void UpdatePosition(this XElement element, int shift)
		{
			if (element.IsContainAttribute(BlockAttribute.POS))
				element.SetAttributeValue(BlockAttribute.POS.ToString(), element.GetPosition() + shift);
		}
		public static int GetEnd(this XElement element)
		{
			int border = element.GetPosition();
			if (border < 0)
				border = 0;
			XElement lastWithPos = element.Elements().LastOrDefault(_ => _.IsContainAttribute(BlockAttribute.POS));
			if (lastWithPos != null)
			{
				if (!element.Name.IsEqual(NodeName.SENT))
					border = 0;
				border += GetEnd(lastWithPos);
				foreach (XElement elem in lastWithPos.ElementsAfterSelf())
					border += elem.Value.Length;
			}
			else
				border += element.Value.Length;
			return border;
		}
        
		/// <summary>
		/// Переструктурирование XML для передачи в Тональность
		/// </summary>
		/// <param name="rdfXml"></param>
		/// <returns></returns>
		public static XDocument PreprocessRdf( string rdfXml )
		{
			var xdoc = XDocument.Parse( rdfXml );

			return (PreprocessRdf( xdoc ));
		}
		public static XDocument PreprocessRdf( XDocument xdoc )
		{
			int lastSubSentenceID = int.Parse(xdoc.Root.Descendants(NodeName.SUB_SENT.ToString()).Last().Attribute(BlockAttribute.ID));
			foreach (XElement sentence in xdoc.Root.Elements(NodeName.SENT).ToArray())
			// проход по предложениям
			{
				RestructuringDirectSpeech(sentence, ref lastSubSentenceID);
				RestructuringApproximatingForceps(sentence, sentence, ref lastSubSentenceID);
				RenameUnmarkingText(sentence);
			}

            //SUB_SENT[ @TYPE=('Participial' || 'DanglingParticiple') ] => SUB_SENT[ @TYPE='Default' ]
            ChangeTypeOfSomeSubsents( xdoc );

			return (xdoc);
		}
		/// <summary>
		/// Переструктурирование блоков текста в скобках
		/// </summary>
		/// <param name="sentence"></param>
		private static void RestructuringApproximatingForceps(XElement parentSentence, XElement parent, ref int lastSubSentenceID)
		{
			var approximatingForceps = GetFirstApproximatingForceps(parent);
			while (approximatingForceps != null)
			{
				var sentences = approximatingForceps.Elements(NodeName.SENT).ToArray();
				if (sentences.Length == 1)
				{
					XElement sent = sentences.First();
					ShiftElements(GetShiftedDescendantsElements(sent), sent.GetPosition());
					sent.AddBeforeSelf(sent.Elements());
					var beforeElements = sent.ElementsBeforeSelf().ToArray();
					var elements = beforeElements.Skip(beforeElements.Length - sent.Elements().Count()).ToArray();
					sent.Remove();
					foreach (var element in elements.Where(_ => _.Name.IsEqual(NodeName.SUB_SENT)))
						RestructuringApproximatingForceps(parentSentence, element, ref lastSubSentenceID);
					approximatingForceps = GetFirstApproximatingForceps(parent);
				}
				else
				{
					XElement first = sentences.First();
					/// открывающая скобка
					first.AddFirst(approximatingForceps.Elements().First());
					ShiftElements(GetShiftedDescendantsElements(first), 1);
					first.UpdatePosition(-1);
					/// закрывающая скобка
					sentences.Last().Add(approximatingForceps.Elements().Last());

					List<XElement> elements = approximatingForceps.ElementsAfterSelf().ToList();
					elements.Remove();
					IEnumerable<XElement> afterElements = GetElementsAfterCurrent(parentSentence, approximatingForceps);
					afterElements.Remove();
					approximatingForceps.Remove();

					XElement newSent = new XElement(NodeName.SENT.ToString());
					newSent.Add(GetNewSubSentence(elements, ref lastSubSentenceID));
					newSent.Add(GetNewSubSentence(afterElements, ref lastSubSentenceID));
					newSent.SetAttributeValue(BlockAttribute.LANG, parentSentence.Attribute(BlockAttribute.LANG));

					ShiftElements(sentences, parentSentence.GetPosition());
					int position = sentences.Last().GetEnd();
					newSent.SetAttributeValue(BlockAttribute.POS, position);
					ShiftElements(GetShiftedDescendantsElements(newSent), -(position - parentSentence.GetPosition()));

					parentSentence.AddAfterSelf(newSent);
					parentSentence.AddAfterSelf(sentences);
					int index = 0;
					foreach (XElement sent in parentSentence.ElementsAfterSelf().ToArray())
					{
						if (index >= sentences.Length)
							break;
						RestructuringApproximatingForceps(sent, sent, ref lastSubSentenceID);
						++index;
					}

					string sentenceSubjId = parentSentence.Attribute(BlockAttribute.SUBJ);
					if (!parentSentence.Elements().Any(_ => _.Attribute(BlockAttribute.ID).IsEqual(sentenceSubjId)))
						parentSentence.SetAttributeValue(BlockAttribute.SUBJ, null);

					parentSentence = newSent;
					approximatingForceps = GetFirstApproximatingForceps(newSent);
				}
			}
		}
		private static XElement GetFirstApproximatingForceps(XElement parent)
		{
			if (parent == null)
				return null;
			foreach (XElement element in parent.Elements())
			{
				if (element.Attribute(BlockAttribute.TYPE).IsEqual(SubSentenceType.ApproximatingForceps))
				{
					if (element.Elements().Any(_ => _.Name.IsEqual(NodeName.SENT)))
						return element;
				}
				else if (element.Name.IsEqual(NodeName.SUB_SENT))
				{
					var result = GetFirstApproximatingForceps(element);
					if (result != null)
						return result;
				}
			}
			return null;
		}
		private static void ShiftElements(IEnumerable<XElement> collection, int shift)
		{
			foreach (XElement element in collection)
				element.UpdatePosition(shift);
		}
		private static IEnumerable<XElement> GetShiftedDescendantsElements(XElement parentElement)
		{
			List<XElement> result = new List<XElement>();
			foreach (XElement element in parentElement.Elements())
			{
				result.Add(element);
				if (!element.Name.IsEqual(NodeName.SENT))
					result.AddRange(GetShiftedDescendantsElements(element));
			}
			return result;
		}
		private static XElement GetNewSubSentence(IEnumerable<XElement> elements, ref int lastSubSentenceID)
		{
			XElement result = null;
			if (elements.Any())
			{
				result = new XElement(NodeName.SUB_SENT.ToString());
				result.SetType(SubSentenceType.Default);
				result.SetAttributeValue(BlockAttribute.ID, ++lastSubSentenceID);
				result.Add(elements);
			}
			return result;
		}
		/// <summary>
		/// Перестркутурирование блоков прямой речи
		/// </summary>
		/// <param name="sentence"></param>
		private static void RestructuringDirectSpeech(XElement sentence, ref int lastSubSentenceID)
		{
			IEnumerable<XElement> directSpeechList =
			(from element in sentence.Descendants()
			 where element.Name.IsEqual(NodeName.SUB_SENT) &&
			 element.Attribute(BlockAttribute.TYPE).IsEqual(SubSentenceType.DirectSpeech)
			 select element).ToArray();
			foreach (XElement directSpeech in directSpeechList)
			/// проход по блокам прямой речи
			{
				XElement preSentParent = directSpeech.Ancestors().FirstOrDefault(_ => (_.Parent != null) && _.Parent.Name.IsEqual(NodeName.SENT));
				if (preSentParent != null)
				{
					XElement newSubSent = directSpeech.AncestorsAndSelf().First(_ => (_ == preSentParent) || _.ElementsBeforeSelf().Any());
					if (newSubSent == preSentParent)
					/// прямая речь идет в начале предложения
					{
						directSpeech.Remove();
						preSentParent.AddBeforeSelf(directSpeech);
					}
					else
					{
						newSubSent = null;
						List<XElement> elements = directSpeech.ElementsAfterSelf().ToList();
						elements.Remove();
						IEnumerable<XElement> afterElements = GetElementsAfterCurrent(preSentParent, directSpeech.Parent);
						directSpeech.Remove();
						afterElements.Remove();
						if (elements.Any() || afterElements.Any())
						{
							newSubSent = new XElement(NodeName.SUB_SENT.ToString());
							newSubSent.SetType(SubSentenceType.Default);
							newSubSent.SetAttributeValue(BlockAttribute.ID, ++lastSubSentenceID);
							newSubSent.Add(elements);
							newSubSent.Add(afterElements);
						}
						preSentParent.AddAfterSelf(directSpeech);
						directSpeech.AddAfterSelf(newSubSent);
					}
				}
			}
		}
		#endregion
	}
}
