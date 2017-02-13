using System;

namespace TonalityMarking
{
    public enum Nouns 
    {
        nNeut = 1,

        nANeg = 2, //action_negative,
        nAPos = 3, //action_positive,
        nNeg  = 4,  //negative,
        nPos  = 5,  //positive,
        nPNeg = 6, //potential_negative,
        nPPos = 7, //potential_positive,
    }
    public enum Verbs
    {
        vNeut   = 8,

        vFlxNeg = 9, //reflexive_negative,
        vFlxPos = 10, //reflexive_positive,
        vLink   = 11,   //related,
        vNeg    = 12,    //negative,
        vPos    = 13,    //positive,
        vNegP   = 14,   //negative_pure,
        vPosP   = 15,   //positive_pure,
        vOppPos = 16, //opposition_positive,
        vOppNeg = 17, //opposition_negative,
    }
    public enum Adjectives
    {
        adjNeut  = 18,

        adjNeg   = 19,   //negative,
        adjPos   = 20,   //positive,
        adjAmplf = 21, //amplificator,
    }
    public enum Adverbs
    {
        advNeut  = 22,

        advNeg   = 23,   //negative,
        advPos   = 24,   //positive,
        advAmplf = 25, //amplificator,
    }
    public enum Collocations
    {
        collNeut = 26,    //neutral

        collNeg   = 27,   //negative,
        collPos   = 28,   //positive,
        collAmplf = 29,   //amplificator,
        collvNeg  = 30,
        collvPos  = 31,

        collvNeut = 32,   //verb-neutral
    }

    public enum TonalityType
    {
        //Nouns
        nNeut = Nouns.nNeut,

        nANeg = Nouns.nANeg, //action_negative,
        nAPos = Nouns.nAPos, //action_positive,
        nNeg  = Nouns.nNeg,  //negative,
        nPos  = Nouns.nPos,  //positive,
        nPNeg = Nouns.nPNeg, //potential_negative,
        nPPos = Nouns.nPPos, //potential_positive,

        //Verbs
        vNeut   = Verbs.vNeut,

        vFlxNeg = Verbs.vFlxNeg, //reflexive_negative,
        vFlxPos = Verbs.vFlxPos, //reflexive_positive,
        vLink   = Verbs.vLink,   //related,
        vNeg    = Verbs.vNeg,    //negative,
        vPos    = Verbs.vPos,    //positive,
        vNegP   = Verbs.vNegP,   //negative_pure,
        vPosP   = Verbs.vPosP,   //positive_pure,
        vOppPos = Verbs.vOppPos, //opposition_positive,
        vOppNeg = Verbs.vOppNeg, //opposition_negative,

        //Adjectives
        adjNeut  = Adjectives.adjNeut,

        adjNeg   = Adjectives.adjNeg,   //negative,
        adjPos   = Adjectives.adjPos,   //positive,
        adjAmplf = Adjectives.adjAmplf, //amplificator,
    
        //Adverbs
        advNeut  = Adverbs.advNeut,

        advNeg   = Adverbs.advNeg,   //negative,
        advPos   = Adverbs.advPos,   //positive,
        advAmplf = Adverbs.advAmplf, //amplificator,
    
        //Collocations
        collNeut  = Collocations.collNeut,

        collNeg   = Collocations.collNeg,   //negative,
        collPos   = Collocations.collPos,   //positive,
        collAmplf = Collocations.collAmplf, //amplificator,
        collvNeg  = Collocations.collvNeg,
        collvPos  = Collocations.collvPos,

        collvNeut = Collocations.collvNeut,
    }

    public static partial class TonalityTypeExtensions
    {
        public static string ToText( this Nouns nounType )
        {
            return (nounType.ToString());

            /*switch ( nounType )
            {
                case Nouns.nANeg: return ("nANeg");
                case Nouns.nAPos: return ("nAPos");
                case Nouns.nNeg:  return ("nNeg");
                case Nouns.nPos:  return ("nPos");
                case Nouns.nPNeg: return ("nPNeg");
                case Nouns.nPPos: return ("nPPos");
                case Nouns.Neut:  return(NEUTRAL);
                default:
                    throw (new ArgumentException(nounType.ToString()));
            }*/
        }
        public static string ToText( this Verbs verbType )
        {
            return (verbType.ToString());

            /*switch ( verbType )
            {
                case Verbs.reflexive_negative:  return ("vFlxNeg");
                case Verbs.reflexive_positive:  return ("vFlxPos");
                case Verbs.related:             return ("vLink");
                case Verbs.negative:            return ("vNeg");
                case Verbs.positive:            return ("vPos");
                case Verbs.negative_pure:       return ("vNegP");
                case Verbs.positive_pure:       return ("vPosP");
                case Verbs.opposition_positive: return ("vOppPos");
                case Verbs.opposition_negative: return ("vOppNeg");
                case Verbs.Neut:             return (NEUTRAL);
                default:
                    throw (new ArgumentException(verbType.ToString()));
            }*/
        }
        public static string ToText( this Adjectives adjType )
        {
            return (adjType.ToString());

            /*switch ( adjType )
            {
                case Adjectives.amplificator: return ("adjAmplf");
                case Adjectives.negative:     return ("adjNeg");
                case Adjectives.positive:     return ("adjPos");
                case Adjectives.Neut:      return (NEUTRAL);
                default:
                    throw (new ArgumentException(adjType.ToString()));
            }*/
        }
        public static string ToText( this Adverbs adverbType )
        {
            return (adverbType.ToString());

            /*switch ( adverbType )
            {
                case Adverbs.amplificator: return ("advAmplf");
                case Adverbs.negative:     return ("advNeg");
                case Adverbs.positive:     return ("advPos");
                case Adverbs.Neut:      return (NEUTRAL);
                default:
                    throw (new ArgumentException(adverbType.ToString()));
            }*/
        }        
        public static string ToText( this Collocations collocationType )
        {
            return (collocationType.ToString());

            /*switch ( collocationType )
            {
                case Collocations.amplificator: return ("collAmplf");
                case Collocations.negative:     return ("collNeg");
                case Collocations.positive:     return ("collPos");
                case Collocations.Neut:      return (NEUTRAL);
                default:
                    throw (new ArgumentException(collocationType.ToString()));
            }*/
        } 
        public static string ToText( this TonalityType tonalityType )
        {
            return (tonalityType.ToString());
        }
    }
}




