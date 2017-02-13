using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace TextParsing
{
	/// <summary>
	/// Предложение
	/// </summary>
	public sealed class Sentence : TextPart
	{
        /// <summary>
        /// Флаг обработки предложения
        /// </summary>
        public bool IsProcessed { get; set; }

		#region [.ctor().]
        public Sentence( string text, int sentenceStart, int sentenceEnd )
            : base( text.Substring( sentenceStart, sentenceEnd - sentenceStart ), sentenceStart )
        {
            IsProcessed = true;
        }

        public Sentence( string sentenceText, int sentenceStart )
            : base( sentenceText, sentenceStart )
        {
            IsProcessed = true;
        }
		#endregion
	}
}
