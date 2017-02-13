using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;

namespace Linguistics.Core
{
	/// <summary>
	/// Информация о персоне
	/// </summary>
    [Serializable]
    public abstract class PersonEntityInfoBase : NamedEntityInfoBase
	{
		/// <summary>
		/// Имя
		/// </summary>
		public string FirstName { get; set; }
		/// <summary>
		/// Фамилия
		/// </summary>
		public string LastName { get; set; }
		/// <summary>
		/// Отчество
		/// </summary>
		public string MiddleName { get; set; }

        /// <summary>
        /// Должность
        /// </summary>
        public string Post { get; set; }

		#region [.ctor().]
		protected PersonEntityInfoBase(EntityType type)
			: base(type)
		{
		}
		#endregion

		#region [ISerializerToRDF]
		public override XElement ToXElement()
		{
			XElement result = base.ToXElement();
			AddNotEmptyAttribute(result, RDF.Attribute.FirstName, FirstName);
			AddNotEmptyAttribute(result, RDF.Attribute.LastName, LastName);
			AddNotEmptyAttribute(result, RDF.Attribute.MiddleName, MiddleName);
            AddNotEmptyAttribute(result, RDF.Attribute.Post, Post);
			return result;
		}
		#endregion

        /// <summary>
        /// Получение объекта должности
        /// </summary>
        /// <param name="children">дочерние сущности</param>
        /// <returns>объект должности</returns>
        public string GetPostObject(IEnumerable<Entity> children)
        {
            if (!string.IsNullOrEmpty(Post))
            {
                var postObject = GetOrgOrPlaceDescendant(children);
                if (postObject != null)
                    return postObject.Text;
            }
            return string.Empty;
        }

        /// <summary>
        /// Получение наследника, имеющего семантику организации или географии
        /// </summary>
        /// <param name="children">дочерние сущности</param>
        /// <returns>наследник</returns>
        private Entity GetOrgOrPlaceDescendant(IEnumerable<Entity> children)
        {
            foreach (var child in children)
            {
                if (child.IsType(EntityType.OrgName) || child.IsType(EntityType.OrgNoInfo) ||
                    child.IsType(EntityType.PlaceName) || child.IsType(EntityType.PlaceNoInfo) || child.IsType(EntityType.PseudoGeo))
                {
                    return child;
                }
                else
                {
                    var OrgOrPlaceDescendant = GetOrgOrPlaceDescendant(child.Children);
                    if (OrgOrPlaceDescendant != null)
                        return OrgOrPlaceDescendant;
                }
            }
            return null;
        }
	}
}
