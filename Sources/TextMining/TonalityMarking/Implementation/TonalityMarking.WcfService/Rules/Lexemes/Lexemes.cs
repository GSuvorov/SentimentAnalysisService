using System;
using System.IO;
using System.Collections.Generic;

namespace TonalityMarking
{
    /// <summary>
    /// 
    /// </summary>
    internal class Lexemes
    {
        public Lexemes()
        {
            AdverbAdverb       = new RuleSet< Adverbs   , Adverbs   , Adverbs    >();
            AdjectiveAdjective = new RuleSet< Adjectives, Adjectives, Adjectives >();
            AdjectiveNoun      = new RuleSet< Adjectives, Nouns     , Nouns      >();
            ParticipleNoun     = new RuleSet< Verbs     , Nouns     , Nouns      >();

            VerbPassiveVoice     = new RuleSet< Verbs, Verbs >();
            VerbReflexivePronoun = new RuleSet< Verbs, Verbs >();
        }

        public RuleSet< Adverbs   , Adverbs   , Adverbs    > AdverbAdverb
        {
            get;
            private set;
        }
        public RuleSet< Adjectives, Adjectives, Adjectives > AdjectiveAdjective
        {
            get;
            private set;
        }
        public RuleSet< Adjectives, Nouns     , Nouns      > AdjectiveNoun
        {
            get;
            private set;
        }
        public RuleSet< Verbs     , Nouns     , Nouns      > ParticipleNoun
        {
            get;
            private set;
        }
        public RuleSet< Verbs, Verbs > VerbPassiveVoice
        {
            get;
            private set;
        }
        public RuleSet< Verbs, Verbs > VerbReflexivePronoun
        {
            get;
            private set;
        }        
    }

    /// <summary>
    /// 
    /// </summary>
    /// <typeparam name="T1"></typeparam>
    /// <typeparam name="T2"></typeparam>
    /// <typeparam name="T3"></typeparam>
    internal class RuleSet< T1, T2, T3 >
        where T3 : struct
    {
        private Dictionary< Pair< T1, T2 >, T3 > _Dictionary;

        public RuleSet()
        {
            _Dictionary = new Dictionary< Pair< T1, T2 >, T3 >();
        }

        public void AppendRule( T1 t1, T2 t2, T3 t3 )
        {
            var pair = new Pair< T1, T2 >( t1, t2 );
            if ( !_Dictionary.ContainsKey( pair ) )
            {
                _Dictionary.Add( pair, t3 );
            }
        }

        public T3? Check( T1 t1, T2 t2 )
        {
            var result = default(T3);
            if ( _Dictionary.TryGetValue( new Pair< T1, T2 >( t1, t2 ), out result ) )
            {
                return (result);
            }

            return (null);
        }
    }

    /// <summary>
    /// 
    /// </summary>
    /// <typeparam name="T1"></typeparam>
    /// <typeparam name="T2"></typeparam>
    internal class RuleSet< T1, T2 >
        where T2 : struct
    {
        private Dictionary< T1, T2 > _Dictionary;

        public RuleSet()
        {
            _Dictionary = new Dictionary< T1, T2 >();
        }

        public void AppendRule( T1 t1, T2 t2 )
        {
            if ( !_Dictionary.ContainsKey( t1 ) )
            {
                _Dictionary.Add( t1, t2 );
            }
        }

        public T2? Check( T1 t1 )
        {
            var result = default(T2);
            if ( _Dictionary.TryGetValue( t1, out result ) )
            {
                return (result);
            }

            return (null);
        }
    }
}



