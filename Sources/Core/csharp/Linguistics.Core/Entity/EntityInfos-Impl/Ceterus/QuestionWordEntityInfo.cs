using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Linguistics.Core
{
	/// <summary>
	/// Информация о вопросительном слове
	/// </summary>
	[Serializable]
	public class QuestionWordEntityInfo : CeterusEntityInfoBase
	{
		#region [.ctor().]
		public QuestionWordEntityInfo()
			: base(EntityType.QuestionWord)
		{
		}
		#endregion
	}
}
