using System;
using System.Linq;
using System.Xml.Linq;
using System.Collections.Generic;

using TextMining.Core;
using TextMining.Service;

namespace OpinionMining
{
    using OpinionMining.DirectIndirectSpeechExtensions;

    /*
    inderectspeech ::= 
    <
          <!-begin->
          <!-end->
    >  
    */
    internal static class inderectspeech
    {
        public static bool IsCondition( XElement subsent )
        {
            return (true);    
        }
    }

    /*
    inderectspeech1 ::= 
    <
          <!-begin->
          <!-end->
          <U> - </U>
    >
    */
    internal static class inderectspeech1
    {
        public static bool IsCondition( XElement subsent )
        {
            return (subsent.IsSubsentEndWithU( separator1.uConditionFunction ));
        }

        public static XElement GetElementUByCondition( IEnumerable< XElement > elements )
        {
            return (elements.GetElementU( separator1.uConditionFunction ));
        }
        public static bool IsConditionPrecede( XElement subsent )
        {
            return (subsent.IsSubsentPrecedeU( separator1.uConditionFunction ));
        }
    }

    /*
    inderectspeech2 ::= 
    <
          <!-begin->
          <!-end->
          <U> , || - </U>
    >
    */
    internal static class inderectspeech2
    {
        public static bool IsCondition( XElement subsent )
        {
            return (subsent.IsSubsentEndWithU( separator2.uConditionFunction ));
        }
        public static bool IsConditionPrecede( XElement subsent )
        {
            return (subsent.IsSubsentPrecedeU( separator2.uConditionFunction ));
        }
    }

    /*
    inderectspeech3 ::= 
    <
          <!-begin->
          <!-first words => "first_word1.txt" ->
          <!-end->
          <U> , || - </U>
    >
    */
    internal static class inderectspeech3
    {
        public static bool IsCondition( XElement subsent, Language language )
        {
            var first = subsent.ElementsExceptT().FirstOrDefault();
            if ( first.IsNull() ) 
                return (false);
            if ( first.IsElementU() )
                 first = first.GetNotEmptyElementAfterSelf();
            if ( first.IsNull() )
                return (false);

            if ( !first.IsVALUEContains_or_IsDescendantsOrSelfPREAttributeContains( language.DictionaryManager.FirstWords1 ) )
                return (false);

            return (true);
        }
    }
    internal static class inderectspeech3_inderectspeech2
    {
        public static bool IsCondition( XElement subsent, Language language )
        {
            return (IsCondition( subsent, subsent, language ));
        }
        public static bool IsCondition( XElement subsent1, XElement subsent2, Language language )
        {
            if ( !inderectspeech3.IsCondition( subsent1, language ) )
                return (false);

            return (inderectspeech2.IsCondition( subsent2 ));
        }
    }

    /*
    inderectspeech4 ::= 
    <
        <!-begin->
        <!-first words->
        <CET TYPE="Pretext" />
        <!-"first_word1.txt" ->
        <!-end->
        <U> , || - </U>
    >
    */
    internal static class inderectspeech4
    {
        public static bool IsCondition( XElement subsent, Language language )
        {
            var first = subsent.ElementsExceptT().FirstOrDefault();
            if ( first.IsNull() ) 
                return (false);
            if ( first.IsElementU() )
                 first = first.GetNotEmptyElementAfterSelf();
            if ( first.IsNull() || !first.IsElementCET() || (first.GetAttributeTYPE() != TypeAttributeValue.Pretext) )
                return (false);

            var second = first.GetNotEmptyElementAfterSelf();
            if ( second.IsNull() )
                return (false);

            if ( !second.IsVALUEContains_or_IsDescendantsOrSelfPREAttributeContains( language.DictionaryManager.FirstWords1 ) )
                return (false);

            return (true);
        }
    }
    internal static class inderectspeech4_inderectspeech2
    {
        public static bool IsCondition( XElement subsent, Language language )
        {
            return (IsCondition( subsent, subsent, language ));
        }
        public static bool IsCondition( XElement subsent1, XElement subsent2, Language language )
        {
            if ( !inderectspeech4.IsCondition( subsent1, language ) )
                return (false);

            return (inderectspeech2.IsCondition( subsent2 ));
        }
    }

    /*
    inderectspeech5 ::= 
    <
        <!-begin->
        <!-first words->
        <ENTITY />
        <!-"first_word1.txt" ->
        <!-end->
        <U> , || - </U>
    >
    */
    internal static class inderectspeech5
    {
        public static bool IsCondition( XElement subsent, Language language )
        {
            var first = subsent.ElementsExceptT().FirstOrDefault();
            if ( first.IsNull() ) 
                return (false);
            if ( first.IsElementU() )
                 first = first.GetNotEmptyElementAfterSelf();
            if ( first.IsNull() || !first.IsElementENTITY() )
                return (false);

            var second = first.GetNotEmptyElementAfterSelf();
            if ( second.IsNull() )
                return (false);

            if ( !second.IsVALUEContains_or_IsDescendantsOrSelfPREAttributeContains( language.DictionaryManager.FirstWords1 ) )
                return (false);

            return (true);
        }
    }
    internal static class inderectspeech5_inderectspeech2
    {
        public static bool IsCondition( XElement subsent, Language language )
        {
            return (IsCondition( subsent, subsent, language ));
        }
        public static bool IsCondition( XElement subsent1, XElement subsent2, Language language )
        {
            if ( !inderectspeech5.IsCondition( subsent1, language ) )
                return (false);

            return (inderectspeech2.IsCondition( subsent2 ));
        }
    }

    /*
    inderectspeech6 ::= 
    <
        <!-begin->
        <!-first words->
        <CET TYPE="Pretext" />
        <ENTITY />
        <!-"first_word1.txt" ->
        <!-end->
        <U> , || - </U>
    >
    */
    internal static class inderectspeech6
    {
        public static bool IsCondition( XElement subsent, Language language )
        {
            var first = subsent.ElementsExceptT().FirstOrDefault();
            if ( first.IsNull() ) 
                return (false);
            if ( first.IsElementU() )
                 first = first.GetNotEmptyElementAfterSelf();
            if ( first.IsNull() || !first.IsElementCET() || (first.GetAttributeTYPE() != TypeAttributeValue.Pretext) )
                return (false);

            var second = first.GetNotEmptyElementAfterSelf();
            if ( second.IsNull() || !second.IsElementENTITY() )
                return (false);

            var three = second.GetNotEmptyElementAfterSelf();
            if ( three.IsNull() )
                return (false);

            if ( !three.IsVALUEContains_or_IsDescendantsOrSelfPREAttributeContains( language.DictionaryManager.FirstWords1 ) )
                return (false);

            return (true);
        }
    }
    internal static class inderectspeech6_inderectspeech2
    {
        public static bool IsCondition( XElement subsent, Language language )
        {
            return (IsCondition( subsent, subsent, language ));
        }
        public static bool IsCondition( XElement subsent1, XElement subsent2, Language language )
        {
            if ( !inderectspeech6.IsCondition( subsent1, language ) )
                return (false);

            return (inderectspeech2.IsCondition( subsent2 ));
        }
    }
}