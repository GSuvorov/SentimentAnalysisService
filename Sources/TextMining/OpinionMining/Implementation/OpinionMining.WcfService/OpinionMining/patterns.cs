using System;
using System.Linq;
using System.Xml.Linq;
using System.Collections.Generic;

using TextMining.Core;
using TextMining.Service;

namespace OpinionMining
{
    using OpinionMining.SubjectsVerbExtensions;
    using OpinionMining.DirectIndirectSpeechExtensions;

    /// <summary>
    /// <!-- ПАРЫ {VERB - ENTITY[ @SUBJECT ]} НЕ МЕНЯЮТСЯ МЕСТАМИ. АЛГОРИТМ ПОИСКА: АНАЛОГИЧНО ПОИСКУ КОЛЛОКАЦИЙ
    /// pattern1 ::= 
    /// <
    ///     <VERB TYPE="Verb" VALUE_FROM="Phrasis1" MI="A" />
    ///     <ENTITY SUBJECT="TRUE" MI="N" />
    /// >
    /// -->
    /// </summary>
    internal static class pattern1
    {
        private const string ENTITY_MI = "N";
        private const string VERB_MI   = "A";

        public static SubjectData GetSubjectData( XElement subsent, Language language, XElement breakSearchElement = null )
        {subsent.ThrowIfNotSubsent();

            return (GetSubjectData( subsent.ElementsExceptT(), language, breakSearchElement ));
        }
        public static SubjectData GetSubjectData( IEnumerable< XElement > elements, Language language, XElement breakSearchElement = null )
        {
            if ( elements.IsNotHasObject() && elements.IsNotInDirectSpeech() )
            {
            var subjectData = default(SubjectData);
            if ( elements.IsContainsSubjectData( language, out subjectData, DictionaryTypePhrasis1.Phrasis1, ENTITY_MI, VERB_MI, breakSearchElement ) )
                return (subjectData);
            }
            return (null);
        }
    }

    /// <summary>
    /// <!-- ПАРЫ НЕ МЕНЯЮТСЯ МЕСТАМИ. АЛГОРИТМ ПОИСКА: АНАЛОГИЧНО ПОИСКУ КОЛЛОКАЦИЙ
    /// pattern2 ::= 
    /// <
    ///     <!-intr_word1.txt->
    ///     <ENTITY SUBJECT="TRUE" MI="G" />
    /// >
    /// -->
    /// </summary>
    internal static class pattern2
    {
        private const string ENTITY_MI = "G";

        public static SubjectData GetSubjectData( XElement subsent, Language language, XElement breakSearchElement = null )
        {subsent.ThrowIfNotSubsent();

            return (GetSubjectData( subsent.ElementsExceptT(), language, breakSearchElement ));
        }
        public static SubjectData GetSubjectData( IEnumerable< XElement > elements, Language language, XElement breakSearchElement = null )
        {
            if ( elements.IsNotHasObject() && elements.IsNotInDirectSpeech() )
            {
            var subjectData = default(SubjectData);
            if ( elements.IsContainsSubjectData( language, out subjectData, DictionaryTypeMultiWords.IntrWords1, ENTITY_MI, breakSearchElement ) )
                return (subjectData);
            }
            return (null);
        }


        public static SubjectData GetSubjectData4Citation( XElement subsent, Language language )
        {subsent.ThrowIfNotSubsent();

            var elements = subsent.ElementsExceptT();

            if ( elements.IsNotHasObject() && elements.IsNotInDirectSpeech() )
            {
            var subjectData = default(SubjectData);
            if ( elements.IsContainsSubjectData( language, out subjectData, DictionaryTypeMultiWords.IntrWords1, ENTITY_MI, null, true ) )
                return (subjectData);
            }
            return (null);
        }
    }

    /// <summary>
    /// <!--  ПАРЫ НЕ МЕНЯЮТСЯ МЕСТАМИ
    /// pattern3 ::=
    /// <
    ///     <CET>с </CET>
    ///     <ENTITY SUBJECT="TRUE" MI="A"/>
    ///     <ENTITY>точка зрение</ENTITY>
    /// >
    /// -->
    /// 
    /// point-of-view - с точки зрения
    /// </summary>
    internal class pattern3_pointview
    {
        private static readonly pattern3_pointview _Instance = new pattern3_pointview();
        public static pattern3_pointview Instance
        {
            get { return (_Instance); }
        }
        protected pattern3_pointview()
        {
        }

        private static readonly string[] _CET_WORDS    = new[] { "с" };
        private static readonly string[] _ENTITY_WORDS = new[] { "точка зрение", "точка зрения" };
        private const string    _ENTITY_MI    = "A";

        protected virtual string[] CET_WORDS
        {
            get { return (_CET_WORDS); }
        }
        protected virtual string[] ENTITY_WORDS
        {
            get { return (_ENTITY_WORDS); }
        }
        protected virtual string   ENTITY_MI
        {
            get { return (_ENTITY_MI); }
        }

        private static string GetPREForVALUE( XElement entity )
        {
            var prof = entity.GetAttributePROF();
            if ( !prof.IsEmptyOrNull() )
                return (prof.ToLowerEx());

            return (entity.GetAttributeVALUE().ToLowerEx());
        }
        private static string[] GetDescendantsAndSelfAttributeVALUE( XElement element )
        {
            return (from e in element.DescendantsAndSelf()
                    where !e.IsElementT()
                    let value = GetPREForVALUE( e )
                    where !value.IsEmptyOrNull()
                    let value2 = value.TrimWhiteSpaces()
                    where !value2.IsEmptyOrNull()
                    select value2
                   )
                   .ToArray();
        }
        private static string[] GetDescendantsAttributeVALUE( XElement element )
        {
            return (from e in element.Descendants()
                    where !e.IsElementT()
                    let value = GetPREForVALUE( e )
                    where !value.IsEmptyOrNull()
                    let value2 = value.TrimWhiteSpaces()
                    where !value2.IsEmptyOrNull()
                    select value2
                   )
                   .ToArray();
        }

        private SubjectAndAnaphoriaEntityTuple GetTuple( XElement startElement, bool breakByQuote )
        {
            foreach ( var e in startElement.ElementsAfterSelfExceptT()
                                           .TakeWhile( _ => !_.IsElementVERBtypeVERB() && 
                                                            !_.IsBreakingPunctuationSign( breakByQuote ) ) 
                                           )
            {
                var result = e.GetSubjectAndAnaphoriaEntityTuple( ENTITY_MI );
                if ( result.IsNotNull() )
                    return (result);
            }
            return (null);
        }
        protected bool EntityCondition( string value )
        {
            return (ENTITY_WORDS.Any( _ => value.StartsWith( _ ) ));
        }
        protected bool IsFollowingCondition( XElement subjectEntity, bool breakByQuote )
        {
            if ( EntityCondition( GetDescendantsAttributeVALUE( subjectEntity ).JoinBySpace() ) )
                return (true);

            foreach ( var e in subjectEntity.ElementsAfterSelfExceptT()
                                            .TakeWhile( _ => !_.IsElementVERBtypeVERB() && 
                                                             !_.IsBreakingPunctuationSign( breakByQuote ) ) 
                                            )
            {
                if ( EntityCondition( GetDescendantsAndSelfAttributeVALUE( e ).JoinBySpace() ) )
                    return (true);
            }
            //var next = subjectEntity.GetNotEmptyElementAfterSelf();
            //if ( next.IsNotNull() &&
            //     EntityCondition( GetDescendantsAndSelfAttributeVALUE( next ).JoinBySpace() ) )
            //    return (true);

            return (false);
        }

        private SubjectData _GetSubjectData( XElement subsent, XElement breakSearchElement, bool breakByQuote )
        {subsent.ThrowIfNotSubsent();

            if ( subsent.IsSubsentHasObject() || subsent.IsSubsentInDirectSpeech() )
                return (null);

            var tuples = from it in subsent.Elements().TakeWhile( _ => _ != breakSearchElement )
                         where it.IsElementCET() && it.IsVALUEStartsWith( CET_WORDS )

                         let @saaet = GetTuple( it, breakByQuote )
                         where @saaet.IsNotNull()

                         where IsFollowingCondition( @saaet.Entity, breakByQuote )
                         select @saaet;

            var aarray = tuples.ToArray();
            if ( !aarray.Any() )
                return (null);

            var verbValue = ENTITY_WORDS.AnyEx() ? CET_WORDS.First() + ' ' + ENTITY_WORDS.First() : null;
            var subjectEssences = new List< SubjectEssence >();
            foreach ( var a in aarray )
            {
                subjectEssences.Add( verbValue.IsNotNull() ? new SubjectEssence( a, verbValue )
                                                           : new SubjectEssence( a ) );
            }

            var subjectData = new SubjectData( subjectEssences, aarray.First().Entity, aarray.Last().Entity );
            return (subjectData);

            #region commented
            /*var a = t.FirstOrDefault();
            if ( a == null )
                return (null);

            var subjectEssences = new[] { ENTITY_WORDS.AnyEx() ? new SubjectEssence( a, CET_WORDS.First() + ' ' + ENTITY_WORDS.First() ) 
                                                               : new SubjectEssence( a ) };
            var subjectData = new SubjectData( subjectEssences, a.Entity, a.Entity );
            return (subjectData);*/            
            #endregion
        }

        public virtual SubjectData GetSubjectData( XElement subsent, XElement breakSearchElement = null )
        {
            return (_GetSubjectData( subsent, breakSearchElement, false ));
        }
        public virtual SubjectData GetSubjectData4Citation( XElement subsent )
        {
            return (_GetSubjectData( subsent, null, true ));
        }
    }

    /// <summary>
    /// <!--  ПАРЫ НЕ МЕНЯЮТСЯ МЕСТАМИ
    /// pattern4 ::= 
    /// <
    ///     <CET>по </CET>
    ///     <ENTITY SUBJECT="TRUE" MI="A" />
    ///     <ENTITY>
    ///       <!-intr_word2.txt->
    ///     </ENTITY>
    /// >
    /// -->
    /// 
    /// in the opinion of - по мнению
    /// </summary>
    internal class pattern4_opinion : pattern3_pointview
    {
        private static readonly pattern4_opinion _Instance = new pattern4_opinion();
        public new static pattern4_opinion Instance
        {
            get { return (_Instance); }
        }
        protected pattern4_opinion()
        {
        }

        private static readonly string[] _CET_WORDS = new[] { "по" };

        protected override string[] CET_WORDS
        {
            get { return (_CET_WORDS); }
        }
        protected override string[] ENTITY_WORDS
        {
            get { return (_Language.DictionaryManager.IntrWords2); }
        }

        private Language _Language;

        public override SubjectData GetSubjectData( XElement subsent, XElement breakSearchElement = null )
        {
            throw new InvalidOperationException();
        }
        public SubjectData GetSubjectData( XElement subsent, Language language, XElement breakSearchElement = null )
        {
            _Language = language;

            return (base.GetSubjectData( subsent, breakSearchElement ));        
        }

        public override SubjectData GetSubjectData4Citation(XElement subsent)
        {
            throw new InvalidOperationException();
        }
        public SubjectData GetSubjectData4Citation( XElement subsent, Language language )
        {
            _Language = language;

            return (base.GetSubjectData4Citation( subsent ));        
        }
    }

    /// <summary>
    /// <!-- ПАРЫ НЕ МЕНЯЮТСЯ МЕСТАМИ
    /// pattern5 ::= 
    /// <
    ///     <CET>на </CET>
    ///     <ENTITY SUBJECT="TRUE" MI="A"/>
    ///     <ENTITY>взгляд</ENTITY>
    /// >
    /// -->
    /// 
    /// on glance - на взгляд
    /// </summary>
    internal class pattern5_onglance : pattern3_pointview
    {
        private static readonly pattern5_onglance _Instance = new pattern5_onglance();
        public new static pattern5_onglance Instance
        {
            get { return (_Instance); }
        }
        protected pattern5_onglance()
        {
        }

        private static readonly string[] _CET_WORDS    = new[] { "на" };
        private static readonly string[] _ENTITY_WORDS = new[] { "взгляд" };

        protected override string[] CET_WORDS
        {
            get { return (_CET_WORDS); }
        }
        protected override string[] ENTITY_WORDS
        {
            get { return (_ENTITY_WORDS); }
        }        
    }

    /// <summary>
    /// <!--  ПАРЫ {VERB/VERB - ENTITY[ @SUBJECT ]}  МЕНЯЮТСЯ МЕСТАМИ
    /// АЛГОРИТМ ПОИСКА: АНАЛОГИЧНО ПОИСКУ КОЛЛОКАЦИЙ С ПРОПУСКОМ СЛОВ
    ///   В СПИСКЕ КАЖДАЯ СТРОКА СОСТОИТ ИЗ ДВУХ ГЛАГОЛОВ, МЕЖДУ КОТОРЫМИ МОЖЕТ БЫТЬ 
    ///   ЛЮБОЕ КОЛ-ВО СЛОВ, КРОМЕ VERB@TYPE=VERB И ЗНАКОВ ПРЕПИНАНИЯ.
    ///   ИСКАТЬ ОБА ГЛАГОЛА С VERB@MI=A
    /// 
    /// pattern6 ::= 
    /// <        
    ///     <ENTITY SUBJECT="TRUE" MI="N" />
    ///     <VERB VALUE_FROM="Phrasis2" MI="A" />
    /// >
    /// -->
    /// </summary>
    internal static class pattern6
    {
        private const string ENTITY_MI = "N";
        private const string VERB_MI   = "A";

        public static SubjectData GetSubjectData( XElement subsent, Language language, XElement breakSearchElement = null )
        {subsent.ThrowIfNotSubsent();

            return (GetSubjectData( subsent.ElementsExceptT(), language, breakSearchElement ));
        }
        public static SubjectData GetSubjectData( IEnumerable< XElement > elements, Language language, XElement breakSearchElement = null )
        {
            if ( elements.IsNotHasObject() && elements.IsNotInDirectSpeech() )
            {
            var subjectData = default(SubjectData);
            if ( elements.IsContainsSubjectData( language, out subjectData, DictionaryTypePhrasis2.Phrasis2, ENTITY_MI, VERB_MI, breakSearchElement ) )
                return (subjectData);
            }
            return (null);
        }


        public static SubjectData GetSubjectData4Citation( XElement subsent, Language language )
        {subsent.ThrowIfNotSubsent();

            var elements = subsent.ElementsExceptT();

            if ( elements.IsNotHasObject() && elements.IsNotInDirectSpeech() )
            {
            var subjectData = default(SubjectData);
            if ( elements.IsContainsSubjectData( language, out subjectData, DictionaryTypePhrasis2.Phrasis2, ENTITY_MI, VERB_MI, null, true ) )
                return (subjectData);
            }
            return (null);
        }
    }

    /// <summary>
    /// <!--  ПАРЫ {VERB - ENTITY[ @SUBJECT ]} МЕНЯЮТСЯ МЕСТАМИ
    /// pattern7 ::= 
    /// <
    ///     <VERB TYPE="Verb" VALUE_FROM="VerbsOpinion2" MI="A" />
    ///     <ENTITY SUBJECT="TRUE" MI="N" />
    /// >
    /// -->
    /// </summary>
    internal static class pattern7
    {
        private const string ENTITY_MI = "N";
        private const string VERB_MI   = "A";

        public static SubjectData GetSubjectData( XElement subsent, Language language, XElement breakSearchElement = null )
        {subsent.ThrowIfNotSubsent();

            return (GetSubjectData( subsent.ElementsExceptT(), language, breakSearchElement ));
        }
        public static SubjectData GetSubjectData( IEnumerable< XElement > elements, Language language, XElement breakSearchElement = null )
        {
            if ( elements.IsNotHasObject() && elements.IsNotInDirectSpeech() )
            {
            var subjectData = default(SubjectData);
            if ( elements.IsContainsSubjectData( language, out subjectData, DictionaryTypeVerbs.VerbsOpinion2, ENTITY_MI, VERB_MI, breakSearchElement ) )
                return (subjectData);
            }
            return (null);
        }
    }

    /// <summary>
    /// <!--  ПАРЫ {VERB - CET+ENTITY[ @SUBJECT ]} МЕНЯЮТСЯ МЕСТАМИ
    /// pattern8 ::=
    /// <
    ///     <VERB TYPE="Verb" VALUE_FROM="VerbsOpinion2" MI="P" />
    ///     <CET>в </CET>
    ///     <ENTITY TYPE="OrgInfo || OrgName || OrgNoInfoOrgName || UnknownProper" SUBJECT="TRUE" MI="L"/>
    /// >
    /// -->
    /// </summary>
    internal static class pattern8
    {
        private const string ENTITY_MI = "L";
        private const string VERB_MI   = "P";

        public static SubjectData GetSubjectData( XElement subsent, Language language, XElement breakSearchElement = null )
        {subsent.ThrowIfNotSubsent();

            return (GetSubjectData( subsent.ElementsExceptT(), language, breakSearchElement ));
        }
        public static SubjectData GetSubjectData( IEnumerable< XElement > elements, Language language, XElement breakSearchElement = null )
        {
            if ( elements.IsNotHasObject() && elements.IsNotInDirectSpeech() )
            {
            var subjectData = default(SubjectData);
            if ( elements.IsContainsSubjectDataOrgNameWithB( language, out subjectData, DictionaryTypeVerbs.VerbsOpinion2, ENTITY_MI, VERB_MI, breakSearchElement ) )
                return (subjectData);
            }
            return (null);
        }
    }

    /// <summary>
    /// <!--  ПАРЫ {VERB - CET+ENTITY[ @SUBJECT ]} МЕНЯЮТСЯ МЕСТАМИ
    /// pattern9 ::=
    /// <
    ///     <VERB TYPE="Verb" VALUE_FROM="VerbsOpinion4" MI="A" />
    ///     <CET>в </CET>
    ///     <ENTITY TYPE="OrgInfo || OrgName || OrgNoInfoOrgName || UnknownProper" SUBJECT="TRUE" MI="N" />
    /// >
    /// --> 
    /// </summary>
    internal static class pattern9
    {
        private const string ENTITY_MI = "L";
        private const string VERB_MI   = "A";

        public static SubjectData GetSubjectData( XElement subsent, Language language, XElement breakSearchElement = null )
        {subsent.ThrowIfNotSubsent();

            return (GetSubjectData( subsent.ElementsExceptT(), language, breakSearchElement ));
        }
        public static SubjectData GetSubjectData( IEnumerable< XElement > elements, Language language, XElement breakSearchElement = null )
        {
            if ( elements.IsNotHasObject() && elements.IsNotInDirectSpeech() )
            {
            var subjectData = default(SubjectData);
            if ( elements.IsContainsSubjectDataOrgNameWithB( language, out subjectData, DictionaryTypeVerbs.VerbsOpinion4, ENTITY_MI, VERB_MI, breakSearchElement ) )
                return (subjectData);
            }
            return (null);
        }
    }

    /// <summary>
    /// <!--  ПАРЫ {VERB - CET+ENTITY/ENTITY[ @SUBJECT ]} МЕНЯЮТСЯ МЕСТАМИ
    /// между ENTITY/ENTITY может быть любое кол-во дочерних элементов ENTITY, T , VERB@TYPE=participle и ENTITY@subject=true, 
    /// находящихся в положении любой вложенности, не выше родительского. 
    /// pattern10 ::=
    /// <
    ///     <VERB TYPE="Verb" VALUE_FROM="VerbsOpinion2" MI="P" />
    ///     <CET>в </CET>
    ///     <ENTITY>
    ///      <ENTITY SUBJECT="TRUE" MI="G"/>
    ///    </ENTITY>
    /// >
    /// --> 
    /// </summary>
    internal static class pattern10
    {
        private const string ENTITY_MI = "G";
        private const string VERB_MI   = "P";

        public static SubjectData GetSubjectData( XElement subsent, Language language, XElement breakSearchElement = null )
        {subsent.ThrowIfNotSubsent();

            return (GetSubjectData( subsent.ElementsExceptT(), language, breakSearchElement ));
        }
        public static SubjectData GetSubjectData( IEnumerable< XElement > elements, Language language, XElement breakSearchElement = null )
        {
            if ( elements.IsNotHasObject() && elements.IsNotInDirectSpeech() )
            {
            var subjectData = default(SubjectData);
            if ( elements.IsContainsSubjectDataNestedWithB( language, out subjectData, DictionaryTypeVerbs.VerbsOpinion2, ENTITY_MI, VERB_MI, breakSearchElement ) )
                return (subjectData);
            }
            return (null);
        }
    }

    /// <summary>
    /// <!--  ПАРЫ {VERB - CET+ENTITY/ENTITY[ @SUBJECT ]} МЕНЯЮТСЯ МЕСТАМИ
    /// между ENTITY/ENTITY может быть любое кол-во дочерних элементов ENTITY, T , VERB@TYPE=participle и ENTITY@subject=true, 
    /// находящихся в положении любой вложенности, не выше родительского. 
    /// pattern11 ::=
    /// <
    ///     <VERB TYPE="Verb" VALUE_FROM="VerbsOpinion4" MI="A" />
    ///     <CET>в </CET>
    ///     <ENTITY>
    ///       <ENTITY SUBJECT="TRUE" MI="G"/>
    ///     </ENTITY>
    /// >
    /// --> 
    /// </summary>
    internal static class pattern11
    {
        private const string ENTITY_MI = "G";
        private const string VERB_MI   = "A";

        public static SubjectData GetSubjectData( XElement subsent, Language language, XElement breakSearchElement = null )
        {subsent.ThrowIfNotSubsent();

            return (GetSubjectData( subsent.ElementsExceptT(), language, breakSearchElement ));
        }
        public static SubjectData GetSubjectData( IEnumerable< XElement > elements, Language language, XElement breakSearchElement = null )
        {
            if ( elements.IsNotHasObject() && elements.IsNotInDirectSpeech() )
            {
            var subjectData = default(SubjectData);
            if ( elements.IsContainsSubjectDataNestedWithB( language, out subjectData, DictionaryTypeVerbs.VerbsOpinion4, ENTITY_MI, VERB_MI, breakSearchElement ) )
                return (subjectData);
            }
            return (null);
        }
    }

    /// <summary>
    /// <!--  ПАРЫ {VERB - ENTITY[ @SUBJECT ]}  МЕНЯЮТСЯ МЕСТАМИ
    /// <
    ///     pattern12 ::=
    ///     <VERB TYPE="Verb" VALUE_FROM="VerbsOpinion1" MI="A" />
    ///     <ENTITY SUBJECT="TRUE" MI="N" />
    /// >
    /// -->
    /// </summary>
    internal static class pattern12
    {
        private const string ENTITY_MI = "N";
        private const string VERB_MI   = "A";

        public static SubjectData GetSubjectData( XElement subsent, Language language, XElement breakSearchElement = null )
        {subsent.ThrowIfNotSubsent();

            return (GetSubjectData( subsent.ElementsExceptT(), language, breakSearchElement ));
        }
        public static SubjectData GetSubjectData( IEnumerable< XElement > elements, Language language, XElement breakSearchElement = null )
        {
            if ( elements.IsNotHasObject() && elements.IsNotInDirectSpeech() )
            {
            var subjectData = default(SubjectData);
            if ( elements.IsContainsSubjectData( language, out subjectData, DictionaryTypeVerbs.VerbsOpinion1, ENTITY_MI, VERB_MI, breakSearchElement ) )
                return (subjectData);
            }
            return (null);
        }

        public static SubjectData GetSubjectData4Citation( XElement subsent, Language language )
        {subsent.ThrowIfNotSubsent();

            var elements = subsent.ElementsExceptT();

            if ( elements.IsNotHasObject() && elements.IsNotInDirectSpeech() )
            {
            var subjectData = default(SubjectData);
            if ( elements.IsContainsSubjectData( language, out subjectData, DictionaryTypeVerbs.VerbsOpinion1, ENTITY_MI, VERB_MI, null, true ) )
                return (subjectData);
            }
            return (null);
        }
    }

    /// <summary>
    /// <!--  ПАРЫ {VERB - CET+ENTITY[ @SUBJECT ]} МЕНЯЮТСЯ МЕСТАМИ
    /// <
    ///     pattern13 ::=
    ///     <CET>в </CET>
    ///     <ENTITY TYPE="OrgInfo || OrgName || OrgNoInfoOrgName || UnknownProper" SUBJECT="TRUE" MI="L" />
    ///     <VERB TYPE="Verb" VALUE_FROM="VerbsOpinion1" MI="P" />
    /// >
    /// --> 
    /// </summary>
    internal static class pattern13
    {
        private const string ENTITY_MI = "L";
        private const string VERB_MI   = "P";

        public static SubjectData GetSubjectData( XElement subsent, Language language, XElement breakSearchElement = null )
        {subsent.ThrowIfNotSubsent();

            return (GetSubjectData( subsent.ElementsExceptT(), language, breakSearchElement ));
        }
        public static SubjectData GetSubjectData( IEnumerable< XElement > elements, Language language, XElement breakSearchElement = null )
        {
            if ( elements.IsNotHasObject() && elements.IsNotInDirectSpeech() )
            {
            var subjectData = default(SubjectData);
            if ( elements.IsContainsSubjectDataOrgNameWithB( language, out subjectData, DictionaryTypeVerbs.VerbsOpinion1, ENTITY_MI, VERB_MI, breakSearchElement ) )
                return (subjectData);
            }
            return (null);
        }

        public static SubjectData GetSubjectData4Citation( XElement subsent, Language language )
        {subsent.ThrowIfNotSubsent();

            var elements = subsent.ElementsExceptT();

            if ( elements.IsNotHasObject() && elements.IsNotInDirectSpeech() )
            {
            var subjectData = default(SubjectData);
            if ( elements.IsContainsSubjectDataOrgNameWithB( language, out subjectData, DictionaryTypeVerbs.VerbsOpinion1, ENTITY_MI, VERB_MI, null, true ) )
                return (subjectData);
            }
            return (null);
        }
    }

    /// <summary>
    /// <!--  ПАРЫ {VERB - CET+ENTITY[ @SUBJECT ]} МЕНЯЮТСЯ МЕСТАМИ
    /// <
    ///     pattern14 ::=
    ///     <CET>в </CET>
    ///     <ENTITY TYPE="OrgInfo || OrgName || OrgNoInfoOrgName || UnknownProper" SUBJECT="TRUE" MI="P" />
    ///     <VERB TYPE="Verb" VALUE_FROM="VerbsOpinion1" MI="L" />
    /// >
    /// --> 
    /// </summary>
    /*internal static class pattern14
    {
        private const string ENTITY_MI = "P";
        private const string VERB_MI   = "L";

        public static SubjectData GetSubjectData( XElement subsent, Language language, XElement breakSearchElement = null )
        {subsent.ThrowIfNotSubsent();

            return (GetSubjectData( subsent.ElementsExceptT(), language, breakSearchElement ));
        }
        public static SubjectData GetSubjectData( IEnumerable< XElement > elements, Language language, XElement breakSearchElement = null )
        {
            if ( elements.IsNotHasObject() && elements.IsNotInDirectSpeech() )
            {
            var subjectData = default(SubjectData);
            if ( elements.IsContainsSubjectDataOrgNameWithB( language, out subjectData, DictionaryTypeVerbs.VerbsOpinion1, ENTITY_MI, VERB_MI, breakSearchElement ) )
                return (subjectData);
            }
            return (null);
        }
    }*/

    /// <summary>
    /// <!--  ПАРЫ {VERB - CET+ENTITY[ @SUBJECT ]} МЕНЯЮТСЯ МЕСТАМИ
    /// <
    ///     pattern15 ::=
    ///     <CET>в </CET>
    ///     <ENTITY TYPE="OrgInfo || OrgName || OrgNoInfoOrgName || UnknownProper" SUBJECT="TRUE" MI="L" />
    ///     <VERB TYPE="Verb" VALUE_FROM="VerbsOpinion3" MI="A" />
    /// >
    /// --> 
    /// </summary>
    internal static class pattern15
    {
        private const string ENTITY_MI = "L";
        private const string VERB_MI   = "A";

        public static SubjectData GetSubjectData( XElement subsent, Language language, XElement breakSearchElement = null )
        {subsent.ThrowIfNotSubsent();

            return (GetSubjectData( subsent.ElementsExceptT(), language, breakSearchElement ));
        }
        public static SubjectData GetSubjectData( IEnumerable< XElement > elements, Language language, XElement breakSearchElement = null )
        {
            if ( elements.IsNotHasObject() && elements.IsNotInDirectSpeech() )
            {
            var subjectData = default(SubjectData);
            if ( elements.IsContainsSubjectDataOrgNameWithB( language, out subjectData, DictionaryTypeVerbs.VerbsOpinion3, ENTITY_MI, VERB_MI, breakSearchElement ) )
                return (subjectData);
            }
            return (null);
        }


        public static SubjectData GetSubjectData4Citation( XElement subsent, Language language )
        {subsent.ThrowIfNotSubsent();

            var elements = subsent.ElementsExceptT();

            if ( elements.IsNotHasObject() && elements.IsNotInDirectSpeech() )
            {
            var subjectData = default(SubjectData);
            if ( elements.IsContainsSubjectDataOrgNameWithB( language, out subjectData, DictionaryTypeVerbs.VerbsOpinion3, ENTITY_MI, VERB_MI, null, true ) )
                return (subjectData);
            }
            return (null);
        }
    }

    /// <summary>
    /// <!--  ПАРЫ {VERB - CET+ENTITY/ENTITY[ @SUBJECT ]} МЕНЯЮТСЯ МЕСТАМИ
    /// между ENTITY/ENTITY может быть любое кол-во дочерних элементов ENTITY, T , VERB@TYPE=participle и ENTITY@subject=true, 
    /// находящихся в положении любой вложенности, не выше родительского. 
    /// <
    ///     pattern16 ::=
    ///     <CET>в </CET>
    ///     <ENTITY>
    ///       <ENTITY SUBJECT="TRUE" MI="G"/>
    ///     </ENTITY>
    ///     <VERB TYPE="Verb" VALUE_FROM="VerbsOpinion1" MI="P" />
    /// >
    /// --> 
    /// </summary>
    internal static class pattern16
    {
        private const string ENTITY_MI = "G";
        private const string VERB_MI   = "P";

        public static SubjectData GetSubjectData( XElement subsent, Language language, XElement breakSearchElement = null )
        {subsent.ThrowIfNotSubsent();

            return (GetSubjectData( subsent.ElementsExceptT(), language, breakSearchElement ));
        }
        public static SubjectData GetSubjectData( IEnumerable< XElement > elements, Language language, XElement breakSearchElement = null )
        {
            if ( elements.IsNotHasObject() && elements.IsNotInDirectSpeech() )
            {
            var subjectData = default(SubjectData);
            if ( elements.IsContainsSubjectDataNestedWithB( language, out subjectData, DictionaryTypeVerbs.VerbsOpinion1, ENTITY_MI, VERB_MI, breakSearchElement ) )
                return (subjectData);
            }
            return (null);
        }


        public static SubjectData GetSubjectData4Citation( XElement subsent, Language language )
        {subsent.ThrowIfNotSubsent();

            var elements = subsent.ElementsExceptT();

            if ( elements.IsNotHasObject() && elements.IsNotInDirectSpeech() )
            {
            var subjectData = default(SubjectData);
            if ( elements.IsContainsSubjectDataNestedWithB( language, out subjectData, DictionaryTypeVerbs.VerbsOpinion1, ENTITY_MI, VERB_MI, null, true ) )
                return (subjectData);
            }
            return (null);
        }
    }

    /// <summary>
    /// <!--  ПАРЫ {VERB - CET+ENTITY/ENTITY[ @SUBJECT ]} МЕНЯЮТСЯ МЕСТАМИ
    /// между ENTITY/ENTITY может быть любое кол-во дочерних элементов ENTITY, T , VERB@TYPE=participle и ENTITY@subject=true, 
    /// находящихся в положении любой вложенности, не выше родительского. 
    /// <
    ///     pattern17 ::=
    ///     <CET>в </CET>
    ///     <ENTITY>
    ///       <ENTITY SUBJECT="TRUE" MI="G"/>
    ///     </ENTITY>
    ///     <VERB TYPE="Verb" VALUE_FROM="VerbsOpinion3" MI="A" />
    /// >
    /// --> 
    /// </summary>
    internal static class pattern17
    {
        private const string ENTITY_MI = "G";
        private const string VERB_MI   = "A";

        public static SubjectData GetSubjectData( XElement subsent, Language language, XElement breakSearchElement = null )
        {subsent.ThrowIfNotSubsent();

            return (GetSubjectData( subsent.ElementsExceptT(), language, breakSearchElement ));
        }
        public static SubjectData GetSubjectData( IEnumerable< XElement > elements, Language language, XElement breakSearchElement = null )
        {
            if ( elements.IsNotHasObject() && elements.IsNotInDirectSpeech() )
            {
            var subjectData = default(SubjectData);
            if ( elements.IsContainsSubjectDataNestedWithB( language, out subjectData, DictionaryTypeVerbs.VerbsOpinion3, ENTITY_MI, VERB_MI, breakSearchElement ) )
                return (subjectData);
            }
            return (null);
        }


        public static SubjectData GetSubjectData4Citation( XElement subsent, Language language )
        {subsent.ThrowIfNotSubsent();

            var elements = subsent.ElementsExceptT();

            if ( elements.IsNotHasObject() && elements.IsNotInDirectSpeech() )
            {
            var subjectData = default(SubjectData);
            if ( elements.IsContainsSubjectDataNestedWithB( language, out subjectData, DictionaryTypeVerbs.VerbsOpinion3, ENTITY_MI, VERB_MI, null, true ) )
                return (subjectData);
            }
            return (null);
        }
    }

    /// <summary>
    /// <!-- ПАРЫ {ENTITY[ @SUBJECT ] - VERB+ENTITY } МЕНЯЮТСЯ МЕСТАМИ. 
    /// pattern18 ::= 
    /// <
    ///     <ENTITY SUBJECT="TRUE" MI="N" />
    ///     <VERB TYPE="Verb" VALUE_FROM="VerbsOpinion5" MI="A" />
    ///     <ENTITY VALUE_FROM="EntityOpinion"/>
    /// >
    /// -->
    /// </summary>
    internal static class pattern18
    {
        private const string ENTITY_MI = "N";
        private const string VERB_MI   = "A";

        public static SubjectData GetSubjectData( XElement subsent, Language language, XElement breakSearchElement = null )
        {subsent.ThrowIfNotSubsent();

            return (GetSubjectData( subsent.ElementsExceptT(), language, breakSearchElement ));
        }
        public static SubjectData GetSubjectData( IEnumerable< XElement > elements, Language language, XElement breakSearchElement = null )
        {
            if ( elements.IsNotHasObject() && elements.IsNotInDirectSpeech() )
            {
                var subjectData = default(SubjectData);
                if ( elements.IsContainsSubjectEntityOpinion( language, out subjectData, DictionaryTypeVerbs.VerbsOpinion5, 
                                                              ENTITY_MI, VERB_MI, breakSearchElement ) )
                {
                    return (subjectData);
                }
            }
            return (null);
        }


        public static SubjectData GetSubjectData4Citation( XElement subsent, Language language )
        {subsent.ThrowIfNotSubsent();

            var elements = subsent.ElementsExceptT();

            if ( elements.IsNotHasObject() && elements.IsNotInDirectSpeech() )
            {
                var subjectData = default(SubjectData);
                if ( elements.IsContainsSubjectEntityOpinion( language, out subjectData, DictionaryTypeVerbs.VerbsOpinion5,
                                                              ENTITY_MI, VERB_MI, null, true ) )
                {
                    return (subjectData);
                }
            }
            return (null);
        }
    }

    /// <summary>
    /// <!-- ПАРЫ {ENTITY[ @SUBJECT ] - VERB+ENTITY } МЕНЯЮТСЯ МЕСТАМИ.
    /// pattern19 ::= 
    /// <
    ///     <ENTITY SUBJECT="TRUE" MI="I" />
    ///     <VERB TYPE="Verb" VALUE_FROM="VerbsOpinion5" MI="P" />
    ///     <ENTITY VALUE_FROM="EntityOpinion"/>
    /// >
    /// -->
    /// </summary>
    internal static class pattern19
    {
        private const string ENTITY_MI = "G";
        private const string VERB_MI   = "P";

        public static SubjectData GetSubjectData( XElement subsent, Language language, XElement breakSearchElement = null )
        {subsent.ThrowIfNotSubsent();

            return (GetSubjectData( subsent.ElementsExceptT(), language, breakSearchElement ));
        }
        public static SubjectData GetSubjectData( IEnumerable< XElement > elements, Language language, XElement breakSearchElement = null )
        {
            if ( elements.IsNotHasObject() && elements.IsNotInDirectSpeech() )
            {
            var subjectData = default(SubjectData);
            if ( elements.IsContainsSubjectEntityOpinion( language, out subjectData, DictionaryTypeVerbs.VerbsOpinion5, ENTITY_MI, VERB_MI, breakSearchElement ) )
                return (subjectData);
            }
            return (null);
        }


        public static SubjectData GetSubjectData4Citation( XElement subsent, Language language )
        {subsent.ThrowIfNotSubsent();

            var elements = subsent.ElementsExceptT();

            if ( elements.IsNotHasObject() && elements.IsNotInDirectSpeech() )
            {
            var subjectData = default(SubjectData);
            if ( elements.IsContainsSubjectEntityOpinion( language, out subjectData, DictionaryTypeVerbs.VerbsOpinion5, ENTITY_MI, VERB_MI, null, true ) )
                return (subjectData);
            }
            return (null);
        }
    }

    /// <summary>
    /// <!-- ПАРЫ  {VERB+ENTITY - CET+ENTITY/ENTITY[ @SUBJECT ]} МЕНЯЮТСЯ МЕСТАМИ.
    /// pattern20 ::= 
    /// <
    ///     <CET>в </CET>
    ///     <ENTITY>
    ///       <ENTITY SUBJECT="TRUE" MI="G"/>
    ///     </ENTITY>
    ///     <VERB TYPE="Verb" VALUE_FROM="VerbsOpinion5" MI="P" />
    ///     <ENTITY VALUE_FROM="EntityOpinion"/>
    /// >
    /// -->
    /// </summary>
    internal static class pattern20
    {
        private const string ENTITY_MI = "G";
        private const string VERB_MI   = "P";

        public static SubjectData GetSubjectData( XElement subsent, Language language, XElement breakSearchElement = null )
        {subsent.ThrowIfNotSubsent();

            return (GetSubjectData( subsent.ElementsExceptT(), language, breakSearchElement ));
        }
        public static SubjectData GetSubjectData( IEnumerable< XElement > elements, Language language, XElement breakSearchElement = null )
        {
            if ( elements.IsNotHasObject() && elements.IsNotInDirectSpeech() )
            {
            var subjectData = default(SubjectData);
            if ( elements.IsContainsSubjectEntityOpinionWithB( language, out subjectData, DictionaryTypeVerbs.VerbsOpinion5, ENTITY_MI, VERB_MI, breakSearchElement ) )
                return (subjectData);
            }
            return (null);
        }


        public static SubjectData GetSubjectData4Citation( XElement subsent, Language language )
        {subsent.ThrowIfNotSubsent();

            var elements = subsent.ElementsExceptT();

            if ( elements.IsNotHasObject() && elements.IsNotInDirectSpeech() )
            {
            var subjectData = default(SubjectData);
            if ( elements.IsContainsSubjectEntityOpinionWithB( language, out subjectData, DictionaryTypeVerbs.VerbsOpinion5, ENTITY_MI, VERB_MI, null, true ) )
                return (subjectData);
            }
            return (null);
        }
    }

    /// <summary>
    /// <!-- ПАРЫ  {VERB+ENTITY - CET+ENTITY/ENTITY[ @SUBJECT ]} МЕНЯЮТСЯ МЕСТАМИ. 
    /// pattern21 ::= 
    /// <
    ///     <CET>в </CET>
    ///     <ENTITY>
    ///       <ENTITY SUBJECT="TRUE" MI="G"/>
    ///     </ENTITY>
    ///     <VERB TYPE="Verb" VALUE_FROM="VerbsOpinion5" MI="A" />
    ///     <ENTITY VALUE_FROM="EntityOpinion"/>
    /// >
    /// -->
    /// </summary>
    internal static class pattern21
    {
        private const string ENTITY_MI = "G";
        private const string VERB_MI   = "A";

        public static SubjectData GetSubjectData( XElement subsent, Language language, XElement breakSearchElement = null )
        {subsent.ThrowIfNotSubsent();

            return (GetSubjectData( subsent.ElementsExceptT(), language, breakSearchElement ));
        }
        public static SubjectData GetSubjectData( IEnumerable< XElement > elements, Language language, XElement breakSearchElement = null )
        {
            if ( elements.IsNotHasObject() && elements.IsNotInDirectSpeech() )
            {
            var subjectData = default(SubjectData);
            if ( elements.IsContainsSubjectEntityOpinionWithB( language, out subjectData, DictionaryTypeVerbs.VerbsOpinion5, ENTITY_MI, VERB_MI, breakSearchElement ) )
                return (subjectData);
            }
            return (null);
        }


        public static SubjectData GetSubjectData4Citation( XElement subsent, Language language )
        {subsent.ThrowIfNotSubsent();

            var elements = subsent.ElementsExceptT();

            if ( elements.IsNotHasObject() && elements.IsNotInDirectSpeech() )
            {
            var subjectData = default(SubjectData);
            if ( elements.IsContainsSubjectEntityOpinionWithB( language, out subjectData, DictionaryTypeVerbs.VerbsOpinion5, ENTITY_MI, VERB_MI, null, true ) )
                return (subjectData);
            }
            return (null);
        }
    }

    /// <summary>
    /// <!-- ПАРЫ {VERB+ENTITY - ENTITY[ @SUBJECT ]}  МЕНЯЮТСЯ МЕСТАМИ. 
    /// pattern22 ::= 
    /// <
    ///     <ENTITY SUBJECT="TRUE" MI="G" />
    ///     <VERB TYPE="Verb" VALUE_FROM="VerbsOpinion6" MI="A" />
    ///     <ENTITY VALUE_FROM="EntityOpinion"/>
    /// >
    /// -->
    /// </summary>
    internal static class pattern22
    {
        private const string ENTITY_MI = "G";
        private const string VERB_MI   = "A";

        public static SubjectData GetSubjectData( XElement subsent, Language language, XElement breakSearchElement = null )
        {subsent.ThrowIfNotSubsent();

            return (GetSubjectData( subsent.ElementsExceptT(), language, breakSearchElement ));
        }
        public static SubjectData GetSubjectData( IEnumerable< XElement > elements, Language language, XElement breakSearchElement = null )
        {
            if ( elements.IsNotHasObject() && elements.IsNotInDirectSpeech() )
            {
            var subjectData = default(SubjectData);
            if ( elements.IsContainsSubjectEntityOpinion( language, out subjectData, DictionaryTypeVerbs.VerbsOpinion6, ENTITY_MI, VERB_MI, breakSearchElement ) )
                return (subjectData);
            }
            return (null);
        }


        public static SubjectData GetSubjectData4Citation( XElement subsent, Language language )
        {subsent.ThrowIfNotSubsent();

            var elements = subsent.ElementsExceptT();

            if ( elements.IsNotHasObject() && elements.IsNotInDirectSpeech() )
            {
            var subjectData = default(SubjectData);
            if ( elements.IsContainsSubjectEntityOpinion( language, out subjectData, DictionaryTypeVerbs.VerbsOpinion6, ENTITY_MI, VERB_MI, null, true ) )
                return (subjectData);
            }
            return (null);
        }
    }

    /// <summary>
    /// <!--  ПАРЫ {VERB - ENTITY[ @SUBJECT ]} МЕНЯЮТСЯ МЕСТАМИ
    /// pattern23 ::= 
    /// <
    /// <VERB TYPE="Verb" VALUE_FROM="VerbsOpinion7" MI="P" />
    /// <ENTITY SUBJECT="TRUE" MI="N" />
    /// >
    /// -->
    /// </summary>
    internal static class pattern23
    {
        private const string ENTITY_MI = "N";
        private const string VERB_MI   = "P";

        public static SubjectData GetSubjectData( XElement subsent, Language language, XElement breakSearchElement = null )
        {subsent.ThrowIfNotSubsent();

            return (GetSubjectData( subsent.ElementsExceptT(), language, breakSearchElement ));
        }
        public static SubjectData GetSubjectData( IEnumerable< XElement > elements, Language language, XElement breakSearchElement = null )
        {
            if ( elements.IsNotHasObject() && elements.IsNotInDirectSpeech() )
            {
            var subjectData = default(SubjectData);
            if ( elements.IsContainsSubjectData( language, out subjectData, DictionaryTypeVerbs.VerbsOpinion7, ENTITY_MI, VERB_MI, breakSearchElement ) )
                return (subjectData);
            }
            return (null);
        }


        public static SubjectData GetSubjectData4Citation( XElement subsent, Language language )
        {subsent.ThrowIfNotSubsent();

            var elements = subsent.ElementsExceptT();

            if ( elements.IsNotHasObject() && elements.IsNotInDirectSpeech() )
            {
            var subjectData = default(SubjectData);
            if ( elements.IsContainsSubjectData( language, out subjectData, DictionaryTypeVerbs.VerbsOpinion7, ENTITY_MI, VERB_MI, null, true ) )
                return (subjectData);
            }
            return (null);
        }
    }

    /// <summary>
    /// <!-- ПАРЫ {VERB + ENTITY[ @SUBJECT ] + ENTITY} НЕ МЕНЯЮТСЯ МЕСТАМИ. 
    /// pattern24 ::= 
    /// <
    /// <VERB TYPE="Verb" VALUE_FROM="VerbsOpinion5" MI="A" />
    /// <ENTITY SUBJECT="TRUE" MI="N" />
    /// <ENTITY VALUE_FROM="EntityOpinion"/>
    /// >
    /// -->
    /// </summary>
    internal static class pattern24
    {
        private const string ENTITY_MI = "N";
        private const string VERB_MI   = "A";

        public static SubjectData GetSubjectData( XElement subsent, Language language, XElement breakSearchElement = null )
        {subsent.ThrowIfNotSubsent();

            return (GetSubjectData( subsent.ElementsExceptT(), language, breakSearchElement ));
        }
        public static SubjectData GetSubjectData( IEnumerable< XElement > elements, Language language, XElement breakSearchElement = null )
        {
            if ( elements.IsNotHasObject() && elements.IsNotInDirectSpeech() )
            {
                var subjectData = default(SubjectData);
                if ( elements.IsContainsSubjectEntityOpinion_2( language, out subjectData, DictionaryTypeVerbs.VerbsOpinion5, 
                                                                ENTITY_MI, VERB_MI, breakSearchElement ) )
                {
                    return (subjectData);
                }
            }
            return (null);
        }


        public static SubjectData GetSubjectData4Citation( XElement subsent, Language language )
        {subsent.ThrowIfNotSubsent();

            var elements = subsent.ElementsExceptT();

            if ( elements.IsNotHasObject() && elements.IsNotInDirectSpeech() )
            {
                var subjectData = default(SubjectData);
                if ( elements.IsContainsSubjectEntityOpinion_2( language, out subjectData, DictionaryTypeVerbs.VerbsOpinion5, 
                                                                ENTITY_MI, VERB_MI, null, true ) )
                {
                    return (subjectData);
                }
            }
            return (null);
        }
    }
}