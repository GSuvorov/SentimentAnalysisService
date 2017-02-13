using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;

namespace Test
{
	class Program
	{
		static void Main(string[] args)
		{
			var selector = new SimpleEntities.SimpleEntitySelector(@"D:\NLP\Lingvistic2012\SimpleEntities\email_test");

			using (StreamReader reader = new StreamReader("1.txt", Encoding.UTF8))
			{
				var entities = selector.SelectSimpleEntities(reader.ReadToEnd());
			}
		}
	}
}
