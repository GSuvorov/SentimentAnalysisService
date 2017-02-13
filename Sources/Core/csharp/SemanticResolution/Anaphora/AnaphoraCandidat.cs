using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;

namespace SemanticResolution
{
	/// <summary>
	/// класс кандидата на разрешение анафории местоимения
	/// </summary>
	internal class AnaphoraCandidat : IComparable<AnaphoraCandidat>
	{
		/// <summary>
		/// сущность
		/// </summary>
		public XElement Entity { get; set; }

		/// <summary>
		/// вероятность разрешения
		/// </summary>
		public double Probability { get; set; }

		/// <summary>
		/// расстояние в подпредложениях
		/// </summary>
		public int SentenceDistance { get; set; }

		/// <summary>
		/// расстояние в сущностях
		/// </summary>
		public int EntityDistance { get; set; }

		public AnaphoraCandidat(XElement entity, int distance)
		{
			Entity = entity;
			Probability = 0;
			SentenceDistance = distance;
			EntityDistance = 0;
		}

		public int CompareTo(AnaphoraCandidat other)
		{
			return Probability.CompareTo(other.Probability);
		}
	}
}
