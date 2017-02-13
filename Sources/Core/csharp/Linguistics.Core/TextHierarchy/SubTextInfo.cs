using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;

namespace Linguistics.Core
{
	/// <summary>
	/// Информация по подтексту
	/// </summary>
    [Serializable]
	public class SubTextInfo : IText
	{
		/// <summary>
		/// Дочерние предложения
		/// </summary>
		/// должен быть всегда заполнен
		public Sentence[] Sentences { get; set; }

		#region [.self properties.]
		/// <summary>
		/// открывающий символ
		/// </summary>
		public char OpenSymbol { get; private set; }
		/// <summary>
		/// закрывающий символ
		/// </summary>
		public char CloseSymbol { get; private set; }
		#endregion

		#region [.ctor().]
		public SubTextInfo(char openSymbol, char closeSymbol)
		{
			OpenSymbol = openSymbol;
			CloseSymbol = closeSymbol;
		}
		#endregion

		#region [ISerializeRDF]
		public XElement ToXElement()
		{
			var result = new XElement( UnitTextType.SUB_SENT.ToString() );
			bool isFirstSymbol = InsertSymbol( result, OpenSymbol );
			foreach (var sentence in Sentences)
			{
				var element = sentence.ToXElement();
				if (isFirstSymbol)
					UpdateStartPosition(element, 1);
				result.Add(element);
			}
			InsertSymbol( result, CloseSymbol );
			return (result);
		}
		#endregion

		/// <summary>
		/// Обновление стартовой позиции элемента
		/// </summary>
		/// <param name="element">элемент</param>
		/// <param name="shift">сдвиг</param>
		public static void UpdateStartPosition(XElement element, int shift)
		{
			element.SetAttributeValue(RDF.Attribute.StartPosition, element.GetStartPosition() + shift);
		}

		/// <summary>
		/// Вставка заданного символа в заданный XML-эелемент
		/// </summary>
		/// <param name="element">XML-эелемент</param>
		/// <param name="symbol">символ</param>
		protected bool InsertSymbol(XElement element, char symbol)
		{
			if (symbol != default(char))
			{
				element.Add(new UnmarkedText(symbol.ToString(), 0).ToXElement());
				return true;
			}
			return false;
		}
	}
}
