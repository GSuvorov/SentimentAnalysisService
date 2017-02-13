using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Xml;
using System.Xml.Linq;

using TextMining.Core;

namespace TonalityMarking
{
    internal static class RulesExtensions
    {
        public static void ExecRules_BuildChains_Participial_DanglingParticiple( this XElement subsent, Language language )
        {
            subsent.ThrowIfNotSubsent();
            subsent.ThrowIfNotTypeParticipialOrDanglingParticiple();

            //Собираем цепочки правилами id=000-099 (order_rules_participial.xml), 
            // для Participle || DanglingParticiple
            foreach ( var rule in language.Rules.SubsentencesRulesByOrder4Participial )
            {
                var result = default(bool);
                do
                {
                    result = rule.Process( subsent );
                }
                while ( rule.RuleCycle == RuleCycleType.RecycleOnSuccsess && result );
            }
        }

        public static void ExecRules_BuildChains( this XElement subsent, Language language )
        {
            subsent.ThrowIfNotSubsent();
            subsent.ThrowIfTypeParticipialOrDanglingParticiple();

            //Собираем цепочки правилами id=000-099 (order_rules.xml), 
            // для НЕ (Participle || DanglingParticiple)
            foreach ( var rule in language.Rules.SubsentencesRulesByOrder )
            {
                var result = default(bool);
                do
                {
                    result = rule.Process( subsent );
                }
                while ( rule.RuleCycle == RuleCycleType.RecycleOnSuccsess && result );
            }
        }

        public static void ExecRules_Sentences( this XElement subsent, Language language )
        {
            subsent.ThrowIfNotSubsent();

            //Отрабатываем правилами id=100_101_102-110-111-112-120-122-125-126
            foreach ( var rule in language.Rules.Sentences.RulesByOrder )
            {
                var result = rule.Process( subsent );
                if ( result && (rule.RuleCycle == RuleCycleType.None) )
                    break;
            }
        }

        public static bool ExecRules_Inquiry( this XElement subsent, Language language )
        {
            subsent.ThrowIfNotSubsent();

            foreach ( var rule in language.Rules.Inquiry.RulesByOrder )
            {
                var result = rule.Process( subsent );
                if ( result )
                    return (true);
            }
            return (false);
        }

        public static bool ExecRules_Sentences_SubsentsNoNoun( this XElement subsent, Language language )
        {
            subsent.ThrowIfNotSubsent();

            foreach ( var rule in language.Rules.Sentences.SubsentsNoNoun )
            {
                var result = rule.Process( subsent );
                if ( result )
                    return (true);
            }
            return (false);
        }

        public static void ExecRules_UnionOfConcessionAndReversIntoductory( this XElement sent, Language language )
        {
            sent.ThrowIfNotSent();

            var skip_next_rule = false;
            foreach ( var rule in language.Rules.UnionOfConcessionAndReversIntoductory.RulesByOrder )
            {
                var result = default(bool);
                do
                {
                    //fusking-crutch
                    if ( skip_next_rule )
                    {
                        skip_next_rule = false;
                        continue;
                    }

                    result = rule.Process( sent );

                    //fusking-crutch
                    if ( result && rule.Id == "308_revers_sentiment_inquery9" )
                    {
                        skip_next_rule = true;
                    }
                }
                while ( rule.RuleCycle == RuleCycleType.RecycleOnSuccsess && result );
            }
        }
        
        public static bool ExecRules_SubordinateSubsent( this XElement subsent, Language language )
        {
            subsent.ThrowIfNotSubsent();

            foreach ( var rule in language.Rules.SubordinateSubsent.RulesByOrder )
            {
                var result = rule.Process( subsent );
                if ( result )
                    return (true);
            }
            return (false);
        }

        public static void ExecRules_HomogenousEntity( this XDocument xdoc, Language language )
        {
            var homogeneouss = 
                from subsent in xdoc.GetSents( language )
                                    .GetDescendantsSubsent()
                from e in subsent.DescendantsAndSelfEntity()
                where ( (e.GetAttribute( TextMining.Core.XLinqExtensions.Consts.TYPE_ATTRIBUTE       ) == XLinqExtensions.HOMOGENEOUS_VALUE) ||
                        (e.GetAttribute( TextMining.Core.XLinqExtensions.Consts.TYPE_SAVED_ATTRIBUTE ) == XLinqExtensions.HOMOGENEOUS_VALUE)
                      )
                select e;

            //Собираем цепочки правилами id=000-099 (order_rules_participial.xml), 
            // для ENTITY[ @TYPE = 'Homogenous' ]
            foreach ( var homogeneous in homogeneouss )
            {
                homogeneous.ExecRules_HomogenousEntity( language );
            }
        }
        private static void ExecRules_HomogenousEntity( this XElement entity, Language language )
        {
            //Собираем цепочки правилами id=000-099 (order_rules_participial.xml), 
            // для ENTITY[ @TYPE = 'Homogenous' ]
            foreach ( var rule in language.Rules.SubsentencesRulesByOrder4Homogenous )
            {
                var result = default(bool);
                do
                {
                    result = rule.Process( entity );
                }
                while ( rule.RuleCycle == RuleCycleType.RecycleOnSuccsess && result );
            }

            //if entity.Elements().Count() == 1
            var first = entity.TryGetSingleElement();
            if ( first != null )
            {
                entity.SetAttributeSNT( first.GetAttributeSNT() );
                entity.SetAttributeFRT( first.GetAttributeFRT() );
            }
            else
            {
                entity.SetAttributeSNT( TonalityType.nNeut );
                entity.SetAttributeFRT( 0 );
            }
        }
    }
}



