using System;

namespace TonalityMarking
{
    /// <summary>
    /// Pair (two-tuple)
    /// </summary>
    public class Pair< TFirst, TSecond >
    {
        public Pair( TFirst first, TSecond second ) //: this()
        {
            this.First  = first;
            this.Second = second;
        }

        public TFirst First
        {
            get;
            private set;
        }
        public TSecond Second
        {
            get;
            private set;
        }

        public override bool Equals( object obj )
        {
            if ( obj is Pair< TFirst, TSecond > )
            {
                var other = (Pair< TFirst, TSecond >) obj;

                return (First.Equals( other.First ) && Second.Equals( other.Second ));
            }
            
            return (base.Equals( obj ));
        }
        public override int GetHashCode()
        {
            return (((First != null) ? First.GetHashCode() : 0) + ((Second != null) ? Second.GetHashCode() : 0));
        }
        public override string ToString()
        {
            return (First.ToString() + '-' + Second.ToString());
        }
    }

    /// <summary>
    /// Three-tuple
    /// </summary>
    public class Tuple3< TFirst, TSecond, TThree > : Pair< TFirst, TSecond >
    {
        public Tuple3(TFirst first, TSecond second, TThree three)
            : base( first, second )
        {
            Three = three;
        }

        public TThree Three
        {
            get;
            private set;
        }

        public override bool Equals(object obj)
        {
            if ( obj is Tuple3< TFirst, TSecond, TThree > )
            {
                var other = (Tuple3< TFirst, TSecond, TThree >) obj;

                return (First.Equals( other.First ) && Second.Equals( other.Second ) && Three.Equals( other.Three ));
            }
            
            return (base.Equals( obj ));
        }
        public override int GetHashCode()
        {
            return (base.GetHashCode() + ((Three != null) ? Three.GetHashCode() : 0));
        }
        public override string ToString()
        {
            return (base.ToString() + '-' + Three.ToString());
        }
    }

    /// <summary>
    /// Four-tuple
    /// </summary>
    public class Tuple4< TFirst, TSecond, TThree, TFour > : Tuple3< TFirst, TSecond, TThree >
    {
        public Tuple4( TFirst first, TSecond second, TThree three, TFour four )
            : base( first, second, three )
        {
            Four = four;
        }

        public TFour Four
        {
            get;
            private set;
        }

        public override bool Equals(object obj)
        {
            if ( obj is Tuple4< TFirst, TSecond, TThree, TFour > )
            {
                var other = (Tuple4< TFirst, TSecond, TThree, TFour >) obj;

                return (First .Equals( other.First  ) && 
                        Second.Equals( other.Second ) && 
                        Three .Equals( other.Three  ) && 
                        Four  .Equals( other.Four   ));
            }
            
            return (base.Equals( obj ));
        }
        public override int GetHashCode()
        {
            return (base.GetHashCode() + ((Four != null) ? Four.GetHashCode() : 0));
        }
        public override string ToString()
        {
            return (base.ToString() + '-' + Four.ToString());
        }
    }
}