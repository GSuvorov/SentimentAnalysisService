using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;

namespace Linguistics.Core
{
	/// <summary>
	/// Интерфейс сериализации RDF
	/// </summary>
	public interface ISerializerToRDF
	{
		/// <summary>
		/// Создание для объекта XML-элемента
		/// </summary>
		/// <returns>XML-элемент</returns>
		XElement ToXElement();
	}
}
