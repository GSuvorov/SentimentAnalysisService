using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;

namespace SimpleEntities
{
	/// <summary>
	/// Тип выделения простой сущности
	/// </summary>
	public enum SelectionType
	{
		/// <summary>
		/// Всегда выделять
		/// </summary>
		True,
		/// <summary>
		/// Никогда не выделять
		/// </summary>
		False,
		/// <summary>
		/// Выделять только в качестве дочерней сущности
		/// </summary>
		Child
	}

	/// <summary>
	/// Класс для хранения информации о регулярном выражении для сущности
	/// </summary>
	public class EntityExpression
	{
		/// <summary>
		/// Строка регулярного выражения
		/// </summary>
		public string Expression { get; private set; }
		/// <summary>
		/// Регулярное выражение
		/// </summary>
		public Regex RegularExpression { get; private set; }
		/// <summary>
		/// Используемые регулярные выражения
		/// </summary>
		public List<string> UsedExpressions { get; set; }
		/// <summary>
		/// Тип выделения
		/// </summary>
		public SelectionType SelectionType { get; set; }
		/// <summary>
		/// Тип сущности
		/// </summary>
		public string EntityType { get; set; }

		#region [.ctor().]
		public EntityExpression()
		{
			UsedExpressions = new List<string>();
			SelectionType = SelectionType.True;
		}

		public EntityExpression(Regex expression)
		{
			UsedExpressions = new List<string>();
			SelectionType = SelectionType.True;
			RegularExpression = expression;
			Expression = expression.ToString();
		}
		#endregion

		/// <summary>
		/// Сохранение регулярного выражения
		/// </summary>
		public void SaveRegex(string expression)
		{
			Expression = expression;
			RegularExpression = new Regex(Expression);
		}
	}
}
