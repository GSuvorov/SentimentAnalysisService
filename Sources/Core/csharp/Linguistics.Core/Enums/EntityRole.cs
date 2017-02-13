using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

#if WITH_OM_TM
namespace Lingvistics
#else
namespace Linguistics.Core
#endif
{
	/// <summary>
	/// Роль сущности в предложении
	/// </summary>
	public enum EntityRole
	{
		/// <summary>
		/// Не задана
		/// </summary>
		Undefined,
		/// <summary>
		/// Обстоятельство
		/// </summary>
		Adj,
		/// <summary>
		/// Прилагательное
		/// </summary>
		Adjective,
		/// <summary>
		/// Наречие
		/// </summary>
		Adverb,
		/// <summary>
		/// Дополнение
		/// </summary>
		Obj,
		/// <summary>
		/// Другая
		/// </summary>
		Other,
		/// <summary>
		/// Подлежащее
		/// </summary>
		Subj,
	}
}
