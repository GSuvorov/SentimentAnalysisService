using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace TextParsing
{
	/// <summary>
	/// Параграф
	/// </summary>
	public sealed class Paragraph : TextPart
	{
		/// <summary>
		/// Коллекция предложений
		/// </summary>
		public Sentence[] Sentences { get; private set; }

		/// <summary>
		/// Задание коллекции предложений.
		/// Для предложений устанавливается свойство Parent на данный параграф
		/// </summary>
		/// <param name="sentences">коллекция предложений</param>
		public void SetSentences( IEnumerable< Sentence > sentences )
		{
			Sentences = sentences.ToArray();
            foreach ( Sentence sentence in Sentences )
            {
                sentence.Parent = this;
            }
		}

		#region [.ctor()]
		public Paragraph(string text, int paragraphStart, int paragraphEnd)
			: base(text.Substring(paragraphStart, paragraphEnd - paragraphStart), paragraphStart)
		{
		}

		public Paragraph(string paragraphText, int startPosition)
			: base(paragraphText, startPosition)
		{
		}
		#endregion
	}
}
