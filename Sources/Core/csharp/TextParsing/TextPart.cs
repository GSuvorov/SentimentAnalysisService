using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace TextParsing
{
    /// <summary>
    /// Часть текста
    /// </summary>
    public abstract class TextPart
    {
        /// <summary>
        /// Родительская часть текста
        /// </summary>
        public TextPart Parent { get; set; }

        /// <summary>
        /// Текст
        /// </summary>
        private StringBuilder _text;
        /// <summary>
        /// Текст
        /// </summary>
        public string Text { get { return _text.ToString(); } }

        /// <summary>
        /// Позиция начала части текста
        /// </summary>
        public int StartPosition { get; private set; }
        /// <summary>
        /// Позиция конца части текста
        /// </summary>
        public int EndPosition { get; private set; }

        #region [.ctor()]
        public TextPart( string text, int startPosition )
        {
            _text = new StringBuilder( text );
            StartPosition = startPosition;
            EndPosition = startPosition + _text.Length;
        }
        #endregion

        /// <summary>
        /// Добавление части текста, следующей непосредственно за данной.
        /// В противном случае добавления не произойдет.
        /// </summary>
        /// <param name="nextPart">следующая часть текста</param>
        /// <returns>результат добавления части</returns>
        public bool AppendNextPart( TextPart nextPart )
        {
            if ( EndPosition == nextPart.StartPosition )
            {
                _text.Append( nextPart._text );
                EndPosition = nextPart.EndPosition;
                return true;
            }
            return false;
        }

        /// <summary>
        /// Добавление части текста, идущей непосредственно перед данной.
        /// В противном случае добавления не произойдет.
        /// </summary>
        /// <param name="nextPart">предыдущая часть текста</param>
        /// <returns>результат добавления части</returns>
        public bool InsertPreviousPart( TextPart previousPart )
        {
            if ( (previousPart.EndPosition == StartPosition) || /// в рамках одного параграфа
                (previousPart.Parent.StartPosition + previousPart.EndPosition == Parent.StartPosition + StartPosition) ) /// соседние параграфы
            {
                _text.Insert( 0, previousPart._text );
                StartPosition = previousPart.StartPosition;
                return true;
            }
            return false;
        }
    }
}
