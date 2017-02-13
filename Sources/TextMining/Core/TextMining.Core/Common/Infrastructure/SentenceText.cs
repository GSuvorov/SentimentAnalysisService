using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;

namespace TextMining.Core
{
    public class SentenceText
    {
        private const string TEXT_XML = "<TEXT/>";

        public SentenceText( XElement sent )
        {
            sent.ThrowIfNotSent();

            XDocument = XDocument.Parse( TEXT_XML );
            XDocument.Root.Add( sent ); 
        }

        public XDocument XDocument
        {
            get;
            private set;
        }
        public XElement Sent
        {
            get { return (GetSent( XDocument )); }
        }

        public static XElement GetSent( XDocument xdocument )
        {
            xdocument.ThrowIfNull("xdocument");

            return (xdocument.Root.Elements().First());
        }

        public static XDocument ToXDocument( IEnumerable< XElement > sents )
        {
            sents.ThrowIfNull("sents");

            var xdocument = XDocument.Parse( TEXT_XML );

            foreach ( var sent in sents )
            {
                //---sent.ThrowIfNotSent();

                xdocument.Root.Add( sent ); 
            }
            return (xdocument);
        }
    }
}
