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
	/// Тип подпредложения
	/// </summary>
	public enum SubSentenceType
	{
		/// <summary>
		/// тип отсутствует
		/// </summary>
		Unknown,
		/// <summary>
		/// неопределенный тип
		/// </summary>
		Default,
		/// <summary>
		/// обстоятельственный оборот
		/// </summary>
		AdjunctPhrase,
		/// <summary>
		/// часть текста в скобках
		/// </summary>
		ApproximatingForceps,
		/// <summary>
		/// деепричастный оборот
		/// </summary>
		DanglingParticiple,
		/// <summary>
		/// прямая речь/цитирование
		/// </summary>
		DirectSpeech,
		/// <summary>
		/// равноправное предложение
		/// </summary>
		EqualInRights,
		/// <summary>
		/// вводное
		/// </summary>
		Introductory,
		/// <summary>
		/// отдельное предложение
		/// </summary>
		NewSent,
		/// <summary>
		/// причастный оборот
		/// </summary>
		Participial,
		/// <summary>
		/// подчиненное предложение
		/// </summary>
		Subordinate,
	}
}
