using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Linguistics.Core
{
	/// <summary>
	/// Интерфейс для доступа к связанным элементам
	/// </summary>
	/// <typeparam name="TElement">тип элемента</typeparam>
	/// <typeparam name="TParent">тип родителя</typeparam>
	public interface IRelations<TElement, TParent>
	{
		/// <summary>
		/// Родительский объект
		/// </summary>
		TParent ParentObject { get; set; }
		/// <summary>
		/// Предыдущий элемент
		/// </summary>
		TElement Previous { get; set; }
		/// <summary>
		/// Следующий элемент
		/// </summary>
		TElement Next { get; set; }
		/// <summary>
		/// Текущий элемент
		/// </summary>
		TElement Current { get; }
	}
}
