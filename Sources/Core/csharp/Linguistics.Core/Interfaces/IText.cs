using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Linguistics.Core
{
	/// <summary>
	/// Интерфейс текста (множества предложений)
	/// </summary>
	public interface IText : ISerializerToRDF
	{
		/// <summary>
		/// Предложения
		/// </summary>
		Sentence[] Sentences { get; set; }
	}
}
